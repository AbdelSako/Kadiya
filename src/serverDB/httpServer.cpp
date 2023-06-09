#define __httpConfig
#include "serverDB/httpServer.hpp"

std::ifstream configStream(configFile);
HttpServerConfig httpServerConfig(configStream);

std::string docRootPath(httpServerConfig.getDocument_Root());

std::ifstream fileStreamToServe;
std::string fileToServe(docRootPath);


void serverDB::httpServer(net::TCPpeer peer) {
	int inStatus, outStatus;
	std::string rawData;
	std::cout << httpServerConfig.getDocument_Root() << std::endl;
	if (inStatus = http::read(peer, rawData)) {
		http::requestParser reqData(rawData);
		std::cout << "[+] request: " << reqData.url_or_host << std::endl;
		if (reqData.url_or_host == "/") {
			fileToServe.append("index.html");
			fileStreamToServe.open(fileToServe);
			while (!fileStreamToServe.eof()) {
				rawData.clear();
				fileToServe.
				outStatus = http::write(peer, )
			}
		}
		//outStatus = http::write(peer, TEST_OK_200);
	}

	peer.killConn();

}
