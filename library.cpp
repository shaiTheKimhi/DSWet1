#include "library1.h"
#include "DataCenterSystem.h"

void *Init() {
    DataCenterSystem *DS = new DataCenterSystem();
    DS->Init();
    return (void*)DS;
}

StatusType AddDataCenter(void *DS, int dataCenterID, int numOfServers) {
    if (DS == NULL) {
        return INVALID_INPUT;
    }
    return ((DataCenterSystem *)DS)->AddDataCenter(dataCenterID,numOfServers);
}

StatusType RemoveDataCenter(void *DS, int dataCenterID) {
    if (DS == NULL) {
        return INVALID_INPUT;
    }
    return ((DataCenterSystem *)DS)->RemoveDataCenter(dataCenterID);
}

StatusType RequestServer(void *DS, int dataCenterID, int serverID, int os, int *assignedID) {
    if (DS == NULL) {
        return INVALID_INPUT;
    }
    return ((DataCenterSystem *)DS)->RequestServer(dataCenterID,serverID,os,assignedID);
}

StatusType FreeServer(void *DS, int dataCenterID, int serverID) {
    if (DS == NULL) {
        return INVALID_INPUT;
    }
    return ((DataCenterSystem *)DS)->FreeServer(dataCenterID,serverID);
}

StatusType GetDataCentersByOS(void *DS, int os, int **dataCenters, int* numOfDataCenters) {
    if (DS == NULL) {
        return INVALID_INPUT;
    }
    return ((DataCenterSystem *)DS)->GetDataCentersByOS(os,dataCenters,numOfDataCenters);
}

void Quit(void **DS) {
    return ((DataCenterSystem *)DS)->Quit(DS);
}