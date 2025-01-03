#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "NetworkingWrapper.h"
#include <string>
#include <thread>
#include <atomic>

class ChatClient {
private:
    std::string serverIp;
    int port;
    int bufferSize;
    std::string nickname;

    SocketType socketFd;
    std::thread receiveThread;
    std::atomic<bool> running;

    void receiveMessages();
    std::string formatMessage(const std::string& message);

public:
    ChatClient(
        const std::string& nickname,
        const std::string& serverIp = "127.0.0.1",
        int port = 12345,
        int bufferSize = 1024
    );

    ChatClient(const ChatClient&) = delete;
    ChatClient& operator=(const ChatClient&) = delete;
    ChatClient(ChatClient&&) noexcept;
    ChatClient& operator=(ChatClient&&) noexcept;
    ~ChatClient();

    bool connect();
    void run();
    void stop();

    std::string getNickname() const { return nickname; }
    std::string getServerIp() const { return serverIp; }
    int getPort() const { return port; }
    int getBufferSize() const { return bufferSize; }
    bool isRunning() const { return running; }
};

#endif // CHAT_CLIENT_H