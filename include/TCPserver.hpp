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

#if !defined(__net_TCPserver)
#define __net_TCPserver

#include "TCPsocket.hpp"
#include "handlers/HTTPhandlers.hpp"
#include <shared_mutex>
#include <condition_variable>
#include <deque>
#include <thread>
#include <chrono>

namespace net
{
	/* net::TCPserver Class */
	class TCPserver: public net::TCPsocket {
		public:
			/* Monitors net::TCPserver::m_shutdownTCPservers and returns
			** only when it value changes to "true" */
			friend void net::wait(void);

            // Initializes the socket and binds it.
			TCPserver(const int Family, const char *serverAddr, uint16_t serverPort)
				: net::TCPsocket(Family)
            {
                ++serverInstances;
                try{
                    TCPsocket::socket();
                    TCPsocket::bind(serverAddr, serverPort);
                } catch (SocketException& e) {
                    throw;
                }
            }

			/* listen */
			int listen(uint16_t maxHost) const throw (net::SocketException);

			/* accept */
			net::TCPpeer*  accept(void) const throw (net::SocketException);

			~TCPserver(void) { --serverInstances;}

			/* This function can be ran right after instantiation...to start the
			** server.
			** Arguments:
			**      threadNum: the number of detached threads to run
			** return value:
            **      -1: If the server fails to start.
            **       0: Success. */
			int startServer(size_t threadNum);

			/* checks if server has started */
			bool hasStarted(void);

			/* checks is server has to shutdown */
			bool hasToShutdown(void);

			/* Signal handler */
			static void signalHandler(int signalNum);

		private:
		    /* Object's count */
			static uint16_t serverInstances;

			/* server control variables */
			static std::condition_variable m_intSigCond;
			static bool m_shutdownTCPservers;
			bool m_serverStarted;
	};

	/* */
	void TCPserverThreadCore(std::shared_ptr<net::TCPserver> _server);

	/* Default connection handler */
	void handleConn(net::TCPpeer &peer);

	/* net::TCPserver::startServer lunches detached thread, therefore, if we don't
	** instruct our program to block somewhere(in main() is a good idea)...the detached
	** threads will receive a kill signal when the main() returns.
	** net::wait() Pauses the application, allowing the detached threads to run until a
	** SIGINT(ctrl-c) is sent to the program.*/
	void wait(void);

};

namespace callbacks
{
	/* polymorphic base */
   struct Server {
	   virtual ~Server(void) {}
   };

   /* A callback to a server code (server codes are located in src/serverDB/* and their headers are
    ** in include/serverDB/*), it could be your own code */
   struct ServerCode : Server {
      ServerCode(void (*connCall)(net::TCPpeer) = nullptr)
      : HandleAndServe(connCall) {}

      void (*HandleAndServe)(net::TCPpeer);
   };

};
#endif
