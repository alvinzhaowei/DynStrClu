#include "DTManager.h"

void DTManager::removeInstance(int _indexToDel) {
    int length = dtInstanceList.size();
    if (_indexToDel == length - 1) {
        dtInstanceList.pop_back();
    } else {
        DTInstance *temp = dtInstanceList[length - 1];
        dtInstanceList[length - 1] = dtInstanceList[_indexToDel];
        dtInstanceList[_indexToDel] = temp;
        dtInstanceList.pop_back();
    }
}
