#include "TCPclient.hpp"
#include "http/http.hpp"

/* Print response headers */
using namespace std;
void printHeaders(const http::responseParser& resInfo);
void httpRead(net::PeerSocket& peer, std::string& data);

int main(void) //(int argc, char *argv[])
{ /* START */

	int port = 80;
	std::string host = "www.google.com";

	net::ClientSocket client;
	net::PeerSocket peer;
    // Connecting to host.
	peer = client.connect(host, port);

	// Check for a successful connection.
	if(!peer.isValid()) {
		std::cout << "[*] failed to connect...\n";
		peer.getLastError();
		return 1;
	}

	// Request variable
	std::string req = "GET http://"+host+"/ HTTP/1.1\r\n"
							"User-Agent: Kadiya\r\n\r\n";
    // response variable
    std::string res;
    
    //Sending request to host.
	uint16_t r = http::write(peer,req);
    
    httpRead(peer, res);
    cout << res << endl;
    
	peer.killConn();
}/* END */

void printHeaders(const http::responseParser& resInfo) {
    // Access the struct's data (http::responseParser)
    cout << "[+] version: " << resInfo.version << '\n';
    cout << "[+] code: " << resInfo.code << '\n';
    cout << "[+] status: " << resInfo.status << '\n';

    for(auto &pair : resInfo.headers) {
        cout << "[+] " << pair.first << ": " << pair.second << '\n';
    }
}

void httpRead(net::PeerSocket& peer, std::string& data) {
    //recv small amount of and check on the header.
    string::size_type pos1 = string::npos, pos2, pos3;
    uint16_t bytes = 0, totalBytes = 0;
    uint16_t bufSize = 1024;
    char *buf = new char[bufSize + 1];
    do {
        bytes = peer.recv(buf, bufSize);
        if(bytes == 0) {
            cout << "[*] Receive error: " << peer.getStatus() << endl;
            return;
        }
        totalBytes += bytes;
        data.append(buf);
        if(pos1 == string::npos)
            pos1 = data.find("\r\n\r\n");
    } while ( pos1 == string::npos &&
             data.substr(pos1 + 4).length() >= 20 );
    
    pos1 += 4;
    pos2 = data.find("\r\n", pos1);
    
    //Now that we have the header plus 20 characters after the header,
    // Let's investigate what's up with the header.
    
    http::responseParser resHeader(data);
    string transType = resHeader.headers["Transfer-Encoding"];
    
    //If the Transfer-Encoding is Chunked, Let's do What has to be done with
    //Chunked data transfer type.
    //Pointing to the first after the header "\r\n\r\n"
    char const *ptr;
    ptr = data.data() + pos2 + 2;
        
    // String Value representing the chunk size
    string hexVal = data.substr(pos1, pos2 - pos1);
    //Chunk size
    uint16_t chunkedSize = stoi(hexVal, 0, 16);
    
    uint16_t tmpSize, ss;
    
    do {
        uint16_t ptrSize = strlen(ptr);
        if(ptrSize < chunkedSize) {
            //ptr = data.data() + pos1 + 10;
            uint16_t chunkBufSize = chunkedSize - ptrSize;
            char *chunkBuf = new char[chunkBufSize + 1];
            bytes = peer.recv(chunkBuf, chunkBufSize); totalBytes += bytes;
            data.append(chunkBuf);
            ptr = data.data() + pos2 + 2;
            tmpSize = strlen(ptr);
            delete [] chunkBuf;
        }
        bytes = peer.recv(buf, bufSize); totalBytes += bytes;
        
        if(bytes == 0)
            break;
        
        pos1 = data.size();
        data.append(buf);
        pos1 += 2;
        pos2 = data.find("\r\n", pos1);
        if(pos2 != string::npos) {
            hexVal = data.substr(pos1, pos2 - pos1);
            try {
                chunkedSize = stoi(hexVal, 0, 16);
            }
            catch (...) {
                chunkedSize = 0;
            }
            ptr = data.data() + pos2 + 2;
            ptrSize = strlen(ptr);
        }
        else
            chunkedSize = 0;
    } while (chunkedSize);
}
