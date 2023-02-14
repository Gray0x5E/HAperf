#include "HttpServer.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

HttpServer::HttpServer(const std::string& address, const std::string& port)
	: address(address), port(port)
{}

void HttpServer::start()
{
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int status;
	if ((status = getaddrinfo(this->address.c_str(), this->port.c_str(), &hints, &res)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return;
	}

	int server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (server_fd < 0)
	{
		perror("socket");
		return;
	}

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		perror("setsockopt");
		return;
	}

	if (bind(server_fd, res->ai_addr, res->ai_addrlen) < 0)
	{
		perror("bind");
		return;
	}

	if (listen(server_fd, SOMAXCONN) < 0)
	{
		perror("listen");
		return;
	}

	while (true)
	{
		int client_fd = accept(server_fd, nullptr, nullptr);
		if (client_fd < 0)
		{
			perror("accept");
			continue;
		}

		std::thread([this, client_fd]()
		{
			handle_request(client_fd);
		}).detach();
	}
}

void HttpServer::handle_request(int socket)
{
	const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];
	int valread = read(socket, buffer, BUFFER_SIZE);
	if (valread < 0)
	{
		perror("read");
		return;
	}

	std::string response_data = "HTTP/1.1 200 OK\r\n"
								"Content-Type: text/plain\r\n";

	std::string request(buffer, valread);
	std::string received = "I received: " + request;
	response_data += "Content-Length: " + std::to_string(received.size()) + "\r\n";
	response_data += "\r\n" + received;

	size_t total_sent = 0;
	const char* response = response_data.c_str();
	size_t response_size = response_data.size();
	while (total_sent < response_size)
	{
		int sent = send(socket, response + total_sent, response_size - total_sent, 0);
		if (sent < 0)
		{
			perror("send");
			break;
		}
		total_sent += sent;
	}

	close(socket);
}

HttpServer::~HttpServer()
{
	for (auto& thread : threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
}
