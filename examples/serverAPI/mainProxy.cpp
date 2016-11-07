#include "TCPserver.hpp"
#include "TCPclient.hpp"
#include "serverDB/httpProxy.hpp"

int main(void)
{
	net::TCPserver *server1;
	net::TCPserver *server2;

	callbacks::ServerCode *cb = new callbacks::ServerCode(serverDB::HTTPproxyServer);

	try {
		server1 = new net::TCPserver(AF_INET, 0, 0, 9090);
		server2 = new net::TCPserver(AF_INET, 0, 0, 8080);
		server2->serverCallbacks = cb;
	} catch (net::SocketException& e) {
		e.display();
		return 1;
	}

	try{
		server1->startServer(1);
		if(server2->startServer(15) == -1) {
            std::cout << "[+] TCPserver failed to start...\n";
            return -1;
		}
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
