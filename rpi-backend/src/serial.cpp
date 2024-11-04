#ifdef DEBUG
#include <iostream>
#endif
#include <string>

#include <wiringSerial.hpp>

#include "serial.hpp"

namespace serial {
    std::string const dev_name = "/dev/ttyAMA0";
    int fd = 0;

    auto init() -> bool {
#ifdef DEBUG
        std::cout << "Initializing serial communication...\n";
        std::cout << "Opening serial on device \"" << dev_name << "\"...\n";
#endif
        if ((fd = serialOpen(dev_name, baudrate)) < 0) {
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

    auto send(std::uint8_t const* buffer, std::size_t size) -> void {
        for (std::size_t i = 0; i < size; ++i) {
            serialPutchar(fd, *(buffer + i));
        }
    }

    auto receive(std::uint8_t* buffer, std::size_t size) -> void {
        // this is sus
        while (serialDataAvail(fd) != size);

        for (std::size_t i = 0; i < size; ++i) {
            *(buffer + i) = serialGetchar(fd);
        }
    }

} // namespace serial
