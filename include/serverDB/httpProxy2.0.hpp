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
#include "handlers/HTTPhandlers.hpp"

namespace serverDB
{
	void httpProxyServer(net::TCPpeer *localPeer);
	void testServer(net::TCPpeer* localpeer);
	void echoServer(net::TCPpeer* peer);

	class HttpSocket
	{
	private:
		unsigned int recvBufferSize = 10000;
		unsigned int sendBufferSize = 10000;
		unsigned int classBuffersize = 10000;

		net::TCPpeer *peer;
		std::string *dataReceived;
		std::string *dataToSend;
		char* classBuffer;
		int transmissionStatus = 0;

	public:
		/* Constructor */
		HttpSocket(net::TCPpeer *peer) {
			this->peer = peer;
			dataReceived = new std::string[recvBufferSize];
			dataToSend = new std::string[sendBufferSize];
			classBuffer = new char[classBuffersize];

		}

		/* Get transmission status */
		int getTransmissionStatus(void);
		/* Set data to send */
		void setDataToSend(std::string data);

		/* Get received data */
		std::string getDataReceived(void);

		/* RECEIVE METHOD */
		void httpRecv(void);
		void httpRecv(std::string& data);

		void httpSend(void);
		void httpSend(const std::string& data);

		/* Set receive buufer size */
		void setRecvBufferSize(unsigned int recvBufferSize) {
			this->recvBufferSize = recvBufferSize;
		}

		/* Send buffer Size */
		void setSendBufferSize(unsigned int sendBufferSize);

		/* Get and set buffer size */
		unsigned int getRecvBufferSize(void);
		unsigned int getSendBufferSize(void);

		/* Destructor */
		~HttpSocket(void)
		{
			delete[] dataReceived;
			delete[] dataToSend;
			delete [] classBuffer;
		}
	};

	//HttpProxy::HttpProxy(net::TCPpeer &localPeer) {};
	

};
#endif