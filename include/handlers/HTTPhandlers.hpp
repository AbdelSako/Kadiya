#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <list>
#include "TCPsocket.hpp"

#if !defined(__http_handlers)
#define __http_handlers
namespace http
{
	static const std::list<std::string> METHODS = {
			"GET",
			"HEAD",
			"POST",
			"PUT",
			"DELETE",
			"CONNECT",
			"OPTIONS",
			"TRACE"
	};

	/* Parses a raw http request */
	struct requestParser {
	    //requestParser(void) {};
		requestParser(std::string rawRequest);

		std::string method;
		std::string url_or_host; /* used for local request, http proxy and tunneling*/
		std::string version;
		std::map<std::string, std::string> headers;
		std::string requestBody;
	};

	/* Parses a raw http response */
	struct responseParser {
	    //responseParser(void) {};
		responseParser(std::string rawResponse);

		std::string version;
		std::string code;
		std::string status;
		std::map<std::string, std::string> headers;
		std::string responseBody;
	};

	/* Parses a url */
	struct urlParser {
	    //urlParser(void) {};
		urlParser(std::string fullUrl);

		std::string proto;
		std::string host;
		std::string url;
		uint16_t port;
	};

	void recvUntilRC(net::TCPpeer& TCPpeer, std::string& rawRequest);

    /* RawResponse might contain multiple LF */
	bool isAllChunk(const std::string rawResponse);

	short recvAll(net::TCPpeer& peer, std::string& rawResponse);

	void pipePeers(net::TCPpeer& from, net::TCPpeer& to);
};
#endif
