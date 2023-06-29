
#define __httpConfig
#include "serverDB/httpServer.hpp"
#include <filesystem>

void printPath(void) {
	std::cout << std::filesystem::current_path() << '\n'; // (1)
	std::filesystem::current_path(std::filesystem::temp_directory_path()); // (3)
	std::cout << "Current path is " << std::filesystem::current_path() << '\n';
}

std::ifstream configStream(configFile);
HttpServerConfig httpServerConfig(configStream);

//TODO:
const std::string DOC_ROOT(httpServerConfig.getDocument_Root());


void serverDB::httpServer(std::shared_ptr<net::TCPserver> server, net::TCPpeer peer) {
	printPath();
	std::ifstream fileStreamToServe;
	std::string pathToDoc(DOC_ROOT);
	//TODO:
	pathToDoc.insert(9, "source/repos/");
	std::string filename;

	int inStatus, outStatus;
	std::string rawData;
	std::string buffer;
	if (inStatus = http::read(peer, rawData)) {
		/* Dont waste your time if the request is empty. */
		if (rawData.empty()) {
			peer.killConn();
			return;
		}

		http::requestParser reqData(rawData);
		std::cout << "[+] request: " << reqData.url_or_host << std::endl;
		if (reqData.url_or_host == "/") {
			filename = "index.html";
			pathToDoc.append(filename);
		}
		else {
			filename = reqData.url_or_host;
			filename.erase(0, 1);
			pathToDoc.append(filename);
		}
		FileHandler fileHandler(pathToDoc);

		if (!fileHandler.isOpen()) {
			http::write(peer, TEST_OK_500 + INTERNAL_ERROR);
			peer.killConn();
			fileHandler.close();
			return;
		}
		
		/* file is Opened */
		HeaderHandler headerHandler;
		headerHandler.setContentLength(fileHandler.getSize());
		headerHandler.setConnection("closed");
		headerHandler.setContentType("text/html");
		std::string header = headerHandler.getHead();
		http::write(peer, header);
		int fileSize = fileHandler.getSize();
		//std::string bb = fileHandler.getChunk(fileSize);
		std::string tmp;
		//fileHandler.seek(0);
		while (!fileHandler.eof()) {
			tmp.clear();
			tmp = fileHandler.getLine() + "\r\n";
			http::write(peer, tmp);
		}
		//int s = bb.length();
		//http::write(peer, bb);
		//http::write(peer, "\r\n\r\n");
		fileHandler.close();
	}
	peer.killConn();

}
