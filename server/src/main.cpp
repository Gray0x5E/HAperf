#include "HttpServer.h"
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
	HttpServer server("::", "80");
	server.start();
	return 0;
}
