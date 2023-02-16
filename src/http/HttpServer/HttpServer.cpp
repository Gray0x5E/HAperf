/*
 * HttpServer.cpp - A simple HTTP and HTTPS server implementation
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
 * This file contains the implementation of the HttpServer class.
 */

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
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <thread>
#include <stdexcept>
#include "HttpServer.h"

/**
 * HttpServer constructor
 *
 * Initializes an HttpServer instance with the specified address, port, certificate file, and key file.
 * If a certificate and key file are specified, SSL is enabled and an SSL context is created.
 *
 * @param const char* address The IP address to listen on
 * @param const char* port The port number to listen on
 * @param const std::string& cert_file The path to the certificate file, if SSL is enabled
 * @param const std::string& key_file The path to the private key file, if SSL is enabled
 *
 * @return void
 */
HttpServer::HttpServer(const char* address, const char* port, const std::string& cert_file, const std::string& key_file)
	: address_(address),
	  port_(port),
	  use_ipv6_(false),
	  cert_file_(cert_file),
	  key_file_(key_file),
	  use_ssl_(!cert_file.empty() && !key_file.empty()),
	  server_fd_(-1),
	  ctx_(nullptr)
{
	// Initialize SSL library and create SSL context if SSL is enabled
	if (use_ssl_)
	{
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
}

/**
 * HttpServer destructor
 *
 * Cleans up the SSL library if SSL is enabled
 *
 * @return void
 */
HttpServer::~HttpServer()
{
	// Clean up SSL library if SSL is enabled
	if (use_ssl_)
	{
		EVP_cleanup();
	}
}

/**
 * Runs the HTTP server, listening for incoming connections and spawning threads to handle them
 *
 * @return void
 *
 * @throws std::runtime_error If an error occurs while running the server
 */
void HttpServer::run()
{
	// Use getaddrinfo to get address information for the specified address and port.
	// The resulting address information is used to create and bind the server socket.
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

		if (use_ssl_)
		{
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
		else
		{
			// Spawn a new thread to handle the unencrypted connection
			std::thread([this, client_fd]() {
				handle_request(client_fd);
			}).detach();
		}
	}
}

/**
 * Returns a formatted string representing the current time
 *
 * @return std::string A string in the format "YYYY-MM-DD HH:MM:SS"
 */
std::string HttpServer::get_formatted_time()
{
	// Get the current time
	auto now = std::chrono::system_clock::now();
	std::time_t current_time = std::chrono::system_clock::to_time_t(now);

	// Format the time string
	std::ostringstream oss;
	oss << std::put_time(std::localtime(&current_time), "%Y-%m-%d %H:%M:%S");

	return oss.str();
}

/**
 * Handles an incoming request on the specified client socket file descriptor
 *
 * @param int client_fd The client socket file descriptor
 *
 * @return void
 */
void HttpServer::handle_request(int client_fd)
{
	char buffer[1024] = {0};
	int valread = read(client_fd, buffer, 1024);

	if (valread == -1)
	{
		perror("read");
		return;
	}

	// Get the current time
	std::string current_time = get_formatted_time();

	// Send the response
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
	response += current_time + " - received:\n\n";
	response += buffer;

	write(client_fd, response.c_str(), response.size());
	close(client_fd);
}


/**
 * Handles an incoming SSL request on the specified SSL socket
 *
 * @param SSL* ssl The SSL socket representing the encrypted client connection
 *
 * @return void
 */
void HttpServer::handle_request_ssl(SSL* ssl)
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

/**
 * Create a new socket for the HttpServer to listen on
 *
 * @param const char* address The IP address to bind to
 * @param const char* port The port number to bind to
 * @param bool use_ipv6 Whether to use IPv6 or IPv4
 *
 * @return The file descriptor for the created socket
 */
int HttpServer::create_socket(const char* address, const char* port, bool use_ipv6)
{
	// Create a socket with the specified IP version and protocol
	int socket_fd = -1;
	if (use_ipv6)
	{
		socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
	}
	else
	{
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	}

	if (socket_fd == -1)
	{
		perror("socket");
		throw std::runtime_error("Failed to create socket");
	}

	return socket_fd;
}

/**
 * Sets socket options to allow reusing the address to avoid "Address already in use" error
 *
 * @param int socket_fd The socket file descriptor
 *
 * @return void
 *
 * @throws std::runtime_error if setting the socket options fails
 */
void HttpServer::set_socket_options(int socket_fd)
{
	int optval = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		perror("setsockopt");
		throw std::runtime_error("Failed to set socket options");
	}
}

/**
 * Binds a socket to the specified address and port
 *
 * @param int socket_fd The socket file descriptor
 * @param const struct sockaddr* address The socket address
 * @param socklen_t address_size The size of the socket address
 *
 * @return void
 *
 * @throws std::runtime_error If binding the socket fails
 */
void HttpServer::bind_socket(int socket_fd, const struct sockaddr* address, socklen_t address_size)
{
	if (bind(socket_fd, address, address_size) == -1)
	{
		perror("bind");
		throw std::runtime_error("Failed to bind socket");
	}
}

/**
 * Listens on the specified socket file descriptor
 *
 * @param int socket_fd The socket file descriptor
 *
 * @return void
 *
 * @throws std::runtime_error If an error occurs while listening on the socket
 */
void HttpServer::listen_on_socket(int socket_fd)
{
	if (listen(socket_fd, SOMAXCONN) == -1)
	{
		perror("listen");
		throw std::runtime_error("Failed to listen on socket");
	}
}
