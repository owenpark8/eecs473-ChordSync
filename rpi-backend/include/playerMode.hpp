#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>

#include <sys/wait.h>

#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include <data.hpp>

namespace py = pybind11;

class ChordAnalyzer {
public:
    ChordAnalyzer() = default;
    ChordAnalyzer(data::songs::SongInfo reference_chord) : m_reference_chord(std::move(reference_chord)) {};

    ~ChordAnalyzer() = default;

    auto record_chord() -> void {
        py::scoped_interpreter guard{};
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, PY_VENV_PATH);
        sys.attr("path").attr("insert")(0, PY_MODULE_PATH);
        try {
            py::module pybind_module = py::module::import("basic_pitch");
            std::cout << "Module imported successfully!" << std::endl;
        } catch (py::error_already_set const& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }


        std::string command = "arecord --duration=" + std::to_string(m_reference_chord.length.count()) + " --rate=88200 --format=S16_LE " +
                              std::to_string(m_reference_chord.id) + "_rec.wav";
        int result = system(command.c_str());

        if (result != 0) {
            std::cerr << "Error executing command: " << command << std::endl;
            throw std::runtime_error("Could not record song!");
        } else {
            std::cout << "Recording completed successfully." << std::endl;
        }


        auto get_record_convert_module = py::module_::import("record_convert");
        py::object get_prediction = get_record_convert_module.attr("prediction");
        get_prediction(m_reference_chord.id, 120);

        py::object get_record_convert = get_record_convert_module.attr("record_convert_offset");
        auto numbers = get_record_convert(m_reference_chord.id).cast<std::vector<std::vector<int>>>();

        py::object get_remove_files = get_record_convert_module.attr("remove_files");
        get_remove_files(m_reference_chord.id);

        for (auto& number: numbers) {
            data::songs::Note entry{};
            entry.midi_note = static_cast<uint8_t>(number[0]);
            entry.start_timestamp_ms = static_cast<uint32_t>(number[1]);
            entry.length_ms = static_cast<uint16_t>(number[2] - entry.start_timestamp_ms);
            m_recorded_chord.push_back(entry);
        }
    }

    auto get_result() -> bool {
        int count = 0;
        for (auto ref_note: m_reference_chord.notes) {
            for (auto note: m_recorded_chord) {
                if (note.midi_note == ref_note.midi_note) {
                    count++;
                    break;
                }
            }
        }

        if (count == m_reference_chord.notes.size() && count != 0) {
            return true;
        }

        return false;
    }

private:
    data::songs::SongInfo m_reference_chord;
    std::vector<data::songs::Note> m_recorded_chord;
};
