#include "TCPserver.hpp"
#include "TCPclient.hpp"
#include "serverDB/httpServer.hpp"


int main(void)
{
	// Server object pointer.
	net::TCPserver* server;

	try {
		/* Allocation and instantiation of the server object.
		** The first argument is the address family; the second is a char* of the address
		** to bind to, zero for any address. */
		server = new net::TCPserver(AF_INET, "127.0.0.1", 8080);

		// Callback of server object is now pointing to the http proxy server function
		server->codePointer.serverCode = serverDB::httpServer;
		//server->serverCode = serverDB::httpServer;
		//server->codePointer = serverDB::httpServer;
	}
	catch (net::SocketException& e) {
		std::cout << "[MAIN}........\n";
		e.display();
		return 1;
	}

	try {
		/* returns 0 on success. */
		server->newStartServer(5);


		// Pauses execution until SIGINT is sent from the keyboard.
		//net::wait();
		std::cout << "before wait...\n";
		server->wait();
		std::cout << "After wait...\n";
#ifdef _WIN32
		WSACleanup();
#endif
		return 0;
	}
	catch (net::SocketException& e) {
		e.display();
		std::cout << "[-] An Error happened in the previous method above; and it wasn't caught\n";
		delete server;
		//delete echo;
		return 0;
	}
	catch (...) {
		std::cout << "[*] Unexpected error caught...\n";
		return 1;
	}
	delete server;
}