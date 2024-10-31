#include "fretboard.hpp"
#include "lcd.hpp"

auto Fretboard::fretboard_init() -> void{
    for (auto& lcd: m_lcds) {
        lcd.init();
    }
    clear_fretboard();
}

auto Fretboard::draw_note(fretboard_location_t fretboard_location, uint16_t color) -> void{
    fret_loc_pixel_dim_t note_location = convert_fret_to_pixels(fretboard_location);
    // Find which lcd(s) will be selected to draw this note
    uint16_t lcd_index_1 = note_location.pixel_loc.x / 480; // 480 is width of LCD screen in pixels
    uint16_t lcd_index_2 = (note_location.pixel_loc.x + note_location.w) / 480; // 480 is width of LCD screen in pixels
    // In both cases, we want to draw a rectangle on the first LCD, our 
    // if condition will confine the set_addr_window to not go beyond the bounds of the screen
    m_lcds[lcd_index_1].draw_rectangle(note_location.pixel_loc, note_location.w, note_location.h, color);
    if (lcd_index_1 != lcd_index_2) { // Note goes across screens
        m_lcds[lcd_index_2].draw_rectangle(note_location.pixel_loc, note_location.w % 480, note_location.h, color);
    }
}

auto Fretboard::draw_string(string_e string, uint16_t color) -> void{
    uint16_t pixel_y;
    uint16_t height;
    convert_fret_y(string, pixel_y, height); // Gives values to pixel_y and height
    for(auto& lcd: m_lcds) {
        lcd.draw_horizontal_line({0, pixel_y}, height, color);
    }
}

auto Fretboard::convert_fret_to_pixels(fretboard_location_t fretboard_location) -> fret_loc_pixel_dim_t {
    uint16_t pixel_x = fret_pixel_array[fretboard_location.x];
    uint16_t pixel_y = 0;
    uint16_t height = 0;
    convert_fret_y(fretboard_location.y, pixel_y, height); // Gives values to pixel_y and height
    // Find width and height of rectangle to be drawn
    uint16_t width = (fretboard_location.x == num_frets-1) ? total_pixel_width - pixel_x : fret_pixel_array[fretboard_location.x+1] - pixel_x;
    return {{pixel_x, pixel_y}, width, height};
}
auto Fretboard::clear_fretboard() -> void{
    for (auto &lcds: m_lcds) {
        lcds.clear_screen();
    }
}

auto Fretboard::convert_fret_y(string_e y, uint16_t &pixel_y, uint16_t &height) -> void {
    switch (y) { // TODO: Check if these conversions are correct with guitar
        case string_e::HIGH_E:
            pixel_y = 0;
            height = 52;
            break;
        case string_e::A:
            pixel_y = 53;
            height = 53;
            break;
        case string_e::D:
            pixel_y = 107;
            height = 53;
            break;
        case string_e::G:
            pixel_y = 160;
            height = 52;
            break;
        case string_e::B:
            pixel_y = 213;
            height = 53;
            break;
        case string_e::LOW_E:
            pixel_y = 267;
            height = 52;
            break;
        default:
            break;
    }
}