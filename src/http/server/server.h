/*
 * server.h - A simple HTTP and HTTPS server implementation
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
 * This file contains the class definition for the HTTP::Server.
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <stdexcept>
#include "settings.h"

/**
 * @namespace HTTP
 * This namespace contains a variety of classes and functions for working with HTTP
 */
namespace HTTP
{

	/**
	 * @brief A simple HTTP server implementation
	 *
	 * This class provides a way to create and run an HTTP server, serving
	 * web pages and resources to clients that connect to it over the network
	 */
	class Server
	{
		public:
			/**
			 * Construct an Server that listens on the specified address and port
			 *
			 * @param const char* address The IP address to listen on, or nullptr to listen on all available addresses
			 * @param const char* port The port to listen on
			 *
			 * return void
			 */
			Server(const char* address, const char* port);

			/**
			 * Destruct the Server and release any resources
			 */
			~Server();

			/**
			 * Run the Server and start listening for incoming connections.
			 * This method blocks and does not return until the server is stopped.
			 *
			 * @note This method can be overridden by child classes.
			 *
			 * return void
			 */
			virtual void run();

		protected:

			/**
			 * Returns a formatted string representing the current time
			 *
			 * @return std::string A string in the format "YYYY-MM-DD HH:MM:SS"
			 */
			std::string get_formatted_time();

			/**
			 * Handle an incoming unencrypted HTTP request
			 *
			 * @param int client_fd The file descriptor for the connected client socket
			 *
			 * @return void
			 */
			void handle_request(int client_fd);

			/**
			 * Create a new socket for the Server to listen on
			 *
			 * @param const char* address The IP address to bind to
			 * @param const char* port The port number to bind to
			 * @param bool use_ipv6 Whether to use IPv6 or IPv4
			 *
			 * @return int The file descriptor for the created socket.
			 */
			int create_socket(const char* address, const char* port, bool use_ipv6);

			/**
			 * Set socket options for the Server socket.
			 *
			 * @param int socket_fd The file descriptor for the socket to set options on.
			 *
			 * @return void
			 */
			void set_socket_options(int socket_fd);

			/**
			 * Bind the Server socket to the specified address and port
			 *
			 * @param int socket_fd The file descriptor for the socket to bind
			 * @param const struct sockaddr* address The address to bind to
			 * @param socklen_t address_size The size of the address structure
			 *
			 * @return void
			 */
			void bind_socket(int socket_fd, const struct sockaddr* address, socklen_t address_size);

			/**
			 * Listen for incoming connections on the Server socket.
			 *
			 * @param int socket_fd The file descriptor for the socket to listen on.
			 *
			 * @return void
			 */
			void listen_on_socket(int socket_fd);

			/**
			 * @var const char* The IP address to listen on, or nullptr to listen on all available addresses.
			 */
			const char* address_;

			/**
			 * @var const char* The port number to listen on.
			 */
			const char* port_;

			/**
			 * @var bool Whether to use IPv6 or IPv4
			 */
			bool use_ipv6_;

			/**
			 * @var int The file descriptor for the server socket
			 */
			int server_fd_;
	};
}

#endif /* HTTP_SERVER_H */
