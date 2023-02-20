/*
 * server_ssl.cpp - A simple HTTP and HTTPS server implementation
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
 * This file contains the implementation of the HTTP::ServerSSL class.
 */

#include "config.h"

#if SSL_SUPPORT == 1

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
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "settings.h"
#include "server_ssl.h"

/**
 * @namespace HTTP
 * This namespace contains a variety of classes and functions for working with HTTP
 */
namespace HTTP
{

	/**
	 * Server constructor
	 *
	 * Initialises an ServerSSL instance with the specified address, port, certificate file, and key file.
	 *
	 * @param const char* address The IP address to listen on
	 * @param const char* port The port number to listen on
	 * @param const std::string& cert_file The path to the certificate file
	 * @param const std::string& key_file The path to the private key file
	 *
	 * @return void
	 */
	ServerSSL::ServerSSL(const char* address, const char* port, const std::string& cert_file, const std::string& key_file)
		: Server(address, port), cert_file_(cert_file), key_file_(key_file)
	{
		// Initialise SSL library and create SSL context if SSL is enabled
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();

		// Create SSL context
		ctx_ = SSL_CTX_new(TLS_server_method());

		// Load server certificate
		if (SSL_CTX_use_certificate_file(ctx_, cert_file_.c_str(), SSL_FILETYPE_PEM) <= 0)
		{
			throw std::runtime_error("Failed to load server certificate");
		}

		// Load private key
		if (SSL_CTX_use_PrivateKey_file(ctx_, key_file_.c_str(), SSL_FILETYPE_PEM) <= 0)
		{
			throw std::runtime_error("Failed to load server private key");
		}

		// Check the private key
		if (!SSL_CTX_check_private_key(ctx_))
		{
			throw std::runtime_error("Server private key does not match the certificate public key");
		}
	}

	/**
	 * Server destructor
	 *
	 * Cleans up the SSL library if SSL is enabled
	 *
	 * @return void
	 */
	ServerSSL::~ServerSSL()
	{
		// Clean up SSL library if SSL is enabled
		EVP_cleanup();
	}

	/**
	 * Runs the HTTPS server, listening for incoming connections and spawning threads to handle them
	 *
	 * @return void
	 *
	 * @throws std::runtime_error If an error occurs while running the server
	 */
	void ServerSSL::run()
	{
		// Use getaddrinfo to get address information for the specified address and port
		// The resulting address information is used to create and bind the server socket
		struct addrinfo hints, *res;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		int status = getaddrinfo(address_, port_, &hints, &res);
		if (status != 0)
		{
			throw std::runtime_error("Failed to get address information");
		}

		if (res == nullptr)
		{
			throw std::runtime_error("Failed to get address information");
		}

		use_ipv6_ = (res->ai_family == AF_INET6);

		// Create and bind server socket
		server_fd_ = create_socket(address_, port_, use_ipv6_);
		set_socket_options(server_fd_);
		bind_socket(server_fd_, res->ai_addr, res->ai_addrlen);
		freeaddrinfo(res);

		// Start listening for incoming connections
		listen_on_socket(server_fd_);

		// Accept incoming connections and spawn threads to handle them
		while (true)
		{
			struct sockaddr_storage client_addr;
			socklen_t client_addr_size = sizeof(client_addr);
			int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &client_addr_size);

			if (client_fd == -1)
			{
				perror("accept");
				continue;
			}

			SSL* ssl = SSL_new(ctx_);
			SSL_set_fd(ssl, client_fd);

			if (SSL_accept(ssl) <= 0)
			{
				SSL_free(ssl);
				close(client_fd);
				continue;
			}

			// Spawn a new thread to handle the SSL connection
			std::thread([this, ssl]() {
				handle_request_ssl(ssl);
				SSL_shutdown(ssl);
				SSL_free(ssl);
			}).detach();
		}
	}

	/**
	 * Handles an incoming SSL request on the specified SSL socket
	 *
	 * @param SSL* ssl The SSL socket representing the encrypted client connection
	 *
	 * @return void
	 */
	void ServerSSL::handle_request_ssl(SSL* ssl)
	{
		char buffer[1024] = {0};
		int valread = SSL_read(ssl, buffer, 1024);

		if (valread <= 0)
		{
			return;
		}

		// Get the current time
		std::string current_time = get_formatted_time();

		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
		response += current_time + " - received:\n\n";
		response += buffer;

		// Send response to client
		int bytes_written = SSL_write(ssl, response.c_str(), response.size());

		if (bytes_written <= 0)
		{
			// Handle error
			std::cerr << "Failed to send response to client" << std::endl;
		}

		SSL_shutdown(ssl);
		close(SSL_get_fd(ssl));
	}
}

#endif /* SSL_SUPPORT */
