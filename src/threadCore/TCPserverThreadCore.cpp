#include "TCPserver.hpp"

void net::TCPserverThreadCore(std::shared_ptr<net::TCPserver> _server)
{
	static int threadCount = 0;
	net::SOCKET remoteSock;
	std::string callbacksID;
	std::thread* thr;

	threadCount += 1;

	net::TCPserver *server = _server.get();

	//std::shared_lock<std::shared_mutex> acceptLock(server->getMutex());
	net::TCPpeer *peer;

	while(true) {
		if(server->hasToShutdown()) return;

		try {;
			/* Accept incoming */
			peer = server->accept();
			/* */
            peer->flags(SET_WILL_CLOSE_SOCKET, 0);
            //acceptLock.unlock();
		} catch(net::SocketException& e) {
			e.display();
			continue;
		} catch(...) {
			std::cout << "\n[*] This behavior needs to be investigated. \n\n[*] High Priority!\n";
			server->signalHandler(SIGKILL);
		}

		if(peer->isValid()) {
            /* peer object */
            peer->serverCallbacks = server->serverCallbacks;

            if(dynamic_cast<callbacks::ServerCode*>(peer->serverCallbacks)) {
                callbacks::ServerCode* callback =
                        dynamic_cast<callbacks::ServerCode*>(peer->serverCallbacks);
                thr = new std::thread(callback->HandleAndServe, *peer);
                thr->detach();
            }
            else {
                handleConn(*peer);

                peer->shutdown(0);
                peer->close();
            }

            delete peer;
            //acceptLock.lock();
		}
		else {
            std::cout << "[*] Failed to accept connection...\n";
            //acceptLock.lock();
            continue;
		}
	}
}

void net::handleConn(net::TCPpeer &peer)
{
    std::string req;

    peer >> req;
    std::cout << "From: " << req << '\n';

    peer << "Response from default handler...\r\n\r\n";
}
