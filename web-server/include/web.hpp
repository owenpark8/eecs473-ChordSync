#pragma once

#include <string>
#include <unordered_set>

#include "web_sources.hpp"

#define DEBUG

namespace web {
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
    auto server() -> void;


} // namespace web
