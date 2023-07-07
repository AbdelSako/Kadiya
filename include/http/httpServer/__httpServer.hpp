#ifndef __httpServer
#define __httpServer
#include <iostream>
#include <fstream>
#include <filesystem>

#define PROJECT_DIR "../../../"
#define SERVER_CONFIG_FILE (std::string)"httpServerConfig.cfg"


#define DOCUMENT_ROOT (std::string)"Document_Root"
#define MAX_HOST (std::string)"Max_Host"

class configKeys {
public:
	static std::string getDocument_Root(void) { return DOCUMENT_ROOT; }
	static std::string getMax_Host(void) { return "Max_Host"; }
};

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
			std::cout << "[+] Config Success.\n";
		}
		else {
			std::cout << "[*] Server Config File Not Found.\n";
			throw;
		}
	}

	std::string getDocument_Root(void) {
		return this->Document_Root;
	}
};
#endif