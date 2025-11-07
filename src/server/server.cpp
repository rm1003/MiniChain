#include <netinet/in.h>
#include <openssl/sha.h>

#include <cstdio>
#include <cstring>

#include "../Blockchain.hpp"
#include "../CustomSocket.hpp"

using namespace std;

string hashF(const string data_to_hash) { return data_to_hash; };

int main(int argc, char *argv[]) {
    Blockchain *blockchain = new Blockchain(&hashF);

    Transation tr;

    tr.client_id = 1;
    tr.value = 100;
    tr.type = DEPOSIT;

    blockchain->Insert(tr);

    tr.client_id = 3;
    tr.value = 50;
    tr.type = WITHDRAW;

    blockchain->Insert(tr);

    blockchain->print();

    if (blockchain->VerifyBlockchainIntegrity()) {
        cout << "OK\n";
    } else {
        cout << "PROBLEM!\n";
    }

    delete blockchain;

    return 0;
}