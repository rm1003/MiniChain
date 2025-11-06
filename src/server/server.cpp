#include <netinet/in.h>
#include <openssl/sha.h>

#include <cstdio>
#include <cstring>

#include "../blockchain.hpp"
#include "../customSocket.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    Message message;
    ServerSocket *socket = new ServerSocket();

    socket->init(htons(atoi(argv[1])));

    socket->listenForConnection();

    while (1) {
        socket->acceptConnection();
        memset(message.buffer, 0, sizeof(message.buffer));
        socket->receiveData(message);
        printf("%s\n", message.buffer);
        socket->closeConnection();
    }

    delete socket;

    return 0;
}