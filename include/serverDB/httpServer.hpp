#include "TCPserver.hpp"
#include "handlers/http.hpp"
#include <fstream>
#include <filesystem>


#ifndef __httpServer
#define __httpServer



#ifdef __httpConfig

std::string TEST_OK_200 = "HTTP/1.1 200 OK\r\n"
"server: N'Nimba 1.0\r\n"
"content-length: 122\r\n"
"content-type: text/html\r\n"
"connection: closed\r\n\r\n";

std::string TEST_OK_500 = "HTTP/1.1 500 OK\r\n"
"server: N'Nimba 1.0\r\n"
//"content-length: 48\r\n"
"content-type: text/html\r\n"
"connection: closed\r\n\r\n";

std::string INTERNAL_ERROR = "<html><h1> Some error happened while"
"N'nimba was trying to open the document</h1></html>\r\n\r\n";

class configKeys {
public:
	static std::string getDocument_Root(void) { return "Document_Root"; }
	static std::string getMax_Host(void) { return "Max_Host"; }
};

//std::string configFile("C:/Users/tech/source/repos/SOCKNET_API/httpServerConfig.cfg");
std::string configFile("../../../httpServerConfig.cfg");

class HttpServerConfig {
private:
	std::string Document_Root;
	int Max_Host;
	std::string tmp;
public:
	HttpServerConfig(std::ifstream& configFile) {
		if (configFile.is_open()) {
			while (!configFile.eof()) {
				configFile >> tmp;

				if (tmp == configKeys::getDocument_Root())
					configFile >> this->Document_Root;
				else if (tmp == configKeys::getMax_Host())
					configFile >> this->Max_Host;
			}
		}
		else {
			std::cout << "[*] File not found.\n";
		}
	}

	std::string getDocument_Root(void) {
		return this->Document_Root;
	}
};

class HeaderHandler {
	const std::string server = "N'Nimba";
	std::string
		contentType,
		connection;
	unsigned int contentLength;
public:
	void setContentLength(unsigned int size) {
		this->contentLength = size;
	}
	void setContentType(const std::string type) {
		this->contentType = type;
	}
	void setConnection(const std::string conn) {
		this->connection = conn;
	}

	std::string getHead(void) {
		std::string head;
		head.append("HTTP/1.1 200 OK\r\n");
		head.append("server: " + server + "\r\n");
		head.append("content-length: " + std::to_string(contentLength) + "\r\n");
		head.append("content-type: " + contentType + "\r\n");
		head.append("connection: " + connection + "\r\n");
		return head.append("\r\n");
	}
};

#endif

namespace serverDB {
	/* This is our HTTP server. In the mainHttpServer.cpp file, the serverCode() method pointer of
		a TCPserver object points to this function. The argument TCPpeer class encapsulates a 
		valid connection. */
	void httpServer(std::shared_ptr<net::TCPserver> server, net::TCPpeer peer);
}
#endif