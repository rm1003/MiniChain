
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

// Estrutura para guardar informacoes do cliente
typedef struct user {
    string username;
    string password;
} user;

// Vetor que guarda os clientes
vector<user> users;

ServerSocket *s_socket;
Blockchain *blockchain;

// Funcao para encerra o servidor liberando socket e memoria da blockchain
void finish(int s) {
    cout << "\nEncerrando servidor...\n";
    delete s_socket;
    delete blockchain;
    exit(s);
}

// Funcao hash SHA256 da biblioteca picosha2
string hashFunction(const string data_to_hash) {
    string hex_str;
    picosha2::hash256_hex_string(data_to_hash, hex_str);
    return hex_str;
};

// Funcao de autenticacao do cliente
void authenticate(Message *msg) {
    unsigned long int i;
    bool found = false;

    // Pega o tempo de acesso local e atual
    long timer = time(0);
    time(&timer);

    string s = ctime(&timer);
    s[s.size() - 1] = '\0';
    cout << "[" << s << "]: ";
    cout << "Tentativa de " << msg->data.login.username
        << " de autenticar...\n";

    // Procura o nome do cliente no vetor para identificar se esse cliente ja
    // conectou alguma vez
    for (i = 0; i < users.size(); ++i) {
        if (msg->data.login.username == users[i].username) {
            found = true;
            break;
        }
    }
    
    // Se ja conectou, entao valida o seu nome e sua senha
    if (found) {
        if (msg->data.login.password == users[i].password) {
            msg->data.login.login_type = MS_VALID;
            msg->client_id = i;
            cout << "Usuario autenticado.\n";
        } else {
            // Caso senha incorreta
            msg->data.login.login_type = MS_INVALID;
            msg->client_id = i;
            cout << "Senha incorreta.\n";
        }
    } else {
        // Se nunca conectou entao cria um novo usuario com o nome e senha
        // passado
        user new_user;
        new_user.username = msg->data.login.username;
        new_user.password = msg->data.login.password;
        users.push_back(new_user);
        msg->data.login.login_type = MS_VALID;
        msg->client_id = users.size() - 1;
        cout << "Novo usuario criado e autenticado | ID: " << msg->client_id
            << '\n';
    }
}

// Funcao para fazer transacao
void transation(Message *msg) {
    Transation tr;
    unsigned long int client_id = msg->client_id;
    double value = msg->data.transation.value;

    tr.client_id = tr.dest_id = client_id;
    tr.value = value;

    // Identifica o tipo da transacao
    tr.type = (msg->data.transation.transation_type == MS_DEPOSIT) ? DEPOSIT
        : WITHDRAW;
    tr.time = time(0);
    time(&tr.time);

    // Cria um novo registro na Blockchain
    // Se deu certo deve mandar um OK para cliente
    if (blockchain->Insert(tr)) {
        msg->message_type = OK;
    } else {
        // Se deu errado deve mandar um EROOR indicando para o cliente
        msg->message_type = ERROR;
        msg->data.transation.transation_type = MS_INSUFFICIENT;
    }

    string op;
    if (tr.type == DEPOSIT) {
        op = "DEPOSITO";
    } else if (tr.type == WITHDRAW) {
        op = "RETIRADA";
    }

    // log print
    string s = ctime(&tr.time);
    s[s.size() - 1] = '\0';
    cout << "[" << s << "]: ";
    cout << "Cliente " << users[client_id].username.c_str() << " [" << client_id
        << "]" << " realizou uma acao de " << op.c_str() << " no valor de "
        << value
        << " MC | Codigo: " << ((msg->message_type == OK) ? "OK" : "ERRO")
        << '\n';
}

// Funcao para transferencia de MiniCoins entre os clientes existentes
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
    // Acha o destinatario
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

    // Se nao achou o destinatario, manda uma mensagem para o cliente avisando
    if (!(found)) {
        cout << "Usuario nao encontrado\n";
        msg->message_type = ERROR;
        msg->data.transation.transation_type = MS_INVALID_USER;
        return;
    }

    tr.dest_id = i;
    // Achou o destinatario, entao cria um bloco novo e envia um OK para
    // o cliente
    if (blockchain->Insert(tr)) {
        msg->message_type = OK;
    } else {
        // Caso saldo insuficiente, avisa o cliente com um erro
        cout << "Saldo insuficiente\n";
        msg->message_type = ERROR;
        msg->data.transation.transation_type = MS_INSUFFICIENT;
        return;
    }

    cout << "Cliente " << users[client_id].username.c_str() << " [" << client_id
        << "]" << " realizou uma acao de TRANSFERENCIA no valor de " << value
        << " MC para o cliente " << users[tr.dest_id].username.c_str() << " ["
        << tr.dest_id << "] "
        << " | Codigo: " << ((msg->message_type == OK) ? "OK" : "ERRO")
        << '\n';
}

// Funcao de consulta do saldo
void query(Message *msg) {
    double balance = 0.0;
    unsigned long int client_id = msg->client_id;

    // Obtem o saldo do cliente
    if (blockchain->GetUserBalance(client_id, &balance)) {
        // Deu certo, manda mensagem para o cliente com OK e o seu saldo
        msg->message_type = OK;
        msg->data.balance = balance;
    } else {
        // Deu erro, avisa o cliente
        msg->message_type = ERROR;
    }

    long timer = time(0);
    time(&timer);

    string s = ctime(&timer);
    s[s.size() - 1] = '\0';
    cout << "[" << s << "]: ";
    cout << "Cliente " << users[client_id].username.c_str() << " [" << client_id
        << "]" << " realizou uma consulta de saldo: " << balance
        << " MC | Codigo: " << ((msg->message_type == OK) ? "OK" : "ERRO")
        << '\n';
}

// Funcao auxiliar para redirecionar as chamadas de funcoes para tratar o tipo
// de mensagem recebido
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

    // Aceita a conexao do cliente
    // Recebe os dados mandados pelo cliente
    // Trata a mensagem
    // Manda a resposta para o cliente
    // Fecha a conexao com o cliente
    while (1) {
        s_socket->acceptConnection();
        s_socket->receiveData(msg);
        handler(&msg);
        s_socket->sendData(msg);
        s_socket->closeConnection();
    }

    // Libera memoria
    delete s_socket;
    delete blockchain;

    return 0;
}