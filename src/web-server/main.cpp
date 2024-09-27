#include <cstddef>
#include <string>
#include <string_view>
#include <thread>

#include <httplib.h>

#include "web.hpp"

constexpr char png_type[] = "image/png";
constexpr char jpeg_type[] = "image/jpeg";
constexpr char html_type[] = "text/html";
constexpr char css_type[] = "text/css";
constexpr char js_type[] = "application/javascript";

auto get_file_extension(std::string_view filename) -> std::string_view {
  std::size_t dot_position = filename.find_last_of('.');
  if (dot_position == std::string_view::npos) {
    return {}; // No extension found
  }
  return filename.substr(dot_position);
}

void web_server() {
  httplib::Server svr;

  if (!svr.is_valid()) {
    printf("server has an error...\n");
    return;
  }

  svr.Get("/", [](httplib::Request const &req, httplib::Response &res) {
#ifdef DEBUG
    std::string index_html = web::get_source_file("index.html");
#elif
        std::string index_html = web::get_source_file(web::source_files_e::INDEX_HTML);
#endif
    if (index_html.empty())
      index_html = "<p>Error: index.html not found</p>";
    res.set_content(index_html, html_type);
  });

#ifdef DEBUG
  svr.set_mount_point("/", web::web_source_directory);
#elif

  svr.Get("/htmx.min.js",
          [](httplib::Request const &req, httplib::Response &res) {
            std::string const htmx_js =
                web::get_source_file(web::source_files_e::HTMX_MIN_JS);
            res.set_content(htmx_js, js_type);
          });

  svr.Get("/compiled.css",
          [](httplib::Request const &req, httplib::Response &res) {
            std::string const style_css =
                web::get_source_file(web::source_files_e::STYLE_CSS);
            res.set_content(style_css, css_type);
          });
#endif

  svr.set_exception_handler([](httplib::Request const &req,
                               httplib::Response &res, std::exception_ptr ep) {
    std::string error;
    try {
      std::rethrow_exception(std::move(ep));
    } catch (std::exception &e) {
      error = e.what();
    } catch (...) {
      error = "Unknown exception";
    }
    res.set_header("Error", error);
    res.status = httplib::StatusCode::InternalServerError_500;
  });

  svr.listen("localhost", 8080);
}

auto main(int argc, char *args[]) -> int {
  std::thread t(web_server);

  t.join();
}
