#include <cstddef>
#include <thread>

#include <httplib.h>

#include "web.hpp"

void web_server() {
    httplib::Server svr;

    if (!svr.is_valid()) {
        printf("server has an error...\n");
        return;
    }

    svr.Get("/", [](httplib::Request const& req, httplib::Response& res) {
#ifdef DEBUG
        std::string index_html = web::get_source_file("index.html");
#else
        std::string index_html = web::get_source_file(web::source_files_e::INDEX_HTML);
#endif
        if (index_html.empty()) index_html = "<p>Error: index.html not found</p>";
        res.set_content(index_html, web::html_type);
    });

#ifdef DEBUG
    svr.set_mount_point("/", web::web_source_directory);
#else
    svr.Get("/htmx.min.js", [](httplib::Request const& req, httplib::Response& res) {
        std::string const htmx_js = web::get_source_file(web::source_files_e::HTMX_MIN_JS);
        res.set_content(htmx_js, web::js_type);
    });

    svr.Get("/compiled.css", [](httplib::Request const& req, httplib::Response& res) {
        std::string const style_css = web::get_source_file(web::source_files_e::STYLE_CSS);
        res.set_content(style_css, web::css_type);
    });
#endif

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
    svr.listen("localhost", 8080);
#else
    svr.listen("0.0.0.0", 80);
#endif
}


auto main(int argc, char* args[]) -> int {
    std::thread t(web_server);

    t.join();
}
