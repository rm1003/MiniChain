
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

#include "../CustomSocket.hpp"

using namespace std;

char *protectPassword(char *data_to_hash) {
    CryptoPP::SHA256 hash;
    std::string result;

    CryptoPP::StringSource ss(
        data_to_hash, true,
        new CryptoPP::HashFilter(
            hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(result))));

    char *out = new char[result.size() + 1];
    if (!out) return nullptr;
    memcpy(out, result.c_str(), result.size() + 1);
    return out;
};

int main(int argc, char *argv[]) {
    int op;
    double value;
    unsigned long int id;
    Message msg;
    memset(&msg, 0, sizeof(msg));

    if (argc != 5) {
        cout << "Uso correto: " << argv[0]
             << " <porta> <nome-servidor> <username> <senha>\n";

        return 0;
    }

    ClientSocket *socket = new ClientSocket();

    int porta = atoi(argv[1]);
    char *host = argv[2];
    char *username = argv[3];
    char *client_password = argv[4];
    char *password = protectPassword(client_password);

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

    cout << "Cliente MiniCoin\n";

    // Login do Cliente
    if (msg.message_type == LOGIN) {
        if (msg.data.login.login_type == MS_VALID) {
            cout << "Autenticação bem sucedida!\n";
            id = msg.client_id;
        } else if (msg.data.login.login_type == MS_INVALID) {
            cout << "Falha na Autenticação\nUsuário ou Senha incorretos\n";
            delete socket;

            return 1;
        }
    }

    delete password;

    op = 1;
    msg.client_id = id;
    while (op != 0) {
        cout << "Operações Disponíveis:\n";
        cout << "    0 - Encerrar programa\n";
        cout << "    1 - Depositar\n";
        cout << "    2 - Retirar\n";
        cout << "    3 - Consultar saldo\n";
        cout << "Insira uma operação: ";
        cin >> op;

        switch (op) {
            case 0:
                cout << "Encerrando Cliente Minicoin\n";
                break;
            case 1:
                cout << "Insira o valor para depositar: ";
                cin >> value;
                msg.message_type = TRANSATION;
                msg.data.transation.transation_type = MS_DEPOSIT;
                msg.data.transation.value = value;
                socket->Connect();
                socket->sendData(msg);
                socket->receiveData(msg);

                if (msg.message_type == OK) {
                    cout << "Depósito de " << value
                         << " MC realizado com sucesso!\n";
                } else {
                    cout << "Falha ao realizar depósito\n";
                }

                break;
            case 2:
                cout << "Insira o valor para retirar: ";
                cin >> value;
                msg.message_type = TRANSATION;
                msg.data.transation.transation_type = MS_WITHDRAW;
                msg.data.transation.value = value;
                socket->Connect();
                socket->sendData(msg);
                socket->receiveData(msg);

                if (msg.message_type == OK) {
                    cout << "Retirada de " << value
                         << " MC realizada com sucesso!\n";
                } else {
                    cout << "Falha ao realizar retirada\nSaldo insuficiente\n";
                }

                break;
            case 3:
                msg.message_type = QUERY;
                msg.data.balance = 0;
                socket->Connect();
                socket->sendData(msg);
                socket->receiveData(msg);

                if (msg.message_type == OK) {
                    cout << "Saldo corrente: " << msg.data.balance << " MC\n";
                } else {
                    cout << "Falha ao consultar saldo\n";
                }

                break;

            default:
                cout << "Insira uma operação válida...\n";
        }
        cout << "\n\n";
        sleep(2);
    }

    delete socket;

    return 0;
}