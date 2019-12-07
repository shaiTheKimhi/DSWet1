//
// Created by Yaara Karmy on 01/12/2019.
//

#ifndef AVLTREE_AVLTREE_H
#define AVLTREE_AVLTREE_H

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

template<class K, class D>
class AVLTree {
    class AVLNode {
        K key;
        D data;
        int height;
        AVLNode* parent;
        AVLNode* leftSon;
        AVLNode* rightSon;

        AVLNode(const K& key, const D& data, AVLNode* p = NULL) : key(key), data(data),
                                                                  height(0),parent(p), leftSon(NULL), rightSon(NULL) {}

        ~AVLNode() {
            if (leftSon != NULL) {
                delete leftSon;
            }
            if (rightSon != NULL) {
                delete rightSon;
            }
        }

        void updateHeight() {
            int left = leftSon ? leftSon->height : -1;
            int right = rightSon ? rightSon->height : -1;
            height = (left > right ? left : right) + 1;
        }

        bool isLeaf() {
            return (!leftSon && !rightSon);
        }

        bool haveTwoSons() {
            return (leftSon && rightSon);
        }

        int getBalanceFactor() {
            return (leftSon ? leftSon->height : -1) - (rightSon ? rightSon->height : -1);
        }

        friend class AVLTree;

    public:
        D getData(){
            return data;
        }
    };

    AVLNode* root; //Any tree is a Node as a root
    int size; //how many nodes

    //main funcs in avl tree
    void insert(K key, D data, AVLNode* parent);
    void deleteNode(AVLNode* toDelete);

    void changeNode(AVLNode* parent, AVLNode* oldNode, AVLNode* newNode);
    void reBalance(AVLNode* node, bool isDelete = false);

public:
    AVLTree() : root(NULL), size(0) {}
    ~AVLTree() {
        if (root != NULL) {
            delete root;
        }
    }
    bool isEmpty();
    //auxiliary funcs to main funcs
    void insert(K key, D data);
    bool deleteKey(K key);
    bool isExist(K key) const;
    bool isExist(K key);

    void changeData(K key, D new_data);
    void destroyData();
    void destroyData(AVLNode* node);

    bool isBalanced() const {
        if (!root) return true;
        int leftSonHeight = getHeight(root->leftSon);
        int rightSonHeight = getHeight(root->rightSon);
        return (abs(leftSonHeight-rightSonHeight) <= 1 && isBalanced(root->rightSon) && isBalanced(root->rightSon));
    }

    //rotation
    void updateParentsAfterRotation(AVLNode* node, AVLNode* sonNode, AVLNode* directedSon) {
        if (directedSon) {
            directedSon->parent = node;
        }
        sonNode->parent = node->parent;
        node->parent = sonNode;
    }
    //rotating left
    void rotateRR(AVLNode* node) {
        AVLNode* temp = node->rightSon;
        node->rightSon = temp->leftSon;
        temp->leftSon = node;

        updateParentsAfterRotation(node,temp,node->rightSon);
        changeNode(temp->parent, node, temp);
        node->updateHeight();
        temp->updateHeight();
    }

    //rotating right
    void rotateLL(AVLNode* node) {
        AVLNode* temp = node->leftSon;
        node->leftSon = temp->rightSon;
        temp->rightSon = node;

        updateParentsAfterRotation(node,temp,node->leftSon);
        changeNode(temp->parent, node, temp);
        node->updateHeight();
        temp->updateHeight();
    }

    //rotate left then right
    void rotateLR(AVLNode* node) {
        rotateRR(node->leftSon);
        rotateLL(node);
    }

    //rotate right then left
    void rotateRL(AVLNode* node) {
        rotateLL(node->rightSon);
        rotateRR(node);
    }

    //getters
    D getData(K key) const;
    int getSize() const {
        return size;
    }
    int getHeight() const {
        return root->height;
    }

    //TODO: check if needed, check how to free? if needed - needs malloc!
    K* inOrderKeyArray() const {
        K* inOrderKeys;
        inOrderKeys = (K*)malloc(size*sizeof(*inOrderKeys));
        if (inOrderKeys) {
            AVLNode* nodes[size];
            int i = 0;
            inOrderNodeArray(root, nodes, &i);
            for (int j = 0; j < size; ++j) {
                inOrderKeys[j] = nodes[j]->key;
            }
            return inOrderKeys;
        }
        return NULL;
    }

    D* inOrderDataArray() {
        D* inOrderData;
        inOrderData =  (D*)malloc(size*sizeof(*inOrderData));
        if (inOrderData) {
            AVLNode* nodes[size];
            int i = 0;
            inOrderNodeArray(root, nodes, &i);
            for (int j = 0; j < size; ++j) {
                inOrderData[j] = nodes[j]->data;
            }
            return inOrderData;
        }
        return NULL;
    }

    void inOrderNodeArray(AVLNode* node, AVLNode* arr[], int* i) const {
        if (node) {
            inOrderNodeArray(node->leftSon, arr, i);
            arr[(*i)] = node;
            (*i)++;
            inOrderNodeArray(node->rightSon, arr, i);
        }
    }

    //TODO: check if needed
    K* postOrderKeyArray() const {
        K* postOrderKeys;
        postOrderKeys = (K*)malloc(size*sizeof(*postOrderKeys));
        if (postOrderKeys) {
            AVLNode* nodes[size];
            int i=0;
            postOrderNodeArray(root, nodes, &i);
            for (int j = 0; j < size; ++j) {
                postOrderKeys[j] = nodes[j]->key;
            }
            return postOrderKeys;
        }
        return NULL;
    }
    void postOrderNodeArray(AVLNode* node, AVLNode* arr[], int* i) const {
        if (node) {
            postOrderNodeArray(node->leftSon, arr, i);
            postOrderNodeArray(node->rightSon, arr, i);
            arr[(*i)] = node;
            (*i)++;
        }
    }

    //TODO: check if needed
    K* preOrderKeyArray() const {
        K* preOrderKeys;
        preOrderKeys = (K*)malloc(size*sizeof(*preOrderKeys));
        if (preOrderKeys) {
            AVLNode* nodes[size];
            int i=0;
            preOrderNodeArray(root, nodes, &i);
            for (int j = 0; j < size; ++j) {
                preOrderKeys[j] = nodes[j]->key;
            }
            return preOrderKeys;
        }
        return NULL;
    }
    void preOrderNodeArray(AVLNode* node, AVLNode* arr[], int* i) const {
        if (node) {
            arr[(*i)] = node;
            (*i)++;
            preOrderNodeArray(node->leftSon, arr, i);
            preOrderNodeArray(node->rightSon, arr, i);
        }
    }
    //TODO:print funcs for debug only - no need to submit (delete before submit)
    void inorderPrint(AVLNode* n) const {
        if (n) {
            inorderPrint(n->leftSon);
            cout << n->key << " ";
            inorderPrint(n->rightSon);
        }
    }




    void print() const {
        if (size == 0) cout << "Empty tree" << endl;
        else inorderPrint(root);
        cout << "\nDone" << endl;
    }
    void printByLevel() const {
        cout << "Printing by levels: ";
        for (int i = 0; i <= root->height; ++i) {
            cout << "\nLevel " << i + 1 << ": ";
            printLevel(root, i);
        }
    }
    void printLevel(AVLNode* node, int level) const {
        if (node) {
            if (level == 0) {
                cout << node->key << " ";
            } else {
                printLevel(node->left, level - 1);
                printLevel(node->right, level - 1);
            }
        }
    }
    void printGraph(bool BF = false) const {
        printGraph(root, "", BF);
    }

    void printGraph(AVLNode* node, const std::string& prefix, bool BF) const {
        if (node) {
            printGraph(node->rightSon, prefix + "     ", BF);
            cout << prefix;
            cout << "|--[";
            cout << node->key;
            if (BF) cout << ", BF: " << node->getBalanceFactor();
            cout << "]" << endl;
            printGraph(node->leftSon, prefix + "     ", BF);
        }
    }

    AVLNode* findAVLNode (K key) {
        AVLNode* current = root;
        while (current) {
            if (current->key == key) return current;
            if (current->key > key) {
                current = current->leftSon;
            } else {
                current = current->rightSon;
            }
        }
        return NULL;
    }
};



template<class K, class D>
void AVLTree<K, D>::insert(K key, D data) {
    if (root == NULL) {
        root = new AVLNode(key, data);
        size++;
    } else {
        insert(key, data, root);
    }
}

template<class K, class D>
void AVLTree<K, D>::destroyData() {
    if (root) {
        destroyData(root);
    }
}
template<class K, class D>
void AVLTree<K, D>::destroyData(AVLNode* node) {
    if (node) {
        destroyData(node->leftSon);
        destroyData(node->rightSon);
        delete node->data;
    }
}


template<class K, class D>
void AVLTree<K, D>::insert(K key, D data, AVLNode* parent) {
    if (key == parent->key) {
        parent->data = data;
        return;
    }
    if (parent->key > key) {
        if (parent->leftSon) insert(key, data, parent->leftSon);
        else {
            parent->leftSon = new AVLNode(key, data, parent);
            size++;
        }
    } else {
        if (parent->rightSon) insert(key, data, parent->rightSon);
        else {
            parent->rightSon = new AVLNode(key, data, parent);
            size++;
        }
    }
    reBalance(parent, false);
}

// Removing nodes
template<class K, class D>
bool AVLTree<K, D>::deleteKey(K key) {
    AVLNode* current = findAVLNode(key);
    if (!current) return false;
    deleteNode(current);
    size--;
    return true;
}
template<class K, class D>
void AVLTree<K, D>::deleteNode(AVLNode* toDelete) {
    AVLNode* ancestor = toDelete->rightSon;
    if (toDelete->haveTwoSons()) { // node has Two sons
        while (ancestor->leftSon) {
            ancestor = ancestor->leftSon;
        }
    } else if (toDelete->leftSon) { // node only have left son
        ancestor = toDelete->leftSon;
    }
    if (ancestor) {
        toDelete->key = ancestor->key;
        toDelete->data = ancestor->data;
        deleteNode(ancestor);
    } else { // node is a leaf or remained a leaf
        AVLNode* parent = toDelete->parent;
        if (!parent) {
            root = nullptr;
        }
        else if (parent->leftSon == toDelete) {
            parent->leftSon = nullptr;
        }
        else {
            parent->rightSon = nullptr;
        }
        delete(toDelete);
        toDelete = nullptr;
        reBalance(parent,true);
    }
}
template<class K, class D>
bool AVLTree<K, D>::isExist(K key) const {
    AVLNode* current = findAVLNode(key);
    return (current != NULL);
}

template<class K, class D>
bool AVLTree<K, D>::isExist(K key) {
    AVLNode* current = findAVLNode(key);
    return (current != NULL);
}
template<class K, class D>
D AVLTree<K, D>::getData(K key) const {
    AVLNode* nodeToGetData = findAVLNode(key);
    if (nodeToGetData) {
        return nodeToGetData->data;
    } else return NULL;
}
template<class K, class D>
void AVLTree<K, D>::changeData(K key, D new_data){
    AVLNode* nodeToChange = findAVLNode(key);
    if (nodeToChange) {
        nodeToChange->data = new_data;
    }
}

template<class K, class D>
void AVLTree<K, D>::reBalance(AVLNode* node, bool isDelete) {
    if (!node) return;
    node->updateHeight();
    int balanceFactor = node->getBalanceFactor();
    if (balanceFactor == 2) {
        if (node->leftSon->getBalanceFactor() >= 0) rotateLL(node);
        else rotateLR(node);
    } else if (balanceFactor == -2) {
        if (node->rightSon->getBalanceFactor() <= 0) rotateRR(node);
        else rotateRL(node);
    }
    if (isDelete) reBalance(node->parent, isDelete);
}

template<class K, class D>
void AVLTree<K, D>::changeNode(AVLNode* parent, AVLNode* oldNode, AVLNode* newNode) {
    if (parent == NULL) root = newNode;
    else if (parent->leftSon == oldNode) parent->leftSon = newNode;
    else parent->rightSon = newNode;
}
template<class K, class D>
bool AVLTree<K, D>::isEmpty()
{
    return this->root == NULL;
}


#endif //AVLTREE_AVLTREE_H
