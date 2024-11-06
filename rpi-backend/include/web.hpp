#pragma once

#include <condition_variable>
#include <mutex>
#include <string>
#include <unordered_set>

#include <httplib.h>

#include "web_sources.hpp"

namespace web {
    constexpr char const png_type[] = "image/png";
    constexpr char const jpeg_type[] = "image/jpeg";
    constexpr char const html_type[] = "text/html";
    constexpr char const css_type[] = "text/css";
    constexpr char const js_type[] = "application/javascript";

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

    constexpr auto get_source_file(source_files_e file) -> char const* {
#define MAKE_SOURCE_FILES_CASES(enum_name, content, ...)                                                                                             \
    case source_files_e::enum_name:                                                                                                                  \
        return content;

        switch (file) {
            SOURCE_FILES_ITER(MAKE_SOURCE_FILES_CASES)

            default:
                return nullptr;
        }
#undef MAKE_SOURCE_FILES_CASES
    }
} // namespace web