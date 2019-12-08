

#include "DataCenterSystem.h"

DataCenterSystem::~DataCenterSystem() {
    delete (this->_dataCentersByLinuxCount);
    this->_dataCentersByLinuxCount = NULL;
    delete(this->_dataCentersByWindowsCount);
    this->_dataCentersByWindowsCount = NULL;
    delete (this->_dataCentersById);
    this->_dataCentersById = NULL;
}
void DataCenterSystem::Init() {
    this->_dataCentersById = new AVLTree<int, DataCenter*>();
    this->_dataCentersByWindowsCount = new AVLTree<int, AVLTree<int,int>*>();
    this->_dataCentersByLinuxCount = new AVLTree<int, AVLTree<int,int>*>();
}

void DataCenterSystem::Quit(void** DS) {
    DataCenterSystem *ds = (DataCenterSystem *) *DS;
    if (ds != NULL) {
        delete(ds);
    }
    *DS = NULL;
}

StatusType DataCenterSystem::AddDataCenter(int dataCenterID, int numOfServers) {
    if (numOfServers <= 0 || dataCenterID <= 0) {
        return INVALID_INPUT;
    }
    if (this->_dataCentersById != NULL){
        if (this->_dataCentersById->findAVLNode(dataCenterID) != NULL) {
            return FAILURE;
        }
    }

    DataCenter *dc = new DataCenter(numOfServers, dataCenterID,this->_dataCentersByLinuxCount, this->_dataCentersByWindowsCount);
    this->_dataCentersById->insert(dataCenterID, dc);
    return SUCCESS;
}

StatusType DataCenterSystem::RemoveDataCenter(int dataCenterID) {
    if (dataCenterID <= 0) {
        return INVALID_INPUT;
    }
    if (!(this->_dataCentersById->isExist(dataCenterID))) {
        return FAILURE;
    }

    DataCenter *dc = this->_dataCentersById->findAVLNode(dataCenterID)->getData();
    this->_dataCentersById->deleteKey(dataCenterID);
    if (this->_dataCentersByLinuxCount->isExist(dc->linuxServerCounter)) {
        AVLTree<int,int>* linuxCounterTreeID =  this->_dataCentersByLinuxCount->findAVLNode(dc->getLinuxCounter())->getData();
        linuxCounterTreeID->deleteKey(dataCenterID);
        if (linuxCounterTreeID->isEmpty()) {
            this->_dataCentersByLinuxCount->deleteKey(dc->linuxServerCounter);
        }
    }
    if (this->_dataCentersByWindowsCount->isExist(dc->windowsServerCounter)) {
        AVLTree<int,int>* windowsCounterTreeID =  this->_dataCentersByWindowsCount->findAVLNode(dc->getWindowsCounter())->getData();
        windowsCounterTreeID->deleteKey(dataCenterID);
        if (windowsCounterTreeID->isEmpty()) {
            this->_dataCentersByWindowsCount->deleteKey(dc->windowsServerCounter);
        }
    }
    delete dc;
    dc = NULL;
    return SUCCESS;
}

StatusType DataCenterSystem::RequestServer(int dataCenterID, int serverID, int os, int *assignedID) {
    if (dataCenterID <= 0 || os < 0 || os > 1 || assignedID == NULL || serverID < 0) {
        return INVALID_INPUT;
    }
    if ((!this->_dataCentersById->isExist(dataCenterID))) {
        return FAILURE;
    }
    DataCenter *dc = this->_dataCentersById->findAVLNode(dataCenterID)->getData();
    return dc->dataCenterRequestServer(serverID, os, assignedID, this->_dataCentersByLinuxCount, this->_dataCentersByWindowsCount);
}

StatusType DataCenterSystem::FreeServer(int dataCenterID, int serverID) {
    if (dataCenterID <= 0 || serverID < 0)
        return INVALID_INPUT;
    if (this->_dataCentersById->findAVLNode(dataCenterID) == NULL)
        return FAILURE;
    DataCenter *dc = this->_dataCentersById->findAVLNode(dataCenterID)->getData();
    return dc->dataCenterFreeServer(serverID);
}

StatusType DataCenterSystem::GetDataCentersByOS(int os, int **dataCenters, int* numOfDataCenters) {
    if (dataCenters == NULL || numOfDataCenters == NULL || os > 1 || os <0 ) {
        return INVALID_INPUT;
    }

    if (this->_dataCentersById->isEmpty()) {
        return FAILURE;
    }

    AVLTree<int,int>** tempArray = (os == 0) ? this->_dataCentersByLinuxCount->inOrderDataArray() : this->_dataCentersByWindowsCount->inOrderDataArray() ;
    if (tempArray == NULL) {
        return ALLOCATION_ERROR;
    }
    int tempArraySize = (os == 0) ? this->_dataCentersByLinuxCount->getSize() : this->_dataCentersByWindowsCount->getSize() ;
    List<AVLTree<int,int>*>* tempList = new List<AVLTree<int,int>*>();
    for (int i = 0; i < tempArraySize; i++) {
        tempList->addNode(tempArray[i]);
    }
    List<int>* resultList = new List<int>();
    for (int i = 0; i < tempArraySize ; i++) {
        AVLTree<int,int>* currTree = tempList->head->value;
        int tempInnerTreeSize = currTree->getSize();
        int* tempIDs = currTree->inOrderKeyArray();
        if (tempIDs == NULL) {
            return ALLOCATION_ERROR;
        }
        for (int j = 0 ; j < tempInnerTreeSize; j++){
            resultList->appendNode(tempIDs[j]);
        }
        free(tempIDs);
        tempIDs = NULL;
        tempList->removeNode(tempList->head);
    }
    free(tempArray);
    tempArray = NULL;
    int size = resultList->getListSize();
    int* dataCentersTemp = (int*)malloc(size* sizeof(int));
    if (dataCentersTemp == NULL) {
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

