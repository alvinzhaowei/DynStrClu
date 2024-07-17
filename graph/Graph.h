#ifndef DYNSCAN_GRAPH_H
#define DYNSCAN_GRAPH_H

#include "Vertex.h"
#include "Jaccard.h"
#include "../MyLib/MyVector.h"
#include <cmath>
#include <random>
#include "../dt/DTManager.h"

using namespace std;

// approximation ratio of Sampling to DT
#define omega 0.5
#define FAILURE_PROB 0.001

class Graph {
protected:
    // The parameters of StrClu clustering.
    double rho;

    // The object to manage all the DT instances.
    DTManager dtManager;

    int permutationNum;

    Jaccard *myJaccard;

    // The list of all the vertices.
    MyVector<dynscan::Vertex *> vList;

public:
    /*
     *  The constructor of Graph.
     */
    Graph(MyVector<dynscan::Vertex *> &_vList, double _rho);
    /*
 *  Insert an edge to the graph.
 */
    int insertEdge(int _vID1, int _vID2);

    /*
     *  Delete an edge from the graph.
     */
    int removeEdge(int _vID1, int _vID2);


    double query(double eps, int mu);

protected:
    /**
     * Make vertex v a large vertex
     * @param v
     * @return
     */
    int makeLarge(dynscan::Vertex *v);

    /**
     * Deal with naive insertion/deletion between small pairs with intersection counts
     * and maintain heap if needed.
     * @param v1
     * @param v2
     * @return
     */
    int insertBetweenSmall(dynscan::Vertex *v1, dynscan::Vertex *v2);

    int deleteBetweenSmall(dynscan::Vertex *v1, dynscan::Vertex *v2);

    /**
     * Deal with naive insertion/deletion between small and large pairs
     * @param v1
     * @param v2
     * @return
     */
    int insertBetweenSmallAndLarge(dynscan::Vertex *v1, dynscan::Vertex *v2);

    int deleteBetweenSmallAndLarge(dynscan::Vertex *v1, dynscan::Vertex *v2);

    /**
     * Deal with naive insertion/deletion between large pairs
     * @param v1
     * @param v2
     * @return
     */
    int insertBetweenLarge(dynscan::Vertex *v1, dynscan::Vertex *v2);

    int deleteBetweenLarge(dynscan::Vertex *v1, dynscan::Vertex *v2);


    // *****************************************
    // Functions deal with vertices and signatures
    // *****************************************

    /**
     *
     * @param _id
     * @return create new vertex with vertex factory and return its pointer.
     */
    dynscan::Vertex* createVertex(const int &_id){
        dynscan::Vertex *newVertex = new dynscan::Vertex(_id);
        vList[_id - 1] = newVertex;
        return newVertex;
    }

    // *****************************************
    // Functions to maintain dt bucket
    // *****************************************

    void checkVertexDTBucket(dynscan::Vertex *curVertex);

    void computePermutationNumber(double rho) {
        int vertex_num = vList.size();
        int max = vertex_num;
        int min = (int) ceil(1  / rho + 0.5);
        int mid = 0;
        while (min <= max) {
            mid = min + (max - min) / 2;
            if (log(rho * mid) * log(mid) + 1000 == rho * mid) {
                break;
            } else if (log(rho * mid) * log(mid) + 1000 > rho * mid) {
                min = mid + 1;
            } else {
                max = mid - 1;
            }
        }
        permutationNum = mid;
    }

};


#endif //DYNSCAN_GRAPH_H
