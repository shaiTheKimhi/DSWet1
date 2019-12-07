#pragma once

#include "List.h"
#include "AVLTree.h"
#include "library1.h"


//typedef enum {
//    ALLOCATION_ERROR,
//    INVALID_INPUT,
//    FAILURE,
//    SUCCESS
//} StatusType;

class Server {
public:
    List<Server> *OS;
    int serverID;

    Server(List<Server> *OS, int serverID): OS(OS),serverID(serverID){};
};


class DataCenter {
public:
    int dataCenterID;
    int windowsServerCounter;
    int linuxServerCounter;
    Node<Server> **arrayServersPointers;
    List<Server>* windowsFree;
    List<Server>* windowsUsed;
    List<Server>* linuxFree;
    List<Server>* linuxUsed;

    DataCenter(int servers, int dataCenterID, AVLTree<int, AVLTree<int,int>*>* treeLinux, AVLTree<int, AVLTree<int,int>*>* treeWindows);
    ~DataCenter();

    StatusType dataCenterRequestServer(int serverID, int os, int *assigned,AVLTree<int, AVLTree<int,int>*>* treeLinux,AVLTree<int, AVLTree<int,int>*>* treeWindows);

    StatusType dataCenterFreeServer(int serverID);

    StatusType requestWindows(int server, int *assignedID,AVLTree<int, AVLTree<int,int>*>* treeLinux,AVLTree<int, AVLTree<int,int>*>* treeWindows);

    StatusType requestLinux(int server, int *assignedID,AVLTree<int, AVLTree<int,int>*>* treeLinux,AVLTree<int, AVLTree<int,int>*>* treeWindows);

    int getLinuxCounter();

    int getWindowsCounter();

    void usingFreeServer(int *assignedID, List<Server> *freeServers, Node<Server> *usedServer);

    void addDSToCountTree(AVLTree<int, AVLTree<int, int> *>* tree, int counter, int id);

    void removeDSFromCountTree(AVLTree<int, AVLTree<int, int> *>* tree, int counter, int id);

    void updateTreesCount(AVLTree<int, AVLTree<int, int> *>* treeAdded, AVLTree<int, AVLTree<int, int> *>* treeRemoved,
                          int *counterAdded, int *counterRemoved);

    void assignServerFromFree(int *assignedID, List<Server> *freeServer, List<Server> *usedServers);
};



