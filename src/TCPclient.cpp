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

#include "TCPclient.hpp"

/* CONNECT TO REMOTE HOST.
the string "remoteAddr" could be a domain name or IPv4/IPv6 address */
net::TCPpeer net::TCPclient::connect(const std::string remoteAddr, uint16_t port)
{
<<<<<<< HEAD
    std::memset(m_hints, 0, sizeof m_hints);
    std::memset(ipstr, 0, sizeof(INET6_ADDRSTRLEN));
    //std::memset(this->m_remoteAddrPtr, 0, sizeof(m_remoteAddrPtr));
    std::memset(this->m_remoteAddrInfo, 0, sizeof(m_remoteAddrInfo));
=======
    struct addrinfo m_hints, *m_remoteAddrInfo, *m_remoteAddrPtr;
    struct net::PeerInfo peerInfo;
    peerInfo.sockfd = -1;
     std::memset(&m_hints, 0, sizeof m_hints);
     //std::memset(m_remoteAddrInfo, 0, sizeof m_remoteAddrInfo);
     m_hints.ai_family = AF_UNSPEC;
     m_hints.ai_socktype = SOCK_STREAM;
     int m_sockfd;
>>>>>>> parent of 04c3a07 (Switching computer from work to home.)

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
    	 return TCPpeer(peerInfo);
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
               close(m_sockfd);
               continue;
          }

          if(::connect(m_sockfd, m_remoteAddrPtr->ai_addr, m_remoteAddrPtr->ai_addrlen) == -1) {
               close(m_sockfd);
               continue;
          }

          break;
     }

     /* if "m_remoteAddrPtr" is equivalent to NULL, that implies that something went wrong */
     if(m_remoteAddrPtr == NULL){
          errno = ESOCKTNOSUPPORT;
          return TCPpeer(peerInfo);
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

     peerInfo.addr = ipstr;
     peerInfo.port = port;
     peerInfo.af = m_remoteAddrPtr->ai_family;
     peerInfo.sockfd = m_sockfd;

     /* well, this line is obvious. */
     freeaddrinfo(m_remoteAddrInfo);
     
     /* TODO: CREATE TWO METHODS, "isValid()" and "getSocket()", and then this mothod could be turned into
     a void method. */
     return TCPpeer(peerInfo);
}
