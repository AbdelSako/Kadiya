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

#if !defined(__net_TCPsocket)
#define __net_TCPsocket

#ifdef _WIN32

#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define SHUT_RDWR SD_BOTH
#define ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT //Socket type not supported
#define ssize_t INT64
#define ioctl ioctlsocket
#define SIGPIPE EPIPE
#define EWOULDBLOCK WSAEWOULDBLOCK

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>

#endif

#include <sys/types.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <exception>

#include <fcntl.h>
#include <csignal>

#include "__TCPsocket.hpp"
#include "SocketException.hpp"
#include <iostream>

namespace net
{
    typedef int SOCKET;

	class TCPsocket
	{
		protected:
			// Class variables
#ifdef _WIN32
			WSADATA m_wsaData;
#endif
			int addrFamily;
			uint8_t sockProtocol;
			SOCKET m_sockfd = -1;
			int m_sockResult;
			bool m_isBlocking = true;
			/* "int timeout" member controls the "bool m_isBlocking" variable */
			u_int recvTimeout = 10;
			u_int sendTimeout = 10;

			struct addrinfo m_hints, * m_remoteAddrInfo, * m_remoteAddrPtr;

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
						this->shutdown(SHUT_RDWR);
                        this->close();
                    }
				}
			}

		protected:
			/* Initializes the socket fd */
			virtual int socket(void);

			/* binds to host:port
			 * int 0 to bind to any address*/
			virtual int bind(const char *bindAddr, uint16_t port) ;

		private:

			/* This function is a callee of operator>>.
			** Reads data from a connected host.*/
			int read(char *inBuffer, uint16_t inBufSize, int timeout);

			

			/* This function is a callee of operator<<.
			** Writes data to a connected host.*/
			int write(const std::string outBuffer, uint16_t outBufSize, int timeout);

			/*  */
			void recvPoll(u_int timeout);

			/* Send poll*/
			void sendPoll(u_int timeout);

		public:
			/*   */
			/* Blocking*/
			void setNonBlocking(bool non_block);

			/* RECEIVE METHOD*/
			int recv(char* inBuffer, uint16_t inBufSize);

			/* send method*/
			int send(const std::string outBuffer, uint16_t outBufSize);

			// Reads from a connected host
			const TCPsocket& operator>> (std::string &raw_data);

			// Writes to a connected host
			const TCPsocket& operator<< (const std::string raw_data);

            /* Gets a flag */
			int flags(net::flags what);

			/* Sets flag value */
            int flags(net::flags what, int value);

			/*    */
            struct net::PeerInfo getPeerInfo(void);

			/* Set recv timeout*/
			void setRecvTimeout(u_int timeout);

			/*  Set send timeout */
			void setSendTimeout(u_int timeout);

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
			int setKeepAlive(bool keep_alive) ;

        public:
			/* checks whether the socket is valid or not */
			bool isValid(void) const {
				return m_sockfd != -1;
			}

			bool isBlocking(void);

			/* Check for error after an operation */
			int getLastError(void);

			std::string getPeerAddr(void);
			uint32_t getPeerPort(void);
	};


	/* net::TCPpeer takes a connected socket as an argument */
	class TCPpeer: public TCPsocket {
		public:
			//TCPpeer(net::SOCKET peerSockfd) : TCPsocket(peerSockfd) {}
			TCPpeer(net::SOCKET peerSockfd, struct net::PeerInfo peerInfo) : TCPsocket(peerSockfd, peerInfo) {}

	//	private:
		    /* Making them inaccessible. */
		/*	int socket(void) {}
			int bind(const char *bindAddr, uint16_t port) {} */
	};

};
#endif
