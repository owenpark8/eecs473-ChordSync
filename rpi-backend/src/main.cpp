#include <iostream>
#include <string>
#include <thread>

#include <SQLiteCpp/SQLiteCpp.h>
#include <ctre.hpp>
#include <fmt/format.h>
#include <httplib.h>

#include "data.hpp"
#include "mcu.hpp"
#include "web.hpp"

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

    // svr.Get("/song-info", [&](Request const& /*req*/, Response& res) {
    //     res.set_chunked_content_provider("text/event-stream", [&](size_t /*offset*/, httplib::DataSink& sink) {
    //         std::unique_lock<std::mutex> lock(mcu::song_mut);
    //
    //         while (true) {
    //             uint8_t current_song_id = mcu::get_current_song_id();
    //
    //             // Send the current song_id to the client
    //             std::string message = "data: " + std::to_string(current_song_id) + "\n\n";
    //             if (!sink.is_writable() || !sink.write(message.data(), message.size())) {
    //                 // If write fails or client disconnects, exit the loop
    //                 break;
    //             }
    //
    //             // Wait until song_id changes
    //             song_cv.wait(lock, [&] { return song_id.load() != current_song_id; });
    //         }
    //
    //         // Return false to indicate the connection should be closed
    //         return false;
    //     });
    // });

    svr.Post("/play-song", [&](Request const& req, Response& res) {
        std::unique_lock<std::mutex> lock(mcu::mut);
        mcu::play_loaded_song();
    });

    svr.Get("/song-select-form", [](Request const& req, Response& res) {
        std::string options;

        SQLite::Database db(data::db_filename);
        for (auto const& song_info: data::songs::get_all_songs(db)) {
            options += fmt::format("<option value={}>{}</option>", song_info.id, song_info.title);
#ifdef DEBUG
            for (auto const& note: song_info.notes) {
                std::cout << "note: ";
                note.print();
            }
#endif
        }

        std::string song_select_form = fmt::format(web::get_source_file(web::source_files_e::SONGSELECTFORM_HTML), options);

        res.set_content(song_select_form, web::html_type);
    });

    svr.Post("/submit-song-select", [&](Request const& req, Response& res) {
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

        SQLite::Database db(data::db_filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        data::songs::SongInfo song = data::songs::get_song_by_id(db, id);

        mcu::start_song_loading(id);

        for (auto const& note: song.notes) {
            mcu::send_note(note.start_timestamp_ms, note.length_ms, note.fret, note.string);
        }
    });

    svr.set_exception_handler([](httplib::Request const& req, httplib::Response& res, std::exception_ptr const& ep) {
        std::string error;
        try {
            std::rethrow_exception(ep);
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
    if (!data::init()) {
        return 1;
    }

    //  if (!serial::init()) {
    //      return 1;
    //  }

#ifdef DEBUG
    data::songs::SongInfo song = {
            .title = "baby shark",
            .artist = "doodoodoo",
            .length = std::chrono::seconds(137),
            .bpm = 115,
            .notes =
                    {
                            data::songs::Note{.start_timestamp_ms = 0, .length_ms = 200, .midi_note = 0, .fret = 0, .string = 0},
                            data::songs::Note{.start_timestamp_ms = 200, .length_ms = 200, .midi_note = 1, .fret = 2, .string = 1},
                            data::songs::Note{.start_timestamp_ms = 400, .length_ms = 200, .midi_note = 2, .fret = 4, .string = 2},
                            data::songs::Note{.start_timestamp_ms = 600, .length_ms = 200, .midi_note = 3, .fret = 6, .string = 3},
                    },
    };
    std::cout << "Inserting baby shark with notes: \n";
    for (auto const& note: song.notes) {
        note.print();
    }
    try {
        SQLite::Database db(data::db_filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        data::songs::insert_new_song(db, song);
    } catch (std::exception& e) {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
    }
#endif

#ifdef DEBUG
    std::cout << "Getting ID of song loaded on MCU...\n";
#endif
    mcu::get_and_update_loaded_song_id();


#ifdef DEBUG
    std::cout << "Starting web server thread...\n";
#endif
    std::thread t(web_server);


    t.join();
}
