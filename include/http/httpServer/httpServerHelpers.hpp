#ifndef __httpServerHelpers
#define __httpServerHelpers
#include <iostream>
#include "helpers.hpp"

std::string INTERNAL_ERROR = "<html><h1> Some error happened while "
"N'nimba was trying to open the document</h1></html>\r\n\r\n";
#define PATH_TO_INTERNAL_SERVER_FILES "../../../src/http/httpServer/"

#define SERVER "N'Nimba"
#define HTTP_200 "HTTP/1.1 200 OK"
#define CONTENT_LENGTH "content-length"
#define CONTENT_TYPE "content-type"
#define CONNECTION "connection"


std::string documentNotFound(void);

class HeaderHandler {
	const std::string server = SERVER;
	std::string contentType;
	std::string connection;
	unsigned int contentLength;
	std::string headers;
public:
	//HeaderHandler() = default;
	/*HeaderHandler(const http::requestParser& headersObj) {

	}*/
	void setContentLength(unsigned int size) {
		this->contentLength = size;
	}
	void setContentType(const std::string type) {
		this->contentType = type;
	}
	void setConnection(const std::string conn) {
		this->connection = conn;
	}

	static std::string conType(std::string& suffix) {
		if (suffix == "png") {
			return "image/png";
		}
		else if (suffix == "html") {
			return "text/html";
		}

		return "text/html";
	}

	std::string getHeader(void) {
		std::string header;
		header.append("HTTP/1.1 200 OK\r\n");
		header.append("server: " + server + "\r\n");
		header.append("content-length: " + std::to_string(contentLength) + "\r\n");
		header.append("content-type: " + contentType + "\r\n");
		header.append("connection: " + connection + "\r\n");
		return header.append("\r\n");
	}
};


std::string documentNotFound(void) {
	printPath();
	std::string response;
	std::string filename(PATH_TO_INTERNAL_SERVER_FILES + std::string("500.html"));
	HeaderHandler header;
	FileHandler file(filename);
	header.setConnection("closed");
	header.setContentType("html");

	if (!file.isOpen()) {
		header.setContentLength(INTERNAL_ERROR.length());
		file.close();
		return (std::string)(header.getHeader() + INTERNAL_ERROR);
	}
	else {
		header.setContentLength(file.getSize());
		while (!file.eof())
			response.append(file.getLine() + "\n");
		file.close();
		return (std::string)(header.getHeader() + response);
	}
}
#endif
