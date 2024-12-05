#pragma once

#include <array>

#include "guitar.hpp"

constexpr std::array<NoteLocation, 5> A_MAJOR_CHORD = {
        NoteLocation{
                .fret = 0,
                .string = string_e::A,
        },
        NoteLocation{
                .fret = 2,
                .string = string_e::D,
        },
        NoteLocation{
                .fret = 2,
                .string = string_e::G,
        },
        NoteLocation{
                .fret = 2,
                .string = string_e::B,
        },
        NoteLocation{
                .fret = 0,
                .string = string_e::HIGH_E,
        },
};
constexpr std::array<NoteLocation, 5> C_MAJOR_CHORD = {
        NoteLocation{
                .fret = 3,
                .string = string_e::A,
        },
        NoteLocation{
                .fret = 2,
                .string = string_e::D,
        },
        NoteLocation{
                .fret = 0,
                .string = string_e::G,
        },
        NoteLocation{
                .fret = 1,
                .string = string_e::B,
        },
        NoteLocation{
                .fret = 0,
                .string = string_e::HIGH_E,
        },
};
constexpr std::array<NoteLocation, 4> D_MAJOR_CHORD{
        NoteLocation{
                .fret = 0,
                .string = string_e::D,
        },
        NoteLocation{
                .fret = 2,
                .string = string_e::G,
        },
        NoteLocation{
                .fret = 3,
                .string = string_e::B,
        },
        NoteLocation{
                .fret = 2,
                .string = string_e::HIGH_E,
        },
};
constexpr std::array<NoteLocation, 6> E_MAJOR_CHORD{
        NoteLocation{
                .fret = 0,
                .string = string_e::LOW_E,
        },
        NoteLocation{
                .fret = 2,
                .string = string_e::A,
        },
        NoteLocation{
                .fret = 2,
                .string = string_e::D,
        },
        NoteLocation{
                .fret = 1,
                .string = string_e::G,
        },
        NoteLocation{
                .fret = 0,
                .string = string_e::B,
        },
        NoteLocation{
                .fret = 0,
                .string = string_e::HIGH_E,
        },
};
constexpr std::array<NoteLocation, 4> F_MAJOR_CHORD = {
        NoteLocation{
                .fret = 3,
                .string = string_e::D,
        },
        NoteLocation{
                .fret = 2,
                .string = string_e::G,
        },
        NoteLocation{
                .fret = 1,
                .string = string_e::B,
        },
        NoteLocation{
                .fret = 1,
                .string = string_e::HIGH_E,
        },
};
constexpr std::array<NoteLocation, 6> G_MAJOR_CHORD = {
        NoteLocation{
                .fret = 3,
                .string = string_e::LOW_E,
        },
        NoteLocation{
                .fret = 2,
                .string = string_e::A,
        },
        NoteLocation{
                .fret = 0,
                .string = string_e::D,
        },
        NoteLocation{
                .fret = 0,
                .string = string_e::G,
        },
        NoteLocation{
                .fret = 0,
                .string = string_e::B,
        },
        NoteLocation{
                .fret = 3,
                .string = string_e::HIGH_E,
        },
};
