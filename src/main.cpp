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
#include "settings.h"
#include "functions.h"
#include "cli_arguments.h"
#include "server.h"

/**
 * @var bool verbose Whether verbose mode is enabled or not. Defaults to false.
 */
bool verbose = false;

/**
 * The main function of the HAperf command-line application
 *
 * @param argc The number of command-line arguments
 * @param argv An array of pointers to the command-line arguments
 *
 * @return int Status code indicating the result of the operation
 */
int main(int argc, char* argv[])
{
	Options opts;
	Commands cmds;
	parse_arguments(argc, argv, opts, cmds);

	// Set verbose mode
	if (opts.verbose)
	{
		verbose = true;
	}

	// Check if record options are valid
	if (cmds.record)
	{
		if (opts.cert_file.empty() || opts.cert_key.empty())
		{
			std::cerr << "\033[1mError:\033[0m Certificate file and key are required to run this command.\n\n";
			exit(1);
		}
	}

	// If we don't have a main activity chosen, then there's not much to do
	// This check if just until we support other commands
	if (!cmds.record)
	{
		std::cout << "Invalid request. For information on usage: " << argv[0] << " --help" << std::endl;
		exit(1);
	}

	// Determine address and port to use
	std::string address_to_use = opts.address.empty() ? "::" : opts.address;
	std::string port_to_use = opts.port.empty() ? "80" : opts.port;
	std::string cert_to_use = opts.cert_file.empty() ? "ssl/server.crt" : opts.cert_file;
	std::string key_to_use = opts.cert_key.empty() ? "ssl/server.key" : opts.cert_key;

	try
	{
		// Start HTTP server on the determined address and port in its own thread
		debug("Starting HTTP server on port %s", port_to_use.c_str());
		std::thread http_thread([=](){
			std::unique_ptr<HTTP::Server> http_server(new HTTP::Server(address_to_use.c_str(), port_to_use.c_str()));
			http_server->run();
		});

		// Start HTTPS server on port 443 in its own thread
		debug("Starting HTTPS server on port %s", port_to_use.c_str());
		std::thread https_thread([=]() {
			std::unique_ptr<HTTP::Server> https_server(new HTTP::Server(address_to_use.c_str(), "443", cert_to_use.c_str(), key_to_use.c_str()));
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
