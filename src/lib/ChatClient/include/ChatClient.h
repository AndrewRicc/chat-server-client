#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <string>
#include <thread>
#include <atomic>
#include <vector>

// La classe ChatClient fornisce un'interfaccia per connettersi a un server di chat
class ChatClient {
private:
    // Configurazione della connessione
    std::string serverIp;
    int port;
    int bufferSize;
    std::string nickname;

    // Stato interno
    int socketFd;
    std::thread receiveThread;
    std::atomic<bool> running;

    // Metodi privati per la gestione della connessione
    void receiveMessages();
    std::string formatMessage(const std::string& message);

public:
    // Costruttore che permette di configurare tutti i parametri
    ChatClient(
        const std::string& nickname,
        const std::string& serverIp = "127.0.0.1",
        int port = 12345,
        int bufferSize = 1024
    );

    // Impedisce la copia dell'oggetto ChatClient
    ChatClient(const ChatClient&) = delete;
    ChatClient& operator=(const ChatClient&) = delete;

    // Permette il movimento dell'oggetto ChatClient
    ChatClient(ChatClient&&) noexcept;
    ChatClient& operator=(ChatClient&&) noexcept;

    // Distruttore
    ~ChatClient();

    // Metodi pubblici per l'utilizzo della chat
    bool connect();  // Connette al server
    void run();     // Avvia il loop principale della chat
    void stop();    // Ferma la chat e chiude la connessione

    // Getters per le configurazioni
    std::string getNickname() const { return nickname; }
    std::string getServerIp() const { return serverIp; }
    int getPort() const { return port; }
    int getBufferSize() const { return bufferSize; }
    bool isRunning() const { return running; }
};

#endif // CHAT_CLIENT_H