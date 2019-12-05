#include<stdlib.h>
#include"DataCenter.h"
#include"List.h"

DataCenter::DataCenter(int servers, int dataCenterID) {
    this->dataCenterID = dataCenterID;
    this->linuxServerCounter = servers;
    this->windowsServerCounter = 0;
    this->arrayServersPointers = new _node<Server> *[servers];
    for (int i = 0; i < servers; i++) {
        Server *temp = new Server();
        temp->OS = this->linuxFree;
        temp->serverID = i;
        this->arrayServersPointers[i] = createNode(*temp);
        addNode(&(this->linuxFree), this->arrayServersPointers[i]);
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

StatusType DataCenter::requestWindows(int serverID, int *assignedID) {
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
            linuxServerCounter--;
            windowsServerCounter++;
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
                linuxServerCounter--;
                windowsServerCounter++;
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

StatusType DataCenter::requestLinux(int serverID, int *assignedID) {
    _node<Server> *originalPtr = arrayServersPointers[serverID];
    if (originalPtr->value.OS == linuxFree || originalPtr->value.OS == windowsFree) {
        arrayServersPointers[serverID] = createNode(getValue(arrayServersPointers[serverID]));
        addNode(&(linuxUsed), arrayServersPointers[serverID]);
        if (originalPtr->value.OS == linuxFree) { //the request server is free
            removeNode(&(linuxFree), originalPtr);
        }
        if (originalPtr->value.OS == windowsFree) { //the request server is free with different os
            removeNode(&(windowsFree), originalPtr);
            linuxServerCounter++;
            windowsServerCounter--;
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
                linuxServerCounter++;
                windowsServerCounter--;
                return SUCCESS;
            }
        }
    }
    return FAILURE; //no available servers
}

StatusType DataCenter::dataCenterRequestServer(int serverID, int os, int *assignedID) {
    if (serverID >= windowsServerCounter + linuxServerCounter) return INVALID_INPUT;
    return (os == 0) ? requestLinux(serverID, assignedID) : requestWindows(serverID, assignedID);
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





