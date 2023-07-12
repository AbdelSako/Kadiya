/*
MIT License

Copyright (c) 2016 Abdel Sako

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "http/http.hpp"
//#include "socketError.hpp"
#include <algorithm>
#include <sstream>

/* ************************************************************************************ */
/*Request Parser */
http::requestParser::requestParser(std::string rawRequest) {
		//std::cout << rawRequest << std::endl;
		std::string::size_type pos;
		// Search the position between the headers and the request body;
		pos = rawRequest.find("\r\n\r\n");
		if (pos == std::string::npos) {
			std::cout << " Something is wrong with this HTTP request data." << std::endl;
		}

		std::stringstream requestStream(rawRequest);
		std::string header, headerValue;

		requestStream >> this->method;
		requestStream >> this->url_or_host;
		requestStream >> this->version;

		//Let's assign the request body
		if(this->method == POST)
			this->requestBody = rawRequest.substr(pos + 4);

		if (this->method == CONNECT) {
			int pos = this->url_or_host.rfind(":");
			this->hostname = this->url_or_host.substr(0, pos);
			this->portNumber = std::stoi(this->url_or_host.substr(pos + 1));
		}
		else {
			http::urlParser urlData(this->url_or_host);
			this->hostname = urlData.host;
			this->protocol = urlData.proto;
			if (urlData.port != 0)
				portNumber = urlData.port;
			else
				portNumber = 80;
		}

		while (requestStream.tellg() > 0 && requestStream.tellg() < pos) {
			requestStream >> header;
			header.pop_back();// remove the colon
			std::getline(requestStream, headerValue, '\r');
			headers[header] = headerValue.erase(0,1); // First ch is a space.
			
		}
}

std::string http::requestParser::getHeader(const std::string& header) {
	return headers[header];
}

bool http::requestParser::isKeepAlive() {
	/* TODO: The values of headers could have more than one value separated by commas or semi-colon.
	Therefore, it has to be handle later. */
	if(auto search = headers.find("Connection"); search != headers.end()) {
		if (search->second != "close")
			return true;
	}
	else if(auto search = headers.find("Proxy-Connection"); search != headers.end()) {
		if (search->second != "close")
			return true;
	}
	else {
		return false;
	}
}

/* ************************************************************************************ */
/* Response Parser */
http::responseParser::responseParser
(const std::string rawResponse)
{
	std::string::size_type n, pos = 0, split;
	std::vector<size_t> rc;

	if(rawResponse.empty()) return;

	if((n = rawResponse.find("\r\n\r\n", pos)) != std::string::npos)
		split = n;
	else
		split = 0;

	while(true) {
		n = rawResponse.find("\r\n", pos);
		if(n == std::string::npos) break;
		pos = n + 1;
		rc.push_back(n);
		if(rc.back() == split) break;
	}

	/* version, code and status */
	if(rc.size() >= 1) {
		pos = 0;
		n = rawResponse.find(' ', pos);
		if(n == std::string::npos) /*TODO: handle it */;
		version = rawResponse.substr(pos, n);
		pos = n + 1;
		n = rawResponse.find(' ', pos);
		if(n == std::string::npos) /* TODO: handle it*/;
		code = rawResponse.substr(pos, n - pos);
		pos = n + 1;
		status = rawResponse.substr(pos, rc[0] - n - 1);
	}

	/* All right! let's deal with the headers, again. */
	pos = 0;
	if(rc.size() >= 2)
		while(true) {
			n = rawResponse.find(':', rc[pos] + 2);
			if(n == std::string::npos)	break;
			if(rc.size() == pos + 1) break;
			headers[rawResponse.substr(rc[pos] + 2, n - (rc[pos] + 2))] =
				(std::string)rawResponse.substr(n + 2, rc[pos + 1] -1 - n);
			++pos;
		}

	if(split != 0)
		if(rawResponse.length() > split + 4)
			responseBody = rawResponse.substr(split + 4);
}

bool http::responseParser::isKeepAlive() {
	/* TODO: The values of headers could have more than one value separated by commas or semi-colon.
	Therefore, it has to be handle later. */
	if (auto search = headers.find("Connection"); search != headers.end()) {
		if (search->second == "close")
			return false;
		else
			return true;
	}
	else
		return false;
}

/* ************************************************************************************ */
/* URL parser */
http::urlParser::urlParser(std::string fullUrl)
{
	std::string::size_type n, pos = 0;

	if(fullUrl.empty()) return;

	if(fullUrl.compare(0, 1, "/") == 0) {
		this->url = fullUrl;
		return;
	}

	/* check first before running through it */
	if(fullUrl.compare(0, 5, "https") != 0)
		if(fullUrl.compare(0, 4, "http") != 0)
			return;

	this->port = 0;

	/* Now let's run through it */
	if(fullUrl.compare(0, 4, "http") == 0) {
		n = 4;
		if(fullUrl.compare(0, 5, "https") == 0) {
			n = 5;
		}
	}


	this->proto = fullUrl.substr(pos, n);

	pos = n + 3; // sets pos to the first character of hostname

	n = fullUrl.find(':', pos);
	if(n != std::string::npos) { // n -> ':'
		std::string::size_type m;

		m = fullUrl.find('/', pos);
		if(m != std::string::npos) { // m -> '/'

			// if port number was given
			if(n > m) {
				// not given
				this->host = fullUrl.substr(pos, m - pos);
				this->port = 0;
				pos = m;
				this->url = fullUrl.substr(pos);
				return;
			}
			else {
				// port given
				this->host = fullUrl.substr(pos, n - pos);
				pos = n + 1; // pos updated to the first char of port number
				try {
					this->port = std::stoi(fullUrl.substr(pos, m - pos));
				} catch(std::invalid_argument& e) {
					/* TODO: malformed request */
					e.what();
					return;
				}
				pos = m; // updated to '/'
				this->url = fullUrl.substr(pos);
				return;
			}
		}

		else {
			this->host = fullUrl.substr(pos, n - pos);
			pos = n + 1; // update pos to first char of port number
			try {
				this->port = std::stoi(fullUrl.substr(pos));
			} catch(std::invalid_argument& e) {
				//TODO: malformed request
				e.what();
			}
			this->url = "/";
			return;
		}
	}

	else {
		n = fullUrl.find('/', pos);
		if(n == std::string::npos) {
			this->host = fullUrl.substr(pos);
			this->port = 0;
			this->url = "/";
		}
		else {
			this->host = fullUrl.substr(pos, n - pos);
			pos = n;
			this->url = fullUrl.substr(pos);
			this->port = 0;
		}
		return;
	}
}

/* ************************************************************************************ */
http::HeaderBuilder::HeaderBuilder(http::requestParser& request, u_int content_length,
	std::string content_type) {
	data.append(HTTP_200 + NL);
	data.append(SERVER + SERVER_NAME + NL);
	data.append(CONTENT_LENGTH + std::to_string(content_length) + NL);
	data.append(CONTENT_TYPE + content_type + NL);

	std::string con = request.getHeader("Connection");
	if (con.empty()) con = "closed";

	data.append(CONNECTION + request.getHeader("Connection"));
	//data.append(CONNECTION + "closed" + NL + NL);
}

std::string http::HeaderBuilder::getHeaders(void) {
	return data;
}

/* ************************************************************************************ */

void http::recvUntilRC(net::TCPpeer& tcppeer, std::string& rawrequest)
{
	char tempbuf[512];
	rawrequest.clear();
	do {
		/* will drop the client's request if the request data does
		 * not have 2 trailing "return carriages" */
		std::memset(tempbuf, 0, 512);
		tcppeer.recv(tempbuf, 512);
		if(std::strlen(tempbuf) == 0) break;
		rawrequest += tempbuf;
		/* oh!, maybe it's "new lines" instead of "return carriages"*/
		if(rawrequest.rfind("\n\n") != std::string::npos)
			break;
	} while(rawrequest.rfind("\r\n\r\n") == std::string::npos);
}


bool http::isAllChunk(const std::string rawResponse)
{
    /* Looking for zero before CRLF * 2 */
    size_t chunkSize;

    try {
        chunkSize = std::stoi((std::string &)*(rawResponse.crbegin() + 4));
        if(chunkSize == 0)
            return true;
        else
            return false;

    } catch(std::invalid_argument& e) {
        return false;
    }
}

int http::read(net::TCPpeer& peer, std::string& rawData) {
	ssize_t bytes, totalBytes = 0;
	char tmpBuf[512];
	rawData.clear();
	do {
		std::memset(tmpBuf, 0, 512);
		bytes = peer.recv(tmpBuf, 512);
		if (bytes > 0) {
			totalBytes += bytes;
			rawData.append(tmpBuf, bytes);
		}
		else if (bytes < 0) {
			if (peer.getLastError() == EWOULDBLOCK) {
				std::cout << "[*] RECV TIMEOUT...\n";
			}
		}
	} while (peer.availToRead());
	
	if (totalBytes == 0) return -1;
	else return totalBytes;
}

int http::write(net::TCPpeer& peer, const std::string& data) {
	int bytes = peer.send((const char*)data.data(),
		data.length());
	return bytes;
}










