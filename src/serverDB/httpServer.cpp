#include "serverDB/__httpServer.hpp"
#include "serverDB/httpServer.hpp"
#include "handlers/http.hpp"


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
			//filename.erase(0, 1);
			pathToDoc.append(filename);
		}
		FileHandler fileHandler(pathToDoc);

		if (!fileHandler.isOpen()) {
			http::write(peer, errorResponse());
			peer.killConn();
			fileHandler.close();
			return;
		}

		/* File ext*/
		std::string fileExt = filename.substr(filename.rfind(".") + 1);
		
		/* file is Opened */
		HeaderHandler headerHandler;
		/* +4 for  "\r\n\r\n" */
		headerHandler.setContentLength(fileHandler.getSize() + 4);
		headerHandler.setConnection("closed");
		headerHandler.setContentType(fileExt);
		std::string header = headerHandler.getHeader();
		http::write(peer, header);

		std::string tmp;
		while (!fileHandler.eof()) {
			tmp.clear();
			tmp = fileHandler.getLine() + "\n";
			http::write(peer, tmp);
		}
		/* Send these characters to mark the end of the http transfer. */
		http::write(peer, "\r\n\r\n"); 
		fileHandler.close();
	}
	peer.killConn();

}