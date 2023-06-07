#define __httpConfig
#include "serverDB/httpServer.hpp"

//std::string configFileName = "C:\\Users\\sakabdel\\httpServerConfig.cfg";
//std::string configFileName = ".\\httpServerConfig.cfg";
std::ifstream configStream(configFile);


HttpServerConfig httpServerConfig(configStream);

void serverDB::httpServer(net::TCPpeer peer) {
	int inStatus, outStatus;
	std::string rawData;
	std::cout << httpServerConfig.getDocumentRoot() << std::endl;
	if (inStatus = http::read(peer, rawData)) {
		http::requestParser reqData(rawData);
		std::cout << "[+] request: " << reqData.url_or_host << std::endl;
		outStatus = http::write(peer, TEST_OK_200);
	}

	peer.killConn();

}
