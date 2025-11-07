#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <cstring>

#include "../CustomSocket.hpp"

using namespace std;

void protectPassword(char *password) {
    char aux;
    char *rev = password;
    unsigned long int size = strlen(rev);

    unsigned long int j;
    for (unsigned long int i = 0; i < size / 2; ++i) {
        j = size - i - 1;
        aux = password[i];
        password[i] = password[j];
        password[j] = aux;
    }
}

int main(int argc, char *argv[]) {
    int op;
    double value;
    unsigned long int id;
    Message msg;
    memset(&msg, 0, sizeof(msg));

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
    protectPassword(password);

    socket->init(porta, host);

    msg.message_type = LOGIN;
    msg.data.login.login_type = MS_REQUEST;

    memset(msg.data.login.username, 0, MAX_USERWORD);
    memset(msg.data.login.password, 0, MAX_USERWORD);

    memcpy(msg.data.login.username, username,
           (strlen(username) > MAX_USERWORD) ? MAX_USERWORD : strlen(username));

    memcpy(msg.data.login.password, password,
           (strlen(password) > MAX_USERWORD) ? MAX_USERWORD : strlen(password));

    socket->Connect();
    socket->sendData(msg);
    socket->receiveData(msg);

    printf("Cliente MiniCoin\n");

    // Login do Cliente
    if (msg.message_type == LOGIN) {
        if (msg.data.login.login_type == MS_VALID) {
            printf("Autenticação bem sucedida!\n");
            id = msg.client_id;
        } else if (msg.data.login.login_type == MS_INVALID) {
            printf("Falha na Autenticação\nUsuário ou Senha incorretos\n");
            delete socket;

            return 1;
        }
    }

    op = 1;
    msg.client_id = id;
    while (op != 0) {
        printf("\n\n");
        printf("Operações Disponíveis:\n");
        printf("    0 - Encerrar programa\n");
        printf("    1 - Depositar\n");
        printf("    2 - Retirar\n");
        printf("    3 - Consultar saldo\n");
        printf("Insira uma operação: ");
        scanf("%d", &op);

        switch (op) {
            case 0:
                printf("Encerrando Cliente Minicoin\n");
                break;
            case 1:
                printf("Insira o valor para depositar: ");
                scanf("%lf", &value);
                msg.message_type = TRANSATION;
                msg.data.transation.transation_type = MS_DEPOSIT;
                msg.data.transation.value = value;
                socket->Connect();
                socket->sendData(msg);
                socket->receiveData(msg);

                break;
            case 2:
                printf("Insira o valor para retirar: ");
                scanf("%lf", &value);
                msg.message_type = TRANSATION;
                msg.data.transation.transation_type = MS_WITHDRAW;
                msg.data.transation.value = value;
                socket->Connect();
                socket->sendData(msg);
                socket->receiveData(msg);

                break;
            case 3:
                msg.message_type = QUERY;
                msg.data.balance = 0;
                socket->Connect();
                socket->sendData(msg);
                socket->receiveData(msg);

                break;

            default:
                printf("Insira uma operação válida...\n");
        }
    }

    delete socket;

    return 0;
}