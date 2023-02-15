#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <openssl/ssl.h>
#include <sys/socket.h>

class HttpServer
{
	public:
		HttpServer(const char* address, const char* port, bool use_ssl, const char* cert_file = nullptr, const char* key_file = nullptr);
		HttpServer(const char* address, const char* port);

		~HttpServer();

		void run();

	private:
		void handle_request(int client_fd);
		void handle_request_ssl(SSL* ssl);

		int create_socket(const char* address, const char* port, bool use_ipv6);
		void set_socket_options(int socket_fd);
		void bind_socket(int socket_fd, const struct sockaddr* address, socklen_t address_size);
		void listen_on_socket(int socket_fd);

		const char* address_;
		const char* port_;
		bool use_ipv6_;
		bool use_ssl_;
		std::string cert_file_;
		std::string key_file_;
		SSL_CTX* ctx_;

		int server_fd_;
};

#endif
