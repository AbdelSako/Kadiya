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
//#define EWOULDBLOCK WSAEWOULDBLOCK

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
	class TCPsocket
	{
	protected:
		// Class variables
#ifdef _WIN32
		WSADATA m_wsaData;
#endif
		int addrFamily;
		uint8_t sockProtocol;
		::SOCKET m_sockfd = -1;
		int m_sockResult;

	public:
		/* Use this constructor when instantiating the TCPserver object */
		/* @Family: AF_INET or AF_INET6 */
		TCPsocket(int Family): addrFamily(Family){}

		/* Use this constructor when instantiating the TCPpeer class */
		TCPsocket(void) {}

		/* Shuts down and closes the socket */
		~TCPsocket(void) {
			if(isValid())
				this->close();
		}

	private:
#ifdef _WIN32
		void WSAStartup(void);
#endif
	protected:
		/* Initializes the socket fd */
		virtual int socket(void);

		/* binds to host:port
			* int 0 to bind to any address*/
		virtual int bind(const char *bindAddr, uint16_t port) ;

	public:
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

    public:
		/* checks whether the socket is valid or not */
		bool isValid(void) const {
			return m_sockfd != -1;
		}

		/* Check for error after an operation */
		int getLastError(void);

		std::string getPeerAddr(void);
		uint32_t getPeerPort(void);
};


	/* net::TCPpeer takes a connected socket as an argument */
	class TCPpeer: public TCPsocket
	{
	private:
		std::string ipAddress;
		uint16_t portNumber;
		bool m_isBlocking = true;
		u_int recvTimeout = 10;
		u_int sendTimeout = 10;

	public:
		TCPpeer(struct net::PeerInfo &peerInfo) {
			this->ipAddress = peerInfo.addr;
			this->portNumber = peerInfo.port;
			this->addrFamily = peerInfo.af;
			this->m_sockfd = peerInfo.sockfd;

		}

	private:
		/* Making them inaccessible. */
		/*int socket(void) {}
		int bind(const char *bindAddr, uint16_t port) {} */
			/*  */
		void recvPoll(u_int timeout);

		/* Send poll*/
		void sendPoll(u_int timeout);

		/* Set recv timeout*/
		void setRecvTimeout(u_int timeout);

		/*  Set send timeout */
		void setSendTimeout(u_int timeout);

		/* is the socket in blocking mode */
		bool isBlocking(void);

		/* enable KEEP-ALIVE */
		/* Not fully implemented yet */
		int setKeepAlive(bool keep_alive);
	public:
		/* RECEIVE METHOD*/
		int recv(char* inBuffer, uint16_t inBufSize);

		/* send method*/
		int send(const std::string outBuffer, uint16_t outBufSize);

		/*   */
		/* Blocking*/
		void setNonBlocking(bool non_block);
	};

};
#endif
