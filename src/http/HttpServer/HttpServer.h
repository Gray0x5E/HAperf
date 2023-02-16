/*
 * HttpServer.h - A simple HTTP and HTTPS server implementation
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
 * This file contains the class definition for HttpServer.
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <openssl/ssl.h>
#include <sys/socket.h>

/**
 * @brief A simple HTTP and HTTPS server implementation.
 *
 * This class provides a way to create and run an HTTP or HTTPS server, serving
 * web pages and resources to clients that connect to it over the network.
 */
class HttpServer
{
	public:
		/**
		 * Construct an HttpServer that listens on the specified address and port with SSL/TLS encryption.
		 *
		 * @param const char* address The IP address to listen on, or nullptr to listen on all available addresses
		 * @param const char* port The port to listen on
		 * @param const std::string&cert_file The path to the SSL/TLS certificate file
		 * @param const std::string&key_file The path to the SSL/TLS private key file
		 *
		 * return void
		 */
		HttpServer(const char* address, const char* port, const std::string& cert_file = "", const std::string& key_file = "");

		/**
		 * Destruct the HttpServer and release any resources
		 */
		~HttpServer();

		/**
		 * Run the HttpServer and start listening for incoming connections.
		 * This method blocks and does not return until the server is stopped.
		 *
		 * return void
		 */
		void run();

	private:

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
		 * Handle an incoming encrypted HTTPS request
		 *
		 * @param SSL* ssl The SSL object representing the encrypted client connection
		 *
		 * @return bool
		 */
		void handle_request_ssl(SSL* ssl);

		/**
		 * Create a new socket for the HttpServer to listen on
		 *
		 * @param const char* address The IP address to bind to
		 * @param const char* port The port number to bind to
		 * @param bool use_ipv6 Whether to use IPv6 or IPv4
		 *
		 * @return int The file descriptor for the created socket.
		 */
		int create_socket(const char* address, const char* port, bool use_ipv6);

		/**
		 * Set socket options for the HttpServer socket.
		 *
		 * @param int socket_fd The file descriptor for the socket to set options on.
		 *
		 * @return void
		 */
		void set_socket_options(int socket_fd);

		/**
		 * Bind the HttpServer socket to the specified address and port
		 *
		 * @param int socket_fd The file descriptor for the socket to bind
		 * @param const struct sockaddr* address The address to bind to
		 * @param socklen_t address_size The size of the address structure
		 *
		 * @return void
		 */
		void bind_socket(int socket_fd, const struct sockaddr* address, socklen_t address_size);

		/**
		 * Listen for incoming connections on the HttpServer socket.
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
		 * @var bool Whether to use SSL/TLS encryption
		 */
		bool use_ssl_;

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

		/**
		 * @var int The file descriptor for the server socket
		 */
		int server_fd_;
};

#endif
