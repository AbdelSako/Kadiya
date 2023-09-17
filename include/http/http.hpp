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

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <list>
#include "TCPsocket.hpp"

#define SERVER_NAME (std::string)"N'Nimba"

#if !defined(__http_handlers)
#define __http_handlers

#define GET (std::string)"GET"
#define POST (std::string)"POST"
#define CONNECT (std::string)"CONNECT"
#define PUT (std::string)"PUT"

#define NL (std::string)"\r\n"
#define HTTP_200 (std::string)"HTTP/1.1 200 OK"
#define SERVER (std::string)"server: "
#define CONTENT_LENGTH (std::string)"content-length: "
#define CONTENT_TYPE (std::string)"content-type: "
#define CONNECTION (std::string)"connection: "

namespace http
{
	static const std::list<std::string> METHODS =
	{
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
		requestParser(std::string rawRequest);

		std::string method;
		std::string url_or_host; /* used for local request, http proxy and tunneling*/
		std::string version;
		std::string protocol;
		std::string hostname;
		uint32_t portNumber;
		std::map<std::string, std::string> headers;
		std::string requestBody;

		bool isKeepAlive();
		std::string getHeader(const std::string& header);
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
		bool isKeepAlive();
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

	class HeaderBuilder {
	private:
		std::string data;
	public:
		HeaderBuilder(requestParser& request, u_int content_length,
			std::string content_type);
		std::string getHeaders(void);
	};

	void recvUntilRC(net::TCPpeer& TCPpeer, std::string& rawRequest);

    /* RawResponse might contain multiple LF */
	bool isAllChunk(const std::string rawResponse);

	short recvAll(net::TCPpeer& peer, std::string& rawResponse);

	void pipePeers(net::TCPpeer& from, net::TCPpeer& to);

	ssize_t read(net::TCPpeer& peer, std::string& rawData, uint16_t bufsize);

	ssize_t write(net::TCPpeer& peer, const std::string& data);

};

bool isKeepAlive(void);
#endif
