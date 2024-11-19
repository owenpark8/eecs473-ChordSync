#ifndef PLAYERMODE_H
#define PLAYERMODE_H


#include <messaging.hpp>
#include <string>
#include <vector>

class playerMode {
public:
    //do the recording here
    playerMode(std::uint8_t const& song_id, std::uint8_t const& mode, std::uint8_t const& duration, std::uint8_t& bpm);
    // EFFECTS returns player's name
    std::string& get_mode() const;

    std::uint8_t& get_bpm() const;

    std::uint8_t& get_resolution() const;

    //this is analysis for song mode.
    //call the user
    vector<std::string> analysis(std::vector<NoteDataMessage>& ref) const;

    //this is analysis for note mode.
    vector<std::string> analysis(std::string& note) const;

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
#endif
