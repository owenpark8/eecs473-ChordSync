#include "data.hpp"
#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace process_gp4 {

    // filename is filename of gp file
    auto dataParseRef(std::string const& filename) -> std::vector<std::vector<int>> {
        py::scoped_interpreter guard{};
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, PY_VENV_PATH);
        sys.attr("path").attr("insert")(0, PY_MODULE_PATH);
        //mode 1 and 2 are record song and single
        auto get_dataParse_module = py::module_::import("parse_gp_files");
        py::object parse_gp_file = get_dataParse_module.attr("parse_gp_file");
        auto numbers = parse_gp_file(filename).cast<std::vector<std::vector<int>>>();
        return numbers;
    }

    auto dataParseUpload(std::string const& filename, uint8_t song_id, std::string const& title, std::string const& artist, uint8_t duration,
                         uint8_t bpm) -> void {
        auto numbers = dataParseRef(filename);
        data::songs::SongInfo song = {.id = song_id, .title = title, .artist = artist, .length = std::chrono::seconds(duration), .bpm = bpm};
        for (auto& number: numbers) {
            auto temp_note = reinterpret_cast<data::songs::Note*>(number.data());
            (song.notes).push_back(*temp_note);
        }
        // std::cout << "Inserting " << title << " with notes (but no information about string and fret number)" << "\n";
        try {
            SQLite::Database db(data::db_filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            data::songs::insert_new_song(db, song);
        } catch (std::exception& e) {
            std::cerr << "SQLite exception: " << e.what() << std::endl;
        }
    }
} // namespace process_gp4