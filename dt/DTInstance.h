#ifndef DYNSCAN_DTINSTANCE_H
#define DYNSCAN_DTINSTANCE_H

#include "DTBucket.h"
#include <cstdio>
#include <math.h>


/*
 *  The class of DT instance for an edge.
 */
class DTInstance {
protected:
    // one DT instance will be related to two different heaps, for example DT(v1, v2) will be both in heap related to v1 and heap related to v2
    DTBucketElement element1;
    DTBucketElement element2;
    // The DT tau. We set tau to negative if the instance receives one report.
    int tau;
    int init_tau;

    int lambda;
    int _exp;
    /**
     *  This is the sum of the counters from the two vertices,
     *  when the current round starts.
     */
    int initialCntSum;

    int roundEndCnt;

    // The number of message received in the current round.
    // Since the value of msgCnt can only be either 0 or 1, actually, we can
    // save this space and use the sign of the entry index to record it.
    int msgCnt;

private:
    inline void set_CntSum(const int &_initialCntSum) {
        initialCntSum = _initialCntSum;
    }

public:
    DTBucketElement *get_element1() { return &element1; }

    DTBucketElement *get_element2() { return &element2; }

//    inline int get_my_bucket_index(const int &_neighborID) const {
//        return (_neighborID == element1.neighborID ? element1 : element2)
//                .bucket_index;
//    }

    inline int get_element_index(const int &_neighborID) const {
        return (_neighborID == element1.neighborID ? element1 : element2)
                .element_index;
    }

    inline DTBucketElement* get_element(const int &_neighborID) {
        return _neighborID == element1.neighborID ? &element1 : &element2;
    }

    inline DTBucketElement *
    get_Another_Bucket_Element(DTBucketElement* _bucketElement) {
        return _bucketElement == &element1 ? &element2 : &element1;
    }

    inline const int &get_exp() const { return _exp; };

    inline const int &get_slack() const { return lambda; };

    inline const int &get_tau() const { return tau; };

    inline void receive_report() { ++msgCnt; }

//    inline bool is_mature() const { return tau == 0; }

    inline bool is_mature() const { return tau <= init_tau / 10; }

    inline bool is_round_end() const { return msgCnt == roundEndCnt; }

    // pow_2_slack
    inline void update_tau_and_slack(const int &_uptCnt1, const int &_uptCnt2) {
        tau = tau - (_uptCnt1 + _uptCnt2 - initialCntSum);
        initialCntSum = _uptCnt1 + _uptCnt2;
        if (tau >= 16) {
            roundEndCnt = 2;
            msgCnt = 0;
            _exp = int(floor(log(tau / 4.0) / log(2)));
            lambda = pow_2[_exp];
        } else if (tau > 0) {
            roundEndCnt = 1;
            msgCnt = 0;
            _exp = 0;
            lambda = 1;
        } else {
            lambda = 0;
            roundEndCnt = 0;
            _exp = 0;
            msgCnt = 0;
        }
    }

    inline DTInstance(const double &_rho,
                      const int &_union_size_lower_bound,
                      const int &_uptCnt1, const int &_uptCnt2,
                      const int &_vid1, const int &_vid2,
                      const int &_dtIndex) {
        reset_status(_rho, _union_size_lower_bound, _uptCnt1, _uptCnt2);

        element1.cnt = _uptCnt1;
        element1.neighborID = _vid2;
        element1.dtIndex = _dtIndex;
        element2.cnt = _uptCnt2;
        element2.neighborID = _vid1;
        element2.dtIndex = _dtIndex;
    }

    inline void reset_status(const double &_rho,
                             const int &_union_size_lower_bound,
                             const int &_uptCnt1, const int &_uptCnt2) {
        tau = floor(_rho * _union_size_lower_bound / 2) + 1;
        init_tau = tau;
        set_CntSum(_uptCnt1 + _uptCnt2);
        update_tau_and_slack(_uptCnt1, _uptCnt2);
    }
};

#endif // DYNSCAN_DTINSTANCE_H
