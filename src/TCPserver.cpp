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

#include "TCPserver.hpp"
#include "SocketException.hpp"
#include "serverDB/httpProxy2.0.hpp"
#include <thread>

/*  */
uint16_t net::TCPserver::serverInstances = 0;
std::condition_variable net::TCPserver::m_intSigCond;
bool net::TCPserver::m_shutdownTCPservers = false;

/* LISTEN FOR INCOMING CONNECTION */
int net::TCPserver::listen(uint16_t maxHost)  
{
     if(!isValid()) {
          errno = EBADF;
          throw net::SocketException("listen()", errno);
     }

     if(::listen(TCPsocket::m_sockfd, maxHost) == -1) {
          throw net::SocketException("listen()", errno);
     }
     else return 0;
}

/* ACCEPT INCOMING CONNECTION */
net::TCPpeer net::TCPserver::accept(void)
{
     if(!isValid()) {
          errno = EBADF;
#ifdef _WIN32
          throw net::SocketException("net::TCPserver::accept()", WSAGetLastError());
#else
          throw net::SocketException("net::TCPserver::accept()", errno);
#endif
     }

     net::SOCKET remoteSockfd;
     void *addr;
     size_t addrsize = this->addrFamily == AF_INET ?
                        INET_ADDRSTRLEN : INET6_ADDRSTRLEN;
     char *addrstr = new char[addrsize];
     uint16_t port;
     std::memset(peerInfo, 0, sizeof peerInfo);

    switch (addrFamily) {
        case AF_INET:
            std::memset(this->peerAddr, 0, sizeof(sockaddr_in));
            *peerAddrSize = sizeof(sockaddr);

            remoteSockfd = ::accept(m_sockfd, (struct sockaddr *)peerAddr,
                                    peerAddrSize);
            addr = (void*)(&peerAddr->sin_addr);
            port = ::ntohs(peerAddr->sin_port);
            ::inet_ntop(AF_INET, addr, addrstr, addrsize);
            break;

        case AF_INET6:
            std::memset(peerAddr6, 0, sizeof(sockaddr_in6));
            *peerAddrSize = sizeof(sockaddr);

            remoteSockfd = ::accept(m_sockfd, (struct sockaddr *)peerAddr6,
                                    peerAddrSize);
            addr = (void*)(&peerAddr6->sin6_addr);
            port = ::ntohs(peerAddr->sin_port);
            ::inet_ntop(AF_INET6, addr, addrstr, addrsize);
            break;
        default:
            //errno = EAFNOSUPPORT;
#ifdef _WIN32
            throw net::SocketException("net::TCPserver::accept()", std::to_string(WSAGetLastError()));
#else
            throw net::SocketException("net::TCPserver::accept()", errno);
#endif
            break;
    }

    if(remoteSockfd == -1)
        throw net::SocketException("net::TCPserver::accept()", this->getLastError());

    else {
 //       net::TCPpeer *peer = new net::TCPpeer(remoteSockfd);
        peerInfo->addr = addrstr;
        peerInfo->port = port;
        peerInfo->af = this->addrFamily;
        peerInfo->sockfd = remoteSockfd;
        net::TCPpeer peer = net::TCPpeer(*peerInfo);
        delete addrstr;
        return peer;

    }
}
/* ***************************************************** */
void net::TCPserver::startServer(uint32_t maxHost) {
    std::signal(SIGINT, TCPserver::signalHandler);
    this->m_serverStarted = true;
    //this->serverCode = serverDB::httpProxyServer;
    net::TCPpeer peer;
    this->listen(maxHost);
    std::cout << "[+] Server is ready to accept peers on port " << this->serverPort << std::endl;
    std::shared_ptr<net::TCPserver> serverSharedPtr(this);
    while (true) {
        peer = this->accept();
        this->serverCode(peer);
        /* this->accept() allocated memory for TCPpeer* peer; always remember to delete it
        at the end of the function */
    }

}

/* ********************************************************** */
/* START THE SERVER */
/* Threading starts here.
    This method launches threadNum amount of listeners. Threaded listener run inside
    TCPserverThreadCore method and each listener could aceept a connection and then spawns
    another function which will be detached from the TCPserverThreadCore method from the use of threading.
    The spawned function would be your server function.

    Before running the startServer() method, you first need to point the member function pointer
    void (*server)(TCPpeer &peer) of this server object to your server code. */
void net::TCPserver::startThreadedServer(unsigned int maxHost,
    unsigned int numberOfThreads)
{
    this->numberOfThreads = numberOfThreads;
    //TODO: 
    if (maxHost == 0) return ;
    if (numberOfThreads == 0) return;

    /* signal handler */
    std::signal(SIGINT, TCPserver::signalHandler);
    std::signal(SIGPIPE, TCPserver::signalHandler);

    std::shared_ptr<net::TCPserver> serverSharedPtr(this);
    

    /* Listen */
    this->listen(maxHost);

    auto acceptor = [serverSharedPtr]
    {
        net::TCPpeer peer;
        while (true) {
            try
            {
                peer = serverSharedPtr->accept();
                std::cout << "[+] Accepted by thread ID: " <<
                    std::this_thread::get_id() << "\n";
            }
            catch (net::SocketException& e)
            {
                e.display();
                return;
            }

            //ToDo: Remember to update the isValid()'s variable.
            if (!peer.isValid())
            {
                std::cout << "[*] Failed to accept connection...\n";
                continue;
            }
            else
            {
                if (serverSharedPtr->codePointer.serverCode != nullptr)
                {
                    /* Detaching connected peers */
                    std::thread peerThread(serverSharedPtr->codePointer.serverCode,
                                                            serverSharedPtr,
                                                            peer);
                    peerThread.detach();
                }
                else
                {
                    handleConn(peer);

                    peer.killConn();
                }
            }
        }
    };

    /* Detaching accpetors */
  
    for (int n = 0; n < this->numberOfThreads; n++) {
        std::thread acceptorThread(acceptor);
        acceptorThread.detach();
    }
}


/*Check if the server is running */
bool net::TCPserver::hasStarted(void)
{
	return m_serverStarted;
}

bool net::TCPserver::hasToShutdown(void)
{
	return m_shutdownTCPservers;
}

void net::TCPserver::signalHandler(int signalNum)
{
    /* Just one broken connection; we can continue. */
	if(signalNum == SIGPIPE) {
		std::cout << "[*] SIGPIPE caught...\n";
		return;
	}

// 	if(signalNum == SIGKILL) exit(signalNum);

	if(m_shutdownTCPservers == true) {
		std::cout << "[+] Terminating the server by force...\n";
		exit(signalNum);
	}
	else {
		std::cout << "[+] Grafully shuting down the server...\n";
		m_shutdownTCPservers = true;
		m_intSigCond.notify_one();
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}
    

void net::handleConn(net::TCPpeer &peer)
{
    
}

/* wait method */
void net::TCPserver::wait(void) {
    std::mutex m;
    std::unique_lock<std::mutex> lock(m);
    TCPserver::m_intSigCond.wait(lock, [] {return TCPserver::m_shutdownTCPservers; });
}

