/*
 * functions.cpp - A simple HTTP and HTTPS server implementation
 *
 * Copyright (C) 2023 HAperf.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * This file contains various functions used within the HAperf application.
 */

#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdarg>
#include "functions.h"

/**
 * Output a message with a timestamp in verbose mode
 *
 * This function outputs a message to the console with a timestamp, but only if
 * verbose mode is enabled. The message can be formatted using a printf-style
 * format string and variable arguments. The timestamp format is "YYYY-MM-DD
 * HH:MM:SS". This function can be used to provide informative output to the
 * user during program execution.
 *
 * Example usage:
 * verbose("Starting HTTP server on %s:%d", options.address.c_str(), options.port);
 *
 * @param message A printf-style format string for the message
 * @param ... Variable arguments to be used with the format string
 *
 * @return void
 */
void debug(const char* message, ...)
{
	if (!verbose)
	{
		return;
	}

    time_t now = std::time(nullptr);
    char timestamp[20];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::va_list args;
    va_start(args, message);
    std::vprintf((std::string(timestamp) + ": " + message + "\n").c_str(), args);
    va_end(args);
}
