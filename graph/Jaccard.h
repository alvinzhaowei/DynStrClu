#ifndef DYNSCAN_JACCARD_H
#define DYNSCAN_JACCARD_H


#include <random>
#include "Vertex.h"

class Jaccard {
private:
    std::random_device rd;
    std::mt19937_64 gen;
    std::uniform_int_distribution<long long> dis;
    unsigned long long size_v;
    unsigned long long size_u;
    unsigned long long index;
    unsigned long long common;
    unsigned long long numSamples;
    double mean;
    double rho;


    long double one_over_failure_prob;
    long double one_over_failure_prob_current_invoke;

    static long long number_invoke;
    static long long batch_num;
    static const int batch_size = 500000;

    inline bool is_a_common_vertex(const dynscan::Vertex _v, const dynscan::Vertex _u) {
        if (index <= size_v) {
            const int v_neighbor = index < size_v ? _v.getNeighborID(index) : _v.id;
            if (_u.has_neighbor(v_neighbor)) {
                return true;
            }
        } else {
            index = index - size_v - 1;
            const int u_neighbor = index < size_u ? _u.getNeighborID(index) : _u.id;
            if (_v.has_neighbor(u_neighbor)) {
                return true;
            }
        }
        return false;
    }

    /**
     *
     * @param _var variance
     * @param _current_failure_prob failure probability allocated at current round
     * @param _num_samples
     * @return
     */
    inline double
    bernstein_interval(const double &_var, const double &_one_over_current_failure_prob,
                       const int &_num_samples) const {
        return std::sqrt((_var * log(3 * _one_over_current_failure_prob)) / _num_samples)
               + (3 * log(3 * _one_over_current_failure_prob) / _num_samples);
    }

public:


    static double jaccard_brute_force(const dynscan::Vertex *_v, const dynscan::Vertex *_u) {
        const dynscan::Vertex &v1 = _v->getDegree() < _u->getDegree() ? *_v : *_u;
        const dynscan::Vertex &v2 = _v->getDegree() < _u->getDegree() ? *_u : *_v;
        const int degree1 = v1.getDegree();
        const int degree2 = v2.getDegree();
        int intersectionCnt = 2;
        for (int i = 0; i < degree1; ++i) {
            if (v2.getAdjacentIndex(v1.getNeighborID(i)) != -1) {
                ++intersectionCnt;
            }
        }
        return intersectionCnt / (double) (degree1 + degree2 + 2 - intersectionCnt);
    }

    /**
     *
     * @param one over the _failure_probability
     * @param _similarity_threshold the epsilon defined on paper
     * @param alpha times rho defined on paper
     */
    Jaccard(long double _one_over_failure_prob, const double &_alpha_rho) :
            gen(rd()), one_over_failure_prob(_one_over_failure_prob), rho(_alpha_rho){
        size_v = 0;
        size_u = 0;
        mean = 0;
        /// oneOverFailure_probability is set to vertex_number, and the power 4 is extracted out of log
        numSamples = 2.0 / rho / rho * log(one_over_failure_prob);
    }


    inline void adjust_failure_prob_current_invoke() {
        ++number_invoke;
        batch_num = number_invoke / batch_size + 1;
        // failure prob = 1.0 / [batch_size * batch_num * (batch_num + 1)] * 1.0 / one_over_failure_prob
        one_over_failure_prob_current_invoke = batch_size * batch_num * (batch_num + 1) * one_over_failure_prob;
    }


    /**
    *
    * @param _v
    * @param _u
    * @return true if similar otherwise false
    */
    inline float compute_similarity(const dynscan::Vertex &_v, const dynscan::Vertex &_u) {

        if (!_v.isLarge()) {
            const int uid = _u.id;
            const int index = _v.getAdjacentIndex(uid);
            // get number of common neighbors between v1 and v2
            const int common = _v.getIntersectionCnt(index);
            const double jaccard = common / (double) (_v.getDegree() + _u.getDegree() + 2 - common);
            return jaccard;
        } else if (!_u.isLarge()) {
            const int vid = _v.id;
            const int index = _u.getAdjacentIndex(vid);
            // get number of common neighbors between v1 and v2
            const int common = _u.getIntersectionCnt(index);
            const double jaccard = common / (double) (_v.getDegree() + _u.getDegree() + 2 - common);
            return jaccard;
        }

        adjust_failure_prob_current_invoke();
        numSamples = 2.0 / rho / rho  *
                     log(one_over_failure_prob_current_invoke);
        size_v = _v.getDegree();
        size_u = _u.getDegree();
        common = 0;
        dis.param(std::uniform_int_distribution<long long>::param_type(0, size_v + size_u + 1));
        for (unsigned long long i = 0; i < numSamples; ++i) {
            index = dis(gen);
            if (is_a_common_vertex(_v, _u)) {
                ++common;
            }
        }
        mean = common / (double) numSamples;
        mean = mean / (2 - mean);
        return mean;
    }
};


#endif //DYNSCAN_JACCARD_H
