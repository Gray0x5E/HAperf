/*
 * cli_options.cpp - A simple HTTP and HTTPS server implementation
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
 * This file implements the command-line options parsing logic.
 */

#include <iostream>
#include "cli_options.h"
#include "app_constants.h"

/**
 * Parses the command-line options passed to the application
 *
 * @param argc The number of command-line arguments
 * @param argv An array of pointers to the command-line arguments
 * @param[out] options The options struct to be populated
 * @param[out] commands The commands struct to be populated
 *
 * @return options Struct containing the parsed options
 */
Options parse_options(int argc, char* argv[], Options& options, Commands& commands)
{
	int opt;
	Options opts;

	// Long options
	const option long_options[] = {
		{"help", no_argument, nullptr, 'h'},
		{"version", no_argument, nullptr, 'V'},
		{"verbose", no_argument, nullptr, 'v'},
		{"cert-file", required_argument, nullptr, 'c'},
		{"cert-key", required_argument, nullptr, 'k'},
		{"address", required_argument, nullptr, 'a'},
		{"port", required_argument, nullptr, 'p'},
		{nullptr, 0, nullptr, 0}
	};

	while ((opt = getopt_long(argc, argv, "hVvc:k:a:p:", long_options, nullptr)) != -1)
	{
		switch (opt)
		{
			case 'h':
				print_usage(argv[0]);
				exit(0);
			case 'V':
				print_version();
				exit(0);
			case 'v':
				opts.verbose = true;
				break;
			case 'c':
				opts.cert_file = optarg;
				break;
			case 'k':
				opts.cert_key = optarg;
				break;
			case 'a':
				opts.address = optarg;
				break;
			case 'p':
				opts.port = optarg;
				break;
			default:
				break;
		}
	}

	// Parse commands
	if (optind < argc)
	{
		std::string command = argv[optind];
		if (command == "record")
		{
			commands.record = true;
		}
		else if (command == "replay")
		{
			commands.replay = true;
		}
	}

	return opts;
}

/**
 * Prints a help message detailing the usage and options of the application
 *
 * @param const char* program_name The name of the program (typically argv[0])
 *
 * return void
 */
void print_usage(const char* program_name)
{
	std::cerr

	<< "\033[1mDescription:\033[0m\n"
	<< "\n"
	<< "  This program records or replays HTTP data.\n"
	<< "\n"
	<< "  To record data, use the \"record\" command with the required certificate file and certificate key options.\n"
	<< "  You may also provide an optional IP address and port number to listen on.\n"
	<< "\n"
	<< "  To replay data (which is currently a work in progress), use the \"replay\" command.\n"
	<< "  This command currently has no options.\n"
	<< "\n"

	<< "\033[1mUsage:\033[0m\n"
	<< "\n"
	<< "  " << program_name << " [--help] [--version]\n"
	<< "  " << program_name << " record --cert-file=<cert_file> --cert-key=<cert_key> [--address=<address>] [--port=<port>] [--verbose]\n"
	<< "  " << program_name << " replay\n"
	<< "\n"

	<< "\033[1mCommands:\033[0m\n"
	<< "\n"
	<< "  record    Record data\n"
	<< "  replay    Replay data (work in progress)\n"
	<< "\n"

	<< "\033[1mOptions:\033[0m\n"
	<< "\n"
	<< "  --help, -h                                 Show this help message and exit\n"
	<< "  --version, -V                              Information about this software version\n"
	<< "  --verbose, -v                              Show more info (for supported commands)\n"
	<< "  --cert-file=<cert_file>, -c <cert_file>    Path to certificate file (required)\n"
	<< "  --cert-key=<cert_key>, -k <cert_key>       Path to certificate key (required)\n"
	<< "  --address=<address>, -a <address>          IP address to record (default: ::)\n"
	<< "  --port=<port>, -p <port>                   Port number to record (default: 80)\n"
	<< "\n"

	<< "\033[1mExamples:\033[0m\n"
	<< "\n"
	<< "  Show this help message:\n"
	<< "      " << program_name << " --help\n"
	<< "\n"
	<< "  Show information about this build of the application:\n"
	<< "      " << program_name << " --help\n"
	<< "\n"
	<< "  To record data on IP address \"192.168.1.2\" and port number \"8080\", with certificate file \"server.crt\" and certificate key \"server.key\":\n"
	<< "      " << program_name << " record -c server.crt -k server.key -a 192.168.1.2 -p 8080\n"
	<< "\n"
	<< "  To record data with certificate file \"server.crt\" and certificate key \"server.key\", using default IP address and port number:\n"
	<< "      " << program_name << " record -c server.crt -k server.key\n"

	<< "\n";
}

void print_version()
{
	std::cerr
	<< APP_NAME << " version " << VERSION
	<< " (built on " __DATE__ "). "
	<< "Copyright " << YEAR << " " << COPYRIGHT << "."
	<< "\n";
}
