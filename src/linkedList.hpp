#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
#include <cstdlib>
#include <string>

#define HASH_SIZE SHA256_DIGEST_LENGTH

#define endl '\n'

using namespace std;

template <typename T>
class LinkedList {
    private:
        enum TRANSATION_TYPE {
            DEPOSIT = 0,
            WITHDRAW = 1,
            TRANSFER = 2
        };

        struct Transation {
            TRANSATION_TYPE Type;
            int client_id;
            int valor;
        };

        struct Block {
            unsigned char actual_hash[HASH_SIZE];
            unsigned char prev_hash[HASH_SIZE];
            Transation transation;
            Block* next;

            Block(const Block* nxt = nullptr): next(nxt) {}
        };

        Node* head;
        Node* tail;
        size_t listSize;

        void error(const string& msg) const {
            cerr << "Error in LinkedList: " << msg << endl;
            exit(EXIT_FAILURE);
        }


    public:
// ==========================================================================
        // constructor
        LinkedList(): head(nullptr), tail(nullptr), listSize(0) {}

        ~LinkedList() {
            clear();
        }

        void clear() {
            Block* current = this->head;
            while (current != nullptr) {
                Block* next = current->next;
                delete current;
                current = next;
            }
            this->head = nullptr;
            this->tail = nullptr;
            this->listSize = 0;
        }
// ==========================================================================

        // sha256 hash calc
        string sha256(const string &str) {
            unsigned char hash[HASH_SIZE];
            SHA256_CTX sha256;
            SHA256_Init(&sha256);
            SHA256_Update(&sha256, str.c_str(), str.size());
            SHA256_Final(hash, &sha256);
            stringstream ss;
            for (int i = 0; i < HASH_SIZE; ++i) {
                ss << hex << setw(2) << setfill('0') << (int)hash[i];
            }
            return ss.str();
        }


        // insert element
        // @@@@ REFAZER
        void insert(const Transation) {
            Block* newNode = new Block(nullptr);
            if (this->tail == nullptr) {
                this->head = newNode;
                this->tail = newNode;
            } else {
                this->tail->next = newNode;
                this->tail = newNode;
            }
            this->listSize++;
        }

        const T& getLastElement() const {
            if (isEmpty()) {
                error("Cannot get last block from LinkedList");
            }
            return this->tail->element;
        }

        const Node& getFirstNode() const {
            if (isEmpty()) {
                error("Cannot get last block from LinkedList");
            }
            return this->head;
        }

        size_t size() const {
            return this->listSize;
        }

        bool isEmpty() const {
            return this->listSize == 0;
        }

        const Node& iterator(const Node& node) {
            if (node == nullptr)
                return this->head;
            return node->next;
        }

        const T& getElement(const Node* node) {
            return node->element;
        }

// ==========================================================================
        void print() const {
            int cnt = 0;
            Node* current = this->head;
            cout << "Lista (Size " << this->listSize << "): " << endl;
            cout << "================================" << endl;
            while (current != nullptr) {
                cout << "Element #" << cnt++ << "|";
                cout << "Next: " << (current->next ? cnt : "END");
                cout << endl;

                cout << "Data: " << current->element << endl;
                cout << "----------------------------------" << endl;
                
                current = current->next;
            }
            cout << endl;
        }
};

#endif
