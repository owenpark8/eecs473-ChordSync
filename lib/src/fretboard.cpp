#include "fretboard.hpp"
#include "lcd.hpp"

void Fretboard::fretboard_init() {
    for (auto& lcds: m_lcds) {
        lcds.init();
    };
    clear_fretboard();
}

void Fretboard::draw_note(fretboard_location_t fretboard_location, int radius, color_t color) {}
void Fretboard::draw_string(string_e string, color_t color) {}
// void Fretboard::draw_pixel(pixel_location_t pixel_location, color_t color) {}
auto Fretboard::convert_fret_to_pixels(fretboard_location_t fretboard_location) -> pixel_location_t {
    uint16_t pixel_x = fret_pixel_array[fretboard_location.x];
    uint16_t pixel_y = 0;
    switch (fretboard_location.y) { // TODO: Check if these conversions are correct with guitar
        case string_e::HIGH_E:
            pixel_y = 0;
            break;
        case string_e::A:
            pixel_y = 53;
            break;
        case string_e::D:
            pixel_y = 107;
            break;
        case string_e::G:
            pixel_y = 160;
            break;
        case string_e::B:
            pixel_y = 213;
            break;
        case string_e::LOW_E:
            pixel_y = 267;
            break;
        default:
            break;
    }
    return {pixel_x, pixel_y};
}
void Fretboard::clear_fretboard() {
    for (auto &lcds: m_lcds) {
        lcds.clear_screen();
    }
}