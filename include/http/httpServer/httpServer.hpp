#ifndef HTTP_SERVER
#define HTTP_SERVER
#include "TCPserver.hpp"


namespace http {
	/* This is our HTTP server. In the mainHttpServer.cpp file, the serverCode() method pointer of
		a TCPserver object points to this function. The argument TCPpeer class encapsulates a
		valid connection. */
	void httpServer(std::shared_ptr<net::TCPserver> server, net::TCPpeer peer);
}

#endif