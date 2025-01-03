#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <algorithm>

const int PORT = 12345;
const int BUFFER_SIZE = 1024;

// Struttura per gestire una connessione client
struct ClientConnection {
    int socket;
    std::thread thread;
    std::atomic<bool> active;
    int id;

    ClientConnection(int sock, int clientId) 
        : socket(sock), active(true), id(clientId) {}
};

class ChatServer {
private:
    std::vector<ClientConnection*> clients;
    std::mutex clientsMutex;
    std::atomic<bool> running{true};
    int serverSocket;

    void handleClient(ClientConnection* client) {
        char buffer[BUFFER_SIZE];
        
        // Notifica di connessione agli altri client
        broadcastSystemMessage("Client " + std::to_string(client->id) + " si è unito alla chat!");
        
        while (running && client->active) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytesRead = recv(client->socket, buffer, BUFFER_SIZE - 1, 0);
            
            if (bytesRead <= 0) {
                std::cout << "Client " << client->id << " disconnesso." << std::endl;
                client->active = false;
                
                // Notifica di disconnessione agli altri client
                broadcastSystemMessage("Client " + std::to_string(client->id) + " si è disconnesso.");
                break;
            }

            std::cout << "Messaggio da client " << client->id << ": " << buffer << std::endl;
            
            // Inoltra il messaggio agli altri client
            forwardMessage(client->id, buffer);
        }

        // Rimozione del client dalla lista
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            auto it = std::find_if(clients.begin(), clients.end(),
                [client](const ClientConnection* c) { return c->id == client->id; });
            
            if (it != clients.end()) {
                // Chiude il socket e libera la memoria
                close((*it)->socket);
                delete *it;
                clients.erase(it);
            }
        }
    }

    void broadcastSystemMessage(const std::string& message) {
        std::lock_guard<std::mutex> lock(clientsMutex);
        std::string sysMsg = "SISTEMA: " + message;
        for (auto& client : clients) {
            if (client->active) {
                send(client->socket, sysMsg.c_str(), sysMsg.length(), 0);
            }
        }
    }

    void forwardMessage(int senderId, const char* message) {
        std::lock_guard<std::mutex> lock(clientsMutex);
        std::string formattedMsg = message;
        
        for (auto& client : clients) {
            if (client->id != senderId && client->active) {
                send(client->socket, formattedMsg.c_str(), formattedMsg.length(), 0);
            }
        }
    }

public:
    ChatServer() : serverSocket(-1) {
        clients.reserve(2);
    }

    bool start() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            std::cerr << "Errore nella creazione del socket" << std::endl;
            return false;
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Errore nel binding del socket" << std::endl;
            return false;
        }

        if (listen(serverSocket, 2) < 0) {
            std::cerr << "Errore nell'ascolto del socket" << std::endl;
            return false;
        }

        std::cout << "Server avviato e in ascolto sulla porta " << PORT << std::endl;
        return true;
    }

    void run() {
        int nextClientId = 1;
        
        while (running) {
            // Accetta nuove connessioni solo se c'è spazio
            if (clients.size() < 2) {
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);
                
                // Imposta il timeout per accept
                struct timeval tv;
                tv.tv_sec = 1;  // 1 secondo di timeout
                tv.tv_usec = 0;
                setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
                
                int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
                
                if (clientSocket < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        // Timeout - continua il loop
                        continue;
                    } else {
                        std::cerr << "Errore nell'accettazione della connessione" << std::endl;
                        continue;
                    }
                }

                // Nuova connessione accettata
                std::cout << "Client " << nextClientId << " connesso!" << std::endl;
                
                auto client = new ClientConnection(clientSocket, nextClientId);
                client->thread = std::thread(&ChatServer::handleClient, this, client);
                client->thread.detach();  // Detach per permettere al server di continuare indipendentemente
                
                {
                    std::lock_guard<std::mutex> lock(clientsMutex);
                    clients.push_back(client);
                }
                
                // Notifica il client del suo ID
                std::string welcome = "Sei il Client " + std::to_string(nextClientId);
                send(clientSocket, welcome.c_str(), welcome.length(), 0);
                
                nextClientId++;
            }
            
            // Breve pausa per evitare di sovraccaricare la CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Rimuove i client inattivi
            cleanupInactiveClients();
        }
    }

    void cleanupInactiveClients() {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(
            std::remove_if(clients.begin(), clients.end(),
                [](ClientConnection* client) {
                    if (!client->active) {
                        delete client;
                        return true;
                    }
                    return false;
                }
            ),
            clients.end()
        );
    }

    ~ChatServer() {
        running = false;

        // Chiude tutte le connessioni client
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            for (auto client : clients) {
                client->active = false;
                close(client->socket);
                delete client;
            }
            clients.clear();
        }

        if (serverSocket != -1) {
            close(serverSocket);
        }
    }
};

int main() {
    ChatServer server;
    
    if (!server.start()) {
        return 1;
    }

    std::cout << "Server avviato. Premi Ctrl+C per terminare." << std::endl;
    server.run();
    
    return 0;
}