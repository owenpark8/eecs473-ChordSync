#include <cstddef>
#include <iostream>
#include <string>
#include <thread>

#include <SQLiteCpp/SQLiteCpp.h>
#include <fmt/format.h>
#include <httplib.h>

#include "data.hpp"
#include "mcu.hpp"
#include "serial.hpp"
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

    svr.Post("/play-song", [&](auto const& req, auto& res) {
        std::unique_lock<std::mutex> lock(mcu::mut);
        bool status = mcu::play_loaded_song();
        if (!status) {
            res.status = httplib::StatusCode::InternalServerError_500;
        }
    });

    svr.Get("/song-select-form", [](httplib::Request const& req, httplib::Response& res) {
        std::string options;

        SQLite::Database db(data::db_filename);
        for (auto const& song_info: data::songs::get_all_songs(db)) {
            options += fmt::format("<option value=\"{}\">{}</option>", song_info.id, song_info.title);
        }

        std::string song_select_form = fmt::format(web::get_source_file(web::source_files_e::SONGSELECTFORM_HTML), options);

        res.set_content(song_select_form, web::html_type);
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

    //     data::songs::SongInfo song = {
    //             .title = "baby shark",
    //             .artist = "doodoodoo",
    //             .length = std::chrono::milliseconds(88),
    //     };
    //     try {
    //         SQLite::Database db(data::db_filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    //         data::songs::insert_new_song(db, song);
    //     } catch (std::exception& e) {
    // #ifdef DEBUG
    //         std::cerr << "SQLite exception: " << e.what() << std::endl;
    // #endif
    //     }


#ifdef DEBUG
    std::cout << "Starting web server thread...\n";
#endif
    std::thread t(web_server);


    t.join();
}
