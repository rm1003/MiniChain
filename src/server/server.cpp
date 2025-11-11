
#include <netinet/in.h>
#include <signal.h>

#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>

#include "../Blockchain.hpp"
#include "../CustomSocket.hpp"
#include "../picosha2.hpp"

using namespace std;

typedef struct user {
    string username;
    string password;
} user;

vector<user> users;

ServerSocket *s_socket;
Blockchain *blockchain;

void finish(int s) {
    cout << "\nEncerrando servidor...\n";
    delete s_socket;
    delete blockchain;
    exit(s);
}

string hashFunction(const string data_to_hash) {
    string hex_str;
    picosha2::hash256_hex_string(data_to_hash, hex_str);
    return hex_str;
};

void authenticate(Message *msg) {
    unsigned long int i;
    bool found = false;

    long timer = time(0);
    time(&timer);

    string s = ctime(&timer);
    s[s.size() - 1] = '\0';
    cout << "[" << s << "]: ";
    cout << "Tentativa de " << msg->data.login.username
         << " de autenticar...\n";
    for (i = 0; i < users.size(); ++i) {
        if (msg->data.login.username == users[i].username) {
            found = true;
            break;
        }
    }

    if (found) {
        if (msg->data.login.password == users[i].password) {
            msg->data.login.login_type = MS_VALID;
            msg->client_id = i;
            cout << "Usuário autenticado.\n";
        } else {
            msg->data.login.login_type = MS_INVALID;
            msg->client_id = i;
            cout << "Senha incorreta.\n";
        }
    } else {
        user new_user;
        new_user.username = msg->data.login.username;
        new_user.password = msg->data.login.password;
        users.push_back(new_user);
        msg->data.login.login_type = MS_VALID;
        msg->client_id = users.size() - 1;
        cout << "Novo usuário criado e autenticado | ID: " << msg->client_id
             << '\n';
    }
}

void transation(Message *msg) {
    Transation tr;
    unsigned long int client_id = msg->client_id;
    double value = msg->data.transation.value;

    tr.client_id = tr.dest_id = client_id;
    tr.value = value;

    tr.type = (msg->data.transation.transation_type == MS_DEPOSIT) ? DEPOSIT
                                                                   : WITHDRAW;
    tr.time = time(0);
    time(&tr.time);

    if (blockchain->Insert(tr)) {
        msg->message_type = OK;
    } else {
        msg->message_type = ERROR;
        msg->data.transation.transation_type = MS_INSUFFICIENT;
    }

    string op;
    if (tr.type == DEPOSIT) {
        op = "DEPOSITO";
    } else if (tr.type == WITHDRAW) {
        op = "RETIRADA";
    }

    string s = ctime(&tr.time);
    s[s.size() - 1] = '\0';
    cout << "[" << s << "]: ";
    cout << "Cliente " << users[client_id].username.c_str() << " [" << client_id
         << "]" << " realizou uma ação de " << op.c_str() << " no valor de "
         << value
         << " MC | Código: " << ((msg->message_type == OK) ? "OK" : "ERRO")
         << '\n';
}

void transfer(Message *msg) {
    Transation tr;
    bool found;
    unsigned long int i;
    unsigned long int client_id = msg->client_id;
    double value = msg->data.transfer.value;

    tr.client_id = client_id;
    tr.value = value;
    tr.type = TRANSFER;
    tr.time = time(0);

    found = false;
    for (i = 0; i < users.size(); ++i) {
        if (msg->data.transfer.destination_username == users[i].username) {
            found = true;
            break;
        }
    }

    time(&tr.time);

    string s = ctime(&tr.time);
    s[s.size() - 1] = '\0';
    cout << "[" << s << "]: ";

    if (!(found)) {
        cout << "Usuário não encontrado\n";
        msg->message_type = ERROR;
        msg->data.transation.transation_type = MS_INVALID_USER;
        return;
    }

    tr.dest_id = i;

    if (blockchain->Insert(tr)) {
        msg->message_type = OK;
    } else {
        cout << "Saldo insuficiente\n";
        msg->message_type = ERROR;
        msg->data.transation.transation_type = MS_INSUFFICIENT;
        return;
    }

    cout << "Cliente " << users[client_id].username.c_str() << " [" << client_id
         << "]" << " realizou uma ação de TRANSFERÊNCIA no valor de " << value
         << " MC para o cliente " << users[tr.dest_id].username.c_str() << " ["
         << tr.dest_id << "] "
         << " | Código: " << ((msg->message_type == OK) ? "OK" : "ERRO")
         << '\n';
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

    long timer = time(0);
    time(&timer);

    string s = ctime(&timer);
    s[s.size() - 1] = '\0';
    cout << "[" << s << "]: ";
    cout << "Cliente " << users[client_id].username.c_str() << " [" << client_id
         << "]" << " realizou uma consulta de saldo: " << balance
         << " MC | Código: " << ((msg->message_type == OK) ? "OK" : "ERRO")
         << '\n';
}

void handler(Message *msg) {
    if (msg->message_type == LOGIN) {
        authenticate(msg);
    } else if (msg->message_type == TRANSATION) {
        transation(msg);
    } else if (msg->message_type == TRANSFERENCE) {
        transfer(msg);
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
        cout << "Uso correto: " << argv[0] << " <porta>\n";
        return 1;
    }

    s_socket = new ServerSocket();
    blockchain = new Blockchain(&hashFunction);

    s_socket->init(atoi(argv[1]));
    s_socket->listenForConnection();

    // setup cout
    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);

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