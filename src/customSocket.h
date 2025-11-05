#ifndef CUSTOM_SOCKET_H
#define CUSTOM_SOCKET_H

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

#define endl '\n'
#define SOCKET_TYPE AF_INET

#define MAX_HOSTNAME    64
#define QUEUE_SIZE      5
#define BUFFER_SIZE     8192


class customSocket {
    private:
        char buffer[BUFFER_SIZE + 1]; //tam = 8192 + 1

        void error(const string& msg) const {
            cerr << "Error in customSocket: " << msg << endl;
            exit(EXIT_FAILURE);
        }

    protected:
        int sock_fd;

    public:
        customSocket(): sock_fd(0) {
            memset(this->buffer, 0, BUFFER_SIZE + 1);
        }

        ~customSocket() {
            if (this->sock_fd > 0) close(this->sock_fd);
        }
        
        // send data
        bool sendData(const string& data) {
            if (this->sock_fd <= 0) {
                error("No connection for sending data");
            }

            int sents = write(this->sock_fd, data.c_str(), data.length());
            if (sents < 0) {
                cerr << "Error sending data" << endl;
                return false;
            }

            return true;
        }
        
        string receiveData() {
            if (this->sock_fd <= 0) {
                error("No connection for receiving data");
            }

            memset(this->buffer, 0, BUFFER_SIZE + 1);
            int received = read(this->sock_fd, this->buffer, BUFFER_SIZE + 1);
            if (received < 0) {
                error("Error reading from socket");
            }

            return string(this->buffer, received);
        }

        bool isConnected() const {
            return this->sock_fd > 0;
        }
};


class serverSocket : public customSocket {
    private:
        int sock_listen;
        struct sockaddr_in local_address, client_address;
        struct hostent *hp;
        char localhost[MAX_HOSTNAME];

        void error(const string& msg) const {
            cerr << "Error in serverSocket: " << msg << endl;
            exit(EXIT_FAILURE);
        }
 
    public:
        serverSocket(): customSocket(), sock_listen(0), hp(nullptr) {
            memset(&this->local_address, 0, sizeof(this->local_address));
            memset(&this->client_address, 0, sizeof(this->client_address));
        }

        ~serverSocket() {
            if (this->sock_listen > 0) close(this->sock_listen);
        }

        // init socket structs
        void init(int port) {

            gethostname(this->localhost, MAX_HOSTNAME);
            if ((this->hp = gethostbyname(this->localhost)) == NULL) {
                error("Not found my IP");
            }

            bcopy((char *) this->hp->h_addr, (char *) &this->local_address.sin_addr, this->hp->h_length);
            this->local_address.sin_family = SOCKET_TYPE;
            this->local_address.sin_port = htons(port);

            // this->local_address.sin_addr.s_addr = INADDR_ANY;

            if ((this->sock_listen = socket(SOCKET_TYPE, SOCK_STREAM, 0)) < 0) {
                error("Unable to open socket (server)");
            }

            if (bind(this->sock_listen, (struct sockaddr *) &this->local_address, sizeof(this->local_address)) < 0) {
                error("It was not possible to bind");
            }
        }

        void listenForConnection() {
            if(listen(this->sock_listen, QUEUE_SIZE) < 0) {
                error("Unable to listen on socket");
            }
            cout << "Server listening on port " << ntohs(this->local_address.sin_port) << endl;
        }

        bool acceptConnection() {
            socklen_t client_length = sizeof(this->client_address);
            sock_fd = accept(this->sock_listen, (struct sockaddr *) &this->client_address, &client_length);
            
            if (sock_fd <= 0) {
                cerr << "Failed to accept connection" << endl;
                return false;
            }
            cout << "Client connected: " << inet_ntoa(this->client_address.sin_addr) << ":" << ntohs(this->client_address.sin_port) << endl;
            return true;
        }

        void closeConnection() {
            if (sock_fd >= 0) {
                close(sock_fd);
                sock_fd = 0;
            }
        }
};

class clientSocket : public customSocket {
    private:
        struct sockaddr_in server_address;
        struct hostent *hp;

        void error(const string& msg) const {
            cerr << "Error in clientSocket: " << msg << endl;
            exit(EXIT_FAILURE);
        }

    public:

        clientSocket(): customSocket(), hp(nullptr) {}

        ~clientSocket() {}

        void init(int port, const char *server_name) {
            if ((this->hp = gethostbyname(server_name)) == NULL) {
                error("Unable to obtain server IP address");
            }
            
            bcopy((char *) this->hp->h_addr, (char *) &this->server_address.sin_addr, this->hp->h_length);
            this->server_address.sin_family = SOCKET_TYPE;
            this->server_address.sin_port = htons(port);

            if ((sock_fd = socket(SOCKET_TYPE, SOCK_STREAM, 0)) < 0) {
                error("Unable to open socket (client)");
            }

            if (connect(sock_fd, (struct sockaddr *) &this->server_address, sizeof(this->server_address)) < 0) {
                error("Unable to connect to server");
            }
        }
};

#endif