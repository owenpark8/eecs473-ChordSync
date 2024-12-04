#include "data.hpp"
#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace process_gp4 {
    // filename is filename of gp file
    auto gen_song_info(std::string const& filename, std::string const& title, std::string const& artist) -> data::songs::SongInfo {
        data::songs::SongInfo song_out = {.title = title, .artist = artist};
        // Pybind
        py::scoped_interpreter guard{};
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, PY_VENV_PATH);
        sys.attr("path").attr("insert")(0, PY_MODULE_PATH);

        auto get_dataParse_module = py::module_::import("parse_gp_files");
        py::object parse_gp_file = get_dataParse_module.attr("parse_gp_file");
        auto song_ints = parse_gp_file(filename).cast<std::vector<std::vector<int>>>();
        py::object get_song_tempo = get_dataParse_module.attr("get_song_tempo");
        song_out.bpm = get_song_tempo(filename).cast<std::uint16_t>();
        // add last elements start timestamp and duration
        int ms_duration_int = !song_ints.empty() ? song_ints.back()[1] + song_ints.back()[2] : 0; 
        
        std::chrono::milliseconds ms_duration(ms_duration_int);
        song_out.length = std::chrono::seconds((ms_duration.count() + 999) / 1000); // round up


        for (auto& note_int : song_ints) {
            data::songs::Note temp_note;
            temp_note.start_timestamp_ms = static_cast<std::uint32_t>(note_int[0]);
            temp_note.length_ms = static_cast<std::uint16_t>(note_int[1]);
            temp_note.midi_note = static_cast<std::uint8_t>(note_int[2]);
            temp_note.fret = static_cast<std::uint8_t>(note_int[3]) & 0xF;
            temp_note.string = static_cast<std::uint8_t>(note_int[4]) & 0xF;

            
            (song_out.notes).push_back(temp_note);
        }

        return song_out;
    }
} // namespace process_gp4