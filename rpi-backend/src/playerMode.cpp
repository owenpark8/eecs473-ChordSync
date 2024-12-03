#include "playerMode.hpp"
#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include <iostream>

namespace py = pybind11;


auto playerMode::recordtoMIDI(uint8_t song_id, uint8_t duration,
               uint8_t bpm) -> std::vector<std::vector<int>>{
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


    auto numbers = get_record_convert(song_id, duration, bpm).cast<std::vector<std::vector<int>>>();

    return numbers;
    }
    
//add in cleanout function
playerMode::playerMode(uint8_t song_id, uint8_t duration,
                       uint8_t bpm)
    : m_rec_song{song_id, "rec", "rec", std::chrono::seconds(duration), bpm} {

    //begin recording from Python.
    auto numbers = recordtoMIDI(song_id, duration, bpm);
    this->recording_numbers = numbers;
    //Process each note in the RECORDING and add it our song vector
    for (auto& number: numbers) {
        auto entry = new data::songs::Note;
        entry->midi_note = static_cast<uint8_t>(number[0]);
        entry->start_timestamp_ms = static_cast<uint32_t>(number[1]);
        entry->length_ms = static_cast<uint16_t>(number[2] - entry->start_timestamp_ms);
        (this->m_rec_song.notes).push_back(*entry);
        delete entry;
    }
    //need to figure out filtering. 
}

auto playerMode::get_bpm() const -> uint8_t { return this->m_rec_song.bpm; }


auto playerMode::compareByStartTime(const noteEntry& entry, std::uint32_t target)->bool {
    return entry.start_time < target;
}

// Comparator for reverse comparison, useful for certain cases
auto playerMode::compareByStartTimeReverse(std::uint32_t target, const noteEntry& entry)->bool{
    return target < entry.start_time;
}

auto playerMode::checkSong() -> void {
    for (auto note: this->m_rec_song.notes){
        checkNote(note);
    }
}

auto playerMode::checkNote(data::songs::Note& rec_note) -> void{
    //this is an extraneous

    auto occurences = this->ref_data.find(rec_note.midi_note);
    //so it exists. 

    if(occurences != ref_data.end()){
        //std::cout << "Found Note " << (uint16_t)rec_note.midi_note << " within reference song. " << std::endl;
        //these are occurences.
        auto lower = std::lower_bound(occurences->second.begin(), occurences->second.end(), rec_note.start_timestamp_ms, compareByStartTime);
        auto upper = std::upper_bound(occurences->second.begin(), occurences->second.end(), rec_note.start_timestamp_ms, compareByStartTimeReverse);

        //std::cout << "Found lower at " << lower->start_time << " and higher at " << upper->start_time << " comparing " << rec_note.start_timestamp_ms << std::endl;
        auto closest = abs((int32_t)lower->start_time - (int32_t)rec_note.start_timestamp_ms) < abs((int32_t)upper->start_time - (int32_t)rec_note.start_timestamp_ms) ? lower : upper;

        if(abs((int32_t)closest->start_time - (int32_t)rec_note.start_timestamp_ms) > 1000){
            closest->seen = false;
            return;
        }
        //std::cout << "Now checking Note " << (uint16_t)rec_note.midi_note << " for duration requirement " << std::endl;

        float error = float(abs((int32_t)closest->duration - (int32_t)rec_note.length_ms)) / (float)(closest->duration);

        if(error <= 0.5){
            closest->seen = true;
            return;
        }

        closest->seen = false;    
    }
}

auto playerMode::analysis() -> std::vector<bool>{
    checkSong();
    std::vector<bool> result(this->ref_size, false);
    
    for (const auto& [key, noteVector] : this->ref_data) {
        // Iterate through the vector of NoteEntry
        for (const auto& note : noteVector) {
            result[note.orig_pos] = note.seen;
        }
    }

    //returns result.
    return result;
}

auto playerMode::analysis(std::string const& note) -> bool {
    uint8_t refNote = this->noteToInt(note);
    std::unordered_map<uint8_t, uint8_t> freq;

    uint8_t maxNote = 0;

    uint8_t maxCount = 0;

    for (auto& note: this->m_rec_song.notes) {
        freq[note.midi_note]++;
    }
    //iterate through and check max freqeucy, then output if it is correct or note.
    for (auto& it: freq) {
        if (it.second > maxCount) {
            maxCount = it.second;
            maxNote = it.first;
        }
    }
    return maxNote == refNote ? true : false;
}

playerMode::~playerMode() = default;


auto playerMode::noteToInt(std::string const& note) -> uint8_t {
    // Map of note names to semitone offsets
    // Extract the note name and octave from the input string
    std::string name = note.substr(0, note.size() - 1); // Extract the note name
    uint8_t octave = note.back() - '0';                 // Convert the last character to an integer

    // Calculate the MIDI note number
    return 12 + (12 * octave) + semitoneOffsets[name];
}

/*auto playerMode::analysis() -> void{
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

    auto numbers = get_record_convert("c", this->song.id, this->song.length, this->song.bpm).cast<std::vector<std::vector<int>>>();

    
}*/
















#ifndef PLAYERMODE_H
#define PLAYERMODE_H


#include <data.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <cmath>

class playerMode {
public:
    //do the recording here
    //potential modes: reference create, record and convert song, record and convert note.
    //can pass in note to convert.
    playerMode(uint8_t song_id, uint8_t duration,
               uint8_t bpm);
    // EFFECTS returns player's name
    [[nodiscard]] auto get_mode() const -> std::string;

    [[nodiscard]] auto get_bpm() const -> uint8_t;

    [[nodiscard]] auto get_resolution() const -> uint8_t;

    [[nodiscard]] auto performanceFeedback() const -> std::vector<std::string>;


    //auto analysis() -> void;
    //so need to have it so that they could either enter in song or 
    
    //call the user
    auto analysis(std::vector<data::songs::Note>& ref) -> std::vector<bool>;
    auto analysis() -> std::vector<bool>;

    //this is analysis for note mode.
    auto analysis(std::string const& note) -> bool;

    //change this to note or overloaded. 
    //auto dataParseRef(std::string const& filename) -> std::vector<std::vector<int>>;

    //function that apploads midi to database.
    //auto dataParseUpload(std::string const& filename, uint8_t song_id, std::string const& title, std::string const& artist, uint8_t duration, uint8_t bpm) -> void;

    std::vector <std::vector<int > >  recording_numbers;
    std::vector <std::vector<int > >  reference_numbers;
    // Needed to avoid some compiler errors
    ~playerMode();

private:
    //song struct has information about song, etc.
    data::songs::SongInfo m_rec_song;

    //map to help me convert between note to number.
    std::unordered_map<std::string, uint8_t> semitoneOffsets = {{"C", 0},  {"C#", 1}, {"Db", 1},  {"D", 2},   {"D#", 3}, {"Eb", 3},
                                                                {"E", 4},  {"F", 5},  {"F#", 6},  {"Gb", 6},  {"G", 7},  {"G#", 8},
                                                                {"Ab", 8}, {"A", 9},  {"A#", 10}, {"Bb", 10}, {"B", 11}};
    //used to convert Note
    auto noteToInt(std::string const& note) -> uint8_t;
    //this is analysis for song mode.

    auto recordtoMIDI(uint8_t song_id, uint8_t duration,
        uint8_t bpm) -> std::vector<std::vector<int>>;
    
    struct noteEntry {
        uint32_t start_time;
        uint16_t duration;
        uint16_t orig_pos;
        bool seen;
    };

    uint32_t ref_size; 

    //ref data.
    std::map<std::uint8_t, std::vector<noteEntry>> ref_data;

    auto organizeRef() -> std::map<std::uint8_t, std::vector<noteEntry>>;

    //checks seen
    auto checkNote(data::songs::Note& ref_note) -> void;

    auto checkSong() -> void;

    //youtube
   static auto compareByStartTime(const noteEntry& entry, std::uint32_t target)->bool;

    // Comparator for reverse comparison, useful for certain cases
    static auto compareByStartTimeReverse(std::uint32_t target, const noteEntry& entry)->bool;

};
#endif


