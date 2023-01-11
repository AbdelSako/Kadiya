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

#ifndef __TCPstuff
#define __TCPstuff

namespace net
{
    /* */
    enum flags
    {
        GET_WILL_CLOSE_SOCKET,
        GET_KEEP_ALIVE,
        GET_TRANS_BUFFER,
        GET_RECV_TIMEOUT,
        GET_SEND_TIMEOUT,
        /* */
        SET_WILL_CLOSE_SOCKET = 0, //
        SET_KEEP_ALIVE,
        SET_TRANS_BUFFER,
        SET_RECV_TIMEOUT,
        SET_SEND_TIMEOUT
    };

    struct PeerInfo{
        PeerInfo() {}
        std::string addr;
        uint16_t port;
        uint8_t af;
        uint32_t sockfd;
    };
};
#endif // __TCPstuff
