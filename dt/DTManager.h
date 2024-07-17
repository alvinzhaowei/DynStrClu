#ifndef DYNSCAN_DTMANAGER_H
#define DYNSCAN_DTMANAGER_H

#include "../MyLib/MyVector.h"
#include "DTInstance.h"

/*
 *  The class to manage all the DT instances.
 */
class DTManager {
protected:
    /**
     *  The list of all the DT instances.
     */
    MyVector<DTInstance *> dtInstanceList;

public:
    /**
     * Insert a DT instance at the end of the list.
     * @param _instance
     * @return 0 for success, and 1 for failure.
     */
    int insertInstance(DTInstance *&_instance) {
        dtInstanceList.push_back(_instance);
        return 0;
    };

    /**
     * Remove a DT instance at the specified index at the list.
     * Note:
     *  When deleting an instance from the list, swap the last one with it and then pop_back.
     *  Furthermore, during the swapping, remember to update the $dtIndex$ in the corresponding DTHeap's.
     * @param _indexToDel
     * @return 0 for success, and 1 for failure.
     */
    void removeInstance(int _indexToDel);

    inline DTInstance *get_instance(const int &_index) {
        return dtInstanceList[_index];
    }

    inline int get_size() const {
        return dtInstanceList.size();
    }

};


#endif //DYNSCAN_DTMANAGER_H
