
#define __httpConfig
#include "serverDB/httpServer.hpp"

std::ifstream configStream(configFile);
HttpServerConfig httpServerConfig(configStream);

const std::string DOC_ROOT(httpServerConfig.getDocument_Root());

char outBuf[512];

FileHandler* fileHandler;

void serverDB::httpServer(std::shared_ptr<net::TCPserver> server, net::TCPpeer peer) {
	bool tmp;
	std::ifstream fileStreamToServe;
	std::string pathToDoc(DOC_ROOT);
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
		if (server->cached.find(filename) == server->cached.end()) {
			std::unique_lock fileLock(server->getFileMutex()); //LOCK
			if (server->cached.find(filename) == server->cached.end()) {
				std::string data, buffer;
				fileHandler = new FileHandler(pathToDoc);
				if (fileHandler->isOpen()) {
					while (fileHandler->eof()) {
						data.append(fileHandler->getLine());
					}
					server->cached.insert({ filename, data });
					fileHandler->close();
				}
				else 
				{
					http::write(peer, TEST_OK_500 + INTERNAL_ERROR);
					peer.killConn();
					return;
				}
			}
			fileLock.unlock(); //UNLOCK
		}

		std::string content = server->cached[filename];
		http::write(peer, TEST_OK_200 + content);

		//if (server->OpenedFiles.find(filename) == server->OpenedFiles.end()) {
		//	/* Trapped all the threads attempting to open the same file */

		//	std::unique_lock fileLock(server->getFileMutex()); //LOCK
		//	if (server->OpenedFiles.find(filename) == server->OpenedFiles.end()) {
		//		fileHandler = new FileHandler(pathToDoc);
		//		server->OpenedFiles.insert({ filename, fileHandler });
		//		tmp = server->OpenedFiles[filename]->isOpen();
		//	}
		//	fileLock.unlock(); //UNLOCK
		//}
		//auto _file = server->OpenedFiles[filename];
		//if (_file->isOpen()) {
		//	std::unique_lock fileLock(server->getFileMutex()); // LOCK
		//	http::write(peer, TEST_OK_200);
		//	_file->seek(0);
		//	rawData = _file->getLine();
		//	while (!rawData.empty()) {
		//		http::write(peer, rawData);
		//		rawData = _file->getLine();
		//	}
		//	fileLock.unlock(); //UNLOCK
		//}
		//else {
		//	http::write(peer, TEST_OK_500 + INTERNAL_ERROR);
		//}
		
	}
	//fileHandler->close();
	peer.killConn();

}
