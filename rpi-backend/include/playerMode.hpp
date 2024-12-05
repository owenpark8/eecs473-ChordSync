#ifndef PLAYERMODE_H
#define PLAYERMODE_H


#include <cmath>
#include <data.hpp>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class playerMode {
public:
    playerMode() = default;
    //do the recording here
    //potential modes: reference create, record and convert song, record and convert note.
    //can pass in note to convert.
    playerMode(data::songs::SongInfo const& get_ref_song);
    // EFFECTS returns player's name

    [[nodiscard]] auto get_bpm() const -> uint8_t;

    //auto analysis() -> void;
    //so need to have it so that they could either enter in song or

    //call the user

    auto analyzeChord() -> bool;

    auto analysis(std::vector<data::songs::Note>& ref) -> std::vector<bool>;
    auto analysis() -> std::vector<bool>;

    //this is analysis for note mode.
    auto analysis(std::string const& note) -> bool;

    //change this to note or overloaded.
    //auto dataParseRef(std::string const& filename) -> std::vector<std::vector<int>>;

    //function that apploads midi to database.
    //auto dataParseUpload(std::string const& filename, uint8_t song_id, std::string const& title, std::string const& artist, uint8_t duration, uint8_t bpm) -> void;

    std::vector<std::vector<int>> recording_numbers;
    std::vector<std::vector<int>> reference_numbers;
    // Needed to avoid some compiler errors
    ~playerMode();

private:
    //song struct has information about song, etc.
    data::songs::SongInfo m_rec_song;

    data::songs::SongInfo m_ref_song;

    //map to help me convert between note to number.
    std::unordered_map<std::string, uint8_t> semitoneOffsets = {{"C", 0},  {"C#", 1}, {"Db", 1},  {"D", 2},   {"D#", 3}, {"Eb", 3},
                                                                {"E", 4},  {"F", 5},  {"F#", 6},  {"Gb", 6},  {"G", 7},  {"G#", 8},
                                                                {"Ab", 8}, {"A", 9},  {"A#", 10}, {"Bb", 10}, {"B", 11}};
    //used to convert Note
    auto m_noteToInt(std::string const& note) -> uint8_t;
    //this is analysis for song mode.

    auto m_recordtoMIDI(uint8_t song_id, uint16_t duration, uint8_t bpm) -> std::vector<std::vector<int>>;

    auto m_basic_pitch_prediction_run(uint8_t song_id, uint8_t bpm) -> void;
    auto m_delete_generated_files(uint8_t song_id) -> void;

    struct m_noteEntry {
        uint32_t start_time;
        uint16_t duration;
        uint16_t orig_pos;
        bool seen;
    };

    std::uint32_t m_ref_size = 0;

    //ref data.
    std::map<std::uint8_t, std::vector<m_noteEntry>> m_ref_data;

    auto m_organizeRef() -> void;

    //checks seen
    auto m_checkNote(data::songs::Note& ref_note) -> void;

    auto m_checkSong() -> void;

    //youtube
    static auto m_compareByStartTime(m_noteEntry const& entry, std::uint32_t target) -> bool;

    // Comparator for reverse comparison, useful for certain cases
    static auto m_compareByStartTimeReverse(std::uint32_t target, m_noteEntry const& entry) -> bool;
};
#endif
