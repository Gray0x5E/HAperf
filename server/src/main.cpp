#include <iostream>
#include <thread>
#include <memory>
#include "HttpServer.h"

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
			std::unique_ptr<HttpServer> https_server(new HttpServer("::", "443", true, "server.crt", "server.key"));
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
