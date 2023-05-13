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
	HttpSocket localHttp(localPeer);
	net::TCPclient* client = new net::TCPclient();
	std::string hostname, url;
	uint32_t portNumber;
	std::string data;

	/* Receive data from the local host. */
	localHttp.httpRecv(data);
	if (data.empty()) {
		localPeer.shutdown(0); localPeer.close();
		return;
	}

	net::TCPpeer remotePeer;

	http::requestParser recvData(data);

	/* Connect to the remote host. */
	try {
		remotePeer = client->connect(recvData.hostname, recvData.portNumber);
	}
	catch (net::SocketException& e) {
		std::cout << "[*] Proxy failed to connect to the remote server... dropping connection.\n";
		e.display();
		localPeer.shutdown(0); localPeer.close();
		return;
	}

	/* NOW WE CAN START SENDING AND RECEIVING DATA TO/FROM THE SERVER AND THE LOCAL CLIENT. */
	HttpSocket remoteHttp(remotePeer);
	try {
		if (recvData.method.compare("CONNECT") != 0) {//IF NOT CONNECT, HANDLE HTTP
			remoteHttp.httpSend(data);
			remoteHttp.httpRecv(data);
			localHttp.httpSend(data);
			std::cout << "[+] HTTP Success!\n";
			return;
		}
	}
	catch (net::SocketException& e) {
		std::cout << "[c1] Proxy failed to send/recv data to/from the remote server. or to the local client.\n";
		e.display();
		std::cout << "[c1] Dropping this connection.\n";
		remotePeer.shutdown(0); remotePeer.close();
		localPeer.shutdown(0); localPeer.close();
		return;

	}
	// Comment out the next two lines to continue to HTTPS.
	std::cout << "[*] Can't process HTTPS... Not implemented yet.\n";
	return;
	// Handle HTTPS

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
			std::cout << "Size from server: " << data.size() << std::endl;

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
	delete this->classBuffer;
	this->classBuffersize = bufferSize;
	this->classBuffer = new char[bufferSize];
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
	int io_res;
	u_long value;
	
	/* Socket will not timeout unless peer.setNonBlocking(true) is executed first. */
	this->peer.setRecvTimeout(1);
	do {
		std::memset(this->classBuffer, 0, this->classBuffersize);
		try {
			bytes = this->peer.recv(this->classBuffer, this->classBuffersize);
			if (bytes > 0) {
				data.append(this->classBuffer, bytes);
			}
			this->peer.setNonBlocking(true);
			io_res = ::ioctl(this->peer.getSocket(), FIONREAD, &value);
			//if (io_res == 0 && value > 0) {
			//	this->peer.setNonBlocking(true);
			//	continue;
			//}
			//else if (io_res == -1) {
			//	//TODO: Need to work here.
			//	std::cout << "[-] ioctl() failed from within HttpSocket::httpRecv(std::string& data)\n";

			//}


			/*if (bytes > 0 && bytes == this->classBuffersize) {
				if (this->peer.isBlocking())
					this->peer.setNonBlocking(true);
			}
			else {
				break;
			}*/
		}
		catch (net::SocketException& e) {
			std::cout << "[*] Something went wrong with HttpSocket::httpRecv()\n";
			e.display();
			/* If recv didn't not timeout then an error occured. */
			/*if (e.getErrorCode() != SOCKET_TIMEOUT )
				throw;
			bytes = 0;*/
			break;
		}

	} while (bytes > 0);
	if (!this->peer.isBlocking()) // if not blocking; set back to blocking.
		this->peer.setNonBlocking(false);
	//Set back to default.
	this->peer.setRecvTimeout(5);
}

/* SEND METHOD*/
void serverDB::HttpSocket::httpSend(const std::string& data) {
	int s = data.length();
	int bytes = peer.send((const char*)data.data(), data.length());
}
