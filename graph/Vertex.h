#ifndef DYNSCAN_VERTEX_H
#define DYNSCAN_VERTEX_H

#include <map>
#include "../MyLib/MyVector.h"
#include "../dt/DTBucket.h"
#include "../dt/DTInstance.h"
#include "../Tessil_robin_map/robin_map.h"

#define hash_map tsl::robin_map

namespace dynscan {
/*
 *  This is a base class of vertex which is also used in the static case.
 */
    class Vertex {
    private:
        static MyVector<int> intersectionCntManager;
    public:
        int id;
    protected:
        int updateCnt;

        bool is_large;

        MyVector<int> adjacentList;
        MyVector<float> neighbor_node_sc;
        MyVector<int> intersectionCntIndex;

        hash_map<int, int> neighborIDAdjacentIndexMap;

        hash_map<int, int> neighborID_DTInstanceIndex_Map;

        // A pointer of the bucket list for managing all the related DT instances.
        DTBucket* dtBucketPtr;



    public:
        multimap<float, int>* NOPtr;
        /**
         * Return neighbor ID at given _index
         * @param _index
         * @return
         */
        inline int getNeighborID(const int &_index) const {
            return adjacentList[_index];
        }


        /**
         *
         * @return return current vertex's degree
         */
        inline int getDegree() const {
            return adjacentList.size();
        };


        Vertex(const int &_id);

//        Vertex(const int &_id, MyVector<int> &_adjacentList);
//
//        Vertex(const int &_id, const int *&_adjacentList, const int &_adjNum);
        int query(double esp, int mu);

        /**
         * Insert a new neighbor into adjacent list
         * @param _neighborID
         */

        void insertNeighbor(const int &_neighborID, const int &_intersectionCntManagerIndex, float simScore);


        /**
         * Delete a neighbor at give index
         * @param _index
         */
        void deleteNeighbor(const int _neighborID);

        inline int *getAdjacentList() {
            return Vertex::adjacentList.get_list();
        };

        void updateNeighborSimScore(float sco, const int _neighborID);

        inline void setIntersectionCnt(const int &_cn, const int index) {
            intersectionCntManager[index] = _cn;
        }

        inline const int get_instance_index_by_neighbor_id(const int &_neighborID) {
            const auto &it = neighborID_DTInstanceIndex_Map.find(_neighborID);
            return it == neighborID_DTInstanceIndex_Map.end() ? -1 : it->second;
        }

        inline void set_instance_index_map_by_neighbor_id(const int &_neighborID, const int &_index) {
            neighborID_DTInstanceIndex_Map[_neighborID] = _index;
        }

        /**
         * @param _neighborID
         * @return the index of the given neighbor ID in the adjacent list of the current vertex.
         */
        inline const int getAdjacentIndex(const int &_neighborID) const {
            auto it = neighborIDAdjacentIndexMap.find(_neighborID);
            return it == neighborIDAdjacentIndexMap.end() ? -1 : it->second;
        }

        /**
         * Caution. Only used for Class Jaccard.
         * @param _neighbor
         * @return
         */
        inline bool has_neighbor(const int &_neighbor) const {
            return neighborIDAdjacentIndexMap.find(_neighbor) != neighborIDAdjacentIndexMap.end()
                   || this->id == _neighbor;
        }

        /**
         * Return intersection count at given _index
         * @param _index
         * @return
         */
        inline const int &getIntersectionCnt(const int &_index) const {
            return intersectionCntManager[intersectionCntIndex[_index]];
        }

        inline const int &increaseIntersectionCnt(const int &_index) {
            return ++intersectionCntManager[intersectionCntIndex[_index]];
        }

        inline void decreaseIntersectionCnt(const int &_index) {
            --intersectionCntManager[intersectionCntIndex[_index]];
        }

        /**
         *
         * @param the ID of vertex to delete from
         */
        inline void DeleteElement(int bucketIndex, int elementIndex) {
            dtBucketPtr->DeleteElement(bucketIndex, elementIndex);
        }


        inline void increaseUpdateCnt() {
            ++updateCnt;
        }

        inline int getCnt() const {
            return updateCnt;
        }

        /**
        * @param bucket index, element to insert
        * @return return the new number elements in the bucket.
        */

        inline int addDTBucketElement(int i, DTBucketElement* e, int updateCnt) {
            return dtBucketPtr->InsertNewELement(i, e, updateCnt);
        }

        inline MyVector<MyVector<DTBucketElement*>> getDTBucket() {
            return dtBucketPtr->buckList;
        }

        inline bool CheckEmptyByIndex(int index) {
            return dtBucketPtr->CheckEmptyByIndex(index);
        }

        inline int listSize() {
            return dtBucketPtr->listSize();
        }

        inline int sizeByIndex(int i) {
            return dtBucketPtr->sizeByIndex(i);
        }

        inline void updateBucketCount(int i, int updateCount) {
            dtBucketPtr->updateCnt(i, updateCount);
        }

        inline int getBucketCount(int i) {
            return dtBucketPtr->getCnt(i);
        }


        inline DTBucketElement* getDTBucketElement(int i, int j) {
            return dtBucketPtr->getElement(i, j);
        }

        inline static int allocate_intersection_cnt_index() {
            int rtn = intersectionCntManager.size();
            intersectionCntManager.push_back(0);
            return rtn;
        }

        /**
         * Change the indicator isLarge of a vertex.
         */
        inline void set_large() {
            is_large = true;
            intersectionCntIndex.clear();
        };


        /**
         * Return true if vertex is large vertex; false if not
         * @return
         */
        inline const bool &isLarge() const {
            return is_large;
        };


    };

}
#endif //DYNSCAN_VERTEX_H
