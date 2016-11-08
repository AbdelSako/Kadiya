#include "TCPserver.hpp"
#include "TCPclient.hpp"
#include "serverDB/httpProxy.hpp"

int main(void)
{
	// Server object pointer.
	net::TCPserver *server;

	// Callback to an http proxy server code
	callbacks::ServerCode *cb = new callbacks::ServerCode(serverDB::HTTPproxyServer);

	try {
		/* Allocation and instantiation of the server object.
        ** The first argument is the address family; the second is a char* of the address
        ** to bind to, zero for any address. */
		server = new net::TCPserver(AF_INET, 0, 8080);

		// Callback of server object is now pointing to the http proxy server function
		server->serverCallbacks = cb;

	} catch (net::SocketException& e) {
		e.display();
		return 1;
	}

	try{
		/* returns 0 on success.
		** Starts 15 detached threads, */
		if(server->startServer(15) == -1) {
            std::cout << "[+] TCPserver failed to start...\n";
            return 1;
		}

		// Pauses execution until SIGINT is sent from the keyboard.
		net::wait();

		return 0;
	} catch(net::SocketException& e) {
		e.display();
		return 1;
	} catch(...) {
		std::cout << "[*] Unexpected error caught...\n";
		return 1;
	}
}
