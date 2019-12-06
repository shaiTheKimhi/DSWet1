#pragma once

#include "List.h"
#include "AVLTree.h"


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

    StatusType dataCenterRequestServer(int serverID, int os, int *assigned,AVLTree<int, AVLTree<int,int>*> treeLinux,AVLTree<int, AVLTree<int,int>*> treeWindows);

    StatusType dataCenterFreeServer(int serverID);

    StatusType requestWindows(int server, int *assignedID,AVLTree<int, AVLTree<int,int>*> treeLinux,AVLTree<int, AVLTree<int,int>*> treeWindows);

    StatusType requestLinux(int server, int *assignedID,AVLTree<int, AVLTree<int,int>*> treeLinux,AVLTree<int, AVLTree<int,int>*> treeWindows);

    int getLinuxCounter();

    int getWindowsCounter();

    void usingFreeServer(int *assignedID, _node<Server> *freeServers, _node<Server> *usedServer);

    void addDSToCountTree(AVLTree<int, AVLTree<int, int> *> tree, int counter, int id);

    void removeDSFromCountTree(AVLTree<int, AVLTree<int, int> *> tree, int counter, int id);
};



