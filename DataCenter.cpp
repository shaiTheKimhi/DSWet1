#include<stdlib.h>
#include"DataCenter.h"
#include"List.h"

DataCenter DataCenter::dataCenterInit(int servers, int dataCenterID) {
    DataCenter* dc = new DataCenter();
    dc->dataCenterID = dataCenterID;
    dc->linuxServerCounter = servers;
    dc->windowsServerCounter = 0;
    for (int i = 0; i < servers; i++)
    {
        Server* temp = new Server();
        temp->OS = dc->linuxFree;
        temp->serverID = i;
        dc->arrayServersPointers[i] = createNode(*temp);
        addNode(&(dc->linuxFree), dc->arrayServersPointers[i]);
    }
    return *dc;
}

void removeList( _node<Server>* toRemove) {
    while(toRemove != nullptr) {
        _node<Server>* next = getNext(toRemove);
        free(getValue(toRemove));
        removeNode(&toRemove, toRemove);
        toRemove = next;
    }
}

void DataCenter::dataCenterRemove(DataCenter toRemove) {
    removeList(toRemove.windowsFree);
    removeList(toRemove.windowsUsed);
    removeList(toRemove.linuxFree);
    removeList(toRemove.linuxUsed);
    delete(toRemove.arrayServersPointers);
    delete(&toRemove);
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

StatusType DataCenter::dataCenterRequestServer(DataCenter toRequest, int serverID, int os, int *assignedID) {
    if (serverID >= toRequest.windowsServerCounter + toRequest.linuxServerCounter) return INVALID_INPUT;
    return (os == 0) ?  requestLinux(toRequest,serverID,assignedID) : requestWindows(toRequest,serverID,assignedID);
}

StatusType DataCenter::dataCenterFreeServer(DataCenter toFree, int serverID) {
    if (serverID >= toFree.windowsServerCounter + toFree.linuxServerCounter) return INVALID_INPUT;
    if ((toFree.arrayServersPointers[serverID]->value.OS == toFree.linuxFree) ||
        (toFree.arrayServersPointers[serverID]->value.OS == toFree.windowsFree)) return FAILURE;
    _node<Server> *originalPtr = toFree.arrayServersPointers[serverID];
    if (toFree.arrayServersPointers[serverID]->value.OS == toFree.linuxUsed) {
        toFree.arrayServersPointers[serverID] = createNode(getValue(toFree.arrayServersPointers[serverID]));
        addNode(&(toFree.linuxFree), toFree.arrayServersPointers[serverID]);
        removeNode(&(toFree.linuxUsed), originalPtr);
        toFree.arrayServersPointers[serverID]->value.OS = toFree.linuxFree;
        return  SUCCESS;
    }
    else if (toFree.arrayServersPointers[serverID]->value.OS == toFree.windowsUsed) {
        toFree.arrayServersPointers[serverID] = createNode(getValue(toFree.arrayServersPointers[serverID]));
        addNode(&(toFree.windowsFree), toFree.arrayServersPointers[serverID]);
        removeNode(&(toFree.windowsUsed), originalPtr);
        toFree.arrayServersPointers[serverID]->value.OS = toFree.linuxFree;
        return  SUCCESS;
    }
    return FAILURE;
}

int DataCenter::getLinuxCounter(DataCenter dataCenter) {
    return dataCenter.linuxServerCounter;
}

int DataCenter::getWindowsCounter(DataCenter dataCenter) {
    return dataCenter.windowsServerCounter;
}





