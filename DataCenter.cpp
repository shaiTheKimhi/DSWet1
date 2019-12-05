#include<stdlib.h>
#include"DataCenter.h"
#include"List.h"

DataCenter::DataCenter(int servers, int dataCenterID) {
    this->dataCenterID = dataCenterID;
    this->linuxServerCounter = servers;
    this->windowsServerCounter = 0;
    this->arrayServersPointers = new _node<Server>*[servers];
    for (int i = 0; i < servers; i++)
    {
        Server* temp = new Server();
        temp->OS = this->linuxFree;
        temp->serverID = i;
        this->arrayServersPointers[i] = createNode(*temp);
        addNode(&(this->linuxFree), this->arrayServersPointers[i]);
    }
}

void removeList( _node<Server>* toRemove) {
    while(toRemove != nullptr) {
        _node<Server>* next = getNext(toRemove);
        free(getValue(toRemove));
        removeNode(&toRemove, toRemove);
        toRemove = next;
    }
}

DataCenter::~DataCenter() {
    removeList(this->windowsFree);
    removeList(this->windowsUsed);
    removeList(this->linuxFree);
    removeList(this->linuxUsed);
    delete(this->arrayServersPointers);
}

StatusType requestWindows(DataCenter toRequest, int serverID, int *assignedID) {
    _node<Server> *originalPtr = toRequest.arrayServersPointers[serverID];
    if (originalPtr->value.OS == toRequest.windowsFree) { //the request server is free
        toRequest.arrayServersPointers[serverID] = createNode(getValue(toRequest.arrayServersPointers[serverID]));
        addNode(&(toRequest.windowsUsed), toRequest.arrayServersPointers[serverID]);
        removeNode(&(toRequest.windowsFree), originalPtr);
        toRequest.arrayServersPointers[serverID]->value.OS = toRequest.windowsUsed;//downloading linux on the server & set it as used
        *assignedID = serverID;
        return SUCCESS;
    }
    if (originalPtr->value.OS == toRequest.linuxFree) { //the request server is free with different os
        toRequest.arrayServersPointers[serverID] = createNode(getValue(toRequest.arrayServersPointers[serverID]));
        addNode(&(toRequest.windowsUsed), toRequest.arrayServersPointers[serverID]);
        removeNode(&(toRequest.linuxFree), originalPtr);
        (*(toRequest.arrayServersPointers[serverID])).value.OS = toRequest.windowsUsed;//downloading linux on the server & set it as used
        *assignedID = serverID;
        toRequest.linuxServerCounter--;
        toRequest.windowsServerCounter++;
        return SUCCESS;
    }
    if (originalPtr->value.OS == toRequest.windowsUsed) { //the requested server is used
        if (toRequest.windowsFree) {
            Server serverToUse = getValue(toRequest.windowsFree);
            _node<Server> *oldServerPtr = toRequest. arrayServersPointers[(serverToUse).serverID];
            toRequest.arrayServersPointers[serverToUse.serverID] = createNode(serverToUse);
            addNode(&(toRequest.windowsUsed),toRequest.arrayServersPointers[(serverToUse).serverID]);
            serverToUse.OS = toRequest.windowsUsed;
            removeNode(&(toRequest.windowsFree), oldServerPtr);
            *assignedID = serverToUse.serverID;
            return SUCCESS;
        }
        else { //there is no available server with the requested OS
            if (toRequest.linuxFree) {
                Server serverToUse = getValue(toRequest.linuxFree);
                _node<Server> *oldServerPtr = toRequest.arrayServersPointers[serverToUse.serverID];
                toRequest.arrayServersPointers[serverToUse.serverID] = createNode(serverToUse);
                addNode(&(toRequest.windowsUsed),toRequest.arrayServersPointers[serverToUse.serverID]);
                serverToUse.OS = toRequest.windowsUsed;
                removeNode(&(toRequest.linuxFree), oldServerPtr);
                *assignedID = serverToUse.serverID;
                toRequest.linuxServerCounter--;
                toRequest.windowsServerCounter++;
                return SUCCESS;
            }
        }
    }
    return FAILURE; //no available servers
}

StatusType requestLinux(DataCenter toRequest, int serverID, int *assignedID) {
    _node<Server> *originalPtr = toRequest.arrayServersPointers[serverID];
    if (originalPtr->value.OS == toRequest.linuxFree) { //the request server is free
        toRequest.arrayServersPointers[serverID] = createNode(getValue(toRequest.arrayServersPointers[serverID]));
        addNode(&(toRequest.linuxUsed), toRequest.arrayServersPointers[serverID]);
        removeNode(&(toRequest.linuxFree), originalPtr);
        toRequest.arrayServersPointers[serverID]->value.OS = toRequest.linuxUsed;//downloading linux on the server & set it as used
        *assignedID = serverID;
        return SUCCESS;
    }
    if (originalPtr->value.OS == toRequest.windowsFree) { //the request server is free with different os
        toRequest.arrayServersPointers[serverID] = createNode(getValue(toRequest.arrayServersPointers[serverID]));
        addNode(&(toRequest.linuxUsed), toRequest.arrayServersPointers[serverID]);
        removeNode(&(toRequest.windowsFree), originalPtr);
        toRequest.arrayServersPointers[serverID]->value.OS = toRequest.linuxUsed;//downloading linux on the server & set it as used
        *assignedID = serverID;
        toRequest.linuxServerCounter++;
        toRequest.windowsServerCounter--;
        return SUCCESS;
    }
    if (originalPtr->value.OS == toRequest.linuxUsed) { //the requested server is used
        if (toRequest.linuxFree) {
            Server serverToUse = getValue(toRequest.linuxFree);
            _node<Server> *oldServerPtr = toRequest. arrayServersPointers[serverToUse.serverID];
            toRequest.arrayServersPointers[serverToUse.serverID] = createNode(serverToUse);
            addNode(&(toRequest.linuxUsed),toRequest.arrayServersPointers[serverToUse.serverID]);
            serverToUse.OS = toRequest.linuxUsed;
            removeNode(&(toRequest.linuxFree), oldServerPtr);
            *assignedID = serverToUse.serverID;
            return SUCCESS;
        }
        else { //there is no available server with the requested OS
            if (toRequest.windowsFree) {
                Server serverToUse = getValue(toRequest.windowsFree);
                _node<Server> *oldServerPtr = toRequest.arrayServersPointers[serverToUse.serverID];
                toRequest.arrayServersPointers[serverToUse.serverID] = createNode(serverToUse);
                addNode(&(toRequest.linuxUsed),toRequest.arrayServersPointers[serverToUse.serverID]);
                serverToUse.OS = toRequest.linuxUsed;
                removeNode(&(toRequest.windowsFree), oldServerPtr);
                *assignedID = serverToUse.serverID;
                toRequest.linuxServerCounter++;
                toRequest.windowsServerCounter--;
                return SUCCESS;
            }
        }
    }
    return FAILURE; //no available servers
}

StatusType DataCenter::dataCenterRequestServer(int serverID, int os, int *assignedID) {
    if (serverID >= this->windowsServerCounter + this->linuxServerCounter) return INVALID_INPUT;
    return (os == 0) ?  requestLinux(*this, serverID,assignedID) : requestWindows(*this ,serverID,assignedID);
}

StatusType DataCenter::dataCenterFreeServer(int serverID) {
    if (serverID >= this->windowsServerCounter + this->linuxServerCounter) return INVALID_INPUT;
    if ((this->arrayServersPointers[serverID]->value.OS == this->linuxFree) ||
        (this->arrayServersPointers[serverID]->value.OS == this->windowsFree)) return FAILURE;
    _node<Server> *originalPtr = this->arrayServersPointers[serverID];
    if (this->arrayServersPointers[serverID]->value.OS == this->linuxUsed) {
        this->arrayServersPointers[serverID] = createNode(getValue(this->arrayServersPointers[serverID]));
        addNode(&(this->linuxFree), this->arrayServersPointers[serverID]);
        removeNode(&(this->linuxUsed), originalPtr);
        this->arrayServersPointers[serverID]->value.OS = this->linuxFree;
        return  SUCCESS;
    }
    else if (this->arrayServersPointers[serverID]->value.OS == this->windowsUsed) {
        this->arrayServersPointers[serverID] = createNode(getValue(this->arrayServersPointers[serverID]));
        addNode(&(this->windowsFree), this->arrayServersPointers[serverID]);
        removeNode(&(this->windowsUsed), originalPtr);
        this->arrayServersPointers[serverID]->value.OS = this->linuxFree;
        return  SUCCESS;
    }
    return FAILURE;
}

int DataCenter::getLinuxCounter()) {
    return this->linuxServerCounter;
}

int DataCenter::getWindowsCounter(DataCenter dataCenter) {
    return this->windowsServerCounter;
}





