#include "TCPsocket.hpp"
#include "handlers/http.hpp"
#include <fstream>


#ifndef __httpServer
#define __httpServer

#ifdef __httpConfig
std::string TEST_OK_200 = "HTTP/1.1 200 OK\r\n"
"server: N'Nimba 1.0\r\n"
"content-length: 48\r\n"
"content-type: text/html\r\n"
"connection: closed\r\n\r\n"
"<html><body><h1>Hello, World!</h1></body></html>\r\n\r\n";

static class HttpServerConfig {
private:
	std::string Document_Root;
	std::string tmp;
public:
	HttpServerConfig(std::ifstream& configFile) {
		if (configFile.is_open()) {
			configFile >> tmp >> Document_Root;
			configFile.close();
		}
		else {
			std::cout << "[*] File not found.\n";
		}
	}

	std::string getDocumentRoot(void) {
		return this->Document_Root;
	}
};



#endif

namespace serverDB {
	/* This is our HTTP server. In the mainHttpServer.cpp file, the serverCode() method pointer of
		a TCPserver object points to this function. The argument TCPpeer class encapsulates a 
		valid connection. */
	void httpServer(net::TCPpeer peer);
}
#endif