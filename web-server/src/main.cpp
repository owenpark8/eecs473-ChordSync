#include <chrono>
#include <cstddef>
#include <filesystem>
#include <format>
#include <iostream>
#include <thread>

#include <httplib.h>

#include "data.hpp"
#include "web.hpp"

namespace fs = std::filesystem;

void web_server() {
    httplib::Server svr;

    if (!svr.is_valid()) {
        printf("server has an error...\n");
        return;
    }

    svr.Get("/", [](httplib::Request const& req, httplib::Response& res) {
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

    svr.Get("/song-select-form", [](httplib::Request const& req, httplib::Response& res) {
        std::string options;
        for (auto const& [id, song_info]: data::songs::m_song_id_to_info) {
            options += std::format("<option value=\"{}\">{}</option>", id, song_info.title);
        }
        std::string song_select_form = std::format(web::get_source_file(web::source_files_e::SONGSELECTFORM_HTML), options);

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
#ifdef DEBUG
    std::cout << "Initializing app data...\n";
    std::cout << "Checking if song directory \"" << data::songs::song_directory << "\" exists...\n";
#endif
    if (!fs::exists(data::songs::song_directory)) {
#ifdef DEBUG
        std::cout << "Song directory does not exist! Creating directory...\n";
#endif
        if (!data::create_directory_if_not_exists(data::songs::song_directory)) {
#ifdef DEBUG
            std::cerr << "Critical error: Data directory could not be created. Exiting.\n";
#endif
            return 1;
        }
    }
#ifdef DEBUG
    else {
        std::cout << "Song directory found!\n";
    }
#endif


    data::songs::SongInfo new_song = {
            .title = "Wow nice song",
            .artist = "Owen Bark",
            .length = std::chrono::milliseconds(1000),
    };

    data::songs::write_song_info_to_file("123", new_song);


#ifdef DEBUG
    std::cout << "Starting web server thread...\n";
#endif
    std::thread t(web_server);

    t.join();
}
