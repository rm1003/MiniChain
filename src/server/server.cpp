#include <openssl/sha.h>

#include "../blockchain.hpp"
#include "../customSocket.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    Blockchain *blockchain = new Blockchain();

    blockchain->print();

    Transation tr;

    tr.client_id = 1;
    tr.type = WITHDRAW;
    tr.value = 1500;

    blockchain->insert(tr);

    tr.client_id = 2;
    tr.type = DEPOSIT;
    tr.value = 2000;

    blockchain->insert(tr);

    blockchain->print();

    delete blockchain;

    return 0;
}