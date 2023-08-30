#include "TCPclient.hpp"
#include "http/http.hpp"


int main(void) //(int argc, char *argv[])
{

	int port = 80;
	std::string host = "www.google.com";

	/*if(argc != 3) {
		std::cout << argv[0] << " host port\n";
		return -1;
	}

	host = argv[1];

	try {
		port = std::stoi(argv[2]);
	} catch(...) {
		std::cout << "[*] invalid port\n";
		return 1;
	}*/

	// Creates a client object
	net::TCPclient client;
	net::TCPpeer peer;
	peer = client.connect(host, port);

	// Connect to host and check if a valid socket is returned
	if(!peer.isValid()) {
		std:: cout << "[*] failed to connect...\n";
		peer.getLastError();
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
	http::write(peer,req);

	// Receive response
	http::read(peer, res);

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

	peer.killConn();
}
