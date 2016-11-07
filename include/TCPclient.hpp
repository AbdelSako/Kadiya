#include <TCPsocket.hpp>

#if !defined(__net_TCPclient)
#define __net_TCPclient

namespace net
{
	/* Not done with this class yet */
	class TCPclient: public net::TCPsocket {
		public:
			TCPclient(void) : net::TCPsocket(-1) {}

			int connect(const std::string remoteAddr, uint16_t port);

			int getPeerInfo(struct net::PeerInfo& peerInfo) {
                return net::TCPsocket::getPeerInfo(peerInfo);
			}
		private:
			int m_Type;
	};
};
#endif
