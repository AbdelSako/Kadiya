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

#include <TCPsocket.hpp>

#if !defined(__net_TCPclient)
#define __net_TCPclient

namespace net
{
	/* Not done with this class yet */
	class TCPclient: public TCPsocket {
	private:
		struct addrinfo* m_hints, * m_remoteAddrInfo, * m_remoteAddrPtr;
		struct net::PeerInfo *peerInfo;
		char* ipstr;
	public:
		TCPclient(void): TCPsocket() {
			this->m_hints = new addrinfo;
			this->m_remoteAddrInfo = new addrinfo;
			//this->m_remoteAddrPtr = new addrinfo;
			this->peerInfo = new PeerInfo;
			this->ipstr = new char[INET6_ADDRSTRLEN];

		}
		~TCPclient(void) {
			delete this->m_hints;
			delete this->m_remoteAddrInfo;
			//delete this->m_remoteAddrPtr;
			delete this->peerInfo;
			delete ipstr;
		}
			TCPpeer connect(const std::string remoteAddr, uint16_t port);

	};
};
#endif
