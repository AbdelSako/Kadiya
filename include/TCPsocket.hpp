#if !defined(__net_TCPsocket)
#define __net_TCPsocket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <cerrno>
#include <exception>

#include <fcntl.h>
#include <csignal>

#include "__TCPsocket.hpp"
#include "SocketException.hpp"
#include <iostream>

namespace callbacks
{
	struct Server;
};

namespace net
{
    typedef int SOCKET;

	class TCPsocket
	{
		public:
			/* Initializes the object with default flags */
			TCPsocket(int Family):
				addrFamily(Family)
            {
                this->m_flags[SET_WILL_CLOSE_SOCKET] = 1;
                this->m_flags[SET_KEEP_ALIVE] = 0;
                this->m_flags[SET_TRANS_BUFFER] = 1000*30;
                this->m_flags[SET_RECV_TIMEOUT] = 1000*10;
                this->m_flags[SET_SEND_TIMEOUT] = 1000*10;
            }

			/* copies a connected tcp socket */
			TCPsocket(SOCKET peerSock, struct net::PeerInfo peerInfo): m_sockfd(peerSock)
			{
			    this->peerInfo = peerInfo;
			    this->addrFamily = this->peerInfo.af;

                this->m_flags[SET_WILL_CLOSE_SOCKET] = 1;
                this->m_flags[SET_KEEP_ALIVE] = 0;
                this->m_flags[SET_TRANS_BUFFER] = 1000*30;
                this->m_flags[SET_RECV_TIMEOUT] = 1000*10;
                this->m_flags[SET_SEND_TIMEOUT] = 1000*10;
			}

			/* Shuts down and closes the socket */
			~TCPsocket(void) {
				if(isValid()) {
                    if(m_flags[GET_WILL_CLOSE_SOCKET]) {
                        TCPsocket::shutdown(::SHUT_RDWR);
                        TCPsocket::close();
                    }
				}
			}

		protected:
			/* Initializes the socket fd */
			virtual int socket(void) throw (net::SocketException);

			/* binds to host:port
			 * int 0 to bind to any address*/
			virtual int bind(const char *bindAddr, uint16_t port) throw (net::SocketException);

		private:
		    /* */
			short poll(int events, int timeout) throw (net::SocketException);

			/* Blocking*/
			void setNonBlocking(bool non_block) throw (net::SocketException);

			/* This function is a callee of operator>>.
			** Reads data from a connected host.*/
			int read(char *inBuffer, uint16_t inBufSize, int timeout)
				throw (net::SocketException);

			/* This function is a callee of operator<<.
			** Writes data to a connected host.*/
			int write(const std::string outBuffer, uint16_t outBufSize, int timeout)
				throw (net::SocketException);

		public:
			// Reads from a connected host
			const TCPsocket& operator>> (std::string &raw_data);

			// Writes to a connected host
			const TCPsocket& operator<< (const std::string raw_data);

            /* Gets a flag */
			int flags(net::flags what);

			/* Sets flag value */
            int flags(net::flags what, int value);

            struct net::PeerInfo getPeerInfo(void);

            /* Shuts down the socket's read, write or both functions.
            *  Arguments:
                (how)
                    4: to shutdown read.
                    2: to shutdown write.
                    0:  both
            *  Return value:
                -1: If it fails or the socket is invalid
                 0: On success*/
			int shutdown(int how);

			/* Close this socket fd */
			int close(void);

        private:
			/* enable KEEP-ALIVE */
			int setKeepAlive(bool keep_alive) throw (net::SocketException);

        public:
			/* checks whether the socket is valid or not */
			bool isValid(void) const {
				return m_sockfd != -1;
			}
		protected:
			// Class variables
			int addrFamily;
			uint8_t sockProtocol;
			SOCKET m_sockfd = -1;

			struct addrinfo m_hints, *m_remoteAddrInfo, *m_remoteAddrPtr;

			struct sockaddr_in m_localSockAddr;
			struct sockaddr_in6 m_localSockAddr6;

			// Store connected peer's info in human readable
            struct net::PeerInfo peerInfo;

            /* Tells the object when to change its behavior:
            ** Per example, when to change the transmission buffer or
            ** to change the recv/send timeout or to set keep-alive.
            ** You can also get the values that are used by the object
            ** or check if keep-alive is enabled.
            ** Consult "net::TCPsocket::flags" overloaded functions and
            ** "net::flags" enumeration in __TCPsocket.hpp */
			int m_flags[10];

		public:
		    /* The callback could be a function from src/serverDB/* or your
		    ** own code. */
			callbacks::Server *serverCallbacks;
	};


	/* net::TCPpeer takes a connected socket as an argument */
	class TCPpeer: public TCPsocket {
		public:
			//TCPpeer(net::SOCKET peerSockfd) : TCPsocket(peerSockfd) {}
			TCPpeer(net::SOCKET peerSockfd, struct net::PeerInfo peerInfo) : TCPsocket(peerSockfd, peerInfo) {}

		private:
		    /* Making them inaccessible. */
			int socket(void) throw (net::SocketException) {}
			int bind(const char *bindAddr, uint16_t port) throw (net::SocketException) {}
	};

};
#endif
