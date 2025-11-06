#ifndef Blockchain_H
#define Blockchain_H

#include <openssl/sha.h>

#include <cstdlib>
#include <iostream>
#include <string>

#define HASH_SIZE SHA256_DIGEST_LENGTH

#define endl '\n'

using namespace std;

enum TRANSATION_TYPE { DEPOSIT = 0, WITHDRAW = 1 };

typedef struct Transation {
    TRANSATION_TYPE type;
    long int client_id;
    long int value;
} Transation;

class Blockchain {
  private:
    struct Block {
        unsigned char actual_hash[HASH_SIZE];
        unsigned char prev_hash[HASH_SIZE];
        Transation transation;
        Block *next;

        Block(Block *nxt = nullptr) : next(nxt) {}
    };

    Block *head;
    Block *tail;
    size_t listSize;

    void error(const string &msg) const {
        cerr << "Error in Blockchain: " << msg << endl;
        exit(EXIT_FAILURE);
    }

  public:
    // ==========================================================================
    // constructor
    Blockchain() : head(nullptr), tail(nullptr), listSize(0) {}

    ~Blockchain() { clear(); }

    void clear() {
        Block *current = this->head;
        while (current != nullptr) {
            Block *next = current->next;
            delete current;
            current = next;
        }
        this->head = nullptr;
        this->tail = nullptr;
        this->listSize = 0;
    }
    // ==========================================================================

    // // sha256 hash calc
    // string sha256(const string &str) {
    //     unsigned char hash[HASH_SIZE];
    //     SHA256_CTX sha256;
    //     SHA256_Init(&sha256);
    //     SHA256_Update(&sha256, str.c_str(), str.size());
    //     SHA256_Final(hash, &sha256);
    //     stringstream ss;
    //     for (int i = 0; i < HASH_SIZE; ++i) {
    //         ss << hex << setw(2) << setfill('0') << (unsigned int)hash[i];
    //     }
    //     return ss.str();
    // }

    // insert element
    void insert(const Transation &tr) {
        Block *newNode = new Block(nullptr);
        newNode->transation = tr;
        if (this->tail == nullptr) {
            this->head = newNode;
            this->tail = newNode;
        } else {
            this->tail->next = newNode;
            this->tail = newNode;
        }
        this->listSize++;
    }

    const Block &getLastElement() const {
        if (isEmpty()) {
            error("Cannot get last block from Blockchain");
        }
        return *this->tail;
    }

    const Block *getFirstNode() const {
        if (isEmpty()) {
            error("Cannot get first block from Blockchain");
        }
        return this->head;
    }

    size_t size() const { return this->listSize; }

    bool isEmpty() const { return this->listSize == 0; }

    Block *iterator(Block *node) {
        if (node == nullptr) return this->head;
        return node->next;
    }

    const Transation &getElement(const Block *node) { return node->transation; }

    // ==========================================================================
    void print() const {
        int cnt = 0;
        Block *current = this->head;
        cout << "Lista (Size " << this->listSize << "): " << endl;
        cout << "================================" << endl;
        while (current != nullptr) {
            cout << "Element #" << cnt++ << " | ";
            cout << "Next: " << (current->next ? "PRESENT" : "END") << endl;

            cout << "Transaction: Type=" << current->transation.type
                 << " ClientID=" << current->transation.client_id
                 << " Valor=" << current->transation.value << endl;
            cout << "----------------------------------" << endl;

            current = current->next;
        }
        cout << endl;
    }
};

#endif
