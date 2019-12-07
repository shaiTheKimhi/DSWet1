

#include "DataCenterSystem.h"

DataCenterSystem::~DataCenterSystem() {
    delete (this->_dataCentersByLinuxCount);
    this->_dataCentersByLinuxCount = nullptr;
    delete(this->_dataCentersByWindowsCount);
    this->_dataCentersByWindowsCount = nullptr;
    delete (this->_dataCentersById);
    this->_dataCentersById = nullptr;
}
void *Init() {
    DataCenterSystem *ds = new DataCenterSystem();
    ds->_dataCentersById = new AVLTree<int, DataCenter*>();
    ds->_dataCentersByWindowsCount = new AVLTree<int, AVLTree<int,int>*>();
    ds->_dataCentersByLinuxCount = new AVLTree<int, AVLTree<int,int>*>();
    return (void *) ds;
}

void Quit(void** DS) {
    DataCenterSystem *ds = (DataCenterSystem *) *DS;
    if (ds != nullptr) {
        delete(ds);
    }
    *DS = nullptr;
}

StatusType AddDataCenter(void *DS, int dataCenterID, int numOfServers) {
    if (numOfServers <= 0 || DS == nullptr || dataCenterID <= 0) {
        return INVALID_INPUT;
    }

    DataCenterSystem *ds = (DataCenterSystem *) DS;
    if (ds->_dataCentersById->findAVLNode(dataCenterID) != nullptr) {
        return FAILURE;
    }

    DataCenter *dc = new DataCenter(numOfServers, dataCenterID,ds->_dataCentersByLinuxCount, ds->_dataCentersByWindowsCount);
    ds->_dataCentersById->insert(dataCenterID, dc);
    return SUCCESS;
}

StatusType RemoveDataCenter(void *DS, int dataCenterID) {
    if (DS == nullptr || dataCenterID <= 0) {
        return INVALID_INPUT;
    }

    DataCenterSystem *ds = (DataCenterSystem *) DS;
    if (!(ds->_dataCentersById->isExist(dataCenterID))) {
        return FAILURE;
    }

    DataCenter *dc = ds->_dataCentersById->findAVLNode(dataCenterID)->getData();
    ds->_dataCentersById->deleteKey(dataCenterID);
    if (ds->_dataCentersByLinuxCount->isExist(dc->linuxServerCounter)) {
        AVLTree<int,int>* linuxCounterTreeID =  ds->_dataCentersByLinuxCount->findAVLNode(dc->getLinuxCounter())->getData();
        linuxCounterTreeID->deleteKey(dataCenterID);
        if (linuxCounterTreeID->isEmpty()) {
            ds->_dataCentersByLinuxCount->deleteKey(dc->linuxServerCounter);
        }
    }
    if (ds->_dataCentersByWindowsCount->isExist(dc->windowsServerCounter)) {
        AVLTree<int,int>* windowsCounterTreeID =  ds->_dataCentersByWindowsCount->findAVLNode(dc->getWindowsCounter())->getData();
        windowsCounterTreeID->deleteKey(dataCenterID);
        if (windowsCounterTreeID->isEmpty()) {
            ds->_dataCentersByWindowsCount->deleteKey(dc->windowsServerCounter);
        }
    }
    delete dc;
    dc = nullptr;
    return SUCCESS;
}

StatusType RequestServer(void *DS, int dataCenterID, int serverID, int os, int *assignedID) {
    if (DS == nullptr || dataCenterID <= 0 || os < 0 || os > 1 || assignedID == nullptr || serverID < 0) {
        return INVALID_INPUT;
    }
    DataCenterSystem *ds = (DataCenterSystem *) DS;
    if ((!ds->_dataCentersById->isExist(dataCenterID))) {
        return FAILURE;
    }
    DataCenter *dc = ds->_dataCentersById->findAVLNode(dataCenterID)->getData();
    return dc->dataCenterRequestServer(serverID, os, assignedID, ds->_dataCentersByLinuxCount, ds->_dataCentersByWindowsCount);
}

StatusType FreeServer(void *DS, int dataCenterID, int serverID) {
    if (DS == nullptr || dataCenterID <= 0 || serverID < 0)
        return INVALID_INPUT;
    DataCenterSystem *ds = (DataCenterSystem*) DS;
    if (ds->_dataCentersById->findAVLNode(dataCenterID) == nullptr)
        return FAILURE;
    DataCenter *dc = ds->_dataCentersById->findAVLNode(dataCenterID)->getData();
    return dc->dataCenterFreeServer(serverID);
}

StatusType GetDataCentersByOS(void* DS, int os, int **dataCenters, int* numOfDataCenters) {
    if (DS == nullptr || dataCenters == nullptr || numOfDataCenters == nullptr || os > 1 || os <0 ) {
        return INVALID_INPUT;
    }
    DataCenterSystem *ds = (DataCenterSystem*) DS;
    if (ds->_dataCentersById->isEmpty()) {
        return FAILURE;
    }

    //writing for linux only- change later
    AVLTree<int,int>** tempArray = (os == 0) ? ds->_dataCentersByLinuxCount->inOrderDataArray(0) : ds->_dataCentersByWindowsCount->inOrderDataArray(0) ;
    if (tempArray == nullptr) {
        return ALLOCATION_ERROR;
    }
    int tempArraySize = (os == 0) ? ds->_dataCentersByLinuxCount->getSize() :ds->_dataCentersByWindowsCount->getSize() ;
    List<AVLTree<int,int>*>* tempList = new List<AVLTree<int,int>*>();
    for (int i = 0; i < tempArraySize; i++) {
        tempList->addNode(tempArray[i]);
    }
    List<int>* resultList = new List<int>();
    for (int i = 0; i < tempArraySize ; i++) {
        AVLTree<int,int>* currTree = tempList->head->value;
        int tempInnerTreeSize = currTree->getSize();
        int* tempIDs = currTree->inOrderKeyArray();
        if (tempIDs == nullptr) {
            return ALLOCATION_ERROR;
        }
        for (int j = 0 ; j < tempInnerTreeSize; j++){
            resultList->appendNode(tempIDs[j]);
        }
        free(tempIDs);
        tempIDs = nullptr;
        tempList->removeNode(tempList->head);
    }
    free(tempArray);
    tempArray = nullptr;
    int size = resultList->getListSize();
    int* dataCentersTemp = (int*)malloc(size* sizeof(int));
    if (dataCentersTemp == nullptr) {
        return ALLOCATION_ERROR;
    }
    *numOfDataCenters = resultList->getListSize();
    for (int i = 0 ; i < *numOfDataCenters; i ++) {
        int idToEnter = resultList->head->value;
        dataCentersTemp[i] = idToEnter;
        resultList->removeNode(resultList->head);
    }
    *dataCenters= dataCentersTemp;
    return SUCCESS;
}

