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

void net::TCPserverThreadCore(std::shared_ptr<net::TCPserver> _server)
{
	static int threadCount = 0;
	net::SOCKET remoteSock;
	std::string callbacksID;
	std::thread* thr;

	threadCount += 1;

	net::TCPserver *server = _server.get();

	net::TCPpeer *peer;

	while(true) {
		if(server->hasToShutdown()) return;

		try {;
			/* Accept incoming */
			peer = server->accept();
			/* */
            peer->flags(SET_WILL_CLOSE_SOCKET, 0);
		} catch(net::SocketException& e) {
			e.display();
			continue;
		} catch(...) {
			std::cout << "\n[*] This behavior needs to be investigated. \n\n[*] High Priority!\n";
			server->signalHandler(SIGKILL);
		}

		if(!peer->isValid()) {
            		std::cout << "[*] Failed to accept connection...\n";
            		continue;
		}
        else {
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
