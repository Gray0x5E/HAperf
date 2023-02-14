#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <vector>
#include <thread>

class HttpServer
{
	public:
		HttpServer(const std::string& address, const std::string& port);
		void start();

		~HttpServer();
	private:
		void handle_request(int socket);

		std::string address;
		std::string port;
		std::vector<std::thread> threads;
};

#endif // HTTP_SERVER_H
