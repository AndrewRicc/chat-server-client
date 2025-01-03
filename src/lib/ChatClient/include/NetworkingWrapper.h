#ifndef NETWORKING_WRAPPER_H
#define NETWORKING_WRAPPER_H

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET SocketType;
    typedef int socklen_t;
    #define SOCKET_ERROR_VALUE INVALID_SOCKET
    #define CLOSE_SOCKET(s) closesocket(s)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int SocketType;
    #define SOCKET_ERROR_VALUE (-1)
    #define CLOSE_SOCKET(s) close(s)
#endif

#include <string>

class NetworkInitializer {
public:
    NetworkInitializer() {
        #ifdef _WIN32
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                throw std::runtime_error("Failed to initialize Winsock");
            }
        #endif
    }

    ~NetworkInitializer() {
        #ifdef _WIN32
            WSACleanup();
        #endif
    }
};

// Initialize networking at program start
static NetworkInitializer netInit;

inline int getLastError() {
    #ifdef _WIN32
        return WSAGetLastError();
    #else
        return errno;
    #endif
}

inline bool wouldBlock() {
    #ifdef _WIN32
        return WSAGetLastError() == WSAEWOULDBLOCK;
    #else
        return errno == EAGAIN || errno == EWOULDBLOCK;
    #endif
}

#endif // NETWORKING_WRAPPER_H