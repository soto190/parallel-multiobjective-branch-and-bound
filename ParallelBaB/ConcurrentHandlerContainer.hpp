//
//  HandlerContainer.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/6/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#ifndef HandlerContainer_hpp
#define HandlerContainer_hpp

#include <stdio.h>
#include "ConcurrentGridContainer.hpp"
#include "tbb/atomic.h"

class ConcurrentHandlerContainer {
    
private:
    double * rangeinx;
    double * rangeiny;
    double maxinx;
    double maxiny;
    unsigned long numberOfElements;
    
    tbb::atomic<unsigned long> activeBuckets;
    tbb::atomic<unsigned long> unexploredBuckets;
    tbb::atomic<unsigned long> disabledBuckets;
    tbb::atomic<int> min_value_found_in_obj [2]; /** TODO: Change to more objectives. Also this doesnt goes here. Fixed to two objectives **/
    
    ConcurrentGridContainer grid;
    std::vector<Solution> paretoFront;
    
public:
    ConcurrentHandlerContainer();
    ConcurrentHandlerContainer(unsigned int width, unsigned int height, double maxValX, double maxValY);
    ConcurrentHandlerContainer(const ConcurrentHandlerContainer& toCopy);
    ~ConcurrentHandlerContainer();
    
    ConcurrentHandlerContainer& operator()(unsigned int width, unsigned int height, double maxValX, double maxValY, int minValX, int minValY);
    
    bool add(const Solution & solution);
    bool set(const Solution & solution, int x, int y);
    bool improvesTheGrid(const Solution & solution);
    
    unsigned long getNumberOfActiveBuckets() const;
    unsigned long getNumberOfUnexploredBuckets() const;
    unsigned long getNumberOfDisabledBuckets() const;
    unsigned int getRows() const;
    unsigned int getCols() const;
    unsigned long getSize() const;
    unsigned long getSizeOf(int x, int y) const;
    BucketState getStateOf(int x, int y) const;
    
    void printGridSize() const;
    void printStates() const;
    void print() const;
    
    double getMaxIn(int dimension);
    int getBestValueFoundIn(int n_obj) const;
    std::vector<Solution>& getParetoFront();
    
private:
    std::vector<Solution>& get(int x, int y);
    bool improvesTheBucket(const Solution & solution, int x, int y);
    void clearContainer(int x, int y);
    void setNonDominatedState(int x, int y);
    void setDominatedState(int x, int y);
    void setUnexploredState(int x, int y);
    void getCoordinateForSolution(const Solution & solution, int * coordinate) const;
    void updateMinValueFound(const Solution& solution);
    void clearContainersDominatedBy(const int x, const int y);
};
#endif /* HandlerContainer_hpp */
