//
//  Grid.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 22/09/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#ifndef Grid_hpp
#define Grid_hpp

#include <stdio.h>
#include <vector>
#include <array>
#include "Solution.hpp"
#include "Dominance.hpp"
#include "myutils.hpp"
#include "ParetoBucket.hpp"
#include "tbb/atomic.h"
#include "tbb/concurrent_vector.h"

/**
 * An example of the Grid vectorization:
 *  https://stackoverflow.com/questions/14015556/how-to-map-the-indexes-of-a-matrix-to-a-1-dimensional-array-c
 **/
#define BIG_VALUE 999999999

class GridContainer {
private:
    unsigned int cols;
    unsigned int rows;
    tbb::atomic<unsigned long> numberOfElements;
    tbb::concurrent_vector<ParetoBucket> m_Data;
    
    size_t getIndexPosition(size_t x, size_t y) const;
public:
    GridContainer(unsigned int width, unsigned int height);
    GridContainer(const GridContainer& toCopy);
    ~GridContainer();
    
    GridContainer& operator()(unsigned int width, unsigned int height);
    int addTo(const Solution& obj, size_t x, size_t y);
    void setNonDominatedState(size_t x, size_t y);
    void setDominatedState(size_t x, size_t y);
    void setUnexploredState(size_t x, size_t y);
    std::vector<Solution>& get(size_t x, size_t y);
    unsigned int getNumberOfCols() const;
    unsigned int getNumberOfRows() const;
    unsigned long getSize() const;
    tbb::atomic<unsigned long> getSizeAtomic() const;
    BucketState getStateOf(size_t x, size_t y) const;
    unsigned long getSizeOf(size_t x, size_t y) const;
    int produceImprovementInBucket(const Solution& obj, size_t x, size_t y);
    unsigned long clear(size_t x, size_t y) ;
    void print() const;
};
#endif /* Grid_hpp */
