#include "ChatClient.h"
#include <iostream>

int main() {
    // Crea un client con configurazione personalizzata
    ChatClient client(
        "NICKNAME_PLACEHOLDER",     // nickname
        "127.0.0.1",      // server IP
        12345,            // porta
        1024              // dimensione buffer
    );
    
    // Oppure usa i valori predefiniti
    // ChatClient client("Alice");  // Solo il nickname è obbligatorio
    
    if (!client.connect()) {
        return 1;
    }

    client.run();
    return 0;
}