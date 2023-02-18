/*
 * cli_options.h - A simple HTTP and HTTPS server implementation
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
 * This file contains the definition of the command-line options.
 */

#ifndef CLI_OPTIONS_H
#define CLI_OPTIONS_H

#include <getopt.h>
#include <string>

/**
 * @struct Options
 *
 * Defines the command-line options that can be parsed by the application
 */
struct Options
{
	bool help_enabled = false;
	bool recorder_enabled = false;
	std::string cert_file;
	std::string cert_key;
	std::string address;
	std::string port;
};

/**
 * Parses the command-line options passed to the application
 *
 * @param argc The number of command-line arguments
 * @param argv An array of pointers to the command-line arguments
 *
 * @return Options Struct containing the parsed options
 */
Options parse_options(int argc, char* argv[]);

/**
 * Prints a help message detailing the usage and options of the application
 *
 * @param program_name The name of the program (typically argv[0])
 *
 * return void
 */
void usage(const char* program_name);

#endif /* CLI_OPTIONS_H */
