#pragma once

#include <exception>
#include"DataCenter.h"
using std::exception;

class DataCenterSystem{
public:
    AVLTree<int, DataCenter*>* _dataCentersById;
    AVLTree<int, AVLTree<int,int>*>* _dataCentersByLinuxCount;//seconds tree houses id's of data centers
    AVLTree<int, AVLTree<int,int>*>* _dataCentersByWindowsCount;

    ~DataCenterSystem();
    void Init();//initializes the dataCenterSystem

    StatusType AddDataCenter(int dataCenterID, int numOfServers);

    StatusType RemoveDataCenter( int dataCenterID);

    StatusType RequestServer(int dataCenterID, int serverID, int os, int *assignedID);

    StatusType FreeServer(int dataCenterID, int serverID);

    StatusType GetDataCentersByOS( int os, int **dataCenters, int* numOfDataCenters);

    void Quit(void **DS);
};

