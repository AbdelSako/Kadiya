/*
MIT License

Copyright (c) 2016 Abdel Sako

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "serverDB/httpProxy2.0.hpp"
std::string OK_200_KEEPALIVE = "HTTP/1.1 200 Connection Established\r\n"
								"Host: WebProject\r\n" 
								"Connection: keep-alive\r\n\r\n";
std::string OK_200 = "HTTP/1.1 200 Connection Established\r\n"
						"Host: WebProject\r\n\r\n";

/* Delete localPeer at the end of the code or else... you know what.*/
void serverDB::echoServer(net::TCPpeer* peer) {
	HttpSocket http(peer);
	std::string data;
	try {
		while (true) {
				//http.httpRecv();
				//http.httpSend(http.getDataReceived());
			http.httpRecv(data);
			if (data.empty())
				break;
			http.httpSend(data);
		}
	}
	catch (net::SocketException& e) {
		std::cout << "[*] Error from echoServer(): ";
		e.display();
	}
	catch (...) {
		std::cout << "[*] echoServer90: Unknown error\n";
	}
	peer->shutdown(0);
	peer->close();
}
void serverDB::testServer(net::TCPpeer* localPeer) {
	try {
		net::TCPclient client = net::TCPclient();
		net::TCPpeer remotePeer = net::TCPpeer(
			client.connect("127.0.0.1", 9000));

		serverDB::HttpSocket localHttp(localPeer);
		serverDB::HttpSocket remoteHttp(&remotePeer);
		while (localPeer->getLastError() == 0) {
			localHttp.httpRecv();
			std::cout << localHttp.getDataReceived() << std::endl;
			remoteHttp.httpSend(localHttp.getDataReceived());
			remoteHttp.httpRecv();
			std::cout << remoteHttp.getDataReceived() << std::endl;
			localHttp.httpSend(remoteHttp.getDataReceived());
			std::cout << "[*] end eend\n";
		}
		std::cout << "[*] I believe this instruction will not be executed\n";
	}
	catch (net::SocketException& e) {
		e.display();
	}
	catch (...) {
		std::cout << "[*] Unknown Error Caught.\n";
	}
}
void serverDB::httpProxyServer(net::TCPpeer *localPeer) {
	//std::signal(SIGINT, net::TCPserver::signalHandler);
	HttpSocket localHttp(localPeer);
	net::TCPclient* client = new net::TCPclient();
	std::string hostname;
	uint32_t portNumber;

	localHttp.httpRecv();
	http::requestParser recvData(localHttp.getDataReceived());
	if (recvData.method == "CONNECT") {
		int pos = recvData.url_or_host.rfind(":");
		hostname = recvData.url_or_host.substr(0,pos);
		portNumber = std::stoi(recvData.url_or_host.substr(pos + 1));
	}

	net::TCPpeer remotePeer = client->connect(hostname, portNumber);

	if (remotePeer.isValid()) {
		localHttp.setDataToSend(OK_200_KEEPALIVE);
		localHttp.httpSend();
	}

	HttpSocket remoteHttp(&remotePeer);
	//localPeer->setNonBlocking(true);
	//remotePeer.setNonBlocking(true);
	//localPeer->setRecvTimeout(5);
	//remotePeer.setRecvTimeout(5);
	std::string data;
	try {
		while (localPeer->getLastError() == 0 && remotePeer.getLastError() == 0) {
			localHttp.httpRecv(data);
			if (data.empty())
				break;
			std::cout << "From Local: " << localPeer->getPeerAddr()
				<< " " << localPeer->getPeerPort() << std::endl;

			remoteHttp.httpSend(data);
			std::cout << "To Remote: " << remotePeer.getPeerAddr()
				<< " " << remotePeer.getPeerPort() << std::endl;

			remoteHttp.httpRecv(data);
			std::cout << "From Remote: " << remotePeer.getPeerAddr()
				<< " " << remotePeer.getPeerPort() << std::endl;

			localHttp.httpSend(data);
			std::cout << "To Local: " << localPeer->getPeerAddr()
				<< " " << localPeer->getPeerPort() << std::endl;

		}
		std::cout << "[*] Exited the loop...\n";
	}
	catch (net::SocketException& e) {
		e.display();
	}
	catch (...) {
		std::cout << "Unexpected error caught\n";
	}

	//net::TCPpeer *peer = new net::TCPpeer(client->connect("127.0.0.1", 8090));
	
	//delete peer;
	delete localPeer;
}

/* ************* */
/* Get transmission status */
int serverDB::HttpSocket::getTransmissionStatus(void) {
	return transmissionStatus;
}

/* Set Data to send*/
void serverDB::HttpSocket::setDataToSend(std::string data) {
	*this->dataToSend = data;
}
/* */
std::string serverDB::HttpSocket::getDataReceived(void) {
	return *this->dataReceived;
}

/* Get Buffer*/
unsigned int serverDB::HttpSocket::getRecvBufferSize(void)
{
	return this->recvBufferSize;
}

/* Get buffer size */
unsigned int serverDB::HttpSocket::getSendBufferSize(void)
{
	return this->sendBufferSize;
}
/* Send buffer Size */
void serverDB::HttpSocket::setSendBufferSize(unsigned int sendBufferSize)
{

	this->sendBufferSize = sendBufferSize;
	this->dataToSend->resize(sendBufferSize);
}

/*   */
void serverDB::HttpSocket::httpRecv(void)
{
	/* Let's receive some data and look for the first double return cariage ;
	that would be this string "\n\r\n\r" */
	//memset(classBuffer, 0, classBuffersize);
	this->dataReceived->clear();
	int bytes = 0;
	int totalBytes;
	bytes = peer->recv(this->classBuffer, this->classBuffersize);
	this->dataReceived->append(this->classBuffer);
	transmissionStatus = true;

}

void serverDB::HttpSocket::httpRecv(std::string& data) {
#ifdef _WIN32
	int SOCKET_TIMEOUT = WSAEWOULDBLOCK;
#else
	int SOCKET_TIMEOUT = EWOULDBLOCK;
#endif
	data.clear();
	int bytes, totalBytes = 0;
	this->peer->setNonBlocking(true);
	this->peer->setRecvTimeout(1);
	do {
		std::memset(this->classBuffer, 0, this->classBuffersize);
		try {
			bytes = this->peer->recv(this->classBuffer, this->classBuffersize);
			if (bytes > 0)
				data.append(this->classBuffer);
		}
		catch (net::SocketException& e) {
			e.display();
			bytes = 0;
			/* If recv didn't not timeout then an error occured. */
			if (e.getErrorCode() != SOCKET_TIMEOUT)
				throw;
		}

	} while (bytes > 0);
	this->peer->setNonBlocking(false);
	this->peer->setRecvTimeout(5);
}

void serverDB::HttpSocket::httpSend(void) {
	if (this->dataToSend->empty())
		return;// Probably throw an exception
	std::memset(this->classBuffer, 0, classBuffersize);
	this->classBuffer = (char*)this->dataToSend->data();
	int bytes = peer->send(this->classBuffer, this->dataToSend->length());
}

void serverDB::HttpSocket::httpSend(const std::string& data) {
	
	int bytes = peer->send((const char*)data.data(), data.length());
}