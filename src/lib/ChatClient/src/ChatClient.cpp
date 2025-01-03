#include "ChatClient.h"
#include <iostream>
#include <sstream>
#include <string.h>

ChatClient::ChatClient(
    const std::string& nickname,
    const std::string& serverIp,
    int port,
    int bufferSize
) : 
    serverIp(serverIp),
    nickname(nickname),
    port(port),
    bufferSize(bufferSize),
    socketFd(SOCKET_ERROR_VALUE),
    running(false)
{}

ChatClient::ChatClient(ChatClient&& other) noexcept :
    serverIp(std::move(other.serverIp)),
    port(other.port),
    bufferSize(other.bufferSize),
    nickname(std::move(other.nickname)),
    socketFd(other.socketFd),
    running(other.running.load())
{
    other.socketFd = SOCKET_ERROR_VALUE;
    other.running = false;
}

ChatClient& ChatClient::operator=(ChatClient&& other) noexcept {
    if (this != &other) {
        stop();
        serverIp = std::move(other.serverIp);
        port = other.port;
        bufferSize = other.bufferSize;
        nickname = std::move(other.nickname);
        socketFd = other.socketFd;
        running = other.running.load();
        
        other.socketFd = SOCKET_ERROR_VALUE;
        other.running = false;
    }
    return *this;
}

ChatClient::~ChatClient() {
    stop();
}

void ChatClient::stop() {
    running = false;
    
    if (socketFd != SOCKET_ERROR_VALUE) {
        CLOSE_SOCKET(socketFd);
        socketFd = SOCKET_ERROR_VALUE;
    }
    
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}

std::string ChatClient::formatMessage(const std::string& message) {
    std::stringstream ss;
    ss << "[" << nickname << "] " << message;
    return ss.str();
}

void ChatClient::receiveMessages() {
    std::vector<char> buffer(bufferSize);
    
    while (running) {
        memset(buffer.data(), 0, bufferSize);
        int bytesRead = recv(socketFd, buffer.data(), bufferSize - 1, 0);
        
        if (bytesRead <= 0) {
            if (!wouldBlock()) {
                std::cout << "\nDisconnesso dal server." << std::endl;
                running = false;
                break;
            }
            continue;
        }
        
        std::cout << "\r" << buffer.data() << std::endl;
        std::cout << "[" << nickname << "] " << std::flush;
    }
}

bool ChatClient::connect() {
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == SOCKET_ERROR_VALUE) {
        std::cerr << "Errore nella creazione del socket" << std::endl;
        return false;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Indirizzo IP non valido" << std::endl;
        return false;
    }

    if (::connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Errore nella connessione al server" << std::endl;
        return false;
    }

    std::cout << "Connesso al server come " << nickname << "!" << std::endl;
    
    std::string initialMsg = "NICKNAME:" + nickname;
    if (send(socketFd, initialMsg.c_str(), initialMsg.length(), 0) < 0) {
        std::cerr << "Errore nell'invio del nickname" << std::endl;
        return false;
    }
    
    running = true;
    receiveThread = std::thread(&ChatClient::receiveMessages, this);
    return true;
}

void ChatClient::run() {
    std::string message;
    std::cout << "\nInzia a scrivere i tuoi messaggi (scrivi 'quit' per uscire):" << std::endl;
    
    while (running) {
        std::cout << "[" << nickname << "] ";
        std::getline(std::cin, message);
        
        if (message.empty()) {
            continue;
        }
        
        if (message == "quit") {
            break;
        }

        std::string formattedMessage = formatMessage(message);
        if (send(socketFd, formattedMessage.c_str(), formattedMessage.length(), 0) < 0) {
            std::cerr << "Errore nell'invio del messaggio" << std::endl;
            break;
        }
    }

    stop();
}