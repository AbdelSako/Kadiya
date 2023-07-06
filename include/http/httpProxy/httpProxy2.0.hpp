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

#ifndef __httpProxy
#define __httpProxy

#include "TCPclient.hpp"
#include "handlers/http.hpp"

namespace serverDB
{
	void httpProxyServer(net::TCPpeer localPeer);
	void testServer(net::TCPpeer localpeer);
	void echoServer(net::TCPpeer peer);


	class HttpSocket
	{
	public:
		net::TCPpeer peer;
	private:
		unsigned int transBufferSize = 5000;
		char* transBuffer;
		int transmissionStatus = 0;
		u_int timeout = this->peer.getDefaultTimeout() - 4;
	public:
		/* Constructor */
		HttpSocket(net::TCPpeer& peer) {
			this->peer = peer;
			transBuffer = new char[transBufferSize];

		}

		u_int getTimeout() {
			return this->timeout;
		}

		void setTimeout(u_int timeout) {
			this->timeout = timeout;
		}

		/* Get transmission status */
		int getTransmissionStatus(void);

		/* RECEIVE METHOD */
		void httpRecv(void);
		void httpRecv(std::string& data);

		void httpSend(void);
		void httpSend(const std::string& data);

		int recvAndSend(HttpSocket from, HttpSocket& to) {
			int buffersize = 500;
			char fromToBuffer[500];
			int n;
			while (n = from.peer.availToRead()) {
				std::memset(fromToBuffer, 0, buffersize);
				from.peer.recv(fromToBuffer, buffersize);
				std::cout << "From: " << from.peer.getPeerAddr()
					<< " " << from.peer.getPeerPort() << std::endl;
				to.peer.send(fromToBuffer, buffersize);
				std::cout << "To " << to.peer.getPeerAddr()
					<< " " << to.peer.getPeerPort() << std::endl;
			}
			return 0;
		}

		static void proxyIT(HttpSocket& conn1, HttpSocket& conn2, std::string data) {
			conn1.httpRecv(data);
			conn2.httpSend(data);
			conn2.httpRecv(data);
			conn1.httpSend(data);
		}

		/* CLEAR BUFFER */
		void clearBuffer(void) {
			std::memset(this->transBuffer, 0, transBufferSize);
		}

		/* Set receive buffer size */
		void resizeBufferSize(unsigned int bufferSize);

		/* Destructor */
		~HttpSocket(void)
		{
			delete [] transBuffer;
		}
	};

	//HttpProxy::HttpProxy(net::TCPpeer &localPeer) {};

};
#endif