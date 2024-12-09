#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

#include <wiringSerial.hpp>

#include "timing.hpp"

class Serial {
public:
    Serial() = default;
    ~Serial() {
        if (m_fd != -1) {
            serialClose(m_fd);
        }
    }

    auto init() -> bool {
#ifdef DEBUG
        std::cout << "Initializing serial communication...\n";
        std::cout << "Opening serial on device \"" << m_dev_name << "\"...\n";
#endif
        if ((m_fd = serialOpen(m_dev_name, timing::UART_BAUDRATE)) < 0) {
            return false;
        }
#ifdef DEBUG
        std::cout << "Setting up wiringPi library...\n";
#endif
        // TODO: (owen) do we need this?
        // if (wiringPiSetup() == -1) {
        //     return false;
        // }
#ifdef DEBUG
        std::cout << "Serial communication successfully set up!\n";
#endif
        return true;
    }

    auto send(std::uint8_t const data) const -> void { serialPutchar(m_fd, data); }
    auto send(std::uint8_t const* buffer, std::size_t size) const -> void { serialPutbuffer(m_fd, reinterpret_cast<char const*>(buffer), size); }

    auto receive(std::uint8_t* buffer, std::size_t size, std::chrono::seconds timeout = std::chrono::seconds(3)) const -> bool {
        auto start_time = std::chrono::steady_clock::now();
        std::size_t received = 0;

        while (received < size) {
            // Check timeout
            auto elapsed = std::chrono::steady_clock::now() - start_time;
            if (elapsed > timeout) {
#ifdef DEBUG
                std::cerr << "Timeout occurred after " << timeout.count() << " seconds\n";
#endif
                return false;
            }

            if (serialDataAvail(m_fd) > 0) {
                int data = serialGetchar(m_fd);
                if (data == -1) {
#ifdef DEBUG
                    std::cerr << "Error: serialGetchar returned -1\n";
#endif
                    return false;
                }
                buffer[received++] = static_cast<std::uint8_t>(data);
            }
        }
        return true;
    }

    auto flush() const -> void { serialFlush(m_fd); }


private:
    static std::string constexpr m_dev_name = "/dev/ttyAMA0";

    int m_fd = -1;
};
