#ifndef __httpServer
#define __httpServer
#include <iostream>
#include <fstream>
#include <filesystem>

#define DOCUMENT_ROOT (std::string)"Document_Root"
#define MAX_HOST (std::string)"Max_Host"

std::string errorResponse(void) {
	std::string INTERNAL_ERROR = "<html><h1> Some error happened while"
		"N'nimba was trying to open the document</h1></html>\r\n\r\n";

	int contentLength = INTERNAL_ERROR.length();

	std::string TEST_OK_500 = "HTTP/1.1 500 OK\r\n"
		"server: N'Nimba 1.0\r\n"
		"content-length: " + std::to_string(contentLength) + "\r\n"
		"content-type: text/html\r\n"
		"connection: closed\r\n\r\n";

	return TEST_OK_500 + INTERNAL_ERROR;
}

class configKeys {
public:
	static std::string getDocument_Root(void) { return DOCUMENT_ROOT; }
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
#endif