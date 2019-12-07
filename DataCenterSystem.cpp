

#include "DataCenterSystem.h"


void *Init() {
    DataCenterSystem *ds = new DataCenterSystem();
    ds->_dataCentersById= AVLTree<int, DataCenter*>();
    ds->_dataCentersByWindowsCount = AVLTree<int, AVLTree<int,int>*>();
    ds->_dataCentersByLinuxCount = AVLTree<int, AVLTree<int,int>*>();
    return (void *) ds;
}

StatusType AddDataCenter(void *DS, int dataCenterID, int numOfServers) {
    if (numOfServers <= 0 || DS == nullptr || dataCenterID <= 0) {
        return INVALID_INPUT;
    }

    DataCenterSystem *ds = (DataCenterSystem *) DS;
    if (ds->_dataCentersById.findAVLNode(dataCenterID) != NULL) {
        return FAILURE;
    }

    DataCenter *dc = new DataCenter(numOfServers, dataCenterID);

    ds->_dataCentersById.insert(dataCenterID, dc);

    if (ds->_dataCentersByLinuxCount.findAVLNode(numOfServers) != NULL) {
        AVLTree<int, int> *t = ds->_dataCentersByLinuxCount.findAVLNode(numOfServers)->getData();
        t->insert(dataCenterID, dataCenterID);
    } else {
        AVLTree<int, int> *t = new AVLTree<int, int>();
        t->insert(dataCenterID, dataCenterID);
        ds->_dataCentersByLinuxCount.insert(numOfServers, t);
    }
    if (ds->_dataCentersByWindowsCount.findAVLNode(numOfServers) != NULL) {
        AVLTree<int, int> *t = ds->_dataCentersByWindowsCount.findAVLNode(numOfServers)->getData();
        t->insert(dataCenterID, dataCenterID);
    } else {
        AVLTree<int, int> *t = new AVLTree<int, int>();
        t->insert(dataCenterID, dataCenterID);
        ds->_dataCentersByWindowsCount.insert(0, t);
    }
    return SUCCESS;
}

StatusType RemoveDataCenter(void *DS, int dataCenterID) {
    if (DS == NULL || dataCenterID <= 0)
        return INVALID_INPUT;

    DataCenterSystem *ds = (DataCenterSystem *) DS;
    if (ds->_dataCentersById.findAVLNode(dataCenterID) == NULL)
        return FAILURE;

    DataCenter *dc = ds->_dataCentersById.findAVLNode(dataCenterID)->getData();
    ds->_dataCentersById.deleteKey(dataCenterID);
    ds->_dataCentersByLinuxCount.findAVLNode(dc->getLinuxCounter())->getData()->deleteKey(dataCenterID);

    AVLTree<int, int> *linuxTree = ds->_dataCentersByLinuxCount.findAVLNode(dc->getLinuxCounter())->getData();
    linuxTree->deleteKey(dataCenterID);
    if (linuxTree->isEmpty()) {//needs to have to ability to check if tree is empty
        delete linuxTree;
        ds->_dataCentersByLinuxCount.deleteKey(dc->getLinuxCounter());
    }
    AVLTree<int, int> *windowsTree = ds->_dataCentersByWindowsCount.findAVLNode(dc->getWindowsCounter())->getData();
    windowsTree->deleteKey(dataCenterID);
    if (windowsTree->isEmpty()) {
        delete windowsTree;
        ds->_dataCentersByWindowsCount.deleteKey(dc->getWindowsCounter());
    }
    delete dc;
    return SUCCESS;
}

StatusType RequestServer(void *DS, int dataCenterID, int serverID, int os, int *assignedID) {
    if (DS == nullptr || dataCenterID <= 0 || os < 0 || os > 1 || assignedID == nullptr || serverID < 0)
        return INVALID_INPUT;
    DataCenterSystem *ds = (DataCenterSystem *) DS;
    if (ds->_dataCentersById.findAVLNode(dataCenterID) == nullptr)
        return FAILURE;
    DataCenter *dc = ds->_dataCentersById.findAVLNode(dataCenterID)->getData();
    int totalServerCount = dc->getLinuxCounter() + dc->getWindowsCounter();
    if (totalServerCount <= serverID) {
        return INVALID_INPUT;
    }

    int linuxCount = dc->getLinuxCounter(), windowsCount = dc->getWindowsCounter();
    dc->dataCenterRequestServer(serverID, os, assignedID, &(ds->_dataCentersByLinuxCount), &(ds->_dataCentersByWindowsCount));

//    //updating linux and windows avls:
//    int diff = dc->getLinuxCounter() - linuxCount;
//    if (diff != 0) {
//        //updating linux count avl:
//        AVLTree<int, int> *dataCentersWithLinuxServers = ds->_dataCentersByLinuxCount.findAVLNode(
//                linuxCount)->getData();
//        dataCentersWithLinuxServers->deleteKey(dataCenterID);
//        if (dataCentersWithLinuxServers->isEmpty()) {
//            delete dataCentersWithLinuxServers;
//            ds->_dataCentersByLinuxCount.deleteKey(linuxCount);
//        }
//        if (ds->_dataCentersByLinuxCount.findAVLNode(dc->getLinuxCounter()) == NULL) {
//            dataCentersWithLinuxServers = new AVLTree<int, int>();
//            dataCentersWithLinuxServers->insert(dataCenterID, dataCenterID);
//            ds->_dataCentersByLinuxCount.insert(dc->getLinuxCounter(), dataCentersWithLinuxServers);
//        } else {
//            dataCentersWithLinuxServers = ds->_dataCentersByLinuxCount.findAVLNode(dc->getLinuxCounter())->getData();
//            dataCentersWithLinuxServers->insert(dataCenterID, dataCenterID);
//        }
//
//        //updating windows count avl:
//        AVLTree<int, int> *dataCentersWithWindowsServers = ds->_dataCentersByWindowsCount.findAVLNode(windowsCount)->getData();
//        dataCentersWithWindowsServers->deleteKey(dataCenterID);
//        if (dataCentersWithWindowsServers->isEmpty()) {
//            delete dataCentersWithWindowsServers;
//            ds->_dataCentersByWindowsCount.deleteKey(windowsCount);
//        }
//        if (ds->_dataCentersByWindowsCount.findAVLNode(dc->getWindowsCounter()) == NULL) {
//            dataCentersWithWindowsServers = new AVLTree<int, int>();
//            dataCentersWithWindowsServers->insert(dataCenterID, dataCenterID);
//            ds->_dataCentersByLinuxCount.insert(dc->getWindowsCounter(), dataCentersWithWindowsServers);
//        } else {
//            dataCentersWithLinuxServers = ds->_dataCentersByLinuxCount.findAVLNode(dc->getWindowsCounter())->getData();
//            dataCentersWithLinuxServers->insert(dataCenterID, dataCenterID);
//        }
   // }


    return SUCCESS;
}

StatusType FreeServer(void *DS, int dataCenterID, int serverID) {
    if (DS == NULL || dataCenterID <= 0 || serverID < 0)
        return INVALID_INPUT;
    DataCenterSystem *ds = (DataCenterSystem*) DS;
    if (ds->_dataCentersById.findAVLNode(dataCenterID) == NULL)
        return FAILURE;
    DataCenter *dc = ds->_dataCentersById.findAVLNode(dataCenterID)->getData();
    if (dc->getLinuxCounter() + dc->getWindowsCounter() >= serverID)
        return INVALID_INPUT;

    dc->dataCenterFreeServer(serverID);
    return SUCCESS;
}
