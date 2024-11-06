#pragma once

#include <cstdint>

/**
 * @enum string_e
 * @brief Represents guitar string identifiers.
 */
enum class string_e : std::uint8_t {
    LOW_E = 0x00,
    A = 0x01,
    D = 0x02,
    G = 0x03,
    B = 0x04,
    HIGH_E = 0x05,
};

/**
 * @typedef fret_t
 * @brief Represents the fret number on a guitar.
 *
 * Holds values in the range [0-23].
 * - Fret `0`: Open string (no fretting).
 * - Fret `1`: Closest to the tuning pegs.
 * - ...
 * - Fret `23`: Closest to the guitar body.
 */
using fret_t = std::uint8_t;

/**
 * @struct note_location_t
 * @brief Represents a location on the guitar fretboard.
 */
struct note_location_t {
    fret_t fret;
    string_e string;
};
