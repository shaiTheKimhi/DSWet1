#pragma once
#include<stdlib.h>
#include "List.h"

template <typename T>
class List;
template <typename T>
class Node;

template <typename T>
class Node {
public:

    T value;
     Node* _next;
     Node* _prev;

     Node():value(NULL),_next(NULL),_prev(NULL){};
     Node(T element, Node* nextInput = NULL, Node* prevInput = NULL): value(element),_next(nextInput),_prev(prevInput){};

     friend class List<T>;
};

template <typename T>
class List {
    int listSize = 0;
public:

    Node<T>* head;
    Node<T>* tail;


    ~List() {
        while(tail) {
            removeNode(tail);
        }
    };


    Node<T>* addNode (T n) {
        Node<T>* temp = new Node<T>(n);
        if (!(this->head)) {
            this->head = temp;
            this->tail = temp;
            this->listSize++;
            return temp;
        }
        (this->head)->_prev = temp;
        temp->_next = (this->head);
        temp->_prev = nullptr;
        (this->head) = temp;
        this->listSize++;
        return temp;
    }
    Node<T>* appendNode(T n) {
        Node<T>* temp = new Node<T>(n);
        if (!(this->tail)) {
            this->tail = temp;
            this->head = temp;
            this->listSize++;
            return temp;
        }
        (this->tail)->_next = temp;
        temp->_prev = (this->tail);
        temp->_next = nullptr;
        (this->tail) = temp;
        this->listSize++;
        return temp;
    }
    void removeNode(Node<T>* node) {
        if (!node || !this->head || !this->tail)
            return;
        if ((this->head) == node) {
            (this->head) = node->_next;
        }
        if ((this->tail) == node) {
            (this->tail) = node->_prev;
        }
        if (node->_next) {
            node->_next->_prev = node->_prev;
        }
        if (node->_prev) {
            node->_prev->_next = node->_next;
        }
        this->listSize--;
        delete(node);
        node = nullptr;
    }

    int getListSize () {
        return this->listSize;
    }
    Node<T>* getNext(Node<T>* node) {
        if (!node)
            return NULL;
        return node->_next;
    }
    Node<T>* getPrevious(Node<T>* node) {
        if (!node)
            return NULL;
        return node->_prev;
    }
    T getValue(Node<T>* node) {
        return node->value;
    }

    bool isEmpty () {
        return (head == nullptr) ;
    }
};

