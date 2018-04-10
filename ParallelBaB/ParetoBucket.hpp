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
#include "myutils.hpp"
#include "ParetoFront.hpp"
#include "tbb/atomic.h"
#include "tbb/queuing_rw_mutex.h"

typedef tbb::queuing_rw_mutex Lock;

/** Wrapping a vector in a concurrent structure **/
//template<class T>
class ParetoBucket {
    
private:
    unsigned long version_update;
    unsigned long posx;
    unsigned long posy;
    Lock updating_lock;
    tbb::atomic<unsigned long> size;
    tbb::atomic<FrontState> state;
    std::vector<Solution> m_vec;
    
public:
    ParetoBucket();
    ParetoBucket(unsigned long posx, unsigned long posy);
    ParetoBucket(const ParetoBucket& toCopy);
    ~ParetoBucket();
    
    void setPosX(unsigned long new_posx);
    void setPosY(unsigned long new_posy);
    void setPositionXY(unsigned long new_posx, unsigned long new_posy);
    void setUnexplored();
    void setNonDominated();
    void setDominated();
    unsigned long getPosx() const;
    unsigned long getPosy() const;
    unsigned long getSize() const;
    FrontState getState() const;
    tbb::atomic<unsigned long> getSizeAtomic() const;
    tbb::atomic<FrontState> getStateAtomic() const;
    std::vector<Solution>& getVector();
    const std::vector<Solution>& getVectorToCopy() const ;
    bool produceImprovement(const Solution& obj);
    bool push_back(const Solution& obj);
    unsigned long getVersionUpdate() const;
    void clear();
    void print() const;
};
#endif /* ParetoBucket_hpp */
