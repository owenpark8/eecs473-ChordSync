#pragma once

#include <cstddef>
#include <cstdint>

namespace serial {
    constexpr unsigned int baudrate = 115200;

    auto init() -> bool;
    auto send(std::uint8_t data) -> void;
    auto send(std::uint8_t const* buffer, std::size_t size) -> void;
    auto receive(std::uint8_t* buffer, std::size_t size) -> void;

} // namespace serial
