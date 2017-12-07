//
//  ParetoBucket.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/6/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#ifndef ParetoBucket_hpp
#define ParetoBucket_hpp

#include <stdio.h>
#include "Solution.hpp"
#include "Dominance.hpp"
#include "myutils.hpp"
#include "tbb/mutex.h"
#include "tbb/spin_rw_mutex.h"
#include "tbb/queuing_rw_mutex.h"
#include "tbb/concurrent_vector.h"

enum BucketState {
    unexplored = 0, nondominated = 1, dominated = 2
};

typedef tbb::queuing_rw_mutex Lock;

/** Wrapping a vector in a concurrent structure **/
//template<class T>
class ParetoBucket{
    
private:
    unsigned long posx;
    unsigned long posy;
    Lock improving_lock;
    tbb::atomic<unsigned long> size;
    tbb::atomic<BucketState> state;
    std::vector<Solution> m_vec;
    
public:
    ParetoBucket();
    ParetoBucket(unsigned long posx, unsigned long posy);
    ParetoBucket(const ParetoBucket& toCopy);
    ~ParetoBucket();
    
    void setPosX(unsigned long new_posx);
    void setPosY(unsigned long new_posy);
    void setPositionXY(unsigned long new_posx, unsigned long new_posy);
    void setState(BucketState new_state);
    void setUnexplored();
    void setNonDominated();
    void setDominated();
    unsigned long getPosx() const;
    unsigned long getPosy() const;
    unsigned long getSize() const;
    BucketState getState() const;
    tbb::atomic<unsigned long> getSizeAtomic() const;
    tbb::atomic<BucketState> getStateAtomic() const;
    std::vector<Solution>& getVector() ;
    const std::vector<Solution>& getVectorToCopy() const ;
    int produceImprovement(const Solution& obj);
    int push_back(const Solution& obj);
    void clear();
    void print() const;
    
    /*
     //Join two buckets;
     Bucket<T> operator+(const Bucket<T>& bucket){
     Bucket<T> new_bucket;
     new_bucket.m_vec = bucket.m_vec.clone();
     new_bucket._vec.push_back(m_vec);
     return new_bucket;
     };
     */
};

#endif /* ParetoBucket_hpp */
