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
    playerMode(uint8_t song_id, uint8_t mode, std::string const& note, std::string const& title, std::string const artist, uint8_t duration,
               uint8_t bpm);
    // EFFECTS returns player's name
    std::string get_mode() const;

    uint8_t get_bpm() const;

    uint8_t get_resolution() const;

    std::vector<std::string> performanceFeedback() const;


    //call the user
    std::vector<bool> analysis(std::vector<Note>& ref);

    //this is analysis for note mode.
    bool analysis(std::string const& note);


    // Needed to avoid some compiler errors
    ~playerMode();

private:
    //song struct has information about song, etc.
    SongInfo song;

    //record note based on mode.
    std::uint8_t mode;

    std::uint8_t resolution;

    std::string note;


    //map to help me convert between note to number.
    std::unordered_map<std::string, uint8_t> semitoneOffsets = {{"C", 0},  {"C#", 1}, {"Db", 1},  {"D", 2},   {"D#", 3}, {"Eb", 3},
                                                                {"E", 4},  {"F", 5},  {"F#", 6},  {"Gb", 6},  {"G", 7},  {"G#", 8},
                                                                {"Ab", 8}, {"A", 9},  {"A#", 10}, {"Bb", 10}, {"B", 11}};

    //used to convert Note
    uint8_t noteToInt(std::string const& note);
    //this is analysis for song mode.
};
#endif
