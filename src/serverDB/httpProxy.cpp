#include "serverDB/httpProxy.hpp"

	void serverDB::HTTPproxyServer(net::TCPpeer localPeer)
	{
	    try {
            localPeer.flags(net::SET_WILL_CLOSE_SOCKET, 1);
            struct net::PeerInfo peerInfo;
            (void)localPeer.getPeerInfo(peerInfo);
            //localPeer.flags(net::SET_TRANS_BUFFER, 1024*50);

            std::string rawReq, rawRes, tempBuf, rcByte;
            net::SOCKET remoteSock;

            /* Read request */
            /* Client application might be a telnet or netcat,
             * if so, let's handle it */
             try {
                http::recvUntilRC(localPeer, rawReq);
                if(rawReq.empty()) /* drop peer */ return;
             } catch(net::SocketException& e) {
                e.display();
                return;
             }

            /* Parse request */
            http::requestParser *reqinfo;
            reqinfo = new http::requestParser(rawReq);

            /* verify method */
            if(reqinfo->method.empty() || reqinfo->url_or_host.empty()) {
                std::cout << "[*] fix it, looks http::requestParser failed to...\n";
                return;
            }

            if(reqinfo->method.compare("CONNECT") == 0) {
                /* tunneling */
                std::string host;
                uint16_t port;
                std::string _hostport = reqinfo->url_or_host; _hostport.pop_back();
                std::string::size_type n = _hostport.find(':');
                if(n != std::string::npos) {
                    host = _hostport.substr(0, n);
                    try {
                        port = std::stoi(_hostport.substr(n + 1));
                    } catch(std::invalid_argument& e) {
                        /* malformed request */
                        e.what();
                        return;
                    }
                }
                else {
                    /* handle malformed host address */
                    return;
                }

                net::TCPclient client;
                if((remoteSock = client.connect(host, port)) == -1) {
                    std::cout << "[https] failed to connect to: "
                            << host << ":" << port << '\n';
                    /* drop peer */
                    return;
                }
                std::cout << "[+] " << peerInfo.addr << ":" << peerInfo.port
                            << " ---> " << host << ":" << port << '\n';

                net::TCPpeer remotePeer(remoteSock);

                auto _connection = reqinfo->headers.find("Connection");
                if(_connection != reqinfo->headers.end()) {
                    std::string connection = _connection->second;
                    connection.pop_back();
                    if(connection.compare("keep-alive") == 0) {
                        localPeer.flags(net::SET_RECV_TIMEOUT, 1000*45);
                        //localPeer.flags(net::SET_SEND_TIMEOUT, 1000*45);
                        localPeer << "HTTP/1.1 200 Connection Established\r\n"
                                "Host: WebProject\r\n"
                                "Connection: keep-alive\r\n\r\n";
                    }
                    else {
                        //localPeer.flags(net::SET_WILL_CLOSE_SOCKET, 1);
                        localPeer << "HTTP/1.1 200 Connection Established\r\n"
                                                    "Host: WebProject\r\n\r\n";
                    }
                }
                else
                    localPeer << "HTTP/1.1 200 Connection Established\r\n"
                                "Host: WebProject\r\n\r\n";

                uint16_t c = 0;
                /* tunneling it now*/
                do {
                    rawReq.clear(); rawRes.clear();
                    localPeer >> rawReq;
                    //http::recvUntilRC(TCPpeer, rawReq);

                    if(rawReq.empty()) break;

                    remotePeer << rawReq;
                    remotePeer >> rawRes;
                    //http::recvUntilRC(remotePeer, rawRes);

                    if(rawRes.empty()) break;

                    localPeer << rawRes;
                    ++c;
                } while(rawReq.length() > 0);

                delete reqinfo;
                return;
            }
/* ****************************************************************************
   **************************************************************************** */
            /* handle HTTP */
            else {
                std::string host, url;
                //http::urlParser *uinfo;

                uint16_t port;
                /* http proxying */
                net::TCPclient client;

                auto _host = reqinfo->headers.find("Host");
                if(_host != reqinfo->headers.end()) {
                    host = _host->second; host.pop_back();
                    std::string::size_type n;
                    n = host.find(':');
                    if(n != std::string::npos) {
                        port = std::stoi(host.substr(n + 1));
                        host = host.substr(0, n - 1);
                    }
                    else
                        port = 80;
                }
                else {
                    url = reqinfo->url_or_host; url.pop_back();
                    http::urlParser uinfo(url);

                    host = uinfo.host;
                    if(uinfo.port != 0)
                        port = uinfo.port;
                    else
                        port = 80;
                }

                /* Connect to target host */
                if((remoteSock = client.connect(host, port)) == -1) {
                    std::cout << "[*] failed to connect to: " << host
                        << ":" << port << '\n';
                    return;
                }
                std::cout << "[+] " << peerInfo.addr << ":" << peerInfo.port
                           << " ---> " << host << ":" << port << '\n';

                net::TCPpeer *remotePeer;
                remotePeer = new net::TCPpeer(remoteSock);

                //remotePeer->flags(net::SET_TRANS_BUFFER, 1024*50);

                *remotePeer << rawReq;
                //*remotePeer >> rawRes;
                http::recvUntilRC(*remotePeer, rawRes);

                if(rawRes.empty()) /* drop peer */ return;

                localPeer << rawRes;

                /* If keep-alive */
                auto _connection = reqinfo->headers.find("Connection");
                if(_connection != reqinfo->headers.end()) {
                    std::string connection = _connection->second;
                    connection.pop_back();
                    if(connection.compare("keep-alive") == 0) {
                        //localPeer.flags(net::SET_RECV_TIMEOUT, 1000*15);
                        //localPeer.flags(net::SET_SEND_TIMEOUT, 1000*15);
                        uint16_t c = 1;
                        try {
                            do {
                                rawReq.clear(); rawRes.clear();
                                localPeer >> rawReq;

                                if(rawReq.empty()) break;

                                    delete reqinfo;
                                    reqinfo = new http::requestParser(rawReq);
                                    url = reqinfo->url_or_host; url.pop_back();
                                    http::urlParser uinfo(url);

                                if(!uinfo.host.empty() && uinfo.host.compare(host) != 0) {
                                    host = uinfo.host;
                                    if((remoteSock = client.connect(host, port)) == -1) {
                                        std::cout << "[*] Failed to connect to: " << host
                                            << ":" << port << '\n';
                                        return;
                                    }
                                    delete remotePeer;
                                    remotePeer = new net::TCPpeer(remoteSock);

                                    //remotePeer->flags(net::SET_TRANS_BUFFER, 1024*50);

                                    std::cout << "[+] " << peerInfo.addr << ":" << peerInfo.port
                                                <<  " ---> "<< host << ":" << port << '\n';
                                }

                                *remotePeer << rawReq;
                                *remotePeer >> rawRes;
                                if(rawRes.empty()) break;
                                localPeer << rawRes;
                                ++c;
                            } while(rawReq.length() > 0);
                        } catch(net::SocketException& e) {
                            e.display();
                            return;
                        }
                        //std::cout << " keep-alive-> " << c << '\n';
                    }
                }
                delete reqinfo;
                delete remotePeer;

                return;
            }
	    } catch(net::SocketException& e) {
            e.display();
            return;
        }  catch (std::exception const &exc)
        {
            std::cerr << "[*] Exception caught " << exc.what() << "\n";
            return;
        }
        catch(...) {
            std::cerr << "[*] Unknown exception caught\n";
            return;
        }
	}
