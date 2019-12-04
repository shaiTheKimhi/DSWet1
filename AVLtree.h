//
// Created by Yaara Karmy on 02/12/2019.
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
            return (this->leftSon && this->rightSon);
        }

        int BalanceFactorCalc() {
            return (leftSon ? leftSon->height : -1) - (rightSon ? rightSon->height : -1);
        }

        friend class AVLTree;
    };

    AVLNode* root;
    int size;

    void insert(K key, D data, AVLNode* parent);
    void remove(AVLNode* toDelete);
    void change(AVLNode* parent, AVLNode* oldNode, AVLNode* newNode);
    void rebalance(AVLNode* node, bool remove = false);

    void inorderPrint(AVLNode* n) const;
    void printLevel(AVLNode* node, int level) const;
    void printGraph(AVLNode* node, const std::string& prefix, bool BF) const;

public:
    AVLTree() : root(NULL), size(0) {}
    ~AVLTree() {
        if (root != NULL) {
            delete root;
        }
    }
    void destroyData();
    void destroyData(AVLNode* node);

    void insert(K key, D data);
    bool remove(K key);
    bool isExist(K key) const;
    void changeData(K key, D new_data);

    //rotating
    void rotateRight(AVLNode* pRoot, AVLNode* pLeft);
    void rotateLeft(AVLNode* pRoot, AVLNode* pRight);

    void rotateRR(AVLNode* node) {
        rotateLeft(node, node->rightSon);
    }
    void rotateLL(AVLNode* node) {
        rotateRight(node, node->leftSon);
    }
    void rotateLR(AVLNode* node) {
        rotateLeft(node->leftSon, node->leftSon->rightSon);
        rotateRight(node, node->leftSon);
    }
    void rotateRL(AVLNode* node) {
        rotateRight(node->rightSon, node->rightSon->leftSon);
        rotateLeft(node, node->rightSon);
    }

    //getters
    D getData(K key) const;
    int getSize() const {
        return size;
    }
    int calcHeight() const {
        return root->height;
    }
    bool isBalanced() const {
        if (!root) return true;
        int height = root->height;
        double min = floor(log2(size));
        double max = ceil(1.44*log2(size + 1) - 0.328);
        return min <= height && height <= max;
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
    void printGraph(bool BF = false) const;

    K* keyArray() const;
    void inorderNodeArray(AVLNode* node, AVLNode* arr[], int* i) const;

    void mergeNodeArrays(AVLNode* arr1[], AVLNode* arr2[], AVLNode* merged[], int len1,
                         int len2) const;

    AVLNode* sortedInsert(AVLNode* sorted[], int start, int end, AVLNode* parent) {
        if (start>end) return NULL;

        int m = (start+end)/2;
        AVLNode* node = new AVLNode(sorted[m]->key, sorted[m]->data, parent);

        node->leftSon = sortedInsert(sorted,start,m-1, node);
        node->rightSon = sortedInsert(sorted,m+1,end, node);
        node->updateHeight();
        return node;
    }
    AVLTree* merge(AVLTree& tree) {
        if (tree.size == 0) return this;
        if (size == 0) return &tree;

        AVLNode* nodes1[size];
        int i=0;
        inorderNodeArray(root, nodes1, &i);

        AVLNode* nodes2[tree.size];
        i=0;
        tree.inorderNodeArray(tree.root, nodes2, &i);

        AVLNode* merged[size+tree.size];
        mergeNodeArrays(nodes1, nodes2, merged, size, tree.size);

        AVLTree* newTree = new AVLTree();
        newTree->root = sortedInsert(merged, 0, size+tree.size-1, NULL);
        newTree->size = size+tree.size;
        return newTree;
    }
};
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
void AVLTree<K, D>::insert(K key, D data) {
    if (root == NULL) {
        root = new AVLNode(key, data);
        size++;
    } else {
        insert(key, data, root);
    }
}
template<class K, class D>
void AVLTree<K, D>::insert(K key, D data, AVLNode* parent) {
    if (key == parent->key) {
        parent->data = data;
        return;
    }

    bool turnLeft = parent->key > key;
    if (turnLeft) {
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
    rebalance(parent, false);
}

// Removing nodes
template<class K, class D>
bool AVLTree<K, D>::remove(K key) {
    AVLNode* current = root;
    while (current) {
        if (current->key == key) break;
        if (current->key > key) {
            current = current->leftSon;
        } else {
            current = current->rightSon;
        }
    }
    if (!current) return false;
    remove(current);
    size--;
    return true;
}
template<class K, class D>
void AVLTree<K, D>::remove(AVLNode* toDelete) {
    AVLNode* successor = toDelete->rightSon;
    if (haveTwoSons(toDelete)) { // node has Two sons
        while (successor->leftSon) {
            successor = successor->leftSon;
        }
    } else if (toDelete->leftSon) { // node only have left son
        successor = toDelete->leftSon;
    }
    if (successor) {
        toDelete->key = successor->key;
        toDelete->data = successor->data;
        remove(successor);
    } else { // node is a leaf or remained a leaf
        AVLNode* parent = toDelete->parent;
        if (parent == NULL) {
            root = NULL;
        }
        else if (parent->leftSon == toDelete) parent->leftSon = NULL;
        else parent->rightSon = NULL;
        delete toDelete;
        rebalance(parent, true);
    }

}
template<class K, class D>
bool AVLTree<K, D>::isExist(K key) const {
    AVLNode* current = root;
    while (current) {
        if (current->key == key) return true;
        if (current->key > key) {
            current = current->leftSon;
        } else {
            current = current->rightSon;
        }
    }
    return false;
}
template<class K, class D>
D AVLTree<K, D>::getData(K key) const {
    AVLNode* current = root;
    while (current) {
        if (current->key == key) return current->data;
        if (current->key > key) {
            current = current->leftSon;
        } else {
            current = current->rightSon;
        }
    }
    return NULL;
}
template<class K, class D>
void AVLTree<K, D>::changeData(K key, D new_data){
    AVLNode* current = root;
    while (current) {
        if (current->key == key) {
            current->data = new_data;
            return;
        }
        else if (current->key > key) {
            current = current->leftSon;
        } else {
            current = current->rightSon;
        }
    }
}

// AVL feautres - rebalance + rotations
template<class K, class D>
void AVLTree<K, D>::rebalance(AVLNode* node, bool remove) {
    if (!node) return;
    node->updateHeight();
    int BF = node->BalanceFactorCalc();
    if (BF == 2) {
        if (node->leftSon->BalanceFactorCalc() >= 0) LL(node);
        else LR(node);
    } else if (BF == -2) {
        if (node->rightSon->BalanceFactorCalc() <= 0) RR(node);
        else RL(node);
    }
    if (remove) rebalance(node->parent, remove);
}
template<class K, class D>
void AVLTree<K, D>::rotateRight(AVLNode* pRoot, AVLNode* pLeft) {
    pRoot->leftSon = pLeft->rightSon;
    pLeft->rightSon = pRoot;

    if (pRoot->leftSon) pRoot->leftSon->parent = pRoot;
    pLeft->parent = pRoot->parent;
    pRoot->parent = pLeft;

    change(pLeft->parent, pRoot, pLeft);
    pRoot->updateHeight();
    pLeft->updateHeight();
}
template<class K, class D>
void AVLTree<K, D>::rotateLeft(AVLNode* pRoot, AVLNode* pRight) {
    pRoot->rightSon = pRight->leftSon;
    pRight->leftSon = pRoot;

    if (pRoot->rightSon) pRoot->rightSon->parent = pRoot;
    pRight->parent = pRoot->parent;
    pRoot->parent = pRight;

    change(pRight->parent, pRoot, pRight);
    pRoot->updateHeight();
    pRight->updateHeight();
}

template<class K, class D>
void AVLTree<K, D>::change(AVLNode* parent, AVLNode* oldNode, AVLNode* newNode) {
    if (parent == NULL) root = newNode;
    else if (parent->leftSon == oldNode) parent->leftSon = newNode;
    else parent->rightSon = newNode;
}

// Printing functions
template<class K, class D>
void AVLTree<K, D>::printGraph(bool BF) const {
    printGraph(root, "", BF);
}
template<class K, class D>
void AVLTree<K, D>::printGraph(AVLNode* node, const std::string& prefix, bool BF) const {
    if (node) {
        printGraph(node->rightSon, prefix + "     ", BF);
        cout << prefix;
        cout << "|--[";
        cout << node->key;
        if (BF) cout << ", BF: " << node->BalanceFactorCalc();
        cout << "]" << endl;
        printGraph(node->leftSon, prefix + "     ", BF);
    }
}
template<class K, class D>
void AVLTree<K, D>::inorderPrint(AVLNode* n) const {
    if (n) {
        inorderPrint(n->leftSon);
        cout << n->key << " ";
        inorderPrint(n->rightSon);
    }
}
template<class K, class D>
void AVLTree<K, D>::printLevel(AVLNode* node, int level) const {
    if (node) {
        if (level == 0) {
            cout << node->key << " ";
        } else {
            printLevel(node->leftSon, level - 1);
            printLevel(node->rightSon, level - 1);
        }
    }
}

// Array and merging functions
template<class K, class D>
K* AVLTree<K, D>::keyArray() const {
    K* keys;
    keys = (K*)malloc(size*sizeof(*keys));
    if (!keys)
        return NULL;
    AVLNode* nodes[size];
    int i=0;
    inorderNodeArray(root, nodes, &i);
    for (int j = 0; j < size; ++j) {
        keys[j] = nodes[j]->key;
    }
    return keys;
}
template<class K, class D>
void AVLTree<K, D>::inorderNodeArray(AVLNode* node, AVLNode* arr[], int* i) const {
    if (node) {
        inorderNodeArray(node->leftSon, arr, i);
        arr[(*i)] = node;
        (*i)++;
        inorderNodeArray(node->rightSon, arr, i);
    }
}
template<class K, class D>
void AVLTree<K, D>::mergeNodeArrays(AVLNode* arr1[], AVLNode* arr2[], AVLNode* merged[],
                                 int len1, int len2) const {
    int i = 0, j = 0, c = 0;
    while (i < len1 && j < len2) {
        if (arr1[i]->key > arr2[j]->key) {
            merged[c] = arr2[j++];
        } else {
            merged[c] = arr1[i++];
        }
        c++;
    }
    while (i < len1) {
        merged[c] = arr1[i];
        i++;
        c++;
    }
    while (j < len2) {
        merged[c] = arr2[j];
        j++;
        c++;
    }
}
#endif //AVLTREE_AVLTREE_H
