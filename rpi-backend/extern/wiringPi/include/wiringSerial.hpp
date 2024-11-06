/*
 * wiringSerial.h:
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
#pragma once

#include <string>

extern int serialOpen(std::string const& device, int baud);
extern int serialOpen(char const* device, int baud);
extern void serialClose(int fd);
extern void serialFlush(int fd);
extern auto serialPutchar(int fd, unsigned char c) -> bool;
extern auto serialPutbuffer(int fd, char const* buffer, size_t length) -> bool;
extern auto serialPuts(int fd, char const* c_str) -> bool;
extern void serialPrintf(int fd, char const* message, ...);
extern int serialDataAvail(int fd);
extern int serialGetchar(int fd);
