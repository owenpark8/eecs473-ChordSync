#pragma once

#include <array>
#include <cstdint>

#include "guitar.hpp"
#include "lcd.hpp"
#include "messaging.hpp"


/**
 * Class for controlling the entire fretboard of the guitar, which spans multiple LCD screens.
 * Interfaced with coordinates on both "fretboard grid" or "pixel grid" as continous grids across LCD screens
 */
class Fretboard {
    static constexpr std::size_t NUM_LCDS = 6;
    static constexpr std::size_t NUM_FRETS = 23;
    static constexpr std::size_t NUM_STRINGS = 6;
    static constexpr uint16_t TOTAL_PIXEL_WIDTH = NUM_LCDS * ILI9486_TFTHEIGHT;
    static constexpr uint32_t WARNING_DELAY = 2500; // Delay between green, yellow, red in ms

    struct note_location_rectangle_t {
        pixel_location_t pixel_loc;
        uint16_t w;
        uint16_t h;
    };

    struct note_timestamps { // note to be played at timestamp x
        note_location_t note_loc;
        uint32_t timestamp;
        uint16_t color;
    };

    enum class uart_state {
        NEW_MSG, // Process a new header and message (2 bytes)
        SONG_ID,
        NOTE
    };


    std::array<LCD, NUM_LCDS> m_lcds{};
    std::size_t m_song_size{};
    std::array<NoteDataMessage, MAX_NOTES_IN_SONG> m_song{}; // elements in this array should be sorted
    std::size_t m_timestamps_size{}; // total size of m_timestamps
    std::size_t m_timestamp_idx{}; // index of next note to be displayed on the fretboard
    std::array<note_timestamps, MAX_NOTES_IN_SONG*4> m_timestamps{}; // first display green, yellow, then clear with white (4 colors)
    UART_HandleTypeDef* m_huart;
    uint32_t m_song_count_ms; // keeps track of the amount of ms elapsed, incremented every 1 ms
    uint8_t m_uart_buf[sizeof(NoteDataMessage) + 1] = {0};
    uart_state m_uart_state;
    uint8_t m_song_id;
    bool m_playing_song; // true iff we are currently playing a song on the fretboard


public:
    /**
     * @brief Default constructor for Fretboard
     *
     */
    Fretboard() = default;

    /**
     * @brief Constructor for Fretboard with 6 LCDs
     *
     */
    Fretboard(LCD const& lcd_1, LCD const& lcd_2, LCD const& lcd_3, LCD const& lcd_4, LCD const& lcd_5, LCD const& lcd_6, UART_HandleTypeDef* huart)
        : m_lcds{lcd_1, lcd_2, lcd_3, lcd_4, lcd_5, lcd_6}, m_huart(huart) {}


    /**
     * @brief Initializes LCD screens on the fretboard
     *
     */
    auto init() -> void {
        m_lcds[0].reset_lcd(); // only need to call it on an arbitrary one since they are all tied together
        for (auto& lcd: m_lcds) {
            lcd.init();
        }
        clear();
        m_playing_song = false;
        m_song_id = 0; // song id 0 means no song currently loaded
        // Initialize UART Protocol
        rec_new_msg();
    }

    /**
     * @brief Writes a circle to a specific location on the fretboard
     * @param fretboard_location coordinates of note on fretboard grid
     * @param color color of circle to be written
     */
    auto draw_note(note_location_t note_location, uint16_t color) -> void {
        note_location_rectangle_t note_location_rectangle = convert_note_to_pixels(note_location);
        // Find which lcd(s) will be selected to draw this note
        uint16_t lcd_index_1 = note_location_rectangle.pixel_loc.x / 480;                               // 480 is width of LCD screen in pixels
        uint16_t lcd_index_2 = (note_location_rectangle.pixel_loc.x + note_location_rectangle.w) / 480; // 480 is width of LCD screen in pixels
        // In both cases, we want to draw a rectangle on the first LCD, our
        // if condition will confine the set_addr_window to not go beyond the bounds of the screen
        if (lcd_index_1 < NUM_LCDS) {
            m_lcds[lcd_index_1].draw_rectangle({note_location_rectangle.pixel_loc.x % 480, note_location_rectangle.pixel_loc.y},
                                               note_location_rectangle.w, note_location_rectangle.h, color);
        }
        if (lcd_index_1 != lcd_index_2) {
            // Note goes across screens
            if (lcd_index_2 < NUM_LCDS) {
                m_lcds[lcd_index_2].draw_rectangle({0, note_location_rectangle.pixel_loc.y},
                                                   (note_location_rectangle.pixel_loc.x + note_location_rectangle.w) % 480, note_location_rectangle.h,
                                                   color);
            }
        }
    }


    /**
     * @brief draws indicator to play open string  TODO: determine what kind of indicator
     * @param string string to play
     * @param color color of indicator
     */
    auto draw_string(string_e string, uint16_t color) -> void {
        uint16_t pixel_y;
        uint16_t height;
        convert_string_to_y(string, pixel_y, height); // Gives values to pixel_y and height
        for (auto& lcd: m_lcds) {
            lcd.draw_horizontal_line({0, pixel_y}, height, color);
        }
    }


    /**
     * @brief Handles a uart message, called on interrupt
     * @note Make sure state transition happens before call to interrupt
     */
    auto handle_uart_message() -> void {
        if (m_uart_buf[0] != 0x01) return; // Header was not received correctly
        if (m_uart_state == uart_state::NEW_MSG) {
            uint8_t& message = m_uart_buf[1]; // Header is byte 0, msg is byte 1
            switch (static_cast<MessageType>(message)) {
                case MessageType::Reset:
                    init();
                    break;
                case MessageType::StartSongLoading:
                    m_song_size = 0;
                    m_uart_state = uart_state::SONG_ID;
                    HAL_UART_Receive_IT(m_huart, m_uart_buf, sizeof(StartSongLoadingDataMessage) + 1); // 1 byte for ID + 1 byte for header
                    break;
                case MessageType::EndSongLoading:
                	m_timestamps_size = 0;
                    process_loaded_song();
                    rec_new_msg();
                    break;
                case MessageType::Note:
                    m_uart_state = uart_state::NOTE;
                    HAL_UART_Receive_IT(m_huart, m_uart_buf, sizeof(NoteDataMessage) + 1); // receive note Data + 1 byte for header
                    break;
                case MessageType::StartSong:
                    m_playing_song = true;
                    m_song_count_ms = 0;
                    m_timestamp_idx = 0;
                    rec_new_msg();
                    break;
                case MessageType::EndSong:
                    m_playing_song = false;
                    rec_new_msg();
                    break;
                case MessageType::RequestSongID: {
                	send_ack();
                    HAL_UART_Transmit(m_huart, LOADED_SONG_ID_MESSAGE.data(), sizeof(LOADED_SONG_ID_MESSAGE), 100);
                    // Receive ACK
                    HAL_UART_Receive(m_huart, m_uart_buf, sizeof(ACK_MESSAGE), HAL_MAX_DELAY);
                    const uint8_t song_id_buf[2] = {MESSAGE_HEADER, m_song_id};
                    HAL_UART_Transmit(m_huart, song_id_buf, sizeof(song_id_buf), 100);
                    // Receive another ACK
                    HAL_UART_Receive(m_huart, m_uart_buf, sizeof(ACK_MESSAGE), HAL_MAX_DELAY);
                    rec_new_msg();
                    return;
                }
                default:
                	rec_new_msg();
                    return;
            }
        } else if (m_uart_state == uart_state::SONG_ID) {
            m_song_id = m_uart_buf[1];
            rec_new_msg();
        } else { // m_uart_state == NOTE
            NoteDataMessage message = *reinterpret_cast<NoteDataMessage*>(m_uart_buf + 1);
            if (m_song_size < MAX_NOTES_IN_SONG) m_song[m_song_size++] = message;
            rec_new_msg();
        }
        send_ack();
    }

    /**
     * @brief increments counter and checks to see if we have to display a note
     */
    auto handle_song_time() -> void {
        if (m_playing_song) {
        while (m_timestamp_idx < m_timestamps_size && m_timestamps[m_timestamp_idx].timestamp <= m_song_count_ms) {
            const auto& tempstamp = m_timestamps[m_timestamp_idx]; // LOL!!!!!!!!!!!!!
            draw_note(tempstamp.note_loc, tempstamp.color);
            ++m_timestamp_idx;
        }
            ++m_song_count_ms;
        }
    }

    auto handle_uart_error() -> void {
    	// Clear all error flags
    	__HAL_UART_CLEAR_FLAG(m_huart, UART_CLEAR_FEF | UART_CLEAR_NEF | UART_CLEAR_OREF | UART_CLEAR_PEF);
    	rec_new_msg();
    	// send_ack();
    }
private:
    /*----------------------BACKDOOR FUNCTIONS-------------------------- */

    /**
     * @brief converts note location to note pixel location
     * @param note_location note location to be converted
     * @return pixel coordinate of note
     */
    auto convert_note_to_pixels(note_location_t note_location) -> note_location_rectangle_t {
        // TODO: replace these numbers with actual pixels after measuring with guitar
        static std::array<uint16_t, NUM_FRETS> const fret_pixel_array = {// keeps track of fret to pixel_location.x
            0,    158,  378,  510,  705,  900,  995, 1166, 1315, 1440, 1520,
            1650, 1775, 1900, 1934, 2040, 2137, 2233, 2315, 2393, 2405, 2476, 2551};

        uint16_t pixel_x = fret_pixel_array[note_location.fret];
        uint16_t pixel_y = 0;
        uint16_t height = 0;
        convert_string_to_y(note_location.string, pixel_y, height); // Gives values to pixel_y and height
        // Find width and height of rectangle to be drawn
        uint16_t width = (note_location.fret == NUM_FRETS - 1) ? TOTAL_PIXEL_WIDTH - pixel_x : fret_pixel_array[note_location.fret + 1] - pixel_x;
        return {{pixel_x, pixel_y}, width, height};
    }

    /**
     * @brief Clears the fretboard LCDs
     */
    auto clear() -> void {
        for (auto& lcds: m_lcds) {
            lcds.clear_screen();
        }
    }

    /*
     * @brief converts string to height and pixel_location.y
     * @param y string to be converted
     * @param pixel_y changes in this function, starting pixel location y
     * @param height changes in this function, height to next pixel
     */
    auto convert_string_to_y(string_e y, uint16_t& pixel_y, uint16_t& height) -> void {
        switch (y) {
            // TODO: Check if these conversions are correct with guitar
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

    /**
     * @brief increments counter and checks to see if we have to display a note
     */
    auto process_loaded_song() -> void {
        for (size_t i = 0; i < m_song_size; ++i) { // parse through notedata and populated timestamps
        	const auto &note_data = m_song[i];
            m_timestamps[m_timestamps_size++] = {{static_cast<fret_t>(note_data.fret), static_cast<string_e>(note_data.string)}, note_data.timestamp_ms, GREEN};
            m_timestamps[m_timestamps_size++] = {{static_cast<fret_t>(note_data.fret), static_cast<string_e>(note_data.string)}, note_data.timestamp_ms + WARNING_DELAY, YELLOW};
            m_timestamps[m_timestamps_size++] = {{static_cast<fret_t>(note_data.fret), static_cast<string_e>(note_data.string)}, note_data.timestamp_ms + WARNING_DELAY*2, RED};
            m_timestamps[m_timestamps_size++] = {{static_cast<fret_t>(note_data.fret), static_cast<string_e>(note_data.string)}, note_data.timestamp_ms + WARNING_DELAY*2 + note_data.length_ms, WHITE};
        }
        // Insertion sort since the array is already mostly sorted
        for (int i = 0; i < m_timestamps_size; ++i) {
            int swapped = i;
            for(int j = i+1; j < m_timestamps_size; ++j) {
                if (m_timestamps[j].timestamp < m_timestamps[swapped].timestamp) swapped = j;
            }
            std::swap(m_timestamps[i], m_timestamps[swapped]);
        }
    }

    auto send_ack() -> void { HAL_UART_Transmit(m_huart, ACK_MESSAGE.data(), sizeof(ACK_MESSAGE), 100); }

    auto rec_new_msg() -> void {
        m_uart_state = uart_state::NEW_MSG;
        HAL_UART_Receive_IT(m_huart, m_uart_buf, 2);
    }
};
