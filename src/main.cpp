/*
 * main.cpp - A simple HTTP and HTTPS server implementation
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
 * This file contains the main entry point for the HTTP and HTTPS server.
 */

#include <iostream>
#include <thread>
#include <memory>
#include <string>
#include <getopt.h>
#include "http_server.h"

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

int main(int argc, char* argv[])
{
	int opt;
	const char* program_name = argv[0];

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

	bool recorder_enabled = false;
	std::string cert_file;
	std::string cert_key;
	std::string address;
	std::string port;
	std::string directory;
	std::string min_age;
	std::string max_age;

	while ((opt = getopt_long(argc, argv, "hc:k:a:p:d:m:M:r", long_options, nullptr)) != -1)
	{
		switch (opt)
		{
			case 'h':
				usage(program_name);
				exit(0);
			case 'r':
				recorder_enabled = true;
				break;
			case 'c':
				cert_file = optarg;
				break;
			case 'k':
				cert_key = optarg;
				break;
			case 'a':
				address = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case '?': {
				std::string opt_name;
				if (optopt)
				{
					opt_name = static_cast<char>(optopt);
				}
				else
				{
					opt_name = long_options[optind - 1].name;
				}
				break;
			}
			default:
				break;
		}
	}

	// Check if recorder options are valid
	if (recorder_enabled)
	{
		if (cert_file.empty() || cert_key.empty())
		{
			std::cerr << "Error: Certificate file and certificate key are required for recorder\n\n";
			usage(program_name);
			exit(1);
		}
	}

	// If we don't have a main activity chosen, then there's not much to do
	if (!recorder_enabled)
	{
		usage(program_name);
		exit(1);
	}

	// Determine address and port to use
	std::string address_to_use = address.empty() ? "::" : address;
	std::string port_to_use = port.empty() ? "80" : port;
	std::string cert_to_use = cert_file.empty() ? "ssl/server.crt" : cert_file;
	std::string key_to_use = cert_key.empty() ? "ssl/server.key" : cert_key;

	try
	{
		// Start HTTP server on the determined address and port in its own thread
		std::thread http_thread([=](){
			std::unique_ptr<HttpServer> http_server(new HttpServer(address_to_use.c_str(), port_to_use.c_str()));
			http_server->run();
		});

		// Start HTTPS server on port 443 in its own thread
		std::thread https_thread([=]() {
			std::unique_ptr<HttpServer> https_server(new HttpServer(address_to_use.c_str(), "443", cert_to_use.c_str(), key_to_use.c_str()));
			https_server->run();
		});

		// Wait for both threads to finish
		http_thread.join();
		https_thread.join();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error running server: " << e.what() << std::endl;
	}

	return 0;
}
