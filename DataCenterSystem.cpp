

#include "DataCenterSystem.h"


void* Init()
{
    DataCenterSystem ds = (DataCenterSystem)malloc(sizeof(struct _dataCenterSystem));
    return (void*)ds;
}

StatusType AddDataCenter(void *DS, int dataCenterID, int numOfServers)
{
    if(numOfServers <= 0 || DS == NULL || dataCenterID <= 0)
        return INVALID_INPUT;

    DataCenterSystem ds = (DataCenterSystem)DS;
    if(ds->_dataCentersById.findAVLNode(dataCenterID) != NULL)
        return FAILURE;

    DataCenter* dc = new DataCenter(numOfServers, dataCenterID);
    if(!dc)
        return ALLOCATION_ERROR;

    ds->_dataCentersById.insert(dataCenterID, dc);

    if(ds->_dataCentersByLinuxCount.findAVLNode(numOfServers) != NULL)
    {
        AVLTree<int,int>* t = ds->_dataCentersByLinuxCount.findAVLNode(numOfServers).data;
        t->insert(dataCenterID, dataCenterID);
    }
    else
    {
        AVLTree<int,int>* t = new AVLTree<int,int>();
        t->insert(dataCenterId, dataCenterID);
        ds->_dataCentersByLinuxCount.insert(numOfServers,t);
    }
    if(ds->_dataCentersByWindowsCount.findAVLNode(numOfServers) != NULL)
    {
        AVLTree<int,int>* t = ds->_dataCentersByWindowsCount.findAVLNode(numOfServers).data;
        t->insert(dataCenterID, dataCenterID);
    }
    else
    {
        t = new AVLTree<int, int>();
        t->insert(dataCenterID, dataCenterID)
        ds->_dataCentersByWindowsCount.insert(0, t);
    }
    return SUCCESS;
}

StatusType RemoveDataCenter(void *DS, int dataCenterID)
{
    if(DS == NULL || dataCenterID <= 0)
        return INVALID_INPUT;

    DataCenterSystem ds = (DataCenterSystem)DS;
    if(ds->_dataCentersById.findAVLNode(dataCenterID) == NULL)
        return FAILURE;

    DataCenter* dc = ds->_dataCentersById.findAVLNode(dataCenterID).data;
    ds->_dataCentersById.deleteKey(dataCenterID);
    ds->_dataCentersByLinuxCount.findAVLNode(dc->getLinuxCounter()).data

    
    AVLTree<int,int>* t = ds->_dataCentersByLinuxCount.findAVLNode(dc->getLinuxCounter()).data;
    t->deleteKey(dataCenterID);
    if(t->isEmpty()){//needs to have to ability to check if tree is empty
        delete t;
        ds->_dataCentersByLinuxCount.deleteKey(dc->getLinuxCounter());
    }
    AVLTree<int,int>* t = ds->_dataCentersByWindowsCount.findAVLNode(dc->getWindowsCounter()).data;
    t->deleteKey(dataCenterID);
    if(t->isEmpty()){
        delete t;
        ds->_dataCentersByWindowsCount.deleteKey(dc->getWindowsCounter());
    }
    delete dc;
    return SUCCESS;
}

StatusType RequestServer(void *DS, int dataCenterID, int serverID, int os, int* assignedID)
{
    if(DS == NULL || dataCenterID <= 0 || os < 0 || os > 1 || assignedID == NULL || serverID < 0)
        return INVALID_INPUT;
    DataCenterSystem ds = (DataCenterSystem)DS;
    if(ds->_dataCentersById.findAVLNode(dataCenterID) == NULL)
        return FAILURE;
    DataCenter* dc = ds->_dataCentersById.findAVLNode(dataCenterID).data;
    if(dc->getLinuxCounter() + dc->getWindowsCounter >= serverID)
        return INVALID_INPUT;
    int linuxCount = dc->getLinuxCounter(), windowsCount = dc->getWindowsCounter();
    dc->dataCenterRequestServer(serverID, os, assignedID);

    //updating linux and windows avls:
    int diff = dc->getLinuxCounter() - linuxCounter;
    if(diff != 0)
    {
        //updating linux count avl:
        AVLTree<int, int>* t = ds->_dataCentersByLinuxCount.findAVLNode(linuxCount).data;
        t.deleteKey(dataCenterID);
        if(t->isEmpty())
        {
            delete t;
            ds->_dataCentersByLinuxCount.deleteKey(linuxCount);
        }
        if(ds->_dataCentersByLinuxCount.findAVLNode(dc->getLinuxCounter()) == NULL)
        {   
            t = new AVLTree<int, int>();
            t.insert(dataCenterID, dataCenterID);
            ds->_dataCentersByLinuxCount.insert(dc->getLinuxCounter(), t);
        }
        else
        {
            t = ds->_dataCentersByLinuxCount.findAVLNode(dc->getLinuxCounter()).data;
            t.insert(dataCenterID, dataCenterID);   
        }
        
        //updating windows count avl:
        AVLTree<int, int>* t = ds->_dataCentersByWindowsCount.findAVLNode(windowsCount).data;
        t.deleteKey(dataCenterID);
        if(t->isEmpty())
        {
            delete t;
            ds->_dataCentersByWindowsCount.deleteKey(windowsCount);
        }
        if(ds->_dataCentersByWindowsCount.findAVLNode(dc->getWindowsCounter()) == NULL)
        {   
            t = new AVLTree<int, int>();
            t.insert(dataCenterID, dataCenterID);
            ds->_dataCentersByLinuxCount.insert(dc->getWindowsCounter(), t);
        }
        else
        {
            t = ds->_dataCentersByLinuxCount.findAVLNode(dc->getWindowsCounter()).data;
            t.insert(dataCenterID, dataCenterID);   
        }
    }
  

    return SUCCESS;
}

StatusType FreeServer(void *DS, int dataCenterID, int serverID)
{
    if(DS == NULL || dataCenterID <= 0 || serverID < 0)
        return INVALID_INPUT;
    DataCenterSystem ds = (DataCenterSystem)DS;
    if(ds->_dataCentersById.findAVLNode(dataCenterID) == NULL)
        return FAILURE;
    DataCenter* dc = ds->_dataCentersById.findAVLNode(dataCenterID).data;
    if(dc->getLinuxCounter() + dc->getWindowsCounter >= serverID)
        return INVALID_INPUT;
    
    dc->dataCenterFreeServer(serverID);
    return SUCCESS;
}
