#include "ChatClient.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sstream>

// Costruttore
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
    socketFd(-1),
    running(false)
{}

// Costruttore di movimento
ChatClient::ChatClient(ChatClient&& other) noexcept :
    serverIp(std::move(other.serverIp)),
    port(other.port),
    bufferSize(other.bufferSize),
    nickname(std::move(other.nickname)),
    socketFd(other.socketFd),
    running(other.running.load())
{
    other.socketFd = -1;
    other.running = false;
}

// Operatore di assegnazione di movimento
ChatClient& ChatClient::operator=(ChatClient&& other) noexcept {
    if (this != &other) {
        // Chiudi le risorse esistenti
        stop();

        // Muovi i dati
        serverIp = std::move(other.serverIp);
        port = other.port;
        bufferSize = other.bufferSize;
        nickname = std::move(other.nickname);
        socketFd = other.socketFd;
        running = other.running.load();

        // Invalida l'altro oggetto
        other.socketFd = -1;
        other.running = false;
    }
    return *this;
}

// Distruttore
ChatClient::~ChatClient() {
    stop();
}

void ChatClient::stop() {
    running = false;
    
    if (socketFd != -1) {
        close(socketFd);
        socketFd = -1;
    }
    
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}

// Formatta il messaggio con il nickname
std::string ChatClient::formatMessage(const std::string& message) {
    std::stringstream ss;
    ss << "[" << nickname << "] " << message;
    return ss.str();
}

// Gestisce la ricezione dei messaggi
void ChatClient::receiveMessages() {
    std::vector<char> buffer(bufferSize);
    
    while (running) {
        memset(buffer.data(), 0, bufferSize);
        int bytesRead = recv(socketFd, buffer.data(), bufferSize - 1, 0);
        
        if (bytesRead <= 0) {
            std::cout << "\nDisconnesso dal server." << std::endl;
            running = false;
            break;
        }
        
        std::cout << "\r" << buffer.data() << std::endl;
        std::cout << "[" << nickname << "] " << std::flush;
    }
}

// Connette al server
bool ChatClient::connect() {
    // Crea il socket
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        std::cerr << "Errore nella creazione del socket" << std::endl;
        return false;
    }

    // Configura l'indirizzo del server
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Indirizzo IP non valido" << std::endl;
        return false;
    }

    // Connette al server
    if (::connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Errore nella connessione al server" << std::endl;
        return false;
    }

    std::cout << "Connesso al server come " << nickname << "!" << std::endl;
    
    // Invia il nickname
    std::string initialMsg = "NICKNAME:" + nickname;
    if (send(socketFd, initialMsg.c_str(), initialMsg.length(), 0) < 0) {
        std::cerr << "Errore nell'invio del nickname" << std::endl;
        return false;
    }
    
    running = true;
    receiveThread = std::thread(&ChatClient::receiveMessages, this);
    return true;
}

// Loop principale della chat
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