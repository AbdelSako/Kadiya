/*
					GNU GENERAL PUBLIC LICENSE
					   Version 2, June 1991
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

	class ServerSocket;
	class CodePointer;
	void TCPserverThreadCore(std::shared_ptr<net::ServerSocket> _server);

	/* Default connection handler */
	void handleConn(net::PeerSocket& peer);
	class HostData;
};

/* Code pointers */
class net::CodePointer {
public:
	CodePointer() = default;
	void (*serverCode)(std::shared_ptr<net::ServerSocket> server, net::PeerSocket peer) = nullptr;
	void (*storeData)(std::shared_ptr<ServerSocket> server, HostData* hostData) = nullptr;
};

/* net::TCPserver Class */
class net::ServerSocket: public net::Socket
{
private:
	/* Object's count */
	static uint16_t serverInstances;

	/* server control variables */
	static std::condition_variable m_intSigCond;
	static bool m_shutdownTCPservers;
	/* This var allow us not to run the startServer method when the server is aleadry running */
	bool m_serverStarted = false;

	/* Retrieving data from threads */
	bool dataReady = false;
	std::condition_variable storeData_cv;
	std::mutex storeData_mutex;
	//std::shared_lock locker(storeData_mutex, std::defer_lock);

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
	ServerSocket(const int Family, const char *serverAddr, uint16_t serverPort)
		: net::Socket(Family), serverPort(serverPort)
    {
        ++serverInstances;
        try{
            Socket::socket();
            Socket::bind(serverAddr, serverPort);
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
	net::PeerSocket accept(void);

	/* Is*/
	~ServerSocket(void) {
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
	void (*serverCode)(PeerSocket peer) = nullptr;
	void (*serverCode2)(std::shared_ptr<net::ServerSocket> server, PeerSocket peer) = nullptr;
	/* Default Response 
	This function is executed if you failed to point "serverCode */
	void defaultResponse(net::PeerSocket& peer);

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

class net::HostData {
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

#endif
