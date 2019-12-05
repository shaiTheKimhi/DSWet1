#pragma once

#include<stdlib.h>
#include "List.h"


template<typename T>
struct _node {
    T value;
    struct _node *_next;
    struct _node *_prev;
};

template<typename T>
_node<T> *createNode(T value);

template<typename T>
void addNode(_node<T> **head, _node<T> *n);

template<typename T>
void removeNode(_node<T> **head, _node<T> *node);

template<typename T>
_node<T> *getNext(_node<T> *node);

template<typename T>
_node<T> *getPrevious(_node<T> *node);

template<typename T>
T getValue(_node<T> *node);


//struct Node functions
template<typename T>
_node<T> *createNode(T value) {
    _node<T> *t = (_node<T> *) malloc(sizeof(_node<T>));
    t->value = value;
    t->_prev = 0;
    t->_next = 0;
    return t;
}

template<typename T>
void addNode(_node<T> **head, _node<T> *n) {
    if (!head || !n)
        return;
    if (!(*head)) {
        *head = n;
        return;
    }
    (*head)->_prev = n;
    n->_next = (*head);
    n->_prev = 0;
    (*head) = n;
}

template<typename T>
void removeNode(_node<T> **head, _node<T> *node) {
    if (!node || !head || !(*head))
        return;
    if ((*head) == node)
        (*head) = node->_next;
    if (node->_next)
        node->_next->_prev = node->_prev;
    if (node->_prev)
        node->_prev->_next = node->_next;
    free(node);
}

template<typename T>
_node<T> *getNext(_node<T> *node) {
    if (!node)
        return NULL;
    return node->_next;
}

template<typename T>
_node<T> *getPrevious(_node<T> *node) {
    if (!node)
        return NULL;
    return node->_prev;
}

template<typename T>
T getValue(_node<T> *node) {
    return node->value;
}
