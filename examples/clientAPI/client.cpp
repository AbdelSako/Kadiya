#include "TCPclient.hpp"
#include "handlers/HTTPhandlers.hpp"

int main(void)
{

	// Creates a client object
	net::TCPclient client;

	// Connects to google and check if a valid socket is returned
	if(client.connect("www.google.com", 80) == -1) {
		std:: cout << "[*] failed to connect...\n";
		return 1;
	}

	// Set recv and send timeouts
	/* */

	// response variable
	std::string res;

	// Request variable
	std::string req = "GET http://10.0.0.1/ HTTP/1.1\r\n"
							"User-Agent: SockAPI\r\n\r\n";

	// Send request
	client << req;

	// Receive request
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
