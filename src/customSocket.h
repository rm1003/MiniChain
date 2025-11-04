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


class serverSocket {
    private:
        int sock_listen;
        int sock_attends;
        char buffer[BUFSIZ + 1]; //tam = 8192
        struct sockaddr_in local_address, client_address;
        struct hostent *hp;
        char localhost[MAX_HOSTNAME];

        void error(const string& msg) const {
            cerr << "Error in serverSocket: " << msg << endl;
            exit(EXIT_FAILURE);
        }
 
    public:
        serverSocket(): sock_listen(0), sock_attends(0), hp(nullptr) {}

        ~serverSocket() {
            exit(EXIT_SUCCESS);
        }

        // init socket structs
        void init(int port) {

            gethostname(this->localhost, MAX_HOSTNAME);
            if ((this->hp = gethostbyname(this->localhost)) == NULL) {
                error("Not found my IP");
            }

            this->local_address.sin_port = htons(port);
            bcopy((char *) this->hp->h_addr, (char *) &this->local_address.sin_addr, this->hp->h_length);
            this->local_address.sin_family = SOCKET_TYPE;

            if ((this->sock_listen = socket(SOCKET_TYPE, SOCK_STREAM, 0)) < 0) {
                error("Unable to open socket (server)");
            }

            if (bind(this->sock_listen, (struct sockaddr *) &this->local_address, sizeof(this->local_address)) < 0) {
                error("It was not possible to bind");
            }
        }

        // function
        // void stabilizeConnection(char* buf) {
        //     socklen_t i;

        //     listen(this->sock_listen, QUEUE_SIZE);
        //     while(1) {
        //         i = sizeof(local_address);
        //         if ((this->sock_attends = accept(this->sock_listen, (struct sockaddr *) &this->client_address, &i)) < 0) {
        //             error("Unable to establish a connection");
        //         }
        //         read(this->sock_attends, this->buffer, BUFSIZ);
        //         memcpy(buf, &buffer, BUFSIZ);
        //         close(sock_attends);
        //     }
        // }
};

class clientSocket {
    private:
        int sock_fd;
        struct sockaddr_in server_address;
        struct hostent *hp;
        char buffer[BUFSIZ + 1];
        char *host;

        void error(const string& msg) const {
            cerr << "Error in clientSocket: " << msg << endl;
            exit(EXIT_FAILURE);
        }

    public:

        clientSocket(): sock_fd(0), hp(nullptr), host(nullptr) {}

        ~clientSocket() {
            exit(EXIT_SUCCESS);
        }

        void init(int port, char *server_name) {
            if ((this->hp = gethostbyname(server_name)) == NULL) {
                error("Unable to obtain server IP address");
            }
            
            bcopy((char *) this->hp->h_addr, (char *) &this->server_address.sin_addr, this->hp->h_length);
            this->server_address.sin_family = SOCKET_TYPE;
            this->server_address.sin_port = htons(port);

            if ((this->sock_fd = socket(SOCKET_TYPE, SOCK_STREAM, 0)) < 0) {
                error("Unable to open socket (client)");
            }

            if (connect(this->sock_fd, (struct sockaddr *) &this->server_address, sizeof(server_address)) < 0) {
                error("Unable to connect to server");
            }
        }

        // Remenber write 
};

#endif