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
void serverDB::echoServer(net::TCPpeer peer) {
	HttpSocket http(peer);
	std::string data;
	try {
		//http.resizeBufferSize(10);
		while (true) {
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
	peer.shutdown(0);
	peer.close();
}
void serverDB::testServer(net::TCPpeer localPeer) {
	try {

		std::string data;
		net::TCPclient client = net::TCPclient();
		net::TCPpeer remotePeer = net::TCPpeer(
			client.connect("127.0.0.1", 9000));

		serverDB::HttpSocket localHttp(localPeer);
		serverDB::HttpSocket remoteHttp(remotePeer);
		while (localPeer.getLastError() == 0) {
			localHttp.httpRecv(data);
			std::cout << data << std::endl;
			remoteHttp.httpSend(data);
			remoteHttp.httpRecv(data);
			std::cout << data << std::endl;
			localHttp.httpSend(data);
			std::cout << "[*] end end\n";
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
void serverDB::httpProxyServer(net::TCPpeer localPeer) {
	//std::signal(SIGINT, net::TCPserver::signalHandler);
	net::TCPpeer remotePeer;
	HttpSocket localHttp(localPeer);
	net::TCPclient* client = new net::TCPclient();
	std::string hostname, url;
	uint32_t portNumber;
	std::string data;
	int keepAliveRes;

	/* Check if the received data and return if it is empty. */
	try {
		localHttp.httpRecv(data);
	}
	catch (net::SocketException& e) {
		e.display();
		return;
	}

	if (data.empty()) {
		localHttp.peer.shutdown(0);
		localHttp.peer.close();
		return;
	}
	std::cout << "[+] New connection...\n";

	/* Parsed the received data from local host and set keepAlive if 
		requested by the local http client. */
	http::requestParser reqData(data);
	if (reqData.isKeepAlive())
		localHttp.peer.setKeepAlive(true);


	/* Try to Establish a connection with the remote http server.
		Throw and exception and return if connection to the remote http server
		fails. */
	try {
		remotePeer = client->connect(reqData.hostname, reqData.portNumber);
	}
	catch (net::SocketException& e) {
		std::cout << "[*] Proxy failed to connect to the remote server\n";
		e.display();
		localPeer.shutdown(0); localPeer.close();
		return;
	}

	/* NOW WE CAN START SENDING AND RECEIVING DATA TO/FROM THE SERVER AND THE LOCAL CLIENT. */
	HttpSocket remoteHttp(remotePeer);

	try {
		if (reqData.method.compare("CONNECT") != 0) {//IF NOT CONNECT method, HANDLE HTTP
			remoteHttp.httpSend(data);

			remoteHttp.httpRecv(data);
			http::responseParser resData(data);
			
			localHttp.httpSend(data);
			/* Check if the remote http server also agreed to the keepAlive request sent earlier in this code.
				If Yes, set the remote http socket to keepAlive and now repeat the transmission between the local
				http client and the remote http client. */
			if (false) { //resData.isKeepAlive()) {
				remoteHttp.peer.setKeepAlive(true);
				int bytes;
				int inLocal = localHttp.peer.availToRead();
				int inRemote = remoteHttp.peer.availToRead();
				do {
					localHttp.httpRecv(data);
					if (data.empty())
						break;
					remoteHttp.httpSend(data);
					remoteHttp.httpRecv(data);
					localHttp.httpSend(data);
				} while (bytes = localHttp.peer.availToRead());
			}

			return; //DONE
		}
	}
	catch (net::SocketException& e) {
		std::cout << "[*] Proxy failed to send/recv data to/from the remote server. or to the local client.\n";
		e.display();
	}
	int inLocal = localHttp.peer.availToRead();
	int inRemote = remoteHttp.peer.availToRead();
	std::cout << "[*] Can't process HTTPS... Not implemented yet.\n";
	/* Close the connection here for now until https is implemented. */
	localHttp.peer.shutdown(0); localHttp.peer.close();
	remoteHttp.peer.shutdown(0); remoteHttp.peer.close();
	return;

	if (remotePeer.isValid()) {
		localHttp.httpSend(OK_200_KEEPALIVE);
	}
	
	try {
		while (localPeer.getLastError() == 0 && remotePeer.getLastError() == 0) {

			localHttp.httpRecv(data);
			if (data.empty())
				break;
			std::cout << "From Local: " << localPeer.getPeerAddr()
				<< " " << localPeer.getPeerPort() << std::endl;

			remoteHttp.httpSend(data);
			std::cout << "To Remote: " << remotePeer.getPeerAddr()
				<< " " << remotePeer.getPeerPort() << std::endl;

			remoteHttp.httpRecv(data);
			std::cout << "From Remote: " << remotePeer.getPeerAddr()
				<< " " << remotePeer.getPeerPort() << std::endl;

			localHttp.httpSend(data);
			std::cout << "To Local: " << localPeer.getPeerAddr()
				<< " " << localPeer.getPeerPort() << std::endl;

		}
		std::cout << "[*] Exited the loop...\n";
	}
	catch (net::SocketException& e) {
		e.display();
	}
	catch (...) {
		std::cout << "Unexpected error caught\n";
	}
	remotePeer.shutdown(0); remotePeer.close();
	localPeer.shutdown(0); localPeer.close();
}

/* ************* */
/* Get transmission status */
int serverDB::HttpSocket::getTransmissionStatus(void) {
	return transmissionStatus;
}

/* Set Buffer Size */
void serverDB::HttpSocket::resizeBufferSize(unsigned int bufferSize) {
	delete this->transBuffer;
	this->transBufferSize = bufferSize;
	this->transBuffer = new char[bufferSize];
}

/* RECEIVE METHOD */
void serverDB::HttpSocket::httpRecv(std::string& data) {
#ifdef _WIN32
	int SOCKET_TIMEOUT = WSAEWOULDBLOCK;
#else
	int SOCKET_TIMEOUT = EWOULDBLOCK;
#endif
	data.clear();
	int bytes, totalBytes = 0;
	int keepAliveRes;
	//char* buf = new char[2000];
	//std::memset(buf, 0, 2000);
	
	bool isBlocking = this->peer.isBlocking();
	/* Socket will not timeout unless peer.setNonBlocking(true) is executed first. */
	this->peer.setRecvTimeout(3);
	if (this->peer.isBlocking())
		this->peer.setNonBlocking(true);

	bytes = this->peer.recv(this->transBuffer /* + totalBytes*/,
		this->transBufferSize /* - totalBytes*/);
	if (bytes > 0)
		data.append(this->transBuffer);

	do {
		std::memset(this->transBuffer, 0, this->transBufferSize);
		try {
			if (!this->peer.availToRead())
				break;
			bytes = this->peer.recv(this->transBuffer /* + totalBytes*/,
				this->transBufferSize /* - totalBytes*/);
			totalBytes += bytes;
			if (bytes > 0)
				data.append(this->transBuffer);

			
		}
		catch (net::SocketException& e) {
			std::cout << "[*] TIMEOUT: ";
			e.display();
			/* If recv didn't not timeout then an error occured. */
			if (e.getErrorCode() != SOCKET_TIMEOUT)
				throw;
			bytes = 0;
		}

	} while (bytes != 0);
	if (isBlocking) // if not blocking; set back to blocking.
		this->peer.setNonBlocking(false);

	//Set back to default.
	this->peer.setRecvTimeout(peer.getDefaultTimeout());
	//keepAliveRes = this->peer.setKeepAlive(false);
}

/* SEND METHOD*/
void serverDB::HttpSocket::httpSend(const std::string& data) {
	int dataSize = data.length();
	std::string::size_type pos = 0;
	/*int bytes = peer.send((const char*)data.substr(pos, transBufferSize).data(),
		transBufferSize);*/
	while (pos <= dataSize) {
		int bytes = peer.send((const char*)data.substr(pos, transBufferSize).data(),
			transBufferSize);
		pos += transBufferSize;
	}
	int h = 1;	
}
