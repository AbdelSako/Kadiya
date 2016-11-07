#include "TCPsocket.hpp"
#include "handlers/HTTPhandlers.hpp"
#include <shared_mutex>
#include <condition_variable>
#include <deque>
#include <thread>
#include <chrono>

#if !defined(__net_TCPserver)
#define __net_TCPserver

namespace net
{
	/* net::TCPserver Class */
	class TCPserver: public net::TCPsocket {
		public:
			/* Monitors net::TCPserver::m_shutdownTCPservers and returns
			** only when it value changes to "true" */
			friend void net::wait(void);

            // Initializes the socket and binds it.
			TCPserver(const int Family, const int Protocol,
				const char *serverAddr, uint16_t serverPort)
				: net::TCPsocket(Family, Protocol)
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

			/* get mutex */
			std::shared_mutex& getMutex(void);

			/* Signal handler */
			static void signalHandler(int signalNum);

		private:
		    /* Object's count */
			static uint16_t serverInstances;

			/* mutex */
			mutable std::shared_mutex m_sharedMutex;

			/* server control variables */
			static std::condition_variable_any m_intSigCond;
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
