#include "TCPclient.hpp"

/* CONNECT TO REMOTE HOST */
int net::TCPclient::connect(const std::string remoteAddr, uint16_t port)
{
     std::memset(&m_hints, 0, sizeof m_hints);
     //std::memset(m_remoteAddrInfo, 0, sizeof m_remoteAddrInfo);
     m_hints.ai_family = AF_UNSPEC;
     m_hints.ai_socktype = m_Type;

     int gai_ret;
     if((gai_ret = ::getaddrinfo(remoteAddr.data(), std::to_string(port).data(),
               &m_hints, &m_remoteAddrInfo)) != 0) {
    	 std::cout << "[*] net::TCPclient::connect() --> ::getadddrinfo(): "
    			 <<::gai_strerror(gai_ret) << "; " << remoteAddr << ":"
    			 << port << '\n';
    	 return -1;
     }

     struct pollfd pollfds[1];
     std::memset((char *) &pollfds, 0, sizeof(pollfds));

     for(m_remoteAddrPtr = m_remoteAddrInfo; m_remoteAddrPtr != NULL;
               m_remoteAddrPtr = m_remoteAddrPtr->ai_next) {
          m_sockfd = -1;
          if ((m_sockfd = ::socket(m_remoteAddrPtr->ai_family, m_remoteAddrPtr->ai_socktype,
                    m_remoteAddrPtr->ai_protocol)) == -1) {
               continue;
          }

          if(m_remoteAddrPtr->ai_socktype != SOCK_STREAM) {
               net::TCPsocket::close();
               continue;
          }

          if(::connect(m_sockfd, m_remoteAddrPtr->ai_addr, m_remoteAddrPtr->ai_addrlen) == -1) {
               net::TCPsocket::close();
               continue;
          }

          break;
     }

     if(m_remoteAddrPtr == NULL){
          errno = ESOCKTNOSUPPORT;
          return m_sockfd;
     }

     char ipstr[addrFamily == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN];
     void *addr;
     switch ((int)m_remoteAddrPtr->ai_family) {
          case AF_INET:
               addr = &((struct sockaddr_in*)m_remoteAddrPtr->ai_addr)->sin_addr;
               inet_ntop(m_remoteAddrPtr->ai_family, addr, ipstr, INET_ADDRSTRLEN);
               break;
          case AF_INET6:
               addr = &((struct sockaddr_in6*)m_remoteAddrPtr->ai_addr)->sin6_addr;
               inet_ntop(m_remoteAddrPtr->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
          break;
          default:
               std::cout << "[*] Failed to convert ip\n";
               break;
     }

     this->peerInfo.addr = ipstr;
     this->peerInfo.port = port;
     this->peerInfo.af = m_remoteAddrPtr->ai_family;

     freeaddrinfo(m_remoteAddrInfo);
     return m_sockfd;
}
