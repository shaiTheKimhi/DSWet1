#pragma once
#include"DataCenter.h"
#include "AVLtree.h"
typedef struct _dataCenterSystem
{
    AVLTree<int, DataCenter> _dataCentersById;
    AVLTree<int, DataCenter> _dataCentersByLinuxCount;
    AVLTree<int, DataCenter> _dataCentersByWindowsCount;
}* DataCenterSystem;