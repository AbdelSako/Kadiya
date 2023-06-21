
#define __httpConfig
#include "serverDB/httpServer.hpp"

std::ifstream configStream(configFile);
HttpServerConfig httpServerConfig(configStream);

const std::string DOC_ROOT(httpServerConfig.getDocument_Root());

std::ifstream fileStreamToServe;
std::string pathToDoc(DOC_ROOT);


char outBuf[512];

void serverDB::httpServer(std::shared_ptr<net::TCPserver> server, net::TCPpeer peer) {
	int inStatus, outStatus;
	std::string rawData;
	std::string buffer;
	if (inStatus = http::read(peer, rawData)) {
		http::requestParser reqData(rawData);
		std::cout << "[+] request: " << reqData.url_or_host << std::endl;
		if (reqData.url_or_host == "/") {
			pathToDoc.append("index.html");
		}
		if(!fileStreamToServe.is_open())
			fileStreamToServe.open(pathToDoc);

		if (fileStreamToServe.is_open()) {
			rawData.clear();
			while (std::getline(fileStreamToServe, buffer)) {
				rawData.append(buffer);
			}
			//fileStreamToServe.close();
			outStatus = http::write(peer, TEST_OK_200+rawData+"\r\n\r\n");
		}
		else {
			std::cout << "[*] I didn't get the file\n";
			outStatus = http::write(peer, TEST_OK_500 + INTERNAL_ERROR);
		}
		//outStatus = http::write(peer, TEST_OK_200);
	}

	peer.killConn();

}
