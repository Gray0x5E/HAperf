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

#include "cli_options.h"
#include <iostream>

/**
 * Parses the command-line options passed to the application
 *
 * @param argc The number of command-line arguments
 * @param argv An array of pointers to the command-line arguments
 *
 * @return Options Struct containing the parsed options
 */
Options parse_options(int argc, char* argv[])
{
	int opt;
	Options options;

	// Long options
	const option long_options[] = {
		{"help", no_argument, nullptr, 'h'},
		{"recorder", no_argument, nullptr, 'r'},
		{"cert-file", required_argument, nullptr, 'c'},
		{"cert-key", required_argument, nullptr, 'k'},
		{"address", required_argument, nullptr, 'a'},
		{"port", required_argument, nullptr, 'p'},
		{nullptr, 0, nullptr, 0}
	};

	while ((opt = getopt_long(argc, argv, "hc:k:a:p:d:m:M:r", long_options, nullptr)) != -1)
	{
		switch (opt)
		{
			case 'h':
				options.help_enabled = true;
				break;
			case 'r':
				options.recorder_enabled = true;
				break;
			case 'c':
				options.cert_file = optarg;
				break;
			case 'k':
				options.cert_key = optarg;
				break;
			case 'a':
				options.address = optarg;
				break;
			case 'p':
				options.port = optarg;
				break;
			case '?':
				usage(argv[0]);
				exit(1);
				break;
			default:
				break;
		}
	}

	return options;
}

/**
 * Prints a help message detailing the usage and options of the application
 *
 * @param program_name The name of the program (typically argv[0])
 *
 * return void
 */
void usage(const char* program_name)
{
	std::cerr << "\n"

	<< "\033[1m  Usage:\033[0m\n"
	<< "\n"
	<< "    " << program_name << " [--help|-h]\n"
	<< "    " << program_name << " record --cert-file=<cert_file> --cert-key=<cert_key> [--address=<address>] [--port=<port>]\n"
	<< "    " << program_name << " replay\n"
	<< "\n"

	<< "\033[1m  Commands:\033[0m\n"
	<< "\n"
	<< "    record    Record data\n"
	<< "    replay    Replay data (work in progress)\n"
	<< "\n"

	<< "\033[1m  Options:\033[0m\n"
	<< "\n"
	<< "    --help, -h                                 Show this help message and exit\n"
	<< "    --cert-file=<cert_file>, -c <cert_file>    Path to certificate file (required)\n"
	<< "    --cert-key=<cert_key>, -k <cert_key>       Path to certificate key (required)\n"
	<< "    --address=<address>, -a <address>          IP address to record (default: ::)\n"
	<< "    --port=<port>, -p <port>                   Port number to record (default: 80)\n"
	<< "\n"

	<< "\033[1m  Description:\033[0m\n"
	<< "\n"
	<< "    This program records or replays HTTP data.\n"
	<< "\n"
	<< "    To record data, use the \"record\" command with the required certificate file and certificate key options.\n"
	<< "    You may also provide an optional IP address and port number to listen on.\n"
	<< "\n"
	<< "    To replay data (which is currently a work in progress), use the \"replay\" command.\n"
	<< "    This command currently has no options.\n"
	<< "\n"

	<< "\033[1m  Examples:\033[0m\n"
	<< "\n"
	<< "    To record data on IP address \"192.168.1.2\" and port number \"8080\", with certificate file \"server.crt\" and certificate key \"server.key\":\n"
	<< "        " << program_name << " record -c cert.pem -k key.pem -a 192.168.1.1 -p 9000\n"
	<< "\n"
	<< "    To record data with certificate file \"cert.pem\" and certificate key \"key.pem\", using default IP address and port number:\n"
	<< "        " << program_name << " record -c cert.pem -k key.pem\n"
	<< "\n"
	<< "    To show the help message:\n"
	<< "        " << program_name << " --help\n"
	<< "\n";
}
