#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

#include <SQLiteCpp/SQLiteCpp.h>
#include <ctre.hpp>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <httplib.h>

#include "data.hpp"
#include "mcu.hpp"
#include "playerMode.hpp"
#include "serial.hpp"
#include "web.hpp"


static std::condition_variable song_id_cv;
static std::condition_variable playing_cv;

void web_server() {
    using httplib::Request, httplib::Response;

    httplib::Server svr;

    if (!svr.is_valid()) {
        printf("server has an error...\n");
        return;
    }

    svr.Get("/", [](Request const& req, Response& res) {
#ifdef DEBUG
        std::string index_html = web::get_source_file("index.html");
        if (index_html.empty()) index_html = "<p>Error: index.html not found</p>";
#else
        char const* index_html = web::get_source_file(web::source_files_e::INDEX_HTML);
        if (index_html == nullptr) index_html = "<p>Error: index.html not found</p>";
#endif
        res.set_content(index_html, web::html_type);
    });

#ifdef DEBUG
    svr.set_mount_point("/", web::web_source_directory);
#else
    svr.Get("/htmx.min.js", [](httplib::Request const& req, httplib::Response& res) {
        char const* htmx_js = web::get_source_file(web::source_files_e::HTMX_MIN_JS);

        if (htmx_js == nullptr) {
            res.status = httplib::StatusCode::InternalServerError_500;
        } else {
            res.set_content(htmx_js, web::js_type);
        }
    });

    svr.Get("/compiled.css", [](httplib::Request const& req, httplib::Response& res) {
        char const* style_css = web::get_source_file(web::source_files_e::STYLE_CSS);
        res.set_content(style_css, web::css_type);
    });
#endif

    svr.Get("/song-info", [&](Request const& /*req*/, Response& res) {
        res.set_chunked_content_provider("text/event-stream", [&](size_t /*offset*/, httplib::DataSink& sink) {
            std::uint8_t last_song_id = 0;

            {
                std::lock_guard<std::mutex> lock(mcu::mut);
                last_song_id = mcu::current_song_id;
            }

            if (last_song_id == 0) {
                constexpr char const* no_song_loaded = "data: <p>No song loaded onto guitar!</p>\n\n";
                if (!sink.write(no_song_loaded, std::strlen(no_song_loaded))) {
                    return false;
                }
            } else {
                SQLite::Database db(data::db_filename);
                data::songs::SongInfo const song_info = data::songs::get_song_by_id(db, last_song_id);
                std::string song_info_html = fmt::format(R"(data: <p id="current-song-title">{}</p><p id="current-song-artist">{}</p>)",
                                                         song_info.title, song_info.artist);
                song_info_html += R"(<div hx-ext="sse" sse-connect="/play-stop-button" sse-swap="message"></div>)";
                song_info_html += "\n\n";

                if (!sink.write(song_info_html.data(), song_info_html.size())) {
                    return false;
                }
            }


            while (sink.is_writable()) {
                std::unique_lock<std::mutex> lock(mcu::mut);

                song_id_cv.wait(lock, [&] { return mcu::current_song_id != last_song_id || !sink.is_writable(); });

                if (!sink.is_writable()) {
                    break;
                }

                last_song_id = mcu::current_song_id;

                lock.unlock();

                SQLite::Database db(data::db_filename);
                data::songs::SongInfo const song_info = data::songs::get_song_by_id(db, last_song_id);
                std::string song_info_html = fmt::format(R"(data: <p id="current-song-title">{}</p><p id="current-song-artist">{}</p>)",
                                                         song_info.title, song_info.artist);
                song_info_html += fmt::format(web::get_source_file(web::source_files_e::BUTTON_HTML), "/play-song", "Play song!");
                song_info_html += "\n\n";

                if (!sink.write(song_info_html.data(), song_info_html.size())) {
                    break;
                }
#ifdef DEBUG
                std::cout << "Song info SSE: " << song_info_html << "\n";
#endif
            }

            return false;
        });
    });

    svr.Get("/play-stop-button", [&](Request const& /*req*/, Response& res) {
        static std::string const play_button_html_data =
                "data: " + fmt::format(web::get_source_file(web::source_files_e::BUTTON_HTML), "/play-song", "Play song!") + "\n\n";

        static std::string const stop_button_html_data =
                "data: " + fmt::format(web::get_source_file(web::source_files_e::BUTTON_HTML), "/stop-song", "Stop song!") + "\n\n";


        res.set_chunked_content_provider("text/event-stream", [&](size_t /*offset*/, httplib::DataSink& sink) {
            bool last_playing;

            {
                std::lock_guard<std::mutex> lock(mcu::mut);
                last_playing = mcu::playing;
            }

            if (last_playing) {
                if (!sink.write(stop_button_html_data.data(), stop_button_html_data.size())) {
                    return false;
                }
            } else {
                if (!sink.write(play_button_html_data.data(), play_button_html_data.size())) {
                    return false;
                }
            }


            while (sink.is_writable()) {
                std::unique_lock<std::mutex> lock(mcu::mut);

                playing_cv.wait(lock, [&] { return mcu::playing != last_playing || !sink.is_writable(); });

                if (!sink.is_writable()) {
                    break;
                }

                last_playing = mcu::playing;

                lock.unlock();

                if (last_playing) {
                    if (!sink.write(stop_button_html_data.data(), stop_button_html_data.size())) {
                        return false;
                    }
                } else {
                    if (!sink.write(play_button_html_data.data(), play_button_html_data.size())) {
                        return false;
                    }
                }
            }

            return false;
        });
    });


    svr.Post("/play-song", [&](Request const& req, Response& res) {
        std::unique_lock<std::mutex> mcu_lock(mcu::mut);
        if (mcu::current_song_id == 0) {
            res.status = httplib::StatusCode::Conflict_409;
            res.set_header("Error", "No song loaded on guitar");
            return;
        }

        mcu::play_loaded_song();

        mcu::playing = true;
        playing_cv.notify_all();
    });

    svr.Post("/stop-song", [&](Request const& req, Response& res) {
        std::unique_lock<std::mutex> mcu_lock(mcu::mut);
        if (!mcu::playing) {
            res.status = httplib::StatusCode::Conflict_409;
            res.set_header("Error", "No content to stop");
            return;
        }

        mcu::end_loaded_song();

        mcu::playing = false;
        playing_cv.notify_all();
    });

    svr.Get("/song-select-form", [](Request const& req, Response& res) {
        std::string options;

        SQLite::Database db(data::db_filename);
        auto songs = data::songs::get_all_songs(db);
        std::sort(songs.begin(), songs.end(), [](data::songs::SongInfo const& a, data::songs::SongInfo const& b) { return a.title < b.title; });
        for (auto const& song: songs) {
            options += fmt::format("<option value={}>{}</option>", song.id, song.title);
#ifdef DEBUG
            for (auto const& note: song.notes) {
                std::cout << "note: ";
                note.print();
            }
#endif
        }

        std::string song_select_form = fmt::format(web::get_source_file(web::source_files_e::SONGSELECTFORM_HTML), options);

        res.set_content(song_select_form, web::html_type);
    });

    svr.Get("/song-select-form-song-info", [](Request const& req, Response& res) {
        using namespace ctre::literals;
        constexpr auto uint8_t_pattern = ctll::fixed_string{R"(^[0-9]{1,2}|1[0-9]{2}|2[0-4][0-9]|25[0-5]$)"};
        auto match = ctre::match<uint8_t_pattern>(req.get_param_value("song-id"));
        if (!match) {
            res.status = httplib::StatusCode::BadRequest_400;
            return;
        }
        std::uint8_t song_id;
        auto view = match.to_view();
        auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), song_id);

        SQLite::Database db(data::db_filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        data::songs::SongInfo song = data::songs::get_song_by_id(db, song_id);

        std::string song_info = fmt::format(web::get_source_file(web::source_files_e::SONGINFO_HTML), song.title, song.artist, song.length, song.bpm);

        res.set_content(song_info, web::html_type);
    });

    svr.Post("/submit-song-select-form", [&](Request const& req, Response& res) {
        using namespace ctre::literals;
        constexpr auto pattern = ctll::fixed_string{R"(^song-id=([0-9]{1,2}|1[0-9]{2}|2[0-4][0-9]|25[0-5])$)"};

        auto match = ctre::match<pattern>(req.body);
        if (!match) {
            res.status = httplib::StatusCode::BadRequest_400;
            return;
        }

        std::uint8_t id;
        auto view = match.get<1>().to_view();
        auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), id);

        if (ec != std::errc()) {
            res.status = httplib::StatusCode::BadRequest_400;
            return;
        }

#ifdef DEBUG
        std::cout << "Song select form submitted with id: " << static_cast<int>(id) << "\n";
#endif

        SQLite::Database db(data::db_filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        data::songs::SongInfo song = data::songs::get_song_by_id(db, id);

        std::unique_lock<std::mutex> mcu_lock(mcu::mut);

        if (mcu::playing) {
            mcu::end_loaded_song();
            mcu::playing = false;
        }

        mcu::send_song(song);

        mcu::current_song_id = id;
        song_id_cv.notify_all();
        playing_cv.notify_all();
    });

    svr.set_exception_handler([](httplib::Request const& req, httplib::Response& res, std::exception_ptr const& ep) {
        std::string error;
        try {
            std::rethrow_exception(ep);
        } catch (mcu::NoACKException& e) {
            res.set_content(fmt::format(web::get_source_file(web::source_files_e::ERROR_HTML), e.what()), web::html_type);
            res.set_header("HX-Retarget", "#error-popup");
            res.set_header("HX-Reswap", "innerHTML");
            res.status = httplib::StatusCode::GatewayTimeout_504;
            return;
        } catch (std::exception& e) {
            error = e.what();
        } catch (...) {
            error = "Unknown exception";
        }
        res.set_header("Error", error);
        res.status = httplib::StatusCode::InternalServerError_500;
    });
#ifdef DEBUG
    svr.listen("0.0.0.0", 8080);
#else
    svr.listen("0.0.0.0", 80);
#endif
}


auto main(int argc, char* args[]) -> int {
// Create a pipe
/*
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "Pipe creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Fork the first child process for "arecord"
    pid_t pid1 = fork();
    if (pid1 == -1) {
        std::cerr << "Fork failed for arecord" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // In the first child process (arecord)
        close(pipefd[0]); // Close reading end of the pipe

        // Redirect stdout to the pipe's writing end
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]); // Close the original writing end after dup2

        // Execute arecord command
        execlp("arecord", "arecord", "-D", "plughw:3,0", "--rate=88200", "--format=S16_LE", "--buffer-time=1", "-", (char *)NULL);
        // If execlp fails
        std::cerr << "Failed to execute arecord" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Fork the second child process for "aplay"
    pid_t pid2 = fork();
    if (pid2 == -1) {
        std::cerr << "Fork failed for aplay" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // In the second child process (aplay)
        close(pipefd[1]); // Close writing end of the pipe

        // Redirect stdin to the pipe's reading end
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]); // Close the original reading end after dup2

        // Execute aplay command
        execlp("aplay", "aplay", "-D", "plughw:2,0" "--buffer-time=1", "-", (char *)NULL);
        // If execlp fails
        std::cerr << "Failed to execute aplay" << std::endl;
        exit(EXIT_FAILURE);
    }*/

    
    if (!data::init()) {
        return 1;
    }

    
    // playerMode(uint8_t song_id, uint8_t mode, std::string const& note, std::string const& title, std::string const& artist, uint8_t duration,
    //uint8_t bpm);

    //The constructor creates a RECORDING, and populates the internal structure with information from this recording. 
    //It also grabs the reference song from the database and populates our class with that.
    auto* player = new playerMode(1, 1, "C4", "1", "1", 55, 95);

    //dataParseUpload(std::string const& filename, uint8_t song_id, std::string const& title, std::string const& artist, uint8_t duration, uint8_t bpm) -> void;
   
    //upload reference midi to the database. 
    player->dataParseUpload("twinkle.mid", 1, "twinkle-twinkle-ref", "unknown", 55, 95);

    //lets print the original
    auto numbers = player->dataParseRef("twinkle.mid");
    std::cout << "REFERENCE:" << std::endl;
    for (int i = 0; i < numbers.size(); i++) {
        std::cout << "[" << numbers[i][0] << "," << numbers[i][1] << "," << numbers[i][2] << "]" << std::endl;
    }

    //lets print the recording
    std::cout << "RECORDING:" << std::endl;
    for (int i = 0; i < player->recording_numbers.size(); i++) {
        std::cout << "[" << player->recording_numbers [i][0] << "," << player->recording_numbers[i][1] << "," << player->recording_numbers[i][2] << "]" << std::endl;
    }

    //player->analysis();
    auto results = player->analysis();

    for(auto result : results){
        std::cout << result << std::endl;
    }
    /*
    bool outcome = player->analysis("C4");

    if (outcome == false) {
        std::cout << "Wrong!" << std::endl;
    }
    else {
        std::cout << "Correct!" << std::endl;
    }
    */

    if (!serial::init()) {
        return 1;
    }

    try {
        SQLite::Database db(data::db_filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        if (!data::songs::song_id_exists(db, 1)) {
#ifdef DEBUG
            std::cout << "Database does not have song ID 1! Adding basic song...\n";
#endif
            data::songs::insert_new_song(db, data::songs::ode_to_joy);
        }

        // #ifdef DEBUG
        //         data::songs::insert_new_song(db, data::songs::baby_shark);
        // #endif

    } catch (std::exception& e) {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
    }

#ifdef DEBUG
    std::cout << "Getting ID of song loaded on MCU...\n";
#endif
    while (true) {
        try {
            mcu::current_song_id = mcu::get_loaded_song_id();
            break;
        } catch (mcu::NoACKException const& e) {
#ifdef DEBUG
            std::cerr << e.what() << "\n";
            break;
#endif
            std::cerr << "Repolling...\n";
        }
    }

    if (mcu::current_song_id == 0) {
        try {
            mcu::send_song(data::songs::ode_to_joy);
        } catch (mcu::NoACKException const& e) {
#ifdef DEBUG
            std::cerr << e.what() << "\n";
#endif
        }
    }

#ifdef DEBUG
    std::cout << "Starting web server thread...\n";
#endif
    std::thread t(web_server);


    t.join();
}
