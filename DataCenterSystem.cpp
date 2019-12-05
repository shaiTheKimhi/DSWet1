

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

    DataCenterSystem ds = (DataCenter)DS;
    if(ds->_dataCentersById.findAVLNode(dataCenterID) != NULL)
        return FAILURE;

    DataCenter* dc = new DataCenter(numOfServers, dataCenterID);
    if(!dc)
        return ALLOCATION_ERROR;

    ds->_dataCentersById.insert(dataCenterID, dc);
    ds->_dataCentersByLinuxCount.insert(numOfServers, dc);
    ds->_dataCentersByWindowsCount.insert(0, dc);

}

