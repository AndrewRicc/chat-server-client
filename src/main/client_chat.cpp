#include "ChatClient.h"
#include <iostream>
#include <string>
#include <limits>

// Funzione per validare il nickname
std::string getNicknameFromUser() {
    std::string nickname;
    bool isValid = false;
    
    // Continua a chiedere finché non ottiene un nickname valido
    while (!isValid) {
        std::cout << "Inserisci il tuo nickname (3-20 caratteri, solo lettere e numeri): ";
        std::getline(std::cin, nickname);
        
        // Verifica la lunghezza
        if (nickname.length() < 3 || nickname.length() > 20) {
            std::cout << "Il nickname deve essere tra 3 e 20 caratteri." << std::endl;
            continue;
        }
        
        // Verifica i caratteri validi
        isValid = true;
        for (char c : nickname) {
            if (!std::isalnum(c)) {
                std::cout << "Il nickname può contenere solo lettere e numeri." << std::endl;
                isValid = false;
                break;
            }
        }
    }
    
    return nickname;
}

std::string getIpAddressFromUser() {
    std::string ip_address;
    bool isValid = false;
    
    // Continua a chiedere finché non ottiene un ip_address valido
    while (!isValid) {
        std::cout << "Inserisci il tuo indirizzo IP (7-15 caratteri, solo lettere e numeri): ";
        std::getline(std::cin, ip_address);
        
        // Verifica la lunghezza
        if (ip_address.length() < 7 || ip_address.length() > 15) {
            std::cout << "L'indirizzo IP deve essere tra 7 e 15 caratteri." << std::endl;
            continue;
        }
        
        // Verifica i caratteri validi
        isValid = true;
        for (char c : ip_address) {
            if (!std::isalnum(c)) {
                std::cout << "L'indirizzo IP può contenere solo lettere e numeri." << std::endl;
                isValid = false;
                break;
            }
        }
    }
    
    return ip_address;
}

int main() {
    std::cout << "Benvenuto nella Chat!" << std::endl;
    
    // Ottiene il nickname dall'utente con validazione
    std::string nickname = getNicknameFromUser();
    std::string ip_address = getIpAddressFromUser();
    
    // Crea il client con il nickname fornito dall'utente
    ChatClient client(
        nickname,          // nickname inserito dall'utente
        ip_address,      // server IP
        12345,            // porta
        1024              // dimensione buffer
    );
    
    // Tentativo di connessione
    if (!client.connect()) {
        std::cerr << "Impossibile connettersi al server. Riprova più tardi." << std::endl;
        return 1;
    }

    // Avvia il loop principale della chat
    client.run();
    return 0;
}