#include "playerMode.hpp"
#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include <iostream>

namespace py = pybind11;

//add in cleanout function
playerMode::playerMode(uint8_t song_id, uint8_t mode, std::string const& note, std::string const& title, std::string const artist, uint8_t duration,
                       uint8_t bpm) {
    this->song.id = song_id;
    this->song.title = title;
    this->song.artist = artist;
    this->song.length = std::chrono::seconds(duration);
    this->song.bpm = bpm;


    this->mode = mode;


    this->note = note;


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

    auto get_record_convert_module = py::module_::import("record_convert");
    py::object get_record_convert = get_record_convert_module.attr("record_convert");

    std::vector<std::vector<int>> numbers = get_record_convert("-r", song_id, duration, bpm).cast<std::vector<std::vector<int>>>();

    for (std::size_t i = 0; i < numbers.size(); i++) {
        Note* entry = new Note;
        entry->midi_note = static_cast<uint8_t>(numbers[i][0]);
        entry->start_timestamp_ms = static_cast<uint8_t>(numbers[i][1]);
        entry->length_ms = static_cast<uint8_t>(numbers[i][2] - entry->start_timestamp_ms);
        (this->song.notes).push_back(*entry);
        delete entry;
    }
}


std::string playerMode::get_mode() const {
    if (this->mode == 1) return "single note";
    return "pls wait";
}
uint8_t playerMode::get_bpm() const { return this->song.bpm; }

uint8_t playerMode::get_resolution() const { return 0; }


std::vector<bool> playerMode::analysis(std::vector<Note>& ref) {
    std::vector<bool> s;
    s.push_back(false);
    return s;
}

bool playerMode::analysis(std::string const& note) {
    uint8_t refNote = this->noteToInt(note);
    std::unordered_map<uint8_t, uint8_t> freq;

    uint8_t maxNote = 0;

    uint8_t maxCount = 0;

    for (int i = 0; i < this->song.notes.size(); i++) {
        freq[(this->song.notes[i]).midi_note]++;
    }
    //iterate through and check max freqeucy, then output if it is correct or note.
    for (auto it = freq.begin(); it != freq.end(); it++) {
        if (it->second > maxCount) {
            maxCount = it->second;
            maxNote = it->first;
        }
    }
    return maxNote == refNote ? true : false;
}

playerMode::~playerMode() {}


uint8_t playerMode::noteToInt(std::string const& note) {
    // Map of note names to semitone offsets
    // Extract the note name and octave from the input string
    std::string name = note.substr(0, note.size() - 1); // Extract the note name
    uint8_t octave = note.back() - '0';                 // Convert the last character to an integer

    // Calculate the MIDI note number
    return 12 + (12 * octave) + semitoneOffsets[name];
}


/*class playerMode {
public:
    //do the recording here
    playerMode(std::uint8_t const& song_id, std::uint8_t const& mode, std::uint8_t const& duration, std::uint8_t& bpm);
    // EFFECTS returns player's name
    std::string& get_mode() const;

    std::uint8_t& get_bpm() const;

    std::uint8_t& get_resolution() const;

    //this is analysis for song mode.
    //call the user
    std::vector<std::string> analysis(std::vector<NoteDataMessage>& ref) const;

    //this is analysis for note mode.
    std::vector<std::string> analysis(std::string& note) const;

    // Needed to avoid some compiler errors
    ~playerMode() {}

private:
    std::uint8_t song_id;
    std::uint8_t bpm;
    std::uint8_t duration;
    std::uint8_t mode;
    std::vector<NoteDataMessage> recording;

    //used to convert Note
    uint8_t noteToInt(std::string& note);
};
#endif*/
