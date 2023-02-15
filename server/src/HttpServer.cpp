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


HttpServer::HttpServer(const char* address, const char* port, bool use_ssl, const char* cert_file, const char* key_file)
	: address_(address), port_(port), use_ipv6_(false), use_ssl_(use_ssl), cert_file_(cert_file), key_file_(key_file), server_fd_(-1), ctx_(nullptr)
{
	if (use_ssl)
	{
		// Initialize SSL
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();

		// Create SSL context
		ctx_ = SSL_CTX_new(TLS_server_method());

		// Load server certificate and private key
		if (SSL_CTX_use_certificate_file(ctx_, cert_file_.c_str(), SSL_FILETYPE_PEM) <= 0)
		{
			throw std::runtime_error("Failed to load server certificate");
		}

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


HttpServer::HttpServer(const char* address, const char* port)
	: address_(address), port_(port), use_ipv6_(false)
{
}


HttpServer::~HttpServer()
{
	// Clean up SSL library
	EVP_cleanup();
}

void HttpServer::run()
{
	// Determine whether to use IPv6
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

			std::thread([this, ssl]() {
				handle_request_ssl(ssl);
				SSL_shutdown(ssl);
				SSL_free(ssl);
			}).detach();
		}
		else
		{
			std::thread([this, client_fd]() {
				handle_request(client_fd);
			}).detach();
		}
	}
}

void HttpServer::handle_request(int client_fd)
{
	char buffer[1024] = {0};
	int valread = read(client_fd, buffer, 1024);

	if (valread == -1)
	{
		perror("read");
		return;
	}

	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
	response += "I received: ";
	response += buffer;
	write(client_fd, response.c_str(), response.size());
	close(client_fd);
}

void HttpServer::handle_request_ssl(SSL* ssl)
{
	char buffer[1024] = {0};
	int valread = SSL_read(ssl, buffer, 1024);

	if (valread <= 0)
	{
		return;
	}

	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
	response += "I received: ";
	response += buffer;
	SSL_write(ssl, response.c_str(), response.size());
	SSL_shutdown(ssl);
	close(SSL_get_fd(ssl));
}

int HttpServer::create_socket(const char* address, const char* port, bool use_ipv6)
{
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

void HttpServer::set_socket_options(int socket_fd)
{
	int optval = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		perror("setsockopt");
		throw std::runtime_error("Failed to set socket options");
	}
}

void HttpServer::bind_socket(int socket_fd, const struct sockaddr* address, socklen_t address_size)
{
	if (bind(socket_fd, address, address_size) == -1)
	{
		perror("bind");
		throw std::runtime_error("Failed to bind socket");
	}
}

void HttpServer::listen_on_socket(int socket_fd)
{
	if (listen(socket_fd, SOMAXCONN) == -1)
	{
		perror("listen");
		throw std::runtime_error("Failed to listen on socket");
	}
}
