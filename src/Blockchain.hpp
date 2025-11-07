#ifndef Blockchain_H
#define Blockchain_H

#include <openssl/sha.h>

#include <cstdlib>
#include <iostream>
#include <string>

#define HASH_SIZE SHA256_DIGEST_LENGTH

#define endl '\n'

using namespace std;

enum TRANSATION_TYPE { NONE, DEPOSIT, WITHDRAW };

typedef struct Transation {
    long int client_id;
    long int value;
    TRANSATION_TYPE type;
} Transation;

class Blockchain {
  private:
    struct Block {
        string actual_hash;
        string prev_hash;
        Transation transation;
        Block *next;

        Block(Block *nxt = nullptr) : next(nxt) {}
    };

    Block *head;
    Block *tail;
    size_t listSize;
    string (*ComputeHash)(const string data_to_hash);

    void error(const string &msg) const {
        cerr << "Error in Blockchain: " << msg << endl;
        exit(EXIT_FAILURE);
    }

  public:
    // ==========================================================================
    // constructor
    Blockchain(string (*hashFunction)(const string data_to_hash))
        : head(nullptr),
          tail(nullptr),
          listSize(0),
          ComputeHash(hashFunction) {}

    ~Blockchain() { Clear(); }

    void Clear() {
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

    size_t Size() const { return this->listSize; }

    bool IsEmpty() const { return this->listSize == 0; }

    void Insert(const Transation &tr) {
        string data_to_hash;

        Block *newBlock = new Block(nullptr);
        newBlock->prev_hash =
            (this->tail != nullptr) ? this->tail->actual_hash : "Primeiro Hash";
        newBlock->transation = tr;

        data_to_hash = newBlock->prev_hash;
        data_to_hash += newBlock->transation.client_id;
        data_to_hash += newBlock->transation.value;
        data_to_hash += newBlock->transation.type;

        newBlock->actual_hash = ComputeHash(data_to_hash);

        if (this->tail == nullptr) {
            this->head = newBlock;
            this->tail = newBlock;
        } else {
            this->tail->next = newBlock;
            this->tail = newBlock;
        }
        this->listSize++;
    }

    const Block &GetLastElement() const {
        if (IsEmpty()) {
            error("Cannot get last block from Blockchain");
        }
        return *this->tail;
    }

    const Block *getFirstNode() const {
        if (IsEmpty()) {
            error("Cannot get first block from Blockchain");
        }
        return this->head;
    }

    bool CheckHash(Block *block) {
        string verify = block->prev_hash;
        verify += block->transation.client_id;
        verify += block->transation.value;
        verify += block->transation.type;

        string hash = this->ComputeHash(verify);

        if (hash != block->actual_hash) return false;

        return true;
    }

    bool VerifyBlockchainIntegrity() {
        Block *current = this->head;

        while (current != nullptr) {
            if (!(CheckHash(current))) return false;
            current = current->next;
        }

        return true;
    }

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
