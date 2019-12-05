#pragma once

#include "List.h"


typedef enum {
    ALLOCATION_ERROR,
    INVALID_INPUT,
    FAILURE,
    SUCCESS
} StatusType;

class Server {
public:
    _node<Server> *OS;//changed from <_server>
    int serverID;
};


class DataCenter {
    StatusType requestWindows(int server, int *assignedID);

    StatusType requestLinux(int server, int *assignedID);

public:
    int dataCenterID;
    int windowsServerCounter;
    int linuxServerCounter;
    _node<Server> **arrayServersPointers;//changed from Server**(_server***)
    _node<Server> *windowsFree;
    _node<Server> *windowsUsed;
    _node<Server> *linuxFree;
    _node<Server> *linuxUsed;

    DataCenter(int servers, int dataCenterID);//servers:amount of servers
    ~DataCenter();

    StatusType dataCenterRequestServer(int serverID, int os, int *assigned);

    StatusType dataCenterFreeServer(int serverID);

    int getLinuxCounter();

    int getWindowsCounter();

    void usingFreeServer(int *assignedID, _node<Server> *freeServers, _node<Server> *usedServer);
};



