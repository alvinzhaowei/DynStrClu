#include "Vertex.h"

using namespace dynscan;
/**
 *  Declare the static variables.
 */

MyVector<int> Vertex::intersectionCntManager;

static const int adj_hash_initialization_size = 11;

Vertex::Vertex(const int &_id) {
    id = _id;
    updateCnt = 0;
    is_large = false;
    dtBucketPtr = new DTBucket;
    adjacentList.reserve(adj_hash_initialization_size);
    neighborIDAdjacentIndexMap.reserve(adj_hash_initialization_size);
    NOPtr = new multimap<float, int>;
}

void Vertex::insertNeighbor(const int &_neighborID, const int &_intersectionCntManagerIndex, float simScore) {
    adjacentList.push_back(_neighborID);
    neighbor_node_sc.push_back(simScore);
    NOPtr->insert(make_pair(simScore, _neighborID));
    neighborIDAdjacentIndexMap[_neighborID] = adjacentList.size() - 1;
    if (!is_large) {
        intersectionCntIndex.push_back(_intersectionCntManagerIndex);
    }
}

void Vertex::updateNeighborSimScore(float sco, const int _neighborID) {
    int _index = neighborIDAdjacentIndexMap.at(_neighborID);
    float pre_sco = neighbor_node_sc[_index];
    auto range = NOPtr->equal_range(pre_sco);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second == _neighborID) {
            NOPtr->erase(it);
            break;
        }
    }
    neighbor_node_sc[_index] = sco;
    NOPtr->insert(make_pair(sco, _neighborID));
}

void Vertex::deleteNeighbor(const int _neighborID) {
    int _index = neighborIDAdjacentIndexMap.at(_neighborID);
    neighborIDAdjacentIndexMap.erase(_neighborID);
    int length = adjacentList.size();
    if (length == _index + 1) {
        adjacentList.pop_back();
        neighbor_node_sc.pop_back();
        if (!is_large) {
            intersectionCntIndex.pop_back();
        }
    } else {
        float pre_sco = neighbor_node_sc[_index];
        auto range = NOPtr->equal_range(pre_sco);
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second == _neighborID) {
                NOPtr->erase(it);
                break;
            }
        }

        adjacentList[_index] = adjacentList[length - 1];
        neighbor_node_sc[_index] = neighbor_node_sc[length - 1];
        adjacentList.pop_back();
        neighbor_node_sc.pop_back();
        intersectionCntIndex[_index] = intersectionCntIndex[length - 1];
        intersectionCntIndex.pop_back();
        neighborIDAdjacentIndexMap.at(Vertex::adjacentList[_index]) = _index;
    }
}

int Vertex::query(double esp, int mu) {
    int count = 0;
    for (auto rit = NOPtr->rbegin(); rit != NOPtr->rend(); ++rit) {
        if(rit->first >= esp){
//            rit->second;
            count++;
        }
        else{
            break;
        }
    }


    return count;
}