#include "TCPserver.hpp"
#include "TCPclient.hpp"
#include "http/httpServer/httpServer.hpp"
#define LISTENERS 5
#define MAXHOST 20


void processData(net::TCPserver server);


int main(void)
{
	// Server object pointer.
	net::TCPserver* server;

	try {
		/* Allocation and instantiation of the server object.
		** The first argument is the address family; the second is a char* of the address
		** to bind to, zero for any address. */
		std::cout << "[+] Preparing threaded acceptors.\n";
		server = new net::TCPserver(AF_INET, "127.0.0.1", 50505);

		// Callback of server object is now pointing to the http proxy server function
		server->codePointer.serverCode = http::httpServer;
	}
	catch (net::SocketException& e) {
		std::cout << "[MAIN}........\n";
		e.display();
		return 1;
	}

	try {
		/* returns 0 on success. */
		std::cout << "[+] Preparing to launch threaded lisneters...\n";
		server->startThreadedServer(MAXHOST, LISTENERS);

		std::cout << "[+] Server is ready to accept connection from " << LISTENERS << " listeners.\n";
		std::cout << "[+] And ready to serve a max of " << MAXHOST << " hosts\n";
		std::cout << "\n[+] Web Document Root is:\n\t\t./Kadiya/www/\n\n";


		// Pauses execution until SIGINT is sent from the keyboard.
		std::cout << "[dev] before wait...\n";
		server->wait();
		/*int n = 0;
		while (server->waitForData()) {
			
		}*/
		std::cout << "[dev] After wait...\n";
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

void processData(net::TCPserver &server, int n) {
	auto data = server.dataFromThread[n];
	std::cout << data << std::endl;
}