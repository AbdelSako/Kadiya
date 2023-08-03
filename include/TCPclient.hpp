/*
					GNU GENERAL PUBLIC LICENSE
					   Version 2, June 1991
*/
#ifndef __TCPclient
#define __TCPclient
#include <TCPsocket.hpp>
#include "__TCPsocket.hpp"

namespace net
{
	/* Not done with this class yet */
	class TCPclient :TCPsocket {
	private:
		struct addrinfo* m_hints;
		struct addrinfo* m_remoteAddrInfo;
		struct addrinfo* m_remoteAddrPtr;
		char* ipstr;

		struct PeerInfo* peerInfo;
	public:
		TCPclient(void): TCPsocket() {
			this->m_hints = new addrinfo;
			this->m_remoteAddrInfo = new addrinfo;
			this->m_remoteAddrPtr = new addrinfo;
			this->peerInfo = new PeerInfo;
			this->ipstr = new char[INET6_ADDRSTRLEN];

		}
		~TCPclient(void) {
			if (this->isValid()) {
				this->shutdown(0);
				this->close();
			}
			delete this->m_hints;
			delete this->m_remoteAddrInfo;
			delete this->m_remoteAddrPtr;
			delete this->peerInfo;
			delete ipstr;
		}
			TCPpeer connect(const std::string remoteAddr, uint16_t port);
	};
};
#endif
