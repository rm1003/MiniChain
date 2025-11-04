#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
#include <cstdlib>
#include <string>

#define endl '\n'

using namespace std;

template <typename T>
class LinkedList {
    private:
        struct Node {
            const T element;
            Node* next;
            Node* prev;
            size_t index;

            Node(const T& elem, Node* nxt = nullptr, Node* prv = nullptr, size_t idx = 0)
                :element(elem), next(nxt), prev(prv), index(idx) {}
        };

        Node* head;
        Node* tail;
        size_t listSize;
        size_t currentIdx;

        void error(const string& msg) const {
            cerr << "Error in LinkedList: " << msg << endl;
            exit(EXIT_FAILURE);
        }


    public:
// ==========================================================================
        // constructor
        LinkedList(): head(nullptr), tail(nullptr), listSize(0), currentIdx(0) {}

        ~LinkedList() {
            clear();
        }

        void clear() {
            Node* current = this->head;
            while (current != nullptr) {
                Node* next = current->next;
                delete current;
                current = next;
            }
            this->head = nullptr;
            this->tail = nullptr;
            this->listSize = 0;
            this->currentIdx = 0;
        }
// ==========================================================================

        // insert element
        void insert(const T& newElement) {
            Node* newNode = new Node(newElement, nullptr, this->tail, currentIdx++);
            if (this->tail == nullptr) {
                this->head = newNode;
                this->tail = newNode;
            } else {
                this->tail->next = newNode;
                this->tail = newNode;
            }
            this->listSize++;
        }
        
        const T& accessWithIdx(size_t index) {
            Node* current = this->head;
            while (current != nullptr) {
                if (current->index == index) {
                    return current->element;
                }
                current = current->next;
            }
            error("Element not found at index: " + to_string(index));
            return this->head->element;
        }

        const T& getLastElement() const {
            if (isEmpty()) {
                error("Cannot get last block from LinkedList");
            }
            return this->tail->element;
        }

        const T& getFirstElement() const {
            if (isEmpty()) {
                error("Cannot get last block from LinkedList");
            }
            return this->head->element;
        }

        size_t size() const {
            return this->listSize;
        }

        bool isEmpty() const {
            return this->listSize == 0;
        }

// ==========================================================================
        void print() const {
            Node* current = this->head;
            cout << "Lista (Size " << this->listSize << "): " << endl;
            cout << "================================" << endl;
            while (current != nullptr) {
                cout << "Element #" << current->index << "|";
                cout << "Prev: " << (current->prev ? to_string(current->prev->index) : "GENESIS");
                cout << " | Next: " << (current->next ? to_string(current->next->index) : "END");
                cout << endl;

                cout << "Data: " << current->element << endl;
                cout << "----------------------------------" << endl;
                
                current = current->next;
            }
            cout << endl;
        }

};

#endif
