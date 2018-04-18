//
//  HandlerContainer3D.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 4/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef HandlerContainer3D_hpp
#define HandlerContainer3D_hpp

#include <stdio.h>
#include <stdio.h>
#include "GridContainer3D.hpp"
#include "ParetoFront.hpp"
#include "tbb/atomic.h"

#define DIMENSIONS 3

class HandlerContainer3D {

private:
    double * rangeinx;
    double * rangeiny;
    double * rangeinz;

    double maxinx;
    double maxiny;
    double maxinz;
    unsigned long numberOfElements;

    unsigned long activeBuckets;
    unsigned long unexploredBuckets;
    unsigned long disabledBuckets;
    double min_value_found_in_obj [DIMENSIONS]; /** TODO: Change to more objectives. Also this doesnt goes here. Fixed to two objectives **/

    GridContainer3D grid;
    ParetoFront pareto_front;

public:
    HandlerContainer3D();
    HandlerContainer3D(unsigned int width, unsigned int height, unsigned int deep_n, double maxValX, double maxValY, double maxValZ);
    HandlerContainer3D(const HandlerContainer3D& toCopy);
    ~HandlerContainer3D();

    HandlerContainer3D& operator()(unsigned int width, unsigned int height, unsigned int deep_n, double maxValX, double maxValY, double maxValZ, double minValX, double minValY, double minValZ);

    bool add(const Solution & solution);
    bool set(const Solution & solution, int x, int y, int z);
    bool improvesTheGrid(const Solution & solution);

    unsigned long getNumberOfActiveBuckets() const;
    unsigned long getNumberOfUnexploredBuckets() const;
    unsigned long getNumberOfDisabledBuckets() const;
    unsigned int getRows() const;
    unsigned int getCols() const;
    unsigned int getDeep() const;
    unsigned int getMaxRangeInX() const;
    unsigned int getMaxRangeInY() const;
    unsigned int getMaxRangeInZ() const;
    unsigned long getNumberOfElements() const;
    unsigned long getSizeOf(int x, int y, int z) const;
    FrontState getStateOf(int x, int y, int z) const;

    void printGridSize() const;
    void printStates() const;
    void print() const;

    double getMaxIn(int dimension);
    double getBestValueFoundIn(int n_obj) const;
    void clear();
    const ParetoFront& generateParetoFront();
    const ParetoFront& getParetoFront() const;
    const GridContainer3D& getGrid() const;

private:
    const ParetoFront& get(int x, int y, int z) const;
    bool improvesTheBucket(const Solution & solution, int x, int y, int z);
    void clearContainer(int x, int y, int z);
    void setNonDominatedState(int x, int y, int z);
    void setDominatedState(int x, int y, int z);
    void setUnexploredState(int x, int y, int z);
    void getCoordinateForSolution(const Solution & solution, int * coordinate) const;
    void updateMinValueFound(const Solution& solution);
    void clearContainersDominatedBy(const int x, const int y, int z);
};
#endif /* HandlerContainer3D_hpp */
