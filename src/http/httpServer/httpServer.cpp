/* This is the Server */
#include "http/http.hpp"
//#include "http/httpServer/httpServerConfig.hpp"
#include "http/httpServer/httpServer.hpp"
#include "helpers.hpp"

std::string DOC_ROOT_DIR = "C:\\Users\\abdel_or7j875\\httpServerHome\\www\\";

/* Function */
void storeData(std::shared_ptr<net::ServerSocket> server, net::HostData* hostData);

int n = 0;

/* This function is called anytime there is a new connection.
 * TODO: I need to re-implement it to be a static function so I can reduce the overhead.
 */
void http::httpServer(std::shared_ptr<net::ServerSocket> server, net::PeerSocket peer) {
	std::ifstream fileStreamToServe;
	std::string filename;
	std::string pathToDoc(DOC_ROOT_DIR);

	int inStatus, outStatus;
	std::string rawData;
	if (inStatus = http::read(peer, rawData, 1024)) {
		/* Dont waste your time if the request is empty. */
		if (rawData.empty()) {
			peer.killConn();
			return;
		}

		http::requestParser reqData(rawData);
		net::HostData* hostData = new net::HostData(reqData.hostname, reqData.portNumber,
			rawData);

		if (n++ <= 3) {
			server->dataFromThread.push_back(hostData);
		}

		std::cout << "[+] request: " << reqData.url_or_host << std::endl;
		if (reqData.url_or_host == "/") {
			filename = "index.html";
			pathToDoc.append(filename);
		}
		else {
			filename = reqData.url_or_host;
			pathToDoc.append(filename);
		}
		printPath();
		FileHandler* fileHandler;
		fileHandler = new FileHandler(pathToDoc);

		/* If the file isn't opened */
		if (!fileHandler->isOpen()) {
			std::string response("Not Found; Working on this");
			http::write(peer, response);
			peer.killConn();
			delete fileHandler;
			return;
		}

		bool isKeepAlive = false;
		do {
			HeaderBuilder headerBuilder(reqData, fileHandler->getSize(), fileHandler->getExtension());
			std::string header = headerBuilder.getHeaders();
			http::write(peer, header + NL + NL);

			std::string tmp;
			while (!fileHandler->eof()) {
				tmp.clear();
				tmp = fileHandler->getLine() + "\n";
				http::write(peer, tmp);
			}
			/* Send these characters to mark the end of the http transfer. */
			http::write(peer, NL + NL);


			//DOTO: Trying to figure how the make the server to allow the all 
			// request from an HTML file through a single connection instead of
			// opening and closing a new connection to get all the content of an HTML file

			/*std::string con = reqData.getHeader("Connection");
			if (reqData.getHeader("Connection") == "keep-alive") {
				isKeepAlive = true;
				if (!peer.isKeepAlive()) {
					peer.setKeepAlive(true);
				}
			}
			else {
				isKeepAlive = false;
				if (peer.isKeepAlive()) {
					peer.setKeepAlive(false);
				}
			}
			if (isKeepAlive) {
				http::read(peer, rawData);
				std::cout << "[*] Keep-Alive test: " << rawData << std::endl;
				reqData = http::requestParser(rawData);
			}*/
		} while (isKeepAlive && !rawData.empty());
		delete fileHandler;
	}
	peer.killConn();

}