/*
 * functions.h - A simple HTTP and HTTPS server implementation
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
 * This file declares various functions used within the HAperf application.
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/**
 * Outputs a verbose message to the console, including a timestamp
 *
 * @param message The message to output.
 * @param ... The optional arguments to format into the message.
 *
 * @return void
 */
void debug(const char* message, ...);

#endif /* FUNCTIONS_H */
