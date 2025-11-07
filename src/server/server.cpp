#include <netinet/in.h>
#include <openssl/sha.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <vector>

#include "../Blockchain.hpp"
#include "../CustomSocket.hpp"

using namespace std;

typedef struct user {
    string username;
    string password;
} user;

vector<user> users;

Blockchain *blockchain;

string hashFunction(const string data_to_hash) {
    string rev = data_to_hash;
    rev += 5;
    reverse(rev.begin(), rev.end());
    return rev;
};

void authenticate(Message *msg) {
    unsigned long int i;
    bool found = false;

    for (i = 0; i < users.size(); ++i) {
        if (msg->data.login.username == users[i].username) {
            found = true;
            break;
        }
    }

    if (found) {
        if (hashFunction(msg->data.login.password) == users[i].password) {
            msg->data.login.login_type = MS_VALID;
            msg->client_id = i;
        } else {
            msg->data.login.login_type = MS_INVALID;
            msg->client_id = i;
        }
    } else {
        user new_user;
        new_user.username = msg->data.login.username;
        new_user.password = hashFunction(msg->data.login.password);
        users.push_back(new_user);
        msg->data.login.login_type = MS_VALID;
        msg->client_id = users.size() - 1;
    }
}

void transation(Message *msg) {}

void query(Message *msg) {
    double balance;
    unsigned long int client_id = msg->client_id;

    balance = blockchain->GetUserBalance(client_id);

    msg->data.balance = balance;
}

void handler(Message *msg) {
    if (msg->message_type == LOGIN) {
        authenticate(msg);
    } else if (msg->message_type == TRANSATION) {
        transation(msg);
    } else {
        query(msg);
    }
}

int main(int argc, char *argv[]) {
    Message msg;

    memset(&msg, 0, sizeof(msg));

    if (argc != 2) {
        printf("Uso correto: %s <porta>\n", argv[1]);
        return 1;
    }

    blockchain = new Blockchain(&hashFunction);
    ServerSocket *socket = new ServerSocket();

    socket->init(atoi(argv[1]));
    socket->listenForConnection();

    while (1) {
        socket->acceptConnection();
        socket->receiveData(msg);
        handler(&msg);
        socket->sendData(msg);
        socket->closeConnection();
    }

    delete socket;
    delete blockchain;

    return 0;
}