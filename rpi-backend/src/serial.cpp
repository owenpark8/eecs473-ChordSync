#ifdef DEBUG
#include <iostream>
#endif
#include <chrono>
#include <string>

#include <wiringSerial.hpp>

#include "serial.hpp"
#include "timing.hpp"

namespace serial {
    std::string const dev_name = "/dev/ttyAMA0";
    int fd = 0;

    auto init() -> bool {
#ifdef DEBUG
        std::cout << "Initializing serial communication...\n";
        std::cout << "Opening serial on device \"" << dev_name << "\"...\n";
#endif
        if ((fd = serialOpen(dev_name, timing::UART_BAUDRATE)) < 0) {
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

    auto send(std::uint8_t const data) -> void { serialPutchar(fd, data); }
    auto send(std::uint8_t const* buffer, std::size_t size) -> void { serialPutbuffer(fd, reinterpret_cast<char const*>(buffer), size); }

    auto receive(std::uint8_t* buffer, std::size_t size, std::chrono::seconds timeout) -> bool {
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

            if (serialDataAvail(fd) > 0) {
                int data = serialGetchar(fd);
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

} // namespace serial
