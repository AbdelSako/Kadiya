/*
					GNU GENERAL PUBLIC LICENSE
					   Version 2, June 1991
*/

#include "TCPsocket.hpp"
#include "SocketException.hpp"
#include <sys/types.h>
#include <cerrno>
#include <exception>
#include <fcntl.h>

#ifdef _WIN32
void net::TCPsocket::startWSA(void) {
	// Initialize Winsock
	//WSADATA wsaData;
	this->m_sockResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (this->m_sockResult != 0) {
		throw net::SocketException("net::TCPsocket::socket(): WSAStartup failed with error:", m_sockResult);
	}
}

void net::TCPsocket::cleanWSA(void) {
	WSACleanup();
}
#endif


/* Definition of net::TCPsocket::socket */
int net::TCPsocket::socket(void)
{
#ifdef _WIN32
	net::TCPsocket::startWSA();
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
			m_localSockAddr = new sockaddr_in;
			std::memset(m_localSockAddr, 0, sizeof m_localSockAddr);
			m_localSockAddr->sin_family = addrFamily;
			m_localSockAddr->sin_port = htons(port);
			if(bindAddr)
				inet_pton_result = ::inet_pton(addrFamily, bindAddr, &m_localSockAddr->sin_addr);
			else
				m_localSockAddr->sin_addr.s_addr = INADDR_ANY;
			break;
		case AF_INET6:
			m_localSockAddr6 = new sockaddr_in6;
			std::memset(m_localSockAddr6, 0, sizeof m_localSockAddr6);
			m_localSockAddr6->sin6_flowinfo = 0;
			m_localSockAddr6->sin6_family = addrFamily;
			m_localSockAddr6->sin6_port = htons(port);
			if(bindAddr)
				inet_pton_result = ::inet_pton(addrFamily, bindAddr, &m_localSockAddr6->sin6_addr);
			else
				m_localSockAddr6->sin6_addr = ::in6addr_any;
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
		this->m_sockResult = ::bind(this->m_sockfd,
			(struct sockaddr*)m_localSockAddr, sizeof(sockaddr));
		delete m_localSockAddr;
		break;
	case AF_INET6:
		this->m_sockResult = ::bind(this->m_sockfd,
			(struct sockaddr*)m_localSockAddr6, sizeof(sockaddr));
		delete m_localSockAddr6;

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

	m_sockResult = ioctlsocket(this->m_sockfd, FIONBIO, &iMode);
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

	byteRecv = ::recv(m_sockfd, inBuffer, inBufSize - 12, 0);
	//byteRecv = ::read(m_sockfd, inBuffer, inBufSize - 12);
	if (byteRecv == -1)
		//this->m_sockResult = -1;
		this->setStatus(this->getLastError());

	//TODO: I wonder is this method should throw an Exception.
	/*if (this->getLastError() != 0) {
		throw net::SocketException("net::TCPsocket::recv(); Port "+(std::to_string(this->getPeerPort()))
			+"; Error: ", this->getLastError());
	}*/

	return byteRecv;
}

/* SEND METHOD*/
int net::TCPpeer::send(const char* outBuffer, uint16_t outBufSize)
{
	ssize_t byteSent;
	if (!this->isBlocking())
		this->sendPoll(this->sendTimeout);

	byteSent = ::send(m_sockfd, outBuffer, outBufSize, 0);
	//byteSent = ::write(m_sockfd, (void*)outBuffer, outBufSize);

	if (byteSent == -1)
		//this->m_sockResult = -1;
		this->setStatus(this->getLastError());

	//TODO: I wonder if this method should throw an Exception.
	/*if (this->getLastError() != 0)
		throw net::SocketException("net::TCPsocket::send()", this->getLastError());*/

	return byteSent;
}

/*  SET RECV TIMEOUT */
void net::TCPpeer::setRecvTimeout(u_int timeout) {
	this->recvTimeout = timeout;
}

/* SET SEND TIMEOUT */
void net::TCPpeer::setSendTimeout(u_int timeout) {
	this->sendTimeout = timeout;
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

//struct net::PeerInfo net::TCPpeer::getPeerInfo(void)
//{
//    return *peerInfo;
//}

std::string net::TCPpeer::getPeerAddr(void) {
	return this->ipAddress;
}
uint32_t net::TCPpeer::getPeerPort(void) {
	return this->portNumber;
}



/* keep alive */
int net::TCPpeer::setKeepAlive(bool keep_alive)
{
#ifdef _WIN32
	const char optval = (int)keep_alive;
#else
	int optval = (int)keep_alive;
#endif
	if (setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0)
		return -1;
	this->setNonBlocking(true);

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
	this->keepAliveStatus = res;
	return res;
}

/* Get Keep Alive Status */
bool net::TCPpeer::isKeepAlive() {
	return this->keepAliveStatus;
}

/* Available to read */
int net::TCPsocket::availToRead() {
	u_long value;
	int reio = ::ioctl(this->m_sockfd, FIONREAD, &value);
	if (reio == 0)
		return value;
	else
		throw net::SocketException("net::TCPsocket::availToRead()", this->getLastError());
}

int net::TCPsocket::getStatus(void) {
	return status;
}

void net::TCPsocket::setStatus(int status) {
	this->status = status;
}