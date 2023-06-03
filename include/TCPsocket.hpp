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

		struct sockaddr_in *m_localSockAddr;
		struct sockaddr_in6 *m_localSockAddr6;


	public:
		/* Initializes the object with default flags */
		TCPsocket(int Family) : addrFamily(Family) {
		}

		/* copies a connected tcp socket 
			We will instantiate the TCPpeer class */
		TCPsocket(void) {}

		/* Shuts down and closes the socket */
		~TCPsocket(void) {
			/* TODO: Don't use this destructor to close connections, use the TCPpeer 
				child class instead. 
				Not sure what to do with this section yet.*/
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
		//int read(char *inBuffer, uint16_t inBufSize, int timeout);
			
		/* This function is a callee of operator<<.
		** Writes data to a connected host.*/
		//int write(const std::string outBuffer, uint16_t outBufSize, int timeout);

	public:
		/*   */
		/* Blocking*/
		void setNonBlocking(bool non_block);

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

		/* Data available to be read */
		int availToRead();
};






	// CLASS
	/* net::TCPpeer takes a connected socket as an argument */
	class TCPpeer: public TCPsocket
	{
	public:

		TCPpeer(struct net::PeerInfo peerInfo) {
			this->ipAddress = peerInfo.addr;
			this->portNumber = peerInfo.port;
			this->addrFamily = peerInfo.af;
			this->m_sockfd = peerInfo.sockfd;

		}

		TCPpeer(void) {
			this->m_sockfd = -1;
		}

		~TCPpeer(void) {
			//this->killConn();
			/* TODO: This desturctor is executed after the TCPserver::accept() 
			call returns* to one of the start(start(), startServer()) methods. 
			To make this work. I will have to make TCPserver::accept() method return 
			a pointer type insterad of a native type. */
		}


	private:
		std::string ipAddress;
		uint16_t portNumber;
		bool keepAliveStatus = false;
		u_int DEFAULT_TIMEOUT = 5;
		u_int recvTimeout = DEFAULT_TIMEOUT;
		u_int sendTimeout = DEFAULT_TIMEOUT;
	//	private:
		    /* Making them inaccessible. */
		/*	int socket(void) {}
			int bind(const char *bindAddr, uint16_t port) {} */
			/*  */
		void recvPoll(u_int timeout);

		/* Send poll*/
		void sendPoll(u_int timeout);

	public:
		struct net::PeerInfo getPeerInfo(void);

		/* Get default timeout */
		u_int getDefaultTimeout(void) {
			return this->DEFAULT_TIMEOUT;
			
		}
		/* Set recv timeout*/
		void setRecvTimeout(u_int timeout);

		/* Get recv timeout */
		int getRecvTimeout(void) {
			return recvTimeout;
		}

		/*  Set send timeout */
		void setSendTimeout(u_int timeout);

		/* get send timeout */
		int getSendTimeout(void) {
			return sendTimeout;
		}

		/* is the socket in blocking mode */
		bool isBlocking(void);

		/* enable KEEP-ALIVE */
		int setKeepAlive(bool keep_alive);

		/* Get keep Alive */
		bool isKeepAlive();

		/* RECEIVE METHOD*/
		int recv(char* inBuffer, uint16_t inBufSize);

		/* send method*/
		int send(const char* outBuffer, uint16_t outBufSize);

		/**/
		std::string getPeerAddr(void);

		/**/
		uint32_t getPeerPort(void);

		/* Shut down and close connection */
		void killConn(void) {
			this->shutdown(0);
			this->close();
		}
	};

};
#endif
