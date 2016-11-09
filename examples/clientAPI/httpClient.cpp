#include "TCPclient.hpp"
#include "handlers/HTTPhandlers.hpp"

int main(int argc, char *argv[])
{

	if(argc != 3) {
		std::cout << argv[0] << " host port\n";
		return -1;
	}

	std::string host = argv[1];
	int port;

	try {
		port = std::stoi(argv[2]);
	} catch(...) {
		std::cout << "[*] invalid port\n";
		return 1;
	}

	// Creates a client object
	net::TCPclient client;

	// Connect to host and check if a valid socket is returned
	if(client.connect(host, port) == -1) {
		std:: cout << "[*] failed to connect...\n";
		return 1;
	}

	// Set recv and send timeouts
	/* */

	// response variable
	std::string res;

	// Request variable
	std::string req = "GET http://"+host+"/ HTTP/1.1\r\n"
							"User-Agent: SocketAPI\r\n\r\n";

	// Send request
	client << req;

	// Receive response
	client >> res;

	// Parse the http response into a struct (http::responseParser)
	struct http::responseParser resinfo(res);

	// Access the struct's data (http::responseParser)
	std::cout << "[+] version: " << resinfo.version << '\n';
	std::cout << "[+] code: " << resinfo.code << '\n';
	std::cout << "[+] status: " << resinfo.status << '\n';

	for(auto &pair : resinfo.headers) {
		std::cout << "[+] " << pair.first << ": " << pair.second << '\n';
	}

	std::cout << resinfo.responseBody;
}
