#include "playerMode.hpp"
#include <iostream>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <sys/wait.h>

namespace py = pybind11;


auto playerMode::m_recordtoMIDI(uint8_t song_id, uint16_t duration, uint8_t bpm) -> std::vector<std::vector<int>> {

    // Start recording player input
    /*pid_t recordwav = fork();

    if (recordwav == -1) {
        // Fork failed
        std::cerr << "Failed to fork a new process" << std::endl;
    } else if (recordwav == 0) {
        // Child process: execute the 'arecord' command
        // Store strings in variables to avoid temporary destruction
        std::string durationStr = std::to_string(duration + 10);
        std::string fileName = std::to_string(song_id) + "_rec.wav";

        execlp("arecord", "arecord", "--duration", durationStr.c_str(), "--rate=88200", "--format=S16_LE", fileName.c_str(), (char*) NULL);
        // If execlp fails
        std::cerr << "Error executing arecord" << std::endl;
        _exit(1); // Exit child process with error status
    }


    int status;
    waitpid(recordwav, &status, 0);


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
    py::object get_record_convert = get_record_convert_module.attr("record_convert_no_offset");


    this->m_basic_pitch_prediction_run(song_id, bpm);

    auto numbers = get_record_convert(song_id, bpm).cast<std::vector<std::vector<int>>>();

    this->m_delete_generated_files(song_id);

    return numbers;*/


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
    py::object get_record_convert = get_record_convert_module.attr("record_convert_offset");



    std::string command = "arecord --duration=" + std::to_string(duration) +
                          " --rate=88200 --format=S16_LE " + std::to_string(song_id) + "_rec.wav";

    // Execute the command
    int result = system(command.c_str());

    // Check if the command was successful
    if (result != 0) {
        // Handle the error
        std::cerr << "Error executing command: " << command << std::endl;
    } else {
        std::cout << "Recording completed successfully." << std::endl;
    }

    this->m_basic_pitch_prediction_run(song_id, bpm);
    auto numbers = get_record_convert(song_id).cast<std::vector<std::vector<int>>>();

    this->m_delete_generated_files(song_id);

    return numbers;
}


auto playerMode::m_delete_generated_files(uint8_t song_id) -> void {
    py::module sys = py::module::import("sys");
    sys.attr("path").attr("insert")(0, PY_VENV_PATH);
    sys.attr("path").attr("insert")(0, PY_MODULE_PATH);

    auto get_record_convert_module = py::module_::import("record_convert");
    py::object get_remove_files = get_record_convert_module.attr("remove_files");

    get_remove_files(song_id);
}


auto playerMode::m_basic_pitch_prediction_run(uint8_t song_id, uint8_t bpm) -> void {
    py::module sys = py::module::import("sys");
    sys.attr("path").attr("insert")(0, PY_VENV_PATH);
    sys.attr("path").attr("insert")(0, PY_MODULE_PATH);

    auto get_record_convert_module = py::module_::import("record_convert");
    py::object get_prediction = get_record_convert_module.attr("prediction");

    get_prediction(song_id, bpm);
}


auto playerMode::m_organizeRef() -> void {
    //judst organize song info

    this->m_ref_size = m_ref_song.notes.size();
    for (int i = 0; i < m_ref_song.notes.size(); i++) {
        m_noteEntry entry = {static_cast<uint32_t>(m_ref_song.notes[i].start_timestamp_ms), static_cast<uint16_t>(m_ref_song.notes[i].length_ms),
                             static_cast<uint16_t>(i), false};
        this->m_ref_data[m_ref_song.notes[i].midi_note].push_back(entry);
    }
}


//add in cleanout function
playerMode::playerMode(data::songs::SongInfo const& get_ref_song) : m_rec_song{get_ref_song.id, "rec", "rec", get_ref_song.length, get_ref_song.bpm} {
    py::scoped_interpreter guard{};
    //begin recording from Python.
   
    auto numbers = m_recordtoMIDI(get_ref_song.id, static_cast<uint16_t>(get_ref_song.length.count()), get_ref_song.bpm);
    this->recording_numbers = numbers;
    //Process each note in the RECORDING and add it our song vector
    for (auto& number: numbers) {
        data::songs::Note entry;
        entry.midi_note = static_cast<uint8_t>(number[0]);
        entry.start_timestamp_ms = static_cast<uint32_t>(number[1]);
        entry.length_ms = static_cast<uint16_t>(number[2] - entry.start_timestamp_ms);
        (this->m_rec_song.notes).push_back(entry);
    }
    //need to figure out filtering. 

    this->m_ref_song = get_ref_song;

    this->m_organizeRef();
}

auto playerMode::get_bpm() const -> uint8_t { return this->m_rec_song.bpm; }


auto playerMode::m_compareByStartTime(m_noteEntry const& entry, std::uint32_t target) -> bool {
    return (int32_t) entry.start_time < (int32_t) target;
}

// Comparator for reverse comparison, useful for certain cases
auto playerMode::m_compareByStartTimeReverse(std::uint32_t target, m_noteEntry const& entry) -> bool {
    return (int32_t) target < (int32_t) entry.start_time;
}

auto playerMode::m_checkSong() -> void {
    for (auto note: this->m_rec_song.notes) {
        m_checkNote(note);
    }
}

auto playerMode::m_checkNote(data::songs::Note& rec_note) -> void {
    //this is an extraneous

    auto occurences = this->m_ref_data.find(rec_note.midi_note);
    //so it exists.

    if (occurences != this->m_ref_data.end()) {
        //std::cout << "Found Note " << (uint16_t)rec_note.midi_note << " within reference song. " << std::endl;
        //these are occurences.
        auto lower = std::lower_bound(occurences->second.begin(), occurences->second.end(), rec_note.start_timestamp_ms, m_compareByStartTime);
        auto upper = std::upper_bound(occurences->second.begin(), occurences->second.end(), rec_note.start_timestamp_ms, m_compareByStartTimeReverse);

        //std::cout << "Found lower at " << lower->start_time << " and higher at " << upper->start_time << " comparing " << rec_note.start_timestamp_ms << std::endl;
        auto closest = abs((int32_t) lower->start_time - (int32_t) rec_note.start_timestamp_ms) <
                                       abs((int32_t) upper->start_time - (int32_t) rec_note.start_timestamp_ms)
                               ? lower
                               : upper;


        printf("%s%d\n", "This is rec note", rec_note.midi_note);
        std::cout << "This is rec start time: " << rec_note.start_timestamp_ms << std::endl;
        std::cout << "This is ref start time: " << closest->start_time << std::endl;
        std::cout << "This is rec duration: " << rec_note.start_timestamp_ms << std::endl;
        std::cout << "This is ref duration: " << closest->start_time << std::endl;
\

        if (abs((int32_t) closest->start_time - (int32_t) rec_note.start_timestamp_ms) > 2000) {
            closest->seen = false;
            return;
        }
        //std::cout << "Now checking Note " << (uint16_t)rec_note.midi_note << " for duration requirement " << std::endl;

        float error = float(abs((int32_t) closest->duration - (int32_t) rec_note.length_ms)) / (float) (closest->duration);

        if (error <= 0.5) {
            closest->seen = true;
            return;
        }

        closest->seen = false;
    }
}

auto playerMode::analysis() -> std::vector<bool> {
    m_checkSong();
    std::vector<bool> result(this->m_ref_size, false);

    for (auto const& [key, noteVector]: this->m_ref_data) {
        // Iterate through the vector of NoteEntry
        for (auto const& note: noteVector) {
            result[note.orig_pos] = note.seen;
        }
    }

    //returns result.
    return result;
}

auto playerMode::analyzeChord() -> bool {

    int count = 0;
    for (auto ref_note: this->m_ref_song.notes) {
        for (auto note: this->m_rec_song.notes) {
            if (note.midi_note == ref_note.midi_note) {
                count++;
                break;
            }
        }
    }

    if (count == m_ref_song.notes.size() && count != 0) {
        return true;
    }

    return false;
}

/*auto playerMode::analysis(std::string const& note) -> bool {
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
}*/

playerMode::~playerMode() = default;


/*auto playerMode::noteToInt(std::string const& note) -> uint8_t {
    // Map of note names to semitone offsets
    // Extract the note name and octave from the input string
    std::string name = note.substr(0, note.size() - 1); // Extract the note name
    uint8_t octave = note.back() - '0';                 // Convert the last character to an integer

    // Calculate the MIDI note number
    return 12 + (12 * octave) + semitoneOffsets[name];
}*/

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
