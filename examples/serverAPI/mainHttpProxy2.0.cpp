#include "TCPserver.hpp"
#include "TCPclient.hpp"
#include "serverDB/httpProxy2.0.hpp"


int main(void)
{
	// Server object pointer.
	net::TCPserver* server;
	net::TCPserver* echo;

	// Callback to an http proxy server code
	//callbacks::ServerCode *cb = new callbacks::ServerCode(serverDB::HTTPproxyServer);

	try {
		/* Allocation and instantiation of the server object.
		** The first argument is the address family; the second is a char* of the address
		** to bind to, zero for any address. */
		server = new net::TCPserver(AF_INET, "127.0.0.1", 8080);
		//echo = new net::TCPserver(AF_INET, "127.0.0.1", 9000);

		// Callback of server object is now pointing to the http proxy server function
		server->serverCode = serverDB::httpProxyServer;
		//echo->serverCode = serverDB::echoServer;

	}
	catch (net::SocketException& e) {
		std::cout << "[MAIN}........\n";
		e.display();
		return 1;
	}

	try {
		/* returns 0 on success.
		** Starts 15 detached threads, */
		//echo->startServer(1);
		server->start(1);
		

		// Pauses execution until SIGINT is sent from the keyboard.
		//net::wait();
		server->wait();
#ifdef _WIN32
		WSACleanup();
#endif
		return 0;
	}
	catch (net::SocketException& e) {
		e.display();
		delete server;
		//delete echo;
		return 0;
	}
	catch (...) {
		std::cout << "[*] Unexpected error caught...\n";
		return 1;
	}
	delete server;
	//delete echo;
}