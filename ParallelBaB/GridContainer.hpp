//
//  GridContainer.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 2/27/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef GridContainer_hpp
#define GridContainer_hpp

#include <stdio.h>
#include <vector>
#include <array>
#include "Dominance.hpp"
#include "myutils.hpp"
#include "ParetoFront.hpp"
#include "ParetoBucket.hpp"

/**
 * An example of the Grid vectorization:
 *  https://stackoverflow.com/questions/14015556/how-to-map-the-indexes-of-a-matrix-to-a-1-dimensional-array-c
 **/

class GridContainer {

private:
    unsigned int cols;
    unsigned int rows;
    unsigned long numberOfElements;

    std::vector<ParetoFront> pareto_buckets;
    ParetoFront pareto_front;
    
    size_t getIndexPosition(size_t x, size_t y) const;

public:
    GridContainer(unsigned int width, unsigned int height);
    GridContainer(const GridContainer& toCopy);
    ~GridContainer();

    GridContainer& operator()(unsigned int width, unsigned int height);

    bool addTo(const Solution& obj, size_t x, size_t y);
    void setNonDominatedState(size_t x, size_t y);
    void setDominatedState(size_t x, size_t y);
    void setUnexploredState(size_t x, size_t y);

    const ParetoFront& getParetoFrontAt(size_t x, size_t y) const;
    const std::vector<ParetoFront> getParetoBuckets() const;
    unsigned int getNumberOfCols() const;
    unsigned int getNumberOfRows() const;
    unsigned long getSize() const;
    unsigned long getSizeOf(size_t x, size_t y) const;
    FrontState getStateOf(size_t x, size_t y) const;
    bool produceImprovementInBucket(const Solution& obj, size_t x, size_t y);
    void clearAll();
    unsigned long clear(size_t x, size_t y) ;
    void print() const;
};
#endif /* GridContainer_hpp */
