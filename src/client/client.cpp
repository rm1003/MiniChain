#include <cstdio>
#include <cstring>
#include <iostream>

#include "../customSocket.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    Message msg;
    ClientSocket *socket = new ClientSocket();

    int porta = (atoi(argv[1]));
    char *host = argv[2];
    char *dados = argv[3];

    socket->init(porta, host);

    memset(msg.buffer, 0, sizeof(msg.buffer));
    printf("Enviando: %s %ld\n", dados, strlen(dados));
    memcpy(msg.buffer, dados, strlen(dados) + 1);
    socket->sendData(msg);
    printf("Mensagem enviada!\n");

    delete socket;

    return 0;
}