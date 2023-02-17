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
#include "http_server.h"

int main()
{
	try
	{
		// Start HTTP server on port 80 in its own thread
		std::thread http_thread([](){
			std::unique_ptr<HttpServer> http_server(new HttpServer("::", "80"));
			http_server->run();
		});

		// Start HTTPS server on port 443 in its own thread
		std::thread https_thread([]() {
			std::unique_ptr<HttpServer> https_server(new HttpServer("::", "443", "ssl/server.crt", "ssl/server.key"));
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
