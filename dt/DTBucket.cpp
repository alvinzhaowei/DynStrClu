#include "DTBucket.h"

MyVector<DTBucketElement*> DTBucket::EmptyBucket;

void DTBucket::DeleteElement(int bucketIndex, int elementIndex) {
    MyVector<DTBucketElement*>& bList = buckList[bucketIndex];
    if (bList.size() == 1) {
        bList.pop_back();
        //this->shrinkToFit();
        return;
    }
    int old_size = bList.size();
    DTBucketElement* swap_e = bList[old_size - 1];
    bList[elementIndex] = swap_e;
    swap_e->element_index = elementIndex;
    bList.pop_back();
    if (bList.size() < bList.capacity() * 0.5) {
        bList.shrink_to_fit();
    }
}

DTBucket::~DTBucket() {
    for (int i = 0; i < buckList.size(); ++i) {
        buckList[i].release_space();
    }
    buckList.release_space();

}

int DTBucket::listSize() {
    return buckList.size();
}

void DTBucket::extendListSize(int index) {
    while (buckList.size() < index) {
        buckList.push_back(EmptyBucket);
        cnt.push_back(0);
    }
}

int DTBucket::InsertNewELement(int i, DTBucketElement* e, int updateCnt) {
    this->extendListSize(i + 1);
    buckList[i].push_back(e);
    if(buckList[i].size() == 1){
        cnt[i] = updateCnt;
    }
    e->element_index = buckList[i].size() - 1;
    return buckList[i].size();
}

bool DTBucket::CheckEmptyByIndex(int index) {
    if (buckList.size() < index) {
        return true;
    } else if (buckList[index].size() == 0) {
        return true;
    }
    return false;

}

int DTBucket::sizeByIndex(int i) {
    if (buckList.size() <= i) {
        return 0;
    }
    return buckList[i].size();
}

void DTBucket::shrinkToFit() {
    int i = buckList.size() - 1;
    while (buckList[i].size() == 0) {
        buckList.pop_back();
        cnt.pop_back();
        i--;
    }
}

void DTBucket::ReleaseSpace() {
    for (int i = 0; i < buckList.size(); ++i) {
        buckList[i].release_space();
    }
    buckList.release_space();
}

DTBucketElement* DTBucket::getElement(int i, int j) const {
    return buckList[i][j];
}

int DTBucket::getCnt(int i) const {
    return cnt[i];
}

void DTBucket::updateCnt(int i, int updateCnt) const {
    cnt[i] = updateCnt;
}


