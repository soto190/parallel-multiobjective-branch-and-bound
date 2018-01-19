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
#include "GridContainer.hpp"
#include "tbb/atomic.h"

class HandlerContainer {
private:
    double * rangeinx;
    double * rangeiny;
    double * rangeinz;
    double maxinx;
    double maxiny;
    double maxinz;
    
    unsigned long numberOfElements;
    
    tbb::atomic<unsigned long> activeBuckets;
    tbb::atomic<unsigned long> unexploredBuckets;
    tbb::atomic<unsigned long> disabledBuckets;
    tbb::atomic<int> min_value_found_in_obj [3]; /** TODO: Change to more objectives. Also this doesnt goes here. Fixed to two objectives **/
    
    GridContainer grid;
    std::vector<Solution> paretoFront;
    
public:
    HandlerContainer();
    HandlerContainer(unsigned int width, unsigned int height, double maxValX, double maxValY);
    HandlerContainer(unsigned int width, unsigned int height, unsigned int deep, double maxValX, double maxValY, double  maxValZ);

    HandlerContainer(const HandlerContainer& toCopy);
    ~HandlerContainer();
    
    HandlerContainer& operator()(unsigned int width, unsigned int height, unsigned int deep, double maxValX, double maxValY, double maxValZ, double minValX, double minValY, double minValZ);
    
    int add(const Solution & solution);
    int set(const Solution & solution, int x, int y, int z);
    int isImprovingTheGrid(const Solution & solution);
    
    unsigned long getNumberOfActiveBuckets() const;
    unsigned long getNumberOfUnexploredBuckets() const;
    unsigned long getNumberOfDisabledBuckets() const;
    unsigned int getRows() const;
    unsigned int getCols() const;
    unsigned int getDeep() const;
    unsigned long getSize() const;
    unsigned long getSizeOf(int x, int y, int z) const;
    BucketState getStateOf(int x, int y, int z) const;
    
    void printGridSize() const;
    void printStates() const;
    void print() const;
    
    double getMaxIn(int dimension);
    int getBestValueFoundIn(int n_obj) const;
    std::vector<Solution>& getParetoFront();
    
private:
    std::vector<Solution>& get(int x, int y, int z);
    int improvesTheBucket(const Solution & solution, int x, int y, int z);
    void clearContainer(int x, int y, int z);
    void setNonDominatedState(int x, int y, int z);
    void setDominatedState(int x, int y, int z);
    void setUnexploredState(int x, int y, int z);
    void getCoordinateForSolution(const Solution & solution, int * coordinate) const;
    void updateMinValueFound(const Solution& solution);
    void clearContainersDominatedBy(const int x, const int y, int z);
};
#endif /* HandlerContainer_hpp */
