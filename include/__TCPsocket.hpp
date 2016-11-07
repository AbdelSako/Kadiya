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
        std::string addr;
        uint16_t port;
        uint8_t af;
    };
};
#endif // __TCPstuff
