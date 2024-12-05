#pragma once

#include <chrono>

namespace timing {
    using namespace std::chrono_literals;

    constexpr std::chrono::milliseconds LCD_BOOTUP_TIME = 2000ms;
    constexpr std::chrono::seconds SONG_START_DELAY = 3s;
    constexpr std::chrono::milliseconds NOTE_WARNING_DELAY = 200ms;
    constexpr unsigned int UART_BAUDRATE = 9600;

} // namespace timing
