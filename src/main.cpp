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
#include "cli_options.h"
#include "http_server.h"


int main(int argc, char* argv[])
{
	Options options = parse_options(argc, argv);

	// Check if recorder options are valid
	if (options.recorder_enabled)
	{
		if (options.cert_file.empty() || options.cert_key.empty())
		{
			std::cerr << "Error: Certificate file and certificate key are required for recorder\n\n";
			usage(argv[0]);
			exit(1);
		}
	}

	// If we don't have a main activity chosen, then there's not much to do
	if (!options.recorder_enabled)
	{
		usage(argv[0]);
		exit(1);
	}

	// Determine address and port to use
	std::string address_to_use = options.address.empty() ? "::" : options.address;
	std::string port_to_use = options.port.empty() ? "80" : options.port;
	std::string cert_to_use = options.cert_file.empty() ? "ssl/server.crt" : options.cert_file;
	std::string key_to_use = options.cert_key.empty() ? "ssl/server.key" : options.cert_key;

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
