#pragma once
#include"DataCenter.h"
#include "AVLtree.h"
typedef struct _dataCenterSystem
{
    AVLTree<int, DataCenter*> _dataCentersById;
    AVLTree<int, AVLTree<int,int>*> _dataCentersByLinuxCount;//seconds tree houses id's of data centers
    AVLTree<int, AVLTree<int,int>*> _dataCentersByWindowsCount;
}* DataCenterSystem;


void * Init();//initializes the dataCenterSystem 

StatusType AddDataCenter(void *DS, int dataCenterID, int numOfServers);

StatusType RemoveDataCenter(void *DS, int dataCenterID);

StatusType RequestServer(void *DS, int dataCenterID, int serverID, int os, int *assignedID);

StatusType FreeServer(void *DS, int dataCenterID, int serverID);

StatusType GetDataCentersByOS(void *DS, int os, int **dataCenters, int* numOfDataCenters);

void Quit(void **DS);


