/*
 * wiringSerial.c:
 *	Handle a serial port
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "wiringSerial.hpp"

/*
 * serialOpen:
 *	Open and initialise the serial port, setting all the right
 *	port parameters - or as many as are required - hopefully!
 *********************************************************************************
 */
auto serialOpen(std::string const& device, int const baud) -> int { return serialOpen(device.c_str(), baud); }

auto serialOpen(char const* device, int const baud) -> int {
    termios options{};
    speed_t myBaud;
    int status, fd;

    switch (baud) {
        case 50:
            myBaud = B50;
            break;
        case 75:
            myBaud = B75;
            break;
        case 110:
            myBaud = B110;
            break;
        case 134:
            myBaud = B134;
            break;
        case 150:
            myBaud = B150;
            break;
        case 200:
            myBaud = B200;
            break;
        case 300:
            myBaud = B300;
            break;
        case 600:
            myBaud = B600;
            break;
        case 1200:
            myBaud = B1200;
            break;
        case 1800:
            myBaud = B1800;
            break;
        case 2400:
            myBaud = B2400;
            break;
        case 4800:
            myBaud = B4800;
            break;
        case 9600:
            myBaud = B9600;
            break;
        case 19200:
            myBaud = B19200;
            break;
        case 38400:
            myBaud = B38400;
            break;
        case 57600:
            myBaud = B57600;
            break;
        case 115200:
            myBaud = B115200;
            break;
        case 230400:
            myBaud = B230400;
            break;
            //    case  460800:	myBaud =  B460800 ; break ;
            //    case  500000:	myBaud =  B500000 ; break ;
            //    case  576000:	myBaud =  B576000 ; break ;
            //    case  921600:	myBaud =  B921600 ; break ;
            //    case 1000000:	myBaud = B1000000 ; break ;
            //    case 1152000:	myBaud = B1152000 ; break ;
            //    case 1500000:	myBaud = B1500000 ; break ;
            //    case 2000000:	myBaud = B2000000 ; break ;
            //    case 2500000:	myBaud = B2500000 ; break ;
            //    case 3000000:	myBaud = B3000000 ; break ;
            //    case 3500000:	myBaud = B3500000 ; break ;
            //    case 4000000:	myBaud = B4000000 ; break ;

        default:
            return -2;
    }

    if ((fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1) return -1;

    fcntl(fd, F_SETFL, O_RDWR);

    // Get and modify current options:

    tcgetattr(fd, &options);

    cfmakeraw(&options);
    cfsetispeed(&options, myBaud);
    cfsetospeed(&options, myBaud);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 100; // Ten seconds (100 deciseconds)

    tcsetattr(fd, TCSANOW, &options);

    ioctl(fd, TIOCMGET, &status);

    status |= TIOCM_DTR;
    status |= TIOCM_RTS;

    ioctl(fd, TIOCMSET, &status);

    usleep(10000); // 10mS

    return fd;
}


/*
 * serialFlush:
 *	Flush the serial buffers (both tx & rx)
 *********************************************************************************
 */

void serialFlush(int const fd) { tcflush(fd, TCIOFLUSH); }


/*
 * serialClose:
 *	Release the serial port
 *********************************************************************************
 */

void serialClose(int const fd) { close(fd); }


/*
 * serialPutchar:
 *	Send a single character to the serial port
 *********************************************************************************
 */

auto serialPutchar(int const fd, unsigned char const c) -> void {
    ssize_t bytes_written = write(fd, &c, 1);
    if (bytes_written != 1) {
        throw std::runtime_error("Could not write all bytes to serial port!");
    }
}

/*
 * serialPutbuffer:
 *	Send a specified sized buffer to the serial port
 *********************************************************************************
 */

auto serialPutbuffer(int const fd, char const* buffer, size_t length) -> void {
    ssize_t bytes_written = write(fd, buffer, length);
    if (bytes_written != static_cast<ssize_t>(length)) {
        throw std::runtime_error("Could not write all bytes to serial port!");
    }
}

/*
 * serialPuts:
 *	Send a string to the serial port
 *********************************************************************************
 */

auto serialPuts(int const fd, char const* c_str) -> void {
    size_t length = strlen(c_str);
    ssize_t bytes_written = write(fd, c_str, length);
    if (bytes_written != static_cast<ssize_t>(length)) {
        throw std::runtime_error("Could not write all bytes to serial port!");
    }
}

/*
 * serialPrintf:
 *	Printf over Serial
 *********************************************************************************
 */

auto serialPrintf(int const fd, char const* message, ...) -> void {
    va_list argp;
    char buffer[1024];

    va_start(argp, message);
    vsnprintf(buffer, 1023, message, argp);
    va_end(argp);

    serialPuts(fd, buffer);
}


/*
 * serialDataAvail:
 *	Return the number of bytes of data avalable to be read in the serial port
 *********************************************************************************
 */

auto serialDataAvail(int const fd) -> int {
    int result;

    if (ioctl(fd, FIONREAD, &result) == -1) return -1;

    return result;
}


/*
 * serialGetchar:
 *	Get a single character from the serial device.
 *	Note: Zero is a valid character and this function will time-out after
 *	10 seconds.
 *********************************************************************************
 */

auto serialGetchar(int const fd) -> int {
    uint8_t x;

    if (read(fd, &x, 1) != 1) return -1;

    return ((int) x) & 0xFF;
}
