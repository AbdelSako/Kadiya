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

#ifndef __TCPserver
#define __TCPserver

#include "TCPsocket.hpp"
#include "__TCPserver.hpp"
#include "SocketException.hpp"
#include <vector>
#include <deque>
#include <shared_mutex>
#include <map>
#include <condition_variable>

#define NUM_OF_THREAD_LISTENER 5

namespace net {
	
 }

namespace net
{
	class HostData;

	class TCPserver;

	/* Code pointers */
	class CodePointer {
	public:
		CodePointer() = default;
		void (*serverCode)(std::shared_ptr<net::TCPserver> server, net::TCPpeer peer) = nullptr;
		void (*storeData)(std::shared_ptr<TCPserver> server, HostData* hostData) = nullptr;
	};

	/* net::TCPserver Class */
	class TCPserver: public net::TCPsocket {
	private:
		/* Object's count */
		static uint16_t serverInstances;

		/* server control variables */
		static std::condition_variable m_intSigCond;
		static bool m_shutdownTCPservers;
		/* This var allow us not to run the startServer method when the server is aleadry running */
		bool m_serverStarted = false;

		/* Retrieving data from threads */
		std::condition_variable storeData_cv;
		std::shared_mutex storeData_mutex;
		//std::shared_lock locker;

		struct sockaddr_in* peerAddr;
		struct sockaddr_in6* peerAddr6;

		struct net::PeerInfo* peerInfo = new net::PeerInfo;

		socklen_t *peerAddrSize;
		uint32_t serverPort;

		//static void TCPserverThreadCore(std::shared_ptr<net::TCPserver> _server);

		/* Modification */
		mutable std::shared_mutex fileMutex;
		std::condition_variable	cv;
		bool readyToProcess = false;
		unsigned int numberOfThreads = NUM_OF_THREAD_LISTENER;

		/* End of Modification */

	public:
		int test;
		void setThreadNumber(unsigned int num) { this->numberOfThreads = num;}
		unsigned int getNumberOfThreads(void) { return this->numberOfThreads; }
		void startThreadedServer(unsigned int maxHost,
			unsigned int numberOfThreads=NUM_OF_THREAD_LISTENER);
		/* Monitors net::TCPserver::m_shutdownTCPservers and returns
		** only when it value changes to "true" */

        // Initializes the socket and binds it.
		TCPserver(const int Family, const char *serverAddr, uint16_t serverPort)
			: net::TCPsocket(Family), serverPort(serverPort)
        {
            ++serverInstances;
            try{
                TCPsocket::socket();
                TCPsocket::bind(serverAddr, serverPort);
				peerAddrSize = new socklen_t;
				if (this->addrFamily == AF_INET)
					peerAddr = new sockaddr_in;
				else
					peerAddr6 = new sockaddr_in6;

				this->setStatus(0);

            } catch (SocketException& e) {
                throw;
            }
        }

		/* listen */
		int listen(uint16_t maxHost);

		/* accept */
		net::TCPpeer accept(void);

		/* Is*/
		~TCPserver(void) {
			delete peerAddrSize;
			if (this->addrFamily == AF_INET)
				delete peerAddr;
			else
				delete peerAddr6;
			delete peerInfo;
			--serverInstances;
			if (serverInstances == 0)
				;
			if(isValid()) {
				this->shutdown(SHUT_RDWR);
				this->close();
			}
			//delete codePointer;
		}

		void startServer(uint32_t maxHost);

		/* checks if server has started */
		bool hasStarted(void);

		/* checks is server has to shutdown */
		bool hasToShutdown(void);

		/* net::TCPserver::startServer lunches detached thread, therefore, if we don't
		** instruct our program to block somewhere(in main() is a good idea)...the detached
		** threads will receive a kill signal when the main() returns.
		** net::wait() Pauses the application, allowing the detached threads to run until a
		** SIGINT(ctrl-c) is sent to the program.*/
		void wait(void);
		bool waitForData(void);

		/* Signal handler */
		static void signalHandler(int signalNum);

		/* This struct has a function pointer which point to your server and takes TCPpeer class as its only argument*/
		void (*serverCode)(TCPpeer peer) = nullptr;
		void (*serverCode2)(std::shared_ptr<net::TCPserver> server, TCPpeer peer) = nullptr;
		/* Default Response 
		This function is executed if you failed to point "serverCode */
		void defaultResponse(net::TCPpeer& peer);

		/************************************************ */
		CodePointer codePointer; // new CodePointer();

		/********************************************/
		//std::map<std::string, FileHandler*> OpenedFiles;

		/**********************************************/
		//std::map<std::string, std::string> cached;

		/***************************************************/
		std::shared_mutex& getFileMutex(void) {
			return fileMutex;
		}
		/********************************* */
		std::deque<HostData*> dataFromThread;

};

class HostData {
private:
	std::string host;
	u_int port;
	std::string url;
	std::string data;
public:
	HostData(std::string& host, u_int& port, std::string data) {
		this->host = host;
		this->port = port;
		this->data = data;
	}

	std::string getHost(void) {
		return host;
	}

	u_int getPort(void) {
		return port;
	}

	std::string getData(void) {
		return data;
	}
};

	/* */
	void TCPserverThreadCore(std::shared_ptr<net::TCPserver> _server);

	/* Default connection handler */
	void handleConn(net::TCPpeer &peer);
};


#endif
