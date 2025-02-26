#include "TCPserver.hpp"
#include "TCPclient.hpp"
#include "http/httpServer/httpServer.hpp"
#include <filesystem>
#define LISTENERS 5
#define MAXHOST 20


void printCurrentDirectory();

void processData(net::ServerSocket server);

std::string getHomeDirectory();

int main(void)
{
	// Server object pointer.
	net::ServerSocket* server;
	printCurrentDirectory();
	std::cout << "Home Directory:" << getHomeDirectory() << std::endl;

	try {
		/* Allocation and instantiation of the server object.
		** The first argument is the address family; the second is a char* of the address
		** to bind to, zero for any address. */
		std::cout << "[+] Preparing threaded acceptors.\n";
		server = new net::ServerSocket(AF_INET, "127.0.0.1", 50505);


		// Callback of server object is now pointing to the http proxy server function
		/* This is where execution of the http server begins. */
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

void processData(net::ServerSocket &server, int n) {
	auto data = server.dataFromThread[n];
	std::cout << data << std::endl;
}


void printCurrentDirectory() {
	try {
		std::filesystem::path currentPath = std::filesystem::current_path();
		std::cout << "Current directory: " << currentPath.string() << std::endl;
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error getting current directory: " << e.what() << std::endl;
	}
}

std::string getHomeDirectory() {
#ifdef _WIN32
	// Windows
	const char* homeDrive = std::getenv("HOMEDRIVE");
	const char* homePath = std::getenv("HOMEPATH");
	if (homeDrive && homePath) {
		return std::string(homeDrive) + std::string(homePath);
	}
#else
	// POSIX (Linux, macOS, etc.)
	const char* homeDir = std::getenv("HOME");
	if (homeDir) {
		return std::string(homeDir);
	}
#endif

	// Fallback using filesystem (C++17)
#ifdef __cpp_lib_filesystem
	try {
		return std::filesystem::path(std::getenv("USERPROFILE")).string(); // windows
	} catch (const std::exception& e) {
		try{
			return std::filesystem::path(std::getenv("HOME")).string(); // posix
		} catch (const std::exception& e2){
			return ""; // Unable to find home directory
		}

	}
#else
	return ""; // Unable to find home directory
#endif

	return ""; // Unable to find home directory
}