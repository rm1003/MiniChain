
#include <unistd.h>

#include <cstdio>
#include <cstring>

#include "../CustomSocket.hpp"
#include "../picosha2.hpp"

using namespace std;

// Faz a hash SHA256 da senha do cliente
// Usando a biblioteca picosha2
char *protectPassword(const char *data_to_hash) {
    string data_str(data_to_hash);
    string hex_str;
    picosha2::hash256_hex_string(data_str, hex_str);

    char *out = new char[hex_str.size() + 1]();
    if (!out) return nullptr;
    memcpy(out, hex_str.c_str(), hex_str.size() + 1);
    return out;
};

int main(int argc, char *argv[]) {
    int op;
    double value;
    unsigned long int id = 0;
    char destination[MAX_USERWORD + 1];
    Message msg;
    memset(&msg, 0, sizeof(msg));

    if (argc != 5) {
        cout << "Uso correto: " << argv[0]
             << " <porta> <nome-servidor> <username> <senha>\n";

        return 0;
    }

    // Inicializacao do client e conecta o usuario com o servidor
    // =========================================================================
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
    memset(destination, 0, MAX_USERWORD);

    memcpy(msg.data.login.username, username,
           (strlen(username) > MAX_USERWORD) ? MAX_USERWORD : strlen(username));

    memcpy(msg.data.login.password, password,
           (strlen(password) > MAX_USERWORD) ? MAX_USERWORD : strlen(password));

    socket->Connect();
    socket->sendData(msg);
    socket->receiveData(msg);
    // =========================================================================

    // setup cout
    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);

    cout << "Cliente MiniCoin (MC)\n";

    // Login do Cliente
    if (msg.message_type == LOGIN) {
        // Autenticando o usuario verificando o retorno da mensagem recebido do
        // servidor
        if (msg.data.login.login_type == MS_VALID) {
            cout << "Autenticacao bem sucedida!\n";
            id = msg.client_id;
        } else if (msg.data.login.login_type == MS_INVALID) {
            // Caso falhe
            cout << "Falha na Autenticacao\nUsuario ou Senha incorretos\n";
            delete socket;
            return 1;
        }
    }

    delete[] password;

    op = 1;
    msg.client_id = id;

    // Enquando a op (operacao) eh diferente de 0 continue executando
    while (op != 0) {
        sleep(1);

        cout << "Operacoes Disponiveis:\n";
        cout << "    0 - Encerrar programa\n";
        cout << "    1 - Depositar\n";
        cout << "    2 - Retirar\n";
        cout << "    3 - Realizar transferencia\n";
        cout << "    4 - Consultar saldo\n";
        cout << "Insira uma operacao (numero): ";
        cin >> op;

        switch (op) {
            // Encerramento do cliente
            case 0:
                cout << "Encerrando Cliente Minicoin\n";
                break;

            // Caso de deposito
            case 1:
                cout << "Insira o valor para depositar: ";
                cin >> value;
                if (value <= 0) {
                    cout << "Insira um valor valido. ( Maior que zero )\n";
                    break;
                }
                // Atribui as informacoes na mensagem
                msg.message_type = TRANSATION;
                msg.data.transation.transation_type = MS_DEPOSIT;
                msg.data.transation.value = value;
                // Conecta com o servidor e manda uma mensagem (msg)
                socket->Connect();
                socket->sendData(msg);
                socket->receiveData(msg);
                // Resposta do servidor OK - tudo correto
                // se nao - deu falha ou algum erro
                if (msg.message_type == OK) {
                    cout << "Deposito de " << value
                         << " MC realizado com sucesso!\n";
                } else {
                    cout << "Falha ao realizar deposito\n";
                }
                break;

            // Caso de retirar algum valor do saldo total
            case 2:
                cout << "Insira o valor para retirar: ";
                cin >> value;
                if (value <= 0) {
                    cout << "Insira um valor valido. ( Maior que zero )\n";
                    break;
                }

                // Atribui informacoes na mensagem
                msg.message_type = TRANSATION;
                msg.data.transation.transation_type = MS_WITHDRAW;
                msg.data.transation.value = value;
                // Conecta com o servidor e manda uma mensagem
                socket->Connect();
                socket->sendData(msg);
                socket->receiveData(msg);
                // Reposta do servidor OK - retirada com sucesso
                // se nao - pode ser uma falha ou saldo insuficiente
                if (msg.message_type == OK) {
                    cout << "Retirada de " << value
                         << " MC realizada com sucesso!\n";
                } else {
                    if (msg.data.transation.transation_type == MS_INSUFFICIENT)
                        cout << "Falha ao realizar retirada\nSaldo "
                                "insuficiente\n";
                }
                break;

            // Caso de Transferencia de saldo para outro usuario
            case 3:
                cout << "Insira o username do destinatario (MAX: 20 char): ";
                cin >> destination;
                cout << "Insira o valor a ser transferido: ";
                cin >> value;
                if (value <= 0) {
                    cout << "Insira um valor valido. ( Maior que zero )\n";
                    break;
                }

                // Atribui informacoes na mensagem
                msg.message_type = TRANSFERENCE;
                msg.data.transfer.transation_type = MS_TRANSFERENCE;
                msg.data.transfer.value = value;

                memcpy(msg.data.transfer.destination_username, destination,
                       MAX_USERWORD);
                // Trata o caso de envio de dinheiro para o proprio usuario
                if (strcmp(username, destination) == 0) {
                    cout << "Nao eh possivel enviar dinheiro para si proprio\n";
                    continue;
                }
                // Manda a mensagem para o servidor
                socket->Connect();
                socket->sendData(msg);
                socket->receiveData(msg);
                // Resposta do servidor OK - transferencia feita
                // se nao - pode ser falha em usuario inexistente ou saldo
                // insuficiente
                if (msg.message_type == OK) {
                    cout << "Transferencia de " << value << " MC para "
                         << destination << " realizada com sucesso!\n";
                } else {
                    if (msg.data.transation.transation_type == MS_INVALID_USER)
                        cout << "Falha ao realizar transferencia\nO usuario "
                             << destination << " nao existe\n";

                    if (msg.data.transation.transation_type == MS_INSUFFICIENT)
                        cout << "Falha ao realizar transferência\nSaldo "
                                "insuficiente\n";
                }
                break;

            // Caso de consulta ao saldo total
            case 4:
                // Atribui informacoes para a mensagem
                msg.message_type = QUERY;
                msg.data.balance = 0;
                // Conecta com o servidor e manda a mensagem
                socket->Connect();
                socket->sendData(msg);
                socket->receiveData(msg);
                // Resposta do servidor OK - Mostra o saldo na tela
                // se nao - ocorreu uma falha na consulta
                if (msg.message_type == OK) {
                    cout << "Saldo corrente: " << msg.data.balance << " MC\n";
                } else {
                    cout << "Falha ao consultar saldo\n";
                }
                break;

            // Tratamento de entradas invalidas
            default:
                cout << "Insira uma operação válida...\n";
        }
        cout << "\n\n";
    }

    delete socket;

    return 0;
}