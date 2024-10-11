#ifdef DEBUG
#include <fstream>
#endif

#include <httplib.h>

#include "web.hpp"

namespace web {
    constexpr char png_type[] = "image/png";
    constexpr char jpeg_type[] = "image/jpeg";
    constexpr char html_type[] = "text/html";
    constexpr char css_type[] = "text/css";
    constexpr char js_type[] = "application/javascript";

#ifdef DEBUG
    std::string const web_source_directory = WEB_SRC_DIR;

    auto read_file_into_string(std::string const& filename) -> std::string {
        std::string const filepath = web_source_directory + "/" + filename;
        std::ifstream ifs(filepath, std::ios_base::in | std::ios_base::binary);
        if (!ifs) return std::string{};
        return {(std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())};
    }

    auto get_source_file(std::string const& filename) -> std::string { return read_file_into_string(filename); }
#endif

    std::unordered_set<std::string> const allowed_image_upload_types = {jpeg_type, png_type};

    auto get_source_file(source_files_e file) -> std::string {
#define MAKE_SOURCE_FILES_CASES(enum_name, content, ...)                                                                                             \
    case source_files_e::enum_name:                                                                                                                  \
        return std::string(content);

        switch (file) {
            SOURCE_FILES_ITER(MAKE_SOURCE_FILES_CASES)

            default:
                return std::string{};
        }
#undef MAKE_SOURCE_FILES_CASES
    }


    void server() {
        httplib::Server svr;

        if (!svr.is_valid()) {
            printf("server has an error...\n");
            return;
        }

        svr.Get("/", [](httplib::Request const& req, httplib::Response& res) {
#ifdef DEBUG
            std::string index_html = web::get_source_file("index.html");
#elif
        std::string index_html = web::get_source_file(web::source_files_e::INDEX_HTML);
#endif
            if (index_html.empty()) index_html = "<p>Error: index.html not found</p>";
            res.set_content(index_html, html_type);
        });

#ifdef DEBUG
        svr.set_mount_point("/", web::web_source_directory);
#elif

        svr.Get("/htmx.min.js", [](httplib::Request const& req, httplib::Response& res) {
            std::string const htmx_js = web::get_source_file(web::source_files_e::HTMX_MIN_JS);
            res.set_content(htmx_js, js_type);
        });

        svr.Get("/compiled.css", [](httplib::Request const& req, httplib::Response& res) {
            std::string const style_css = web::get_source_file(web::source_files_e::STYLE_CSS);
            res.set_content(style_css, css_type);
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

        svr.listen("localhost", 8080);
    }


} // namespace web
