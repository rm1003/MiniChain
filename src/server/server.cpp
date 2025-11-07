#include <netinet/in.h>
#include <openssl/sha.h>

#include <cstdio>
#include <cstring>

#include "../Blockchain.hpp"
#include "../CustomSocket.hpp"

using namespace std;

string hashF(const string data_to_hash) { return data_to_hash; };

int main(int argc, char *argv[]) {
    Message message;

    ServerSocket *socket = new ServerSocket();

    socket->init(atoi(argv[1]));
    socket->listenForConnection();

    while (1) {
        socket->acceptConnection();
        socket->receiveData(message);
        socket->closeConnection();
    }

    delete socket;

    return 0;
}