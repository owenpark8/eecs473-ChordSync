#include "web.hpp"

#ifdef DEBUG
#include <fstream>
#endif

#include <httplib.h>

namespace web {
    std::unordered_set<std::string> const allowed_image_upload_types = {jpeg_type, png_type};

#ifdef DEBUG
    std::string const web_source_directory = WEB_SRC_DIR;

    auto read_file_into_string(std::string const& filename) -> std::string {
        std::string const filepath = web_source_directory + filename;
        std::ifstream ifs(filepath, std::ios_base::in | std::ios_base::binary);
        if (!ifs) return std::string{};
        return {(std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())};
    }

    auto get_source_file(std::string const& filename) -> std::string { return read_file_into_string(filename); }
#endif

} // namespace web
