#include "playerMode.hpp"
#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include <iostream>


//add in cleanout function
playerMode::playerMode(std::uint8_t const& song_id, std::uint8_t const& mode, std::uint8_t const& duration, std::uint8_t& bpm) {
    this->song_id = song_id;
    this->bpm = bpm;
    this->mode = mode;
    this->duration = duration;

    py::scoped_interpreter guard{};

    py::module sys = py::module::import("sys");
    sys.attr("path").attr("insert")(0, PY_VENV_PATH);
    sys.attr("path").attr("insert")(0, PY_MODULE_PATH);

    try {
        py::module pybind_module = py::module::import("basic_pitch");
        std::cout << "Module imported successfully!" << std::endl;
    } catch (py::error_already_set const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    auto get_record_convert_module = py::module_::import("record_convert");
    py::object get_record_convert = get_record_convert_module.attr("record_convert");

    std::vector<std::vector<int>> numbers = get_record_convert("-r", song_id, duration, bpm).cast<std::vector<std::vector<uint8_t>>>();

    for (std::size_t i = 0; i < numbers.size(); i++) {
        NoteDataMessage* entry = new NoteDataMessage;
        entry->note = numbers[i][0];
        entry->timestamp_ms = numbers[i][1];
        entry->length_ms = numbers[i][2] - entry->timestamp_ms;
        this->recording.push_back(entry);
    }

    std::cout << std::endl;
    return 0;
}


std::string& playerMode::get_mode() const { return this->mode; }
std::uint8_t& playerMode::get_bpm() const { return this->bpm; }

std::uint8_t& playerMode::get_resolution() const {return 0}


std::vector<std::string> playerMode::analysis(std::vector<NoteDataMessage>& ref) const {
    std::vector<string> s = new std::vector<string>;
    s.push_back("hi");
    return s;
}

std::vector<std::string> playerMode::analysis(std::string& note) const {
    uint8_t refNote = this->noteToInt(note);
    std::unordered_map<uint8_t, uint8_t> freq; //make this into data variable.

    uint8_t maxNote = 0;
    //iterate through and check max freqeucy, then output if it is correct or note.
    for (int i = 0; i < freq.size(); i++) {}
}

~playerMode() {}


uint8_t playerMode::noteToInt(std::string& note) {
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
