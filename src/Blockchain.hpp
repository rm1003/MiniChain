#ifndef Blockchain_H
#define Blockchain_H

#include <cstdlib>
#include <iostream>
#include <string>

#define endl '\n'

using namespace std;

enum TRANSATION_TYPE { NONE, DEPOSIT, WITHDRAW, CHECK };

typedef struct Transation {
    unsigned long int client_id;
    double value;
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
        cerr << "Erro na Blockchain: " << msg << endl;
        exit(EXIT_FAILURE);
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

    bool Insert(const Transation &tr) {
        double balance;
        string data_to_hash;

        if (tr.type == WITHDRAW) {
            if (this->GetUserBalance(tr.client_id, &balance)) {
                if (balance - tr.value < 0) {
                    return false;
                }
            }
        }

        Block *new_block = new Block(nullptr);
        new_block->prev_hash = (this->tail != nullptr)
                                   ? this->tail->actual_hash
                                   : ComputeHash("BLOCKCHAIN_GENESIS");
        new_block->transation = tr;

        data_to_hash = new_block->prev_hash;
        data_to_hash += new_block->transation.client_id;
        data_to_hash += new_block->transation.value;
        data_to_hash += new_block->transation.type;

        new_block->actual_hash = ComputeHash(data_to_hash);

        if (this->tail == nullptr) {
            this->head = new_block;
            this->tail = new_block;
        } else {
            this->tail->next = new_block;
            this->tail = new_block;
        }
        this->listSize++;

        return true;
    }

    const Block *getFirstNode() const {
        if (IsEmpty()) {
            error("Nao foi possivel obter o primeiro bloco da Blockchain");
        }
        return this->head;
    }

    const Block &GetLastElement() const {
        if (IsEmpty()) {
            error("Nao foi possivel obter o ultimo bloco da Blockchain");
        }
        return *this->tail;
    }

    bool GetUserBalance(unsigned long int id, double *balance) {
        double s = 0.0;
        Block *current = this->head;

        while (current != nullptr) {
            if (!(CheckHash(current))) return false;
            if (current->transation.client_id == id) {
                if (current->transation.type == DEPOSIT) {
                    s += current->transation.value;
                } else if (current->transation.type == WITHDRAW) {
                    s -= current->transation.value;
                }
            }
            current = current->next;
        }

        (*balance) = s;

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
        cout << "Lista (Tamanho " << this->listSize << "): " << endl;
        cout << "================================" << endl;
        while (current != nullptr) {
            cout << "Elemento #" << cnt++ << " | ";
            cout << "Prox: " << (current->next ? "PRESENTE" : "FIM") << endl;

            cout << "Transacao: Tipo=" << current->transation.type
                 << " ClienteID=" << current->transation.client_id
                 << " Valor=" << current->transation.value << endl;
            cout << "----------------------------------" << endl;

            current = current->next;
        }
        cout << endl;
    }
};

#endif
