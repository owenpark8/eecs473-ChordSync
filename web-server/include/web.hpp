#pragma once

#include <string>
#include <unordered_set>

#include "web_sources.hpp"

namespace web {
    constexpr char png_type[] = "image/png";
    constexpr char jpeg_type[] = "image/jpeg";
    constexpr char html_type[] = "text/html";
    constexpr char css_type[] = "text/css";
    constexpr char js_type[] = "application/javascript";

    enum class source_files_e {
#define MAKE_SOURCE_FILES_ENUM(enum_name, ...) enum_name,
        SOURCE_FILES_ITER(MAKE_SOURCE_FILES_ENUM)
#undef MAKE_SOURCE_FILES_ENUM
    };

    extern std::unordered_set<std::string> const allowed_image_upload_types;

#ifdef DEBUG
    extern std::string const web_source_directory;
    auto read_file_into_string(std::string const& filename) -> std::string;
    auto get_source_file(std::string const& filename) -> std::string;
#endif

    auto get_source_file(source_files_e file) -> std::string;

} // namespace web
