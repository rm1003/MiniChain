#ifndef CUSTOM_SOCKET_H
#define CUSTOM_SOCKET_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

using namespace std;

#define endl '\n'
#define SOCKET_TYPE AF_INET

#define MAX_HOSTNAME 64
#define QUEUE_SIZE 5
#define MAX_USERWORD 20

enum MESSAGE_TYPE { EMPTY, LOGIN, TRANSATION, TRANSFERENCE, QUERY, OK, ERROR };
enum MS_TRANSATION_TYPE {
    MS_NONE,
    MS_DEPOSIT,
    MS_WITHDRAW,
    MS_TRANSFERENCE,
    MS_INVALID_USER,
    MS_INSUFFICIENT
};
enum MS_LOGIN_TYPE { MS_REQUEST, MS_VALID, MS_INVALID };

typedef struct Message {
    unsigned long int client_id;
    union {
        struct login {
            char username[MAX_USERWORD + 1];
            char password[MAX_USERWORD + 1];
            MS_LOGIN_TYPE login_type;
        } login;

        struct transation {
            double value;
            MS_TRANSATION_TYPE transation_type;
        } transation;

        struct transfer {
            char destination_username[MAX_USERWORD + 1];
            double value;
            MS_TRANSATION_TYPE transation_type;
        } transfer;

        double balance;
    } data;
    MESSAGE_TYPE message_type;
} Message;

template <typename T>
class CustomSocket {
  private:
    void error(const string &msg) const {
        cerr << "Erro em CustomSocket: " << msg << endl;
        exit(EXIT_FAILURE);
    }

  protected:
    int sock_fd;

  public:
    CustomSocket() : sock_fd(-1) {}

    ~CustomSocket() {
        if (this->sock_fd >= 0) close(this->sock_fd);
    }

    bool sendData(const T &data) {
        if (this->sock_fd < 0) {
            error("Sem conexao para enviar dados");
        }

        long int sent = 1;
        unsigned long int total_sent = 0;
        const unsigned char *buff_data = (unsigned char *)&data;

        while ((total_sent < sizeof(T)) && (sent > 0)) {
            sent = write(this->sock_fd, buff_data + total_sent,
                         sizeof(T) - total_sent);
            if (sent < 0) {
                cerr << "Erro ao enviar dados" << endl;
                return false;
            }

            total_sent += sent;
        }

        return true;
    }

    bool receiveData(T &output) {
        if (this->sock_fd < 0) {
            error("Sem conexao para receber dados");
        }

        long int received = 1;
        unsigned long int total_received = 0;
        unsigned char *buff_data = (unsigned char *)&output;

        while ((total_received < sizeof(T) && (received > 0))) {
            received = read(this->sock_fd, buff_data + total_received,
                            sizeof(T) - total_received);
            if (received < 0) {
                cerr << "Erro ao receber dados" << endl;
                return false;
            }

            total_received += received;
        }

        return true;
    }

    bool isConnected() const { return this->sock_fd >= 0; }
};

class ServerSocket : public CustomSocket<Message> {
  private:
    int sock_listen;
    struct sockaddr_in local_address, client_address;
    struct hostent *hp;
    char localhost[MAX_HOSTNAME];

    void error(const string &msg) const {
        cerr << "Erro em ServerSocket: " << msg << endl;
        exit(EXIT_FAILURE);
    }

  public:
    ServerSocket() : CustomSocket(), sock_listen(0), hp(nullptr) {
        memset(&this->local_address, 0, sizeof(this->local_address));
        memset(&this->client_address, 0, sizeof(this->client_address));
    }

    ~ServerSocket() {
        if (this->sock_listen > 0) close(this->sock_listen);
    }

    // init socket structs
    void init(int port) {
        gethostname(this->localhost, MAX_HOSTNAME);
        if ((this->hp = gethostbyname(this->localhost)) == NULL) {
            error("Meu IP nao foi encontrado");
        }

        bcopy((char *)this->hp->h_addr, (char *)&this->local_address.sin_addr,
              this->hp->h_length);
        this->local_address.sin_family = SOCKET_TYPE;
        this->local_address.sin_port = htons(port);

        // this->local_address.sin_addr.s_addr = INADDR_ANY;

        if ((this->sock_listen = socket(SOCKET_TYPE, SOCK_STREAM, 0)) < 0) {
            error("Nao foi possivel abrir o socket (server)");
        }

        if (bind(this->sock_listen, (struct sockaddr *)&this->local_address,
                 sizeof(this->local_address)) < 0) {
            error("Nao foi possivel fazer o bind");
        }
    }

    void listenForConnection() {
        if (listen(this->sock_listen, QUEUE_SIZE) < 0) {
            error("Nao foi possivel escutar o socket");
        }
        cout << "Server ouvindo na porta: "
             << ntohs(this->local_address.sin_port) << endl;
    }

    bool acceptConnection() {
        socklen_t client_length = sizeof(this->client_address);
        this->sock_fd =
            accept(this->sock_listen, (struct sockaddr *)&this->client_address,
                   &client_length);

        if (this->sock_fd <= 0) {
            cerr << "Falha ao aceitar conexao" << endl;
            return false;
        }
        cout << "Cliente conectado: "
             << inet_ntoa(this->client_address.sin_addr) << ":"
             << ntohs(this->client_address.sin_port) << endl;
        return true;
    }

    void closeConnection() {
        if (this->sock_fd >= 0) {
            close(this->sock_fd);
            this->sock_fd = 0;
        }
    }
};

class ClientSocket : public CustomSocket<Message> {
  private:
    struct sockaddr_in server_address;
    struct hostent *hp;

    void error(const string &msg) const {
        cerr << "Erro em ClientSocket: " << msg << endl;
        exit(EXIT_FAILURE);
    }

  public:
    ClientSocket() : CustomSocket(), hp(nullptr) {}

    ~ClientSocket() {}

    void init(int port, const char *server_name) {
        if ((this->hp = gethostbyname(server_name)) == NULL) {
            error("Nao foi possivel obter o endereço IP do servidor");
        }

        bcopy((char *)this->hp->h_addr, (char *)&this->server_address.sin_addr,
              this->hp->h_length);
        this->server_address.sin_family = SOCKET_TYPE;
        this->server_address.sin_port = htons(port);
    }

    bool Connect() {
        if (this->sock_fd >= 0) {
            close(this->sock_fd);
            this->sock_fd = -1;
        }

        this->sock_fd = socket(SOCKET_TYPE, SOCK_STREAM, 0);
        if (this->sock_fd < 0) {
            cerr << "Nao foi possivel abrir o socket (client)" << endl;
            return false;
        }

        if (connect(this->sock_fd, (struct sockaddr *)&this->server_address,
                    sizeof(this->server_address)) < 0) {
            cerr << "Nao foi possivel se conectar ao servidor" << endl;
            close(this->sock_fd);
            this->sock_fd = -1;
            return false;
        }
        return true;
    }
};

#endif