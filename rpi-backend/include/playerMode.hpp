#ifndef PLAYERMODE_H
#define PLAYERMODE_H


#include <data.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class playerMode {
public:
    //do the recording here
    //potential modes: reference create, record and convert song, record and convert note.
    //can pass in note to convert.
    playerMode(uint8_t song_id, uint8_t mode, std::string const& note, std::string const& title, std::string const& artist, uint8_t duration,
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
    auto dataParseRef(std::string const& filename) -> std::vector<std::vector<int>>;

    //function that apploads midi to database.
    auto dataParseUpload(std::string const& filename, uint8_t song_id, std::string const& title, std::string const& artist, uint8_t duration, uint8_t bpm) -> void;



    // Needed to avoid some compiler errors
    ~playerMode();

private:
    //song struct has information about song, etc.
    data::songs::SongInfo song;

    //record note based on mode.
    std::uint8_t mode;

    std::uint8_t resolution;

    std::string note;


    //map to help me convert between note to number.
    std::unordered_map<std::string, uint8_t> semitoneOffsets = {{"C", 0},  {"C#", 1}, {"Db", 1},  {"D", 2},   {"D#", 3}, {"Eb", 3},
                                                                {"E", 4},  {"F", 5},  {"F#", 6},  {"Gb", 6},  {"G", 7},  {"G#", 8},
                                                                {"Ab", 8}, {"A", 9},  {"A#", 10}, {"Bb", 10}, {"B", 11}};

    //used to convert Note
    auto noteToInt(std::string const& note) -> uint8_t;
    //this is analysis for song mode.

    auto midiParse(uint8_t song_id, uint8_t duration,
        uint8_t bpm) -> std::vector<std::vector<int>>;
};
#endif
