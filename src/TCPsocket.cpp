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

#include "TCPsocket.hpp"
#include "SocketException.hpp"

/* Definition of net::TCPsocket::socket */
int net::TCPsocket::socket(void)
{
#ifdef _WIN32
	// Initialize Winsock
	//WSADATA wsaData;
	this->m_sockResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (this->m_sockResult != 0) {
		throw net::SocketException("net::TCPsocket::socket(): WSAStartup failed with error:", m_sockResult);
	}
#endif
	this->m_sockfd = ::socket(this->addrFamily, SOCK_STREAM, 0);
	if(!this->isValid())
		throw net::SocketException("net::TCPsocket::socket()", this->getLastError());

#ifdef _WIN32
	const char optval = 1;
#else
	int optval = 1;
#endif
	socklen_t optlen = sizeof optval;

	if(
		(this->addrFamily == AF_INET ?
			::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen) :
			::setsockopt(m_sockfd, IPPROTO_IPV6, SO_REUSEADDR, &optval, optlen)
		) == -1) {
		throw SocketException("::setsockopt() in net::TCPsocket::socket()", this->getLastError());
	}
	return this->m_sockfd;
}


/* Definition of net::TCPsocket::bind */
int net::TCPsocket::bind(const char *bindAddr, uint16_t port)

{
	int inet_pton_result;

	switch (addrFamily) {
		case AF_INET:
			std::memset(&m_localSockAddr, 0, sizeof m_localSockAddr);
			m_localSockAddr.sin_family = addrFamily;
			m_localSockAddr.sin_port = htons(port);
			if(bindAddr)
				inet_pton_result = ::inet_pton(addrFamily, bindAddr, &m_localSockAddr.sin_addr);
			else
				m_localSockAddr.sin_addr.s_addr = INADDR_ANY;
			break;
		case AF_INET6:
			std::memset(&m_localSockAddr6, 0, sizeof m_localSockAddr6);
			m_localSockAddr6.sin6_flowinfo = 0;
			m_localSockAddr6.sin6_family = addrFamily;
			m_localSockAddr6.sin6_port = htons(port);
			if(bindAddr)
				inet_pton_result = ::inet_pton(addrFamily, bindAddr, &m_localSockAddr6.sin6_addr);
			else
				m_localSockAddr6.sin6_addr = ::in6addr_any;
			break;
		default:
			throw SocketException("net::TCPsocket::bind()",
				"Unknown Address Family...not implemented yet.");
			break;
	}

	if(bindAddr) {
		if(inet_pton_result == 0) {
			throw SocketException("::inet_pton() in net::TCPsocket::bind()", "Bind address is invalid");
		}
		else if(inet_pton_result == -1) {
			throw SocketException("::inet_pton() in net::TCPsocket::bind()", this->getLastError());
		}
	}

	switch (addrFamily) {
	case AF_INET:
		this->m_sockResult = ::bind(m_sockfd,
			(struct sockaddr *)&m_localSockAddr, sizeof m_localSockAddr);
		break;
	case AF_INET6:
		this->m_sockResult = ::bind(m_sockfd,
			(struct sockaddr *)&m_localSockAddr6, sizeof m_localSockAddr6);
		break;
	}

	if(this->m_sockResult == -1) {
		throw SocketException("net::TCPsocket::bind()", this->getLastError());
	}
	else return 0;
}

/* POLL METHOD ___________________*/
void net::TCPpeer::recvPoll(u_int timeout)
{
#ifdef _WIN32
	int nfds = 1;
	fd_set readfds;
	timeval timeVal;
	int maxfds = this->m_sockfd + 1;

	std::memset((char*)&timeVal, 0, sizeof(timeVal));
	timeVal.tv_sec = timeout;
	timeVal.tv_usec = 0;
#else
	struct pollfd pollfds[1];

	std::memset((char*)pollfds, 0, sizeof(pollfds));
	pollfds[0].fd = this->m_sockfd;
#endif

	if (!this->isBlocking()) {
#ifdef _WIN32
		std::memset((char*)&readfds, 0, sizeof(fd_set));
		readfds.fd_array[0] = this->m_sockfd;
		readfds.fd_count = 1;
		this->m_sockResult = select(maxfds, &readfds, 0, 0, &timeVal);
		if (this->m_sockResult < 0)
			throw net::SocketException("poll(): ", this->getLastError());
#else
		pollfds[0].events = POLLIN | POLLERR;
		/* unlike select(), ::poll()'s timeout is in millisecond */
		this->m_sockResult = ::poll(pollfds, 1, this->recvTimeout * 1000);
#endif
	}
}

/* SEND POLL */
void net::TCPpeer::sendPoll(u_int timeout) {
#ifdef _WIN32
	int nfds = 1;
	fd_set writefds;
	timeval timeVal;
	int maxfds = this->m_sockfd + 1;

	std::memset((char*)&timeVal, 0, sizeof(timeVal));
	timeVal.tv_sec = timeout;
	timeVal.tv_usec = 0;
#else
	struct pollfd pollfds[1];

	std::memset((char*)pollfds, 0, sizeof(pollfds));
	pollfds[0].fd = this->m_sockfd;
#endif

	if (!this->isBlocking()) {
#ifdef _WIN32
		std::memset((char*)&writefds, 0, sizeof(fd_set));
		writefds.fd_array[0] = this->m_sockfd;
		writefds.fd_count = 1;
		this->m_sockResult = select(maxfds, 0, &writefds, 0, &timeVal);
		if (this->m_sockResult < 0)
			throw net::SocketException("poll(): ", this->getLastError());
#else
		pollfds[0].events = POLLOUT | POLLERR;
		/* Unlike select (), ::poll()'s timeout is in millisecond */
		this->m_sockResult = ::poll(pollfds, 1, this->sendTimeout * 1000);
#endif

	}

	if (this->m_sockResult < 0) {
		throw net::SocketException("net::TCPsocket::poll():", this->getLastError());
	}
}

/* Definition of net::TCPsocket::setNonBlocking */
void net::TCPsocket::setNonBlocking(bool nonBlocking)
{
#ifdef _WIN32
	//Winsock doesn't provide a way to check if blocking or non-blocking is set.
	u_long iMode = (u_long)nonBlocking;

	m_sockResult = ioctlsocket(m_sockfd, FIONBIO, &iMode);
	if (m_sockResult != NO_ERROR)
		throw net::SocketException("net::TCPsocket::setNonBlocking", this->getLastError());

#else
	const int flags = fcntl(m_sockfd, F_GETFL, 0);
	if(flags == -1)
		throw net::SocketException("GET TCPsocket::fcntl()", this->getLastError());

	if(this->m_sockResult = fcntl(
		m_sockfd, F_SETFL, nonBlocking ? 
		(flags | O_NONBLOCK) : 
		(flags & ~O_NONBLOCK) ) == -1)
			throw net::SocketException("SET TCPsocket::fcntl()", this->getLastError());
#endif
	this->m_isBlocking = !nonBlocking;
}



/* NEW RECEIVE METHOD */
int net::TCPpeer::recv(char* inBuffer, uint16_t inBufSize)
{
	ssize_t byteRecv;

	std::memset(inBuffer, 0, inBufSize);

	if (!this->isBlocking())
		this->recvPoll(this->recvTimeout);

	byteRecv = ::recv(m_sockfd, inBuffer, inBufSize, 0);

	if (this->getLastError() != 0) {
		throw net::SocketException("net::TCPsocket::recv", this->getLastError());
	}

	return byteRecv;
}

/* SEND METHOD*/
int net::TCPpeer::send(const std::string outBuffer, uint16_t outBufSize)
{
	ssize_t byteSent;
	if (!this->isBlocking())
		this->sendPoll(this->sendTimeout);

	byteSent = ::send(m_sockfd, outBuffer.data(), outBufSize, 0);

	if (this->getLastError() != 0)
		throw net::SocketException("net::TCPsocket::send()", this->getLastError());
	return byteSent;
}

/*  SET RECV TIMEOUT */
void net::TCPpeer::setRecvTimeout(u_int timeout) {
	this->recvTimeout = timeout;

	if (timeout < 0) {
		this->setNonBlocking(false);
	}
	else {
		this->setNonBlocking(true);
	}
}

/* SET SEND TIMEOUT */
void net::TCPpeer::setSendTimeout(u_int timeout) {
	this->sendTimeout = timeout;

	if (timeout < 0) {
		this->setNonBlocking(false);
	}
	else {
		this->setNonBlocking(true);
	}
}

/* GET LAST ERROR METHOOD*/
int net::TCPsocket::getLastError(void) {
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

bool net::TCPpeer::isBlocking(void) {
	/* TODO: This method will be implemented later, for now let's just return true */
	return this->m_isBlocking;
}

/* flags's method will soon be removed because it's making the class complicated */
int net::TCPsocket::flags(net::flags what)
{
    switch(what)
    {
    case GET_WILL_CLOSE_SOCKET:
        return this->m_flags[what];

    case GET_KEEP_ALIVE:
        return this->m_flags[what];

    case GET_TRANS_BUFFER:
        return this->m_flags[what];

    case GET_RECV_TIMEOUT:
        return this->m_flags[what];

    case GET_SEND_TIMEOUT:
        return this->m_flags[what];

    default:
        return -1;
    }
}

int net::TCPsocket::flags(net::flags what, int value)
{
    switch(what)
    {
    case SET_WILL_CLOSE_SOCKET:
        this->m_flags[what] = value;
        return 0;

    case SET_KEEP_ALIVE:
        if((net::TCPsocket::setKeepAlive((bool)value)) == -1)
            return -1;
        else
            return 0;

    case SET_TRANS_BUFFER:
        if(this->m_flags[what] != value) {
            if(value < 1) return -1;

            this->m_flags[what] = value;
            return 0;
        }
        return 0;

    case SET_RECV_TIMEOUT:
        this->m_flags[what] = value;
        return 0;

    case SET_SEND_TIMEOUT:
        this->m_flags[what] = value;
        return 0;

    default:
        return -1;
    }
}

/* */
int net::TCPsocket::shutdown(int how)
{
	if(isValid()) {
		if(::shutdown(m_sockfd, how) == -1)
			return -1;
		else
			return 0;
	} else
		return -1;
}

/* CLOSE socket(net::SOCKET) */
int net::TCPsocket::close(void)
{
	if (isValid()) {
#ifdef _WIN32
		::closesocket(this->m_sockfd);
		return 0;
#else
		//m_sockResult = ::close(this->m_sockfd);
		if (::close(this->m_sockfd) == 0)
			return 0;
		else
			return -1;
#endif
	}
	else
		return -1;
}

struct net::PeerInfo net::TCPsocket::getPeerInfo(void)
{
    return peerInfo;
}

std::string net::TCPsocket::getPeerAddr(void) {
	return this->peerInfo.addr;
}
uint32_t net::TCPsocket::getPeerPort(void) {
	return this->peerInfo.port;
}



/* keep alive */
int net::TCPsocket::setKeepAlive(bool keep_alive)
{
#ifdef _WIN32
	const char optval = (int)keep_alive;
#else
	int optval = (int)keep_alive;
#endif
	if (setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0)
		return -1;

/*
	optval = 2;
	if (setsockopt(m_sockfd, SOL_TCP, TCP_KEEPCNT, &optval, sizeof(optval)) < 0)
		return -1;

	optval = 5;
	if (setsockopt(m_sockfd, SOL_TCP, TCP_KEEPIDLE, &optval, sizeof(optval)) < 0)
		return -1;

	optval = 2;
	if (setsockopt(m_sockfd, SOL_TCP, TCP_KEEPINTVL, &optval, sizeof(optval)) < 0)
		return -1;
*/

	int res;

	keep_alive ? res = 1 : res = 0;
	return res;
}
