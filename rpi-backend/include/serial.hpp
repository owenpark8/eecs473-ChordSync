#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace serial {
    auto init() -> bool;
    auto send(std::uint8_t data) -> void;
    auto send(std::uint8_t const* buffer, std::size_t size) -> void;
    auto receive(std::uint8_t* buffer, std::size_t size, std::chrono::seconds timeout = std::chrono::seconds(10)) -> bool;
    auto flush() -> void;
} // namespace serial
