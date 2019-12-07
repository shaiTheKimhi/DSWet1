#include "library1.h"
#include "DataCenterSystem.h"

void *Init() {
    DataCenterSystem *DS = new DataCenterSystem();
    return (void*)DS;
}

StatusType AddDataCenter(void *DS, int dataCenterID, int numOfServers) {
    return ((DataCenterSystem *)DS)->AddDataCenter(DS, dataCenterID,numOfServers);
}

StatusType RemoveDataCenter(void *DS, int dataCenterID) {
    return ((DataCenterSystem *)DS)->RemoveDataCenter(DS,dataCenterID);
}

StatusType RequestServer(void *DS, int dataCenterID, int serverID, int os, int *assignedID) {
    return ((DataCenterSystem *)DS)->RequestServer(DS,dataCenterID,serverID,os,assignedID);
}

StatusType FreeServer(void *DS, int dataCenterID, int serverID) {
    return ((DataCenterSystem *)DS)->FreeServer(DS,dataCenterID,serverID);
}

StatusType GetDataCentersByOS(void *DS, int os, int **dataCenters, int* numOfDataCenters) {
    return ((DataCenterSystem *)DS)->GetDataCentersByOS(DS,os,dataCenters,numOfDataCenters);
}

void Quit(void **DS) {
    return ((DataCenterSystem *)DS)->Quit(DS);
}