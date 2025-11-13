#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

#define endl '\n'

using namespace std;

// Tipos de transacao
enum TRANSATION_TYPE { 
    NONE, 
    DEPOSIT, 
    WITHDRAW, 
    TRANSFER, 
    CHECK
};

// Informacoes da transacao
typedef struct Transation {
    unsigned long int client_id;
    unsigned long int dest_id;
    double value;
    time_t time;
    TRANSATION_TYPE type;
} Transation;


// superclass BlockChain (lista ligada)
class Blockchain {
    private:

        // Registro
        struct Block {
            string actual_hash;     // hash atual
            string prev_hash;       // hash anterior
            Transation transation;  // Informacoes da transacao
            Block *next;            // Proximo bloco

            Block(Block *nxt = nullptr) : next(nxt) {}
        };

        Block *head;
        Block *tail;
        size_t listSize;

        // Ponteiro para funcao de criptografia com hash
        string (*ComputeHash)(const string data_to_hash);

        void error(const string &msg) const {
            cerr << "Erro na Blockchain: " << msg << endl;
            exit(EXIT_FAILURE);
        }

        // Calcula a hash atual
        bool CheckHash(Block *block) {
            string verify = block->prev_hash;
            verify += block->transation.client_id;
            verify += block->transation.dest_id;
            verify += block->transation.value;
            verify += block->transation.time;
            verify += block->transation.type;
            
            // Chama a funcao de criptografia
            string hash = this->ComputeHash(verify);

            if (hash != block->actual_hash) return false;

            return true;
        }

    public:
        // =====================================================================
        // Construtor
        Blockchain(string (*hashFunction)(const string data_to_hash))
            : head(nullptr),
            tail(nullptr),
            listSize(0),
            ComputeHash(hashFunction) {}

        // Destrutor
        ~Blockchain() { Clear(); }

        // Libera a memoria de todos os blocos
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
        // =====================================================================

        // Metodo para obter o tamanho da lista
        size_t Size() const { return this->listSize; }

        // Metodo para verificar se a lista esta vazia
        bool IsEmpty() const { return this->listSize == 0; }

        // Metodo para insercao de uma nova transacao
        // Adicionar registro
        bool Insert(const Transation &tr) {
            double balance;
            string data_to_hash;
            
            // Verifica o tipo da transacao
            if (tr.type == WITHDRAW || tr.type == TRANSFER) {
                // Obtem o valor atual da conta do cliente
                if (this->GetUserBalance(tr.client_id, &balance)) {
                    if (balance - tr.value < 0) {
                        return false;
                    }
                }
            }
            
            // Verifica a integridade da Block Chain
            if (!(VerifyBlockchainIntegrity())) return false;

            // Aloca memoria para novo bloco
            Block *new_block = new Block(nullptr);
            new_block->prev_hash = (this->tail != nullptr)
                ? this->tail->actual_hash
                : ComputeHash("BLOCKCHAIN_GENESIS");
            new_block->transation = tr;
            
            // Concatena tudo em uma string
            data_to_hash = new_block->prev_hash;
            data_to_hash += new_block->transation.client_id;
            data_to_hash += new_block->transation.dest_id;
            data_to_hash += new_block->transation.value;
            data_to_hash += new_block->transation.time;
            data_to_hash += new_block->transation.type;
            
            // Faz a criptografia da string
            new_block->actual_hash = ComputeHash(data_to_hash);

            // Se for inicio coloca na cabeca da lista se nao coloca no final
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

        // Metodo para obter o primeiro nodo (na qual contem o primeiro bloco)
        const Block *getFirstNode() const {
            if (IsEmpty()) {
                error("Nao foi possivel obter o primeiro bloco da Blockchain");
            }
            return this->head;
        }

        // Metodo para obter o ultimo nodo
        const Block &GetLastElement() const {
            if (IsEmpty()) {
                error("Nao foi possivel obter o ultimo bloco da Blockchain");
            }
            return *this->tail;
        }

        // Metodo para obter o valor da conta do cliente
        bool GetUserBalance(unsigned long int id, double *balance) {
            double s = 0.0;
            Block *current = this->head;

            // Percorre desde o inicio da lista ligada e ir calculando ate
            // ultimo bloco, se corresponde ao cliente que esta querendo saber
            // o saldo computa, se nao passa para proximo nodo
            while (current != nullptr) {

                // Faz a verificacao da hash, se a hash nao bater significa que
                // teve algum erro sobre o bloco
                if (!(CheckHash(current))) return false;
                if (current->transation.client_id == id) {
                    if (current->transation.type == DEPOSIT) {
                        s += current->transation.value;
                    } else {
                        s -= current->transation.value;
                    }
                }
                if (current->transation.type == TRANSFER) {
                    if (current->transation.dest_id == id) {
                        s += current->transation.value;
                    }
                }

                current = current->next;
            }

            (*balance) = s;

            return true;
        }

        // Metodo para verificar a integridade da block chain
        bool VerifyBlockchainIntegrity() {
            Block *current = this->head;

            // Percorre a lista ligada inteira calculando as hashes
            while (current != nullptr) {
                if (!(CheckHash(current))) return false;
                current = current->next;
            }

            return true;
        }

        // =====================================================================
        // Debug print
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
                    << " DestinatarioID=" << current->transation.dest_id
                    << " Valor=" << current->transation.value << endl;
                cout << "----------------------------------" << endl;

                current = current->next;
            }
            cout << endl;
        }
};

#endif
