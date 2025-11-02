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
            T element;
            Node* next;
            Node* prev;
            Node(const T& elem, Node* nxt = nullptr, Node* prv = nullptr)
                :element(elem), next(nxt), prev(prv) {}
        };

        Node* head;
        Node* tail;
        unsigned int listSize;

        void error(const string& msg) const {
            cerr << "Error in LinkedList: " << msg << endl;
            exit(EXIT_FAILURE);
        }


    public:
        // constructor
        LinkedList(): head(nullptr), tail(nullptr), listSize(0) {}
        // copy constructor
        LinkedList(const LinkedList& other):head(nullptr), tail(nullptr), listSize(0) {
            Node* current = other.head;
            while (current != nullptr) {
                insertTail(current->element);
                current = current->next;
            }
        }
        
        // zero-copy
        LinkedList& operator=(const LinkedList& other) {
            if (this != &other) {
                clear();
                Node* current = other.head;
                while (current != nullptr) {
                    insertTail(current->element);
                    current = current->next;
                }
            }
            return *this;
        }

        ~LinkedList() {
            clear();
        }

        // insert (head)
        void insertHead(const T& element) {
            Node* newNode = new Node(element,head,nullptr);
            if (head != nullptr) {
                head->prev = newNode;
            }
            head = newNode;
            if (tail == nullptr) {
                tail = newNode;
            }
            listSize++;
        }

        // remove (head)
        T removeHead() {
            if (isEmpty()) {
                error("Cannot remove from an empty list");
            }
            Node* remove = head;
            T element = remove->element;
            head = head->next;
            if (head == nullptr) {
                tail = nullptr;
            } else {
                head->prev = nullptr;
            }
            delete remove;
            listSize--;
            return element;
        }
        
        // insert (tail)
        void insertTail(const T& element) {
            Node* newNode = new Node(element,nullptr,tail);
            if (isEmpty()) {
                head = tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
            listSize++;
        }

        // remove (normal)
        T remove() {
            if (isEmpty()) {
                error("Cannot remove from an empty list");
            }

            Node* removeNode = tail;
            T element = removeNode->element;
    
            if (head == tail) {
                head = tail = nullptr;
            } else {
                tail = tail->prev;
                tail->next = nullptr;
            }

            delete removeNode;
            listSize--;
            return element;
        }
        
        // return list size
        unsigned int getSize() const {
            return listSize;
        }

        bool isEmpty() const {
            return listSize == 0;
        }

        void clear() {
            Node* current = head;
            while (current != nullptr) {
                Node* next = current->next;
                delete current;
                current = next;
            }
            head = tail = nullptr;
            listSize = 0;
        }


        T& front() {
            if (isEmpty()) {
                error("Cannot access front of empty list");
            }
            return head->element;
        }

        const T& front() const {
            if (isEmpty()) {
                error("Cannot access front of empty list");
            }
            return head->element;
        }


        T& back() {
            if (isEmpty()) {
                error("Cannot access back of empty list");
            }
            return tail->element;
        }

        const T& back() const {
            if (isEmpty()) {
                error("Cannot access back of empty list");
            }
            return tail->element;
        }

        void print() const {
            Node* current = head;
            cout << "Lista: ";
            while (current != nullptr) {
                cout << current->element;
                if (current->next != nullptr) {
                    cout << " <-> ";
                }
                current = current->next;
            }
            cout << endl;
        }

};

#endif
