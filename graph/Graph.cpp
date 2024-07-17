#include <algorithm>
#include <queue>
#include "Graph.h"
#include "../MyLib/MyTimer.h"

Graph::Graph(MyVector<dynscan::Vertex *> &_vList, double _rho) {
    vList.swap(_vList);
    rho = _rho;
    int vertex_number = (int) vList.size();
    computePermutationNumber(omega * _rho);
//    myJaccard = new Jaccard((long double) 1.0 / FAILURE_PROB, omega * rho);
     myJaccard = new Jaccard((long double) 1.0 / (1 / vertex_number), omega * rho);
}

int Graph::insertEdge(int _vID1, int _vID2) {

    auto *v1 = (dynscan::Vertex *) vList[_vID1 - 1];
    auto *v2 = (dynscan::Vertex *) vList[_vID2 - 1];
    // check if these vertices already exist. If not, create new vertex/vertices
    // with vertex factory and insert it/them into unordered map.
    if (v1 == NULL) {
        v1 = (dynscan::Vertex *) createVertex(_vID1);
    }
    if (v2 == NULL) {
        v2 = (dynscan::Vertex *) createVertex(_vID2);
    }
    if (!v1->isLarge() && !v2->isLarge()) {
        if (v1->getDegree() >= permutationNum - 1 &&
            v2->getDegree() >= permutationNum - 1) {
            makeLarge(v1);
            makeLarge(v2);
        }
    } else if (!v1->isLarge() && v1->getDegree() >= permutationNum - 1 &&
               v2->isLarge()) {
        makeLarge(v1);
    } else if (!v2->isLarge() && v2->getDegree() >= permutationNum - 1 &&
               v1->isLarge()) {
        makeLarge(v2);
    }

    if (v1->isLarge() && !v2->isLarge() || v1->getDegree() > v2->getDegree()) {
        std::swap(_vID1, _vID2);
        std::swap(v1, v2);
    }
    if (!v1->isLarge()) {
        if (!v2->isLarge()) {
            insertBetweenSmall(v1, v2);
        } else {
            insertBetweenSmallAndLarge(v1, v2);
        }
    } else {
        insertBetweenLarge(v1, v2);
    }

    return 0;
}

int Graph::removeEdge(int _vID1, int _vID2) {
    auto *v1 = (dynscan::Vertex *) vList[_vID1 - 1];
    auto *v2 = (dynscan::Vertex *) vList[_vID2 - 1];

    if (v1 == NULL || v2 == NULL)
        return 1;
    v1->deleteNeighbor(_vID2);
    v2->deleteNeighbor(_vID1);

    const int dtIndex = v1->get_instance_index_by_neighbor_id(_vID2);
    if (dtIndex >= 0) {
        DTInstance *instance = dtManager.get_instance(dtIndex);
        const int _bucket_index = instance->get_exp();
        const int element_index_1 = instance->get_element_index(_vID2);
        const int element_index_2 = instance->get_element_index(_vID1);
        v1->DeleteElement(_bucket_index, element_index_1);
        v2->DeleteElement(_bucket_index, element_index_2);
        dtManager.removeInstance(dtIndex);
    }
    if ((v1->isLarge() && !v2->isLarge()) ||
        v1->getDegree() > v2->getDegree()) {
        std::swap(_vID1, _vID2);
        std::swap(v1, v2);
    }
    if (!v1->isLarge() && !v2->isLarge()) {
        deleteBetweenSmall(v1, v2);
    } else if (!v1->isLarge() && v2->isLarge()) {
        deleteBetweenSmallAndLarge(v1, v2);
    } else {
        deleteBetweenLarge(v1, v2);
    }
    return 0;
}

int Graph::insertBetweenSmall(dynscan::Vertex *v1, dynscan::Vertex *v2) {
    int commonCnt = 2;
    const int &vID1 = v1->id;
    const int &vID2 = v2->id;
    const int degree1 = v1->getDegree();
    const int degree2 = v2->getDegree();

    int *adjacentList1 = v1->getAdjacentList();
    for (int i = 0; i < degree1; ++i) {
        const int &neighborID = adjacentList1[i];
        auto *neighbor_v = (dynscan::Vertex *) vList[neighborID - 1];
        const int index = v2->getAdjacentIndex(neighborID);
        if (index != -1) {
            ++commonCnt;
            if (neighbor_v->isLarge())
                continue;
            /// only small neighbors are saved; large neighbors are left to
            /// checkVertexDT()
            v1->increaseIntersectionCnt(i);
            v2->increaseIntersectionCnt(index);
        }
    }

    // +4 here as degree increased for both vertex
    double jSimilarity =
            commonCnt / (double) (degree1 + degree2 + 4 - commonCnt);
    int intersection_cnt_new_index =
            dynscan::Vertex::allocate_intersection_cnt_index();
    v1->insertNeighbor(vID2, intersection_cnt_new_index, jSimilarity);
    v2->insertNeighbor(vID1, intersection_cnt_new_index, jSimilarity);
    v1->setIntersectionCnt(commonCnt, intersection_cnt_new_index);
    v2->setIntersectionCnt(commonCnt, intersection_cnt_new_index);
    checkVertexDTBucket(v1);
    checkVertexDTBucket(v2);

    return 0;
}

int Graph::deleteBetweenSmall(dynscan::Vertex *v1, dynscan::Vertex *v2) {
    int degree1 = v1->getDegree();
    int *adjacentList1 = v1->getAdjacentList();

    for (int i = 0; i < degree1; i++) {
        int neighborID = adjacentList1[i];
        dynscan::Vertex *neighbor_v =
                (dynscan::Vertex *) vList[neighborID - 1];
        int index = v2->getAdjacentIndex(neighborID);
        if (neighbor_v->isLarge()) {
            continue;
        }
        // if neighbor vertex is a common neighbor of v1 and v2
        if (index != -1) {
            v1->decreaseIntersectionCnt(i);
            v2->decreaseIntersectionCnt(index);
        }
    }
    checkVertexDTBucket(v1);
    checkVertexDTBucket(v2);

    return 0;
}

int Graph::insertBetweenSmallAndLarge(dynscan::Vertex *v1,
                                      dynscan::Vertex *v2) {
    int commonCnt = 2;
    int vID1 = v1->id;
    int vID2 = v2->id;

    int degree1 = v1->getDegree();
    int degree2 = v2->getDegree();
    for (int i = 0; i < degree1; i++) {
        int neighborID = v1->getNeighborID(i);
        dynscan::Vertex *neighbor_v =
                (dynscan::Vertex *) vList[neighborID - 1];
        int index = v2->getAdjacentIndex(neighborID);
        // if neighbor vertex is a common neighbor of v1 and v2
        if (index != -1) {
            commonCnt++;
            if (neighbor_v->isLarge())
                continue;
            v1->increaseIntersectionCnt(i);
        }
    }
    double jSimilarity =
            commonCnt / (double) (degree1 + degree2 + 4 - commonCnt);
    int intersection_cnt_new_index =
            dynscan::Vertex::allocate_intersection_cnt_index();
    v1->insertNeighbor(vID2, intersection_cnt_new_index, jSimilarity);
    v2->insertNeighbor(vID1, -1, jSimilarity);
    checkVertexDTBucket(v1);
    checkVertexDTBucket(v2);

    int unionSize = degree1 + degree2 + 4 - commonCnt;
    int updateCnt1 = v1->getCnt();
    int updateCnt2 = v2->getCnt();
    int dtIndex = dtManager.get_size();
    DTInstance *curInstance =
            new DTInstance((1-omega)*rho * rho, unionSize, updateCnt1,
                           updateCnt2, v1->id, v2->id, dtIndex);
    dtManager.insertInstance(curInstance);

    int _exp = curInstance->get_exp();
    v1->addDTBucketElement(_exp, curInstance->get_element1(), updateCnt1);
    v2->addDTBucketElement(_exp, curInstance->get_element2(), updateCnt2);
    v1->set_instance_index_map_by_neighbor_id(v2->id, dtIndex);
    v2->set_instance_index_map_by_neighbor_id(v1->id, dtIndex);
    return 0;
}

int Graph::deleteBetweenSmallAndLarge(dynscan::Vertex *v1,
                                      dynscan::Vertex *v2) {

    int degree1 = v1->getDegree();

    for (int i = 0; i < degree1; i++) {
        int neighborID = v1->getNeighborID(i);
        dynscan::Vertex *neighbor_v =
                (dynscan::Vertex *) vList[neighborID - 1];
        if (neighbor_v->isLarge())
            continue;
        int index = v2->getAdjacentIndex(neighborID);
        // if neighbor vertex is a common neighbor of v1 and v2
        if (index != -1) {
            v1->decreaseIntersectionCnt(i);

        }
    }
    checkVertexDTBucket(v1);
    checkVertexDTBucket(v2);
    return 0;
}

int Graph::insertBetweenLarge(dynscan::Vertex *v1,
                              dynscan::Vertex *v2) {
    int vID1 = v1->id;
    int vID2 = v2->id;
    // current degree of v1(v2) counts in v2(v1), we don't have to consider this
    // pair at this moment.
    int degree1 = v1->getDegree();
    int degree2 = v2->getDegree();
    double SimScore = myJaccard->compute_similarity(*v1, *v2);
    v1->insertNeighbor(vID2, -1, SimScore);
    v2->insertNeighbor(vID1, -1, SimScore);
    checkVertexDTBucket(v1);
    checkVertexDTBucket(v2);

    int maxDegree = (degree1 > degree2 ? degree1 : degree2) + 1;

    int updateCnt1 = v1->getCnt();
    int updateCnt2 = v2->getCnt();

    int dtIndex = dtManager.get_size();
    DTInstance *curInstance = new DTInstance(
            (1-omega)*rho * rho, maxDegree, updateCnt1,
            updateCnt2, v1->id, v2->id, dtIndex);

    dtManager.insertInstance(curInstance);

    int _exp = curInstance->get_exp();
    v1->addDTBucketElement(_exp, curInstance->get_element1(), updateCnt1);
    v2->addDTBucketElement(_exp, curInstance->get_element2(), updateCnt2);
    v1->set_instance_index_map_by_neighbor_id(v2->id, dtIndex);
    v2->set_instance_index_map_by_neighbor_id(v1->id, dtIndex);
#ifdef _DEBUG_
    double end_time = getCurrentTime();
    time_GraphDynamic_insertBetweenLarge += (end_time - start_time);
#endif
    return 0;
}

int Graph::deleteBetweenLarge(dynscan::Vertex *v1,
                              dynscan::Vertex *v2) {

    checkVertexDTBucket(v1);
    checkVertexDTBucket(v2);

    return 0;
}

void Graph::checkVertexDTBucket(dynscan::Vertex *curVertex) {
    curVertex->increaseUpdateCnt();
    int VID1 = curVertex->id;
    int updateCnt = curVertex->getCnt();
    for (int i = 0; i < curVertex->listSize(); i++) {
        MyVector<DTInstance *> newRounds;
        if (curVertex->sizeByIndex(i) == 0) {
            continue;
        } else {
            //the first element in the bucket should have the smallest last count
            int c_B = curVertex->getBucketCount(i);
            int lambda_B = pow_2[i];
            int comp = floor(updateCnt / lambda_B) - floor(c_B / lambda_B);
            if (comp == 0) {
                break;
            }
            if (comp >= 1) {
                curVertex->updateBucketCount(i, updateCnt);
                for (int j = 0; j < curVertex->sizeByIndex(i); j++) {
                    DTBucketElement *bucket_element = curVertex->getDTBucketElement(i, j);
                    DTInstance *curInstance =
                            dtManager.get_instance(bucket_element->get_dt_index());
                    bucket_element->update_cnt(updateCnt);
                    curInstance->receive_report();
                    if (!curInstance->is_round_end()) {
                        // not new round

                    } else {
                        newRounds.push_back(curInstance);
                    }
                }
            }
        }
        // delete and insert the elements here
        for (int j = 0; j < newRounds.size(); j++) {
            DTInstance *curInstance = newRounds[j];
            DTBucketElement *neighbor_bucket_element = curInstance->get_element(VID1);
            DTBucketElement *bucket_element = curInstance->get_Another_Bucket_Element(neighbor_bucket_element);
            int neighborID = bucket_element->get_neighbor_id();
            auto *neighborVertex =
                    (dynscan::Vertex *) vList[neighborID - 1];
            int neighborUpdateCnt = neighborVertex->getCnt();
            //update tau since it is a new round
            int bucket_index = curInstance->get_exp();

            curVertex->DeleteElement(bucket_index, curInstance->get_element_index(neighborID));
            neighborVertex->DeleteElement(bucket_index, curInstance->get_element_index(VID1));

            curInstance->update_tau_and_slack(updateCnt, neighborUpdateCnt);
            if (!curInstance->is_mature()) {
                // new round but not mature
                bucket_index = curInstance->get_exp();
                //add to the new bucket
                curVertex->addDTBucketElement(bucket_index, bucket_element, updateCnt);
                neighbor_bucket_element->update_cnt(neighborUpdateCnt);
                neighborVertex->addDTBucketElement(bucket_index, neighbor_bucket_element, neighborUpdateCnt);

            } else {
                // mature, relabel (curVertex, neighbor)

                double new_sim_score = myJaccard->compute_similarity(
                        *curVertex, *neighborVertex);

                curVertex->updateNeighborSimScore(new_sim_score, neighborVertex->id);
                neighborVertex->updateNeighborSimScore(new_sim_score, VID1);


                // reset DT instance threshold
                int union_lower_bound = 1 + std::max(curVertex->getDegree(), neighborVertex->getDegree());
                curInstance->reset_status(rho, union_lower_bound, updateCnt, neighborUpdateCnt);


                bucket_index = curInstance->get_exp();
                //add to the new bucket
                curVertex->addDTBucketElement(bucket_index, bucket_element, updateCnt);
                DTBucketElement *el2 = curInstance->get_Another_Bucket_Element(bucket_element);
                el2->update_cnt(neighborUpdateCnt);
                neighborVertex->addDTBucketElement(bucket_index, el2, neighborUpdateCnt);
            }
        }
    }
}


int Graph::makeLarge(dynscan::Vertex *v) {
    double start = getCurrentTime();

    int vID = v->id;
    int vDegree = v->getDegree();
    int vUpdateCnt = v->getCnt();
    for (int i = 0; i < vDegree; i++) {
        int neighborID = v->getNeighborID(i);
        dynscan::Vertex *neighbor_v =
                (dynscan::Vertex *) vList[neighborID - 1];

        if (neighbor_v->isLarge()){
            // already has DT instance with large ones so skip
            continue;
        }
        int neighborDegree = neighbor_v->getDegree();
        int neighborUpdateCnt = neighbor_v->getCnt();
        int unionSize = vDegree + 3 + neighborDegree - v->getIntersectionCnt(i);

        int dtIndex = dtManager.get_size();

        DTInstance *newInstance =
                new DTInstance((1 - omega) * rho * rho,
                               unionSize, vUpdateCnt,
                               neighborUpdateCnt, v->id, neighbor_v->id, dtIndex);
        dtManager.insertInstance(newInstance);
        int _exp = newInstance->get_exp();
        v->addDTBucketElement(_exp, newInstance->get_element1(), vUpdateCnt);
        neighbor_v->addDTBucketElement(_exp, newInstance->get_element2(), neighborUpdateCnt);
        v->set_instance_index_map_by_neighbor_id(neighborID, dtIndex);
        neighbor_v->set_instance_index_map_by_neighbor_id(vID, dtIndex);

    }
    v->set_large();

    double end = getCurrentTime();
    printf("insert:*%.9lf*\n", end - start);

    return 0;
}

double Graph::query(double eps, int mu) {
    int core_num = 0;
    int m_C = 0;
    double q_time = 0;
    double q_start, q_end;
    MyVector<dynscan::Vertex *> cores;
    for (int i = 0, vertex_number = vList.size(); i < vertex_number; i++) {
        dynscan::Vertex *v = (dynscan::Vertex *) vList[i];
        if(v->getDegree() <= mu){
            continue;
        }
        q_start = getCurrentTime();
        int temp_m_C = v->query(eps, mu);
        q_end = getCurrentTime();
        q_time += q_end - q_start;
        if(temp_m_C == 0){
            continue;
        }
        else{
            core_num += 1;
            m_C += temp_m_C;
            cores.push_back(v);

        }
    }
    // BFS to get the results (pruned by epsilon)
    int vertex_number = (int) vList.size();
    int* visited = new int[vertex_number];
    for (int i = 0; i < vertex_number; ++i) {
        visited[i] = 0;
    }
    queue<dynscan::Vertex *> Q;
    for (int i = 0, core_number = cores.size(); i < core_number; i++) {
        dynscan::Vertex *v = cores[i];
        if(visited[v->id] == 1){
            continue;
        }
        Q.push(v);
        visited[v->id] = 1;
        MyVector<int> C;
        while(!Q.empty()){
            dynscan::Vertex *u = Q.front();
            Q.pop();
            for (auto rit = u->NOPtr->rbegin(); rit != u->NOPtr->rend(); ++rit) {
                if(rit->first >= eps){
                    int w = rit->second;
                    visited[w] = 1;
                    C.push_back(w);
                }
                else{
                    break;
                }
            }
        }
        //print out the cluster
//        for (int j = 0, cluster_size = C.size(); j < cluster_size; j++) {
//            printf("%d,", C[j]);
//        }
//        printf("\n");
    }
//    return C;
    return q_time;
}