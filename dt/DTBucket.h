#ifndef DYNSTRCLU_DTBUCKET_H
#define DYNSTRCLU_DTBUCKET_H

#include "../MyLib/MyVector.h"
#include <algorithm>

using namespace std;

static int pow_2[30] = {1,
                        2,
                        4,
                        8,
                        16,
                        32,
                        64,
                        128,
                        256,
                        512,
                        1024,
                        2048,
                        4096,
                        8192,
                        16384,
                        32768,
                        65536,
                        131072,
                        262144,
                        524288,
                        1048576,
                        2097152,
                        4194304,
                        8388608,
                        16777216,
                        33554432,
                        67108864,
                        134217728,
                        268435456,
                        536870912};

/*
 *  This is the element class of the buckets for DT instances.
 */
class DTBucketElement {
    friend class DTBucket;

    friend class DTInstance;

protected:
    // records the last count
    int cnt;
    // records the IDs of current vertex's neighbor.
    int neighborID;
    // We use dtIndex to find the corresponding DT instance.
    int dtIndex;
    // We use the bucket_index to find the record which bucket it is in.
//    int bucket_index;
    // store the element index
    unsigned int element_index;

public:
    DTBucketElement() {
        neighborID = -1;
        dtIndex = -1;
        cnt = 0;
        element_index = 0;
    }

    inline const int &get_dt_index() const {
        return dtIndex;
    }

    inline const int &get_neighbor_id() {
        return neighborID;
    };

    inline const int &get_cnt() const {
        return cnt;
    }

    inline void update_cnt(int cnt) {
        this->cnt = cnt;
    }

    inline const unsigned int &get_element_index() const {
        return element_index;
    }

};


/*
 *  The class of buckets for DT instances.
 */
class DTBucket {
public:

    MyVector<MyVector<DTBucketElement *>> buckList;

    MyVector<int> cnt;

    //constructor
    DTBucket() {

    }

    static MyVector<DTBucketElement *> EmptyBucket;

    int listSize();

    int sizeByIndex(int i);

    void extendListSize(int size);

    void shrinkToFit();

    int InsertNewELement(int i, DTBucketElement *e, int updateCnt);

    bool CheckEmptyByIndex(int index);

    // delete element by bucket index and element index
    void DeleteElement(int bucketIndex, int elementIndex);

    DTBucketElement *getElement(int i, int j) const;

    int getCnt(int i) const;

    ~DTBucket();

    void ReleaseSpace();

    void updateCnt(int i, int updateCnt) const;
};

#endif //DYNSTRCLU_DTBUCKET_H
