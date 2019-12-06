#include<stdlib.h>
#include"DataCenter.h"
#include"List.h"

DataCenter::DataCenter(int servers, int dataCenterID) {
    this->dataCenterID = dataCenterID;
    this->linuxServerCounter = servers;
    this->windowsServerCounter = 0;
    this->arrayServersPointers = new _node<Server> *[servers];
    this->linuxUsed = nullptr;
    this->windowsFree = nullptr;
    this->windowsUsed = nullptr;
    for (int i = 0; i < servers; i++) {
        Server *temp = new Server();
        if (i == 0) {
            this->linuxFree = (createNode(*temp));
            this->arrayServersPointers[i] = this->linuxFree;
            this->arrayServersPointers[i]->value.OS = this->linuxFree;
        } else {
            this->arrayServersPointers[i] = createNode(*temp);
            addNode(&(this->linuxFree), this->arrayServersPointers[i]);
            this->arrayServersPointers[i]->value.OS = this->linuxFree;
        }
        temp->serverID = i;
    }
}

void removeList(_node<Server> *toRemove) {
    while (toRemove != nullptr) {
        _node<Server> *next = getNext(toRemove);
        removeNode(&toRemove, toRemove);
        toRemove = next;
    }
}

DataCenter::~DataCenter() {
    removeList(this->windowsFree);
    removeList(this->windowsUsed);
    removeList(this->linuxFree);
    removeList(this->linuxUsed);
    delete (this->arrayServersPointers);
}

StatusType DataCenter::requestWindows(int serverID, int *assignedID,AVLTree<int, AVLTree<int,int>*> treeLinux,AVLTree<int, AVLTree<int,int>*> treeWindows) {
    _node<Server> *originalPtr = arrayServersPointers[serverID];


    if (originalPtr->value.OS == windowsFree || originalPtr->value.OS == linuxFree) {
        arrayServersPointers[serverID] = createNode(getValue(arrayServersPointers[serverID]));
        addNode(&(windowsUsed), arrayServersPointers[serverID]);

        if (originalPtr->value.OS == windowsFree) { //the request server is free
            removeNode(&(windowsFree), originalPtr);
            arrayServersPointers[serverID]->value.OS = windowsUsed;//downloading linux on the server & set it as used
        }
        if (originalPtr->value.OS == linuxFree) { //the request server is free with different os
            removeNode(&(linuxFree), originalPtr);
            (*(arrayServersPointers[serverID])).value.OS = windowsUsed;//downloading linux on the server & set it as used
            removeDSFromCountTree(treeLinux, this->linuxServerCounter, this->dataCenterID);
            removeDSFromCountTree(treeWindows, this->windowsServerCounter, this->dataCenterID);
            linuxServerCounter--;
            windowsServerCounter++;
            addDSToCountTree(treeLinux, this->linuxServerCounter, this->dataCenterID);
            addDSToCountTree(treeWindows, this->windowsServerCounter, this->dataCenterID);
        }
        *assignedID = serverID;
        return SUCCESS;
    }

    if (originalPtr->value.OS == windowsUsed) { //the requested server is used
        if (windowsFree) {
            usingFreeServer(assignedID, windowsFree, windowsUsed);
            return SUCCESS;
        } else { //there is no available server with the requested OS
            if (linuxFree) {
                usingFreeServer(assignedID, linuxFree, windowsUsed);
                removeDSFromCountTree(treeLinux, this->linuxServerCounter, this->dataCenterID);
                removeDSFromCountTree(treeWindows, this->windowsServerCounter, this->dataCenterID);
                linuxServerCounter--;
                windowsServerCounter++;
                addDSToCountTree(treeLinux, this->linuxServerCounter, this->dataCenterID);
                addDSToCountTree(treeWindows, this->windowsServerCounter, this->dataCenterID);
                return SUCCESS;
            }
        }
    }
    return FAILURE; //no available servers
}

void DataCenter::usingFreeServer(int *assignedID, _node<Server> *freeServers, _node<Server> *usedServer) {
    Server serverToUse = getValue(freeServers);
    _node<Server> *oldServerPtr = arrayServersPointers[(serverToUse).serverID];
    arrayServersPointers[serverToUse.serverID] = createNode(serverToUse);
    addNode(&(usedServer), arrayServersPointers[(serverToUse).serverID]);
    serverToUse.OS = usedServer;
    removeNode(&(freeServers), oldServerPtr);
    *assignedID = serverToUse.serverID;
}

void DataCenter::removeDSFromCountTree(AVLTree<int, AVLTree<int,int>*> tree, int oldServersAmount, int dataCenterID) {
    AVLTree<int,int>* iDsTree = tree.findAVLNode(oldServersAmount)->getData();
    iDsTree->deleteKey(dataCenterID);
    if (iDsTree->getSize() == 0) {
        tree.deleteKey(oldServersAmount);
    }
}

void DataCenter::addDSToCountTree(AVLTree<int, AVLTree<int,int>*> tree, int newServersAmount, int dataCenterID) {
    if (tree.isExist(newServersAmount)) {
        AVLTree<int,int>* iDsTree = tree.findAVLNode(newServersAmount)->getData();
        iDsTree->insert(dataCenterID,0);
    }
    else {
        AVLTree<int, int> iDsTree = AVLTree<int,int>();
        iDsTree.insert(dataCenterID,0);
        tree.insert(newServersAmount,&iDsTree);
    }
}

StatusType DataCenter::requestLinux(int serverID, int *assignedID,AVLTree<int, AVLTree<int,int>*> treeLinux,AVLTree<int, AVLTree<int,int>*> treeWindows) {
    _node<Server> *originalPtr = arrayServersPointers[serverID];
    if (originalPtr->value.OS == linuxFree || originalPtr->value.OS == windowsFree) {
        arrayServersPointers[serverID] = createNode(getValue(arrayServersPointers[serverID]));
        addNode(&(linuxUsed), arrayServersPointers[serverID]);
        if (originalPtr->value.OS == linuxFree) { //the request server is free
            removeNode(&(linuxFree), originalPtr);
        }
        if (originalPtr->value.OS == windowsFree) { //the request server is free with different os
            removeNode(&(windowsFree), originalPtr);
            removeDSFromCountTree(treeLinux, this->linuxServerCounter, this->dataCenterID);
            removeDSFromCountTree(treeWindows, this->windowsServerCounter, this->dataCenterID);
            linuxServerCounter++;
            windowsServerCounter--;
            addDSToCountTree(treeLinux, this->linuxServerCounter, this->dataCenterID);
            addDSToCountTree(treeWindows, this->windowsServerCounter, this->dataCenterID);
        }
        arrayServersPointers[serverID]->value.OS = linuxUsed;//downloading linux on the server & set it as used
        *assignedID = serverID;
        return SUCCESS;
    }

    if (originalPtr->value.OS == linuxUsed) { //the requested server is used
        if (linuxFree) {
            usingFreeServer(assignedID, linuxFree, linuxUsed);
            return SUCCESS;
        } else { //there is no available server with the requested OS
            if (windowsFree) {
                usingFreeServer(assignedID, windowsFree, linuxUsed);
                removeDSFromCountTree(treeLinux, this->linuxServerCounter, this->dataCenterID);
                removeDSFromCountTree(treeWindows, this->windowsServerCounter, this->dataCenterID);
                linuxServerCounter++;
                windowsServerCounter--;
                addDSToCountTree(treeLinux, this->linuxServerCounter, this->dataCenterID);
                addDSToCountTree(treeWindows, this->windowsServerCounter, this->dataCenterID);
                return SUCCESS;
            }
        }
    }
    return FAILURE; //no available servers
}

StatusType DataCenter::dataCenterRequestServer(int serverID, int os, int *assignedID,AVLTree<int, AVLTree<int,int>*> treeLinux,AVLTree<int, AVLTree<int,int>*> treeWindows) {
    if (serverID >= windowsServerCounter + linuxServerCounter) return INVALID_INPUT;
    return (os == 0) ? requestLinux(serverID, assignedID,treeLinux,treeWindows) : requestWindows(serverID, assignedID,treeLinux,treeWindows);
}

StatusType DataCenter::dataCenterFreeServer(int serverID) {
    if (serverID >= windowsServerCounter + linuxServerCounter) return INVALID_INPUT;
    if ((arrayServersPointers[serverID]->value.OS == linuxFree) ||
        (arrayServersPointers[serverID]->value.OS == windowsFree))
        return FAILURE;
    _node<Server> *originalPtr = arrayServersPointers[serverID];
    if (arrayServersPointers[serverID]->value.OS == linuxUsed) {
        arrayServersPointers[serverID] = createNode(getValue(arrayServersPointers[serverID]));
        addNode(&(linuxFree), arrayServersPointers[serverID]);
        removeNode(&(linuxUsed), originalPtr);
        arrayServersPointers[serverID]->value.OS = linuxFree;
        return SUCCESS;
    } else if (arrayServersPointers[serverID]->value.OS == windowsUsed) {
        arrayServersPointers[serverID] = createNode(getValue(arrayServersPointers[serverID]));
        addNode(&(windowsFree), arrayServersPointers[serverID]);
        removeNode(&(windowsUsed), originalPtr);
        arrayServersPointers[serverID]->value.OS = linuxFree;
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





