/*
                    GNU GENERAL PUBLIC LICENSE
                       Version 2, June 1991
*/

#include "TCPclient.hpp"
#include <iostream>
#include "SocketException.hpp"

/* CONNECT TO REMOTE HOST.
the string "remoteAddr" could be a domain name or IPv4/IPv6 address */
net::PeerSocket net::ClientSocket::connect(const std::string &remoteAddr, uint16_t port)
{
    struct addrinfo m_hints, *m_remoteAddrInfo, *m_remoteAddrPtr;
    struct net::PeerInfo peerInfo;
    peerInfo.sockfd = -1;
     std::memset(&m_hints, 0, sizeof m_hints);
     //std::memset(m_remoteAddrInfo, 0, sizeof m_remoteAddrInfo);
     m_hints.ai_family = AF_UNSPEC;
     m_hints.ai_socktype = SOCK_STREAM;
     int m_sockfd;

     /* Get the remoteAddr address info.
     If "remoteAddr" is a domain name, then the struct "m_remoteAddrInfo" will be
     populated by all IP addresses(v4 & v6) that point to that domain name.
     Otherwise, the struct will contain only the IPv4/IPv6 address you passed as argument. */
     int gai_ret;
     if((gai_ret = ::getaddrinfo(remoteAddr.data(), std::to_string(port).data(),
               &m_hints, &m_remoteAddrInfo)) != 0) {
    	 std::cout << "[*] net::TCPclient::connect() --> ::getadddrinfo(): "
    			 <<::gai_strerror(gai_ret) << "; " << remoteAddr << ":"
    			 << port << '\n';
    	 return PeerSocket(peerInfo);
     }

     struct pollfd pollfds[1];
     std::memset((char *) &pollfds, 0, sizeof(pollfds));

     /* If remoteAddr is a domain name, the loop will break after connection has been
     established. */
     for(m_remoteAddrPtr = m_remoteAddrInfo; m_remoteAddrPtr != NULL;
               m_remoteAddrPtr = m_remoteAddrPtr->ai_next) {
          m_sockfd = -1;
          if ((m_sockfd = ::socket(m_remoteAddrPtr->ai_family, m_remoteAddrPtr->ai_socktype,
                    m_remoteAddrPtr->ai_protocol)) == -1) {
               continue;
          }

          if(m_remoteAddrPtr->ai_socktype != SOCK_STREAM) {
               this->close();
               continue;
          }

          if(::connect(m_sockfd, m_remoteAddrPtr->ai_addr, m_remoteAddrPtr->ai_addrlen) == -1) {
               this->close();
               continue;
          }

          break;
     }

     /* if "m_remoteAddrPtr" is equivalent to NULL, that implies that something went wrong */
     if(m_remoteAddrPtr == NULL){
          //errno = ESOCKTNOSUPPORT;
         throw net::SocketException("[*] Connect Failed: ", this->getLastError());
          return PeerSocket(peerInfo);
     }

     /* The following lines, extract info of the remote address and store them in human readable in
     a member struct called "peerInfo". */
     char *ipstr = new char[AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN];
     void *addr;
     switch ((int)m_remoteAddrPtr->ai_family) {
        case AF_INET:
            //ipstr = new char[INET_ADDRSTRLEN];
            addr = &((struct sockaddr_in*)m_remoteAddrPtr->ai_addr)->sin_addr;
            inet_ntop(m_remoteAddrPtr->ai_family, addr, ipstr, INET_ADDRSTRLEN);
            break;

        case AF_INET6:
            //ipstr = new char[INET6_ADDRSTRLEN];
            addr = &((struct sockaddr_in6*)m_remoteAddrPtr->ai_addr)->sin6_addr;
            inet_ntop(m_remoteAddrPtr->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
            break;

        default:
            std::cout << "[*] Failed to convert ip\n";
            break;
     }

     peerInfo.addr = ipstr; //TODO: This line causes a runtime error on linux.
     peerInfo.port = port;
     peerInfo.af = m_remoteAddrPtr->ai_family;
     peerInfo.sockfd = m_sockfd;

     /* well, this line is obvious. */
     freeaddrinfo(m_remoteAddrInfo);
     
     /* TODO: CREATE TWO METHODS, "isValid()" and "getSocket()", and then this mothod could be turned into
     a void method. */
     return PeerSocket(peerInfo);
}
