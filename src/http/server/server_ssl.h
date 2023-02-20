/*
 * server_ssl.h - A simple HTTP and HTTPS server implementation
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
 * This file contains the class definition for the HTTP::ServerSSL class.
 */

#include "config.h"

#if SSL_SUPPORT == 1

#ifndef SERVER_SSL_H
#define SERVER_SSL_H

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
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "server.h"

/**
 * @namespace HTTP
 * This namespace contains a variety of classes and functions for working with HTTP
 */
namespace HTTP
{

	/**
	 * @brief Overloads the HTTP server implementation with encryption
	 *
	 * This class provides a way to create and run an HTTPS server, serving
	 * web pages and resources to clients that connect to it over the network
	 */
	class ServerSSL : public Server
	{
		public:
			/**
			 * Construct an instance of ServerSSL that listens on the specified address and port with SSL/TLS encryption
			 *
			 * @param const char* address The IP address to listen on, or nullptr to listen on all available addresses
			 * @param const char* port The port to listen on
			 * @param const std::string&cert_file The path to the SSL/TLS certificate file
			 * @param const std::string&key_file The path to the SSL/TLS private key file
			 *
			 * return void
			 */
			ServerSSL(const char* address, const char* port, const std::string& cert_file, const std::string& key_file);

			/**
			 * Destruct the ServerSSL and release any resources
			 */
			virtual ~ServerSSL();

			/**
			 * Run the server and start listening for incoming connections
			 * This method blocks and does not return until the server is stopped
			 *
			 * @note This method can be overridden by child classes.
			 *
			 * return void
			 */
			virtual void run() override;

		protected:

			/**
			 * Handle an incoming encrypted HTTPS request
			 *
			 * @param SSL* ssl The SSL object representing the encrypted client connection
			 *
			 * @return bool
			 */
			void handle_request_ssl(SSL* ssl);

			/**
			 * @var std::string The path to the SSL/TLS certificate file
			 */
			std::string cert_file_;

			/**
			 * @var std::string The path to the SSL/TLS private key file
			 */
			std::string key_file_;

			/**
			 * @var SSL_CTX* The SSL/TLS context for the server
			 */
			SSL_CTX* ctx_;
	};
}

#endif /* SERVER_SSL_H */
#endif /* SSL_SUPPORT */
