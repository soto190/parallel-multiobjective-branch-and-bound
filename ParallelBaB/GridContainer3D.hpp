//
//  GridContainer3D.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 4/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef GridContainer3D_hpp
#define GridContainer3D_hpp

#include <stdio.h>
#include <vector>
#include <array>
#include "myutils.hpp"
#include "ParetoFront.hpp"
#include "ParetoBucket.hpp"
/**
 *
 * The GridContainer follows a 'row-major' order.
 *
 * An example of the Grid vectorization and 'row-major' order:
 *  https://stackoverflow.com/questions/14015556/how-to-map-the-indexes-of-a-matrix-to-a-1-dimensional-array-c
 **/
#define BIG_VALUE 999999999

class GridContainer3D {

private:
    unsigned int cols;
    unsigned int rows;
    unsigned int deep;

    unsigned long number_of_elements;
    unsigned long active_buckets;
    unsigned long unexplore_buckets;
    unsigned long disabled_buckets;

    std::vector<ParetoFront> pareto_buckets;
    ParetoFront pareto_front;

    size_t getIndexPosition(size_t x, size_t y, size_t z) const;

public:
    GridContainer3D(unsigned int width, unsigned int height, unsigned int deep);
    GridContainer3D(const GridContainer3D& toCopy);
    ~GridContainer3D();

    GridContainer3D& operator()(unsigned int width, unsigned int height, unsigned int deep);

    bool addTo(const Solution& obj, size_t x, size_t y, size_t z);
    void setNonDominatedState(size_t x, size_t y, size_t z);
    void setDominatedState(size_t x, size_t y, size_t z);
    void setUnexploredState(size_t x, size_t y, size_t z);

    const ParetoFront& getParetoFrontAt(size_t x, size_t y, size_t z) const;
    const std::vector<ParetoFront> getParetoBuckets() const;
    unsigned int getNumberOfCols() const;
    unsigned int getNumberOfRows() const;
    unsigned int getNumberOfDeep() const;
    unsigned long getNumberOfBuckets() const;
    unsigned long getNumberOfElements() const;
    unsigned long getSizeOf(size_t x, size_t y, size_t z) const;
    FrontState getStateOf(size_t x, size_t y, size_t z) const;
    bool produceImprovementInBucket(const Solution& obj, size_t x, size_t y, size_t z);
    void clearAll();
    unsigned long clear(size_t x, size_t y, size_t z);
    void print() const;
};
#endif /* GridContainer3D_hpp */
