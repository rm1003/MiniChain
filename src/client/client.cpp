#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "../CustomSocket.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    Message msg;

    if (argc != 5) {
        printf("Uso correto: %s <porta> <nome-servidor> <username> <senha>",
               argv[0]);

        return 0;
    }

    ClientSocket *socket = new ClientSocket();

    int porta = atoi(argv[1]);
    char *host = argv[2];
    char *username = argv[3];
    char *password = argv[4];

    socket->init(porta, host);

    delete socket;

    return 0;
}