#include <netinet/in.h>
#include <openssl/sha.h>
#include <signal.h>

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

ServerSocket *s_socket;
Blockchain *blockchain;

void finish(int s) {
    printf("\nEncerrando servidor...\n");
    delete s_socket;
    delete blockchain;
    exit(s);
}

string hashFunction(const string data_to_hash) {
    string rev = data_to_hash;
    rev += 5;
    reverse(rev.begin(), rev.end());
    return rev;
};

void authenticate(Message *msg) {
    unsigned long int i;
    bool found = false;

    printf("Tentativa de %s de autenticar...\n", msg->data.login.username);
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
            printf("Usuário autenticado.\n");
        } else {
            msg->data.login.login_type = MS_INVALID;
            msg->client_id = i;
            printf("Senha incorreta.\n");
        }
    } else {
        user new_user;
        new_user.username = msg->data.login.username;
        new_user.password = hashFunction(msg->data.login.password);
        users.push_back(new_user);
        msg->data.login.login_type = MS_VALID;
        msg->client_id = users.size() - 1;
        printf("Novo usuário criado e autenticado | ID: %lu\n", msg->client_id);
    }
}

void transation(Message *msg) {
    Transation tr;
    unsigned long int client_id = msg->client_id;
    double value = msg->data.transation.value;

    tr.client_id = client_id;
    tr.value = value;
    tr.type = (msg->data.transation.transation_type == MS_DEPOSIT) ? DEPOSIT
                                                                   : WITHDRAW;

    if (blockchain->Insert(tr)) {
        msg->message_type = OK;
    } else {
        msg->message_type = ERROR;
    }

    string op;
    if (tr.type == DEPOSIT) {
        op = "DEPOSITO";
    } else if (tr.type == WITHDRAW) {
        op = "RETIRADA";
    }

    printf(
        "Cliente %s [%lu] realizou uma ação de %s no valor de %lf MC | Código: "
        "%s\n",
        users[client_id].username.c_str(), client_id, op.c_str(), value,
        (msg->message_type == OK) ? "OK" : "ERRO");
}

void query(Message *msg) {
    double balance = 0.0;
    unsigned long int client_id = msg->client_id;

    if (blockchain->GetUserBalance(client_id, &balance)) {
        msg->message_type = OK;
        msg->data.balance = balance;
    } else {
        msg->message_type = ERROR;
    }

    printf(
        "Cliente %s [%lu] realizou uma consulta de saldo: %lf MC | Código: "
        "%s\n",
        users[client_id].username.c_str(), client_id, balance,
        (msg->message_type == OK) ? "OK" : "ERRO");
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
    struct sigaction sigIntHandler;

    memset(&msg, 0, sizeof(msg));
    memset(&sigIntHandler, 0, sizeof(sigIntHandler));

    sigIntHandler.sa_handler = finish;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    if (argc != 2) {
        printf("Uso correto: %s <porta>\n", argv[0]);
        return 1;
    }

    s_socket = new ServerSocket();
    blockchain = new Blockchain(&hashFunction);

    s_socket->init(atoi(argv[1]));
    s_socket->listenForConnection();

    while (1) {
        s_socket->acceptConnection();
        s_socket->receiveData(msg);
        handler(&msg);
        s_socket->sendData(msg);
        s_socket->closeConnection();
    }

    delete s_socket;
    delete blockchain;

    return 0;
}