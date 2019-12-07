#include<stdlib.h>
#include"DataCenter.h"
#include"List.h"

DataCenter::DataCenter(int servers, int dataCenterID, AVLTree<int, AVLTree<int,int>*>* treeLinux, AVLTree<int, AVLTree<int,int>*>* treeWindows) {
    this->dataCenterID = dataCenterID;
    this->linuxServerCounter = servers;
    this->windowsServerCounter = 0;
    this->arrayServersPointers = new Node<Server> *[servers];
    this->linuxUsed = new List<Server>();
    this->windowsFree = new List<Server>();
    this->windowsUsed = new List<Server>();
    this->linuxFree = new List<Server>();
    for (int i = 0 ; i < servers ; i++) {
        Server *temp = new Server(linuxFree,i);
        this->arrayServersPointers[i] = linuxFree->appendNode(*temp);
    }
    addDSToCountTree(treeLinux,this->linuxServerCounter, dataCenterID);
    addDSToCountTree(treeWindows, this->windowsServerCounter, dataCenterID);
}

DataCenter::~DataCenter() {
    delete(this->windowsFree);
    delete(this->windowsUsed);
    delete(this->linuxFree);
    delete(this->linuxUsed);
    delete[](this->arrayServersPointers);

}

StatusType DataCenter::requestWindows(int serverID, int *assignedID,AVLTree<int, AVLTree<int,int>*>* treeLinux,AVLTree<int, AVLTree<int,int>*>* treeWindows) {
    Node<Server>* originalPtr = this->arrayServersPointers[serverID];
    if (originalPtr == nullptr) {
        return FAILURE;
    }
    if(originalPtr->value.OS == linuxFree ||originalPtr->value.OS == windowsFree) {
        Server *temp = new Server(windowsUsed,serverID);
        arrayServersPointers[serverID] = windowsUsed->addNode(*temp);
        if (originalPtr->value.OS == linuxFree) {
            updateTreesCount(treeWindows, treeLinux, &windowsServerCounter, &linuxServerCounter);
            linuxFree->removeNode(originalPtr);
        } else {
            windowsFree->removeNode(originalPtr);
        }
        *assignedID = serverID;
        return SUCCESS;
    }
    if (!windowsFree->isEmpty()) {
        assignServerFromFree(assignedID, windowsFree, windowsUsed);
        return SUCCESS;
    }
    if (!linuxFree->isEmpty()) {
        assignServerFromFree(assignedID, linuxFree, windowsUsed);
        updateTreesCount(treeWindows, treeLinux, &windowsServerCounter, &linuxServerCounter);
        return SUCCESS;
    }
    return FAILURE;
}

void DataCenter::removeDSFromCountTree(AVLTree<int, AVLTree<int,int>*>* tree, int oldServersAmount, int dataCenterID) {
    if (tree->findAVLNode(oldServersAmount)){
        AVLTree<int,int>* iDsTree = tree->findAVLNode(oldServersAmount)->getData();
        iDsTree->deleteKey(dataCenterID);
        if (iDsTree->getSize() == 0) {
            tree->deleteKey(oldServersAmount);
        }
    }

}

void DataCenter::addDSToCountTree(AVLTree<int, AVLTree<int,int>*>* tree, int newServersAmount, int dataCenterID) {
    if (tree->isExist(newServersAmount)) {
        AVLTree<int,int>* iDsTree = tree->findAVLNode(newServersAmount)->getData();
        iDsTree->insert(dataCenterID,0);
    }
    else {
        AVLTree<int, int>* iDsTree = new AVLTree<int,int>();
        iDsTree->insert(dataCenterID,0);
        tree->insert(newServersAmount,iDsTree);
    }
}

StatusType DataCenter::requestLinux(int serverID, int *assignedID,AVLTree<int, AVLTree<int,int>*>* treeLinux,AVLTree<int, AVLTree<int,int>*>* treeWindows) {
    Node<Server>* originalPtr = this->arrayServersPointers[serverID];
    if (originalPtr == nullptr) {
        return FAILURE;
    }
    if(originalPtr->value.OS == linuxFree ||originalPtr->value.OS == windowsFree) {
        Server *temp = new Server(linuxUsed,serverID);
        arrayServersPointers[serverID] = linuxUsed->addNode(*temp);
        if (originalPtr->value.OS == windowsFree) {
            updateTreesCount(treeLinux, treeWindows, &linuxServerCounter, &windowsServerCounter);
            windowsFree->removeNode(originalPtr);
        } else {
            linuxFree->removeNode(originalPtr);
        }
        *assignedID = serverID;
        return SUCCESS;
    }
    if (!linuxFree->isEmpty()) {
        assignServerFromFree(assignedID, linuxFree, linuxUsed);
        return SUCCESS;
    }
    if (!windowsFree->isEmpty()) {
        assignServerFromFree(assignedID, windowsFree, linuxUsed);
        updateTreesCount(treeLinux, treeWindows, &linuxServerCounter, &windowsServerCounter);
        return SUCCESS;
    }
    return FAILURE;
}

void DataCenter::assignServerFromFree(int *assignedID, List<Server>* freeServer, List<Server>* usedServers) {
    int currID = freeServer->head->value.serverID;
    Server *temp = new Server(usedServers, currID);
    arrayServersPointers[currID] = usedServers->addNode(*temp);
    freeServer->removeNode(freeServer->head);
    *assignedID = currID;
}

void DataCenter::updateTreesCount(AVLTree<int, AVLTree<int,int>*>* treeAdded , AVLTree<int, AVLTree<int,int>*>* treeRemoved, int* counterAdded, int* counterRemoved) {
    removeDSFromCountTree(treeAdded, *counterAdded, dataCenterID);
    removeDSFromCountTree(treeRemoved, *counterRemoved, dataCenterID);
    (*counterAdded)++;
    (*counterRemoved)--;
    addDSToCountTree(treeAdded, *counterAdded, dataCenterID);
    addDSToCountTree(treeRemoved, *counterRemoved, dataCenterID);
}

StatusType DataCenter::dataCenterRequestServer(int serverID, int os, int *assignedID,AVLTree<int, AVLTree<int,int>*>* treeLinux,AVLTree<int, AVLTree<int,int>*>* treeWindows) {
    if (serverID >= windowsServerCounter + linuxServerCounter) {
        return INVALID_INPUT;
    }
    return (os == 0) ? requestLinux(serverID, assignedID,treeLinux,treeWindows) : requestWindows(serverID, assignedID,treeLinux,treeWindows);
}

StatusType DataCenter::dataCenterFreeServer(int serverID) {
    if (serverID >= windowsServerCounter + linuxServerCounter) return INVALID_INPUT;
    Node<Server>* originalPtr = arrayServersPointers[serverID];
    if (originalPtr == nullptr || originalPtr->value.OS == linuxFree ||  originalPtr->value.OS == windowsFree ) {
        return FAILURE;
    }
    if (originalPtr->value.OS == linuxUsed) {
        Server *temp = new Server(linuxFree, serverID);
        linuxUsed->removeNode(originalPtr);
        arrayServersPointers[serverID] = linuxFree->appendNode(*temp);
        return SUCCESS;
    }
    if (originalPtr->value.OS == windowsUsed) {
        Server *temp = new Server(windowsFree, serverID);
        windowsUsed->removeNode(originalPtr);
        arrayServersPointers[serverID] = windowsFree->appendNode(*temp);
        return SUCCESS;
    }
    return FAILURE;
}

int DataCenter::getLinuxCounter() {
    return linuxServerCounter;
}

int DataCenter::getWindowsCounter() {
    return windowsServerCounter;
}





