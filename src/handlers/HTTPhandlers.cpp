#include "handlers/HTTPhandlers.hpp"
#include <algorithm>

/*Request Parser */
http::requestParser::requestParser
(const std::string rawRequest)
{
	std::string::size_type n, pos = 0, split;
	std::vector<size_t> rc; // return carriage indexes
	std::string delim;

	if(rawRequest.empty()) return;

	n = rawRequest.find("\r\n\r\n", pos);
	if(n != std::string::npos) {
		delim = "\r\n";
		split = n;
	}
	else {
		n = rawRequest.find("\n\n");
		if(n != std::string::npos) {
			delim = "\n";
			split = n;
		}
	}
	/* return, if delim is empty */
	if(delim.empty())
		return;

	while(true) {
		n = rawRequest.find(delim, pos);
		if(n == std::string::npos) break;
		rc.push_back(n);
		pos = n + 1;
		if(rc.back() == split) break;
	}

	/* method url/host and version */
	if(rc.size() >= 1) {
		pos = 0;
		n = rawRequest.find(' ', pos);
		if(n == std::string::npos) /* throw http::exception() */;
		method = rawRequest.substr(pos, n);

		pos = n + 1;
		n = rawRequest.find(' ', pos);
		if(n == std::string::npos);

		url_or_host = rawRequest.substr(pos, n - method.length());

		pos = n + 1;
		version = rawRequest.substr(pos, rc[0] - method.length() - url_or_host.length() - 1 );
	}

	/* All right! let's deal with headers */
	pos = 0;
	if(rc.size() >= 2)
		while(true) {
			n = rawRequest.find(':', rc[pos] + 2);
			if(n == std::string::npos)	break;
			if(rc.size() == pos + 1) break;
			headers[rawRequest.substr(rc[pos] + 2, n - (rc[pos] + 2))] =
				(std::string)rawRequest.substr(n + 2, rc[pos + 1] -1 - n);
			++pos;
		}
	/* body*/
	if(split != 0)
		if(method.compare("POST") == 0)
			if(rawRequest.length() > split + 4)
				requestBody = rawRequest.substr(split + 4);
}

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
		if(n == std::string::npos) /*handle it */;
		version = rawResponse.substr(pos, n);
		pos = n + 1;
		n = rawResponse.find(' ', pos);
		if(n == std::string::npos) /* handle it*/;
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

void http::recvUntilRC(net::TCPpeer& TCPpeer, std::string& rawRequest)
{
	std::string tempBuf;
	do {
		/* Will drop the client's request if the request data does
		 * not have 2 trailing "return carriages" */
		TCPpeer >> tempBuf;
		if(tempBuf.empty()) break;
		rawRequest += tempBuf;
		/* Oh!, maybe it's "new lines" instead of "return carriages"*/
		if(rawRequest.rfind("\n\n") != std::string::npos)
			break;
	} while(rawRequest.rfind("\r\n\r\n") == std::string::npos);
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

short http::recvAll(net::TCPpeer& peer, std::string& rawResponse)
{
    /* recv first package and check if the chunked encoding is set.
    *  if so check if everything's been recv'd.
    *  if not, continue recv'ing until... */
    http::recvUntilRC(peer, rawResponse);

    http::responseParser resinfo(rawResponse);

    auto transHead = resinfo.headers.find("Transfer-Encoding");
    if(transHead != resinfo.headers.end()) {

    }
}












