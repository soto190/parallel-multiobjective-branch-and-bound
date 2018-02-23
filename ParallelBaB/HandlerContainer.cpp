//
//  HandlerContainer.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/6/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "HandlerContainer.hpp"

HandlerContainer::HandlerContainer():
rangeinx(nullptr),
rangeiny(nullptr),
maxinx(0),
maxiny(0),
numberOfElements(0),
grid(0,0) {

};

HandlerContainer::HandlerContainer(const HandlerContainer& toCopy):
maxinx(toCopy.maxinx),
maxiny(toCopy.maxiny),
numberOfElements(toCopy.numberOfElements),
activeBuckets(toCopy.getNumberOfActiveBuckets()),
unexploredBuckets(toCopy.getNumberOfUnexploredBuckets()),
disabledBuckets(toCopy.getNumberOfDisabledBuckets()),
grid(toCopy.grid) {
    
    rangeinx = new double[toCopy.getCols()];
    rangeiny = new double[toCopy.getRows()];
    
    for (int indexy = 0; indexy < toCopy.getRows(); indexy++)
        rangeiny[indexy] = toCopy.rangeiny[indexy];
    
    for (int indexx = 0; indexx < toCopy.getCols(); indexx++)
        rangeinx[indexx] = toCopy.rangeinx[indexx];
    
    for (int obj = 0; obj < 2; ++obj)
        min_value_found_in_obj[obj].fetch_and_store(toCopy.getBestValueFoundIn(obj));
}

HandlerContainer::HandlerContainer(unsigned int rows, unsigned int cols, double maxValX, double maxValY):
grid(cols, rows) {
    //    grid(maxValX < cols?maxValX:cols, maxValY < rows?maxValY:rows) {
    /*
     if (maxValX < cols)
     cols = maxValX;
     if (maxValY < rows)
     rows = maxValY;
     */
    numberOfElements = 0;
    unexploredBuckets = rows * cols;
    activeBuckets = 0;
    disabledBuckets = 0;
    
    rangeinx = new double[cols];
    rangeiny = new double[rows];
    maxinx = maxValX;
    maxiny = maxValY;
    int divs = 0;
    
    double rx = maxValX / cols;
    double ry = maxValY / rows;
    
    rangeinx[divs] = 0;
    rangeiny[divs] = 0;
    
    for (divs = 1; divs < cols; divs++)
        rangeinx[divs] = rangeinx[divs - 1] + rx;
    
    for (divs = 1; divs < rows; divs++)
        rangeiny[divs] = rangeiny[divs - 1] + ry;
    
    for (int obj = 0; obj < 2; ++obj)
        min_value_found_in_obj[obj].fetch_and_store(BIG_VALUE);
}

HandlerContainer::~HandlerContainer() {
    delete[] rangeinx;
    delete[] rangeiny;
    paretoFront.clear();
}

HandlerContainer& HandlerContainer::operator()(unsigned int rows, unsigned int cols, double maxValX, double maxValY, int minValX, int minValY) {
    /*
     if (maxValX < cols)
     cols = maxValX;
     if (maxValY < rows)
     rows = maxValY;
     */
    grid(cols, rows);
    
    //grid(maxValX < cols?maxValX:cols, maxValY < rows?maxValY:rows);
    
    numberOfElements = 0;
    unexploredBuckets = rows * cols;
    activeBuckets = 0;
    disabledBuckets = 0;
    
    if(rangeinx != nullptr)
        delete rangeinx;
    if(rangeiny != nullptr)
        delete rangeiny;
    
    paretoFront.clear();
    
    rangeinx = new double[cols];
    rangeiny = new double[rows];
    maxinx = maxValX;
    maxiny = maxValY;
    int divs = 0;
    
    double rx = maxValX / cols;
    double ry = maxValY / rows;
    
    rangeinx[divs] = minValX;
    rangeiny[divs] = minValY;
    
    for (divs = 1; divs < cols; ++divs)
        rangeinx[divs] = rangeinx[divs - 1] + rx;
    
    for (divs = 1; divs < rows; ++divs)
        rangeiny[divs] = rangeiny[divs - 1] + ry;
    
    for (int obj = 0; obj < 2; ++obj)
        min_value_found_in_obj[obj].fetch_and_store(BIG_VALUE);
    
    return *this;
}

void HandlerContainer::getCoordinateForSolution(const Solution &solution, int * coordinate) const {
    coordinate[0] = binarySearch(solution.getObjective(0), rangeinx, getCols());
    coordinate[1] = binarySearch(solution.getObjective(1), rangeiny, getRows());
}

/**
 *
 * Sets a copy of the solution.
 * This could requiere a Mutex.
 * Returns 1 if the solution was added 0 in other case.
 *
 */
bool HandlerContainer::set(const Solution & solution, int x, int y) {
    bool updated = false;
    BucketState state = grid.getStateOf(x, y);
    switch (state) {
        case BucketState::Unexplored:
            clearContainersDominatedBy(x, y);
            grid.addTo(solution, x, y);
            grid.setNonDominatedState(x, y);
            activeBuckets.fetch_and_increment();
            unexploredBuckets.fetch_and_decrement();
            updated = true;
            break;
            
        case BucketState::NonDominated:
            updated = grid.addTo(solution, x, y);
            break;
            
        case BucketState::dominated:
            updated = false;
            /** If the bucket is dominated (State = 2) the element is not added. Then do nothing**/
            break;
    }
    
    if(updated)
        updateMinValueFound(solution);
    
    return updated;
}

void HandlerContainer::clearContainersDominatedBy(const int x, const int y) {
    for (int nRow = y + 1; nRow < grid.getNumberOfRows(); nRow++)
        for (int nCol = x + 1; nCol < grid.getNumberOfCols(); nCol++)
            if (grid.getStateOf(nCol, nRow) == BucketState::dominated)
                nCol = grid.getNumberOfCols(); /** If the bucket in (nCol, nRow) is dominated then the exploration continues in next row. **/
            else
                clearContainer(nCol, nRow);
}

void HandlerContainer::updateMinValueFound(const Solution &solution) {
    for (int objective = 0; objective < 2; ++objective)
        if (solution.getObjective(objective) < min_value_found_in_obj[objective])
            min_value_found_in_obj[objective].fetch_and_store(solution.getObjective(objective));
}

int HandlerContainer::getBestValueFoundIn(int obj) const {
    return min_value_found_in_obj[obj];
}

/**
 * It uses a binary search tree to locate the bucket which will contain the new solution.
 **/
bool HandlerContainer::add(const Solution & solution) {
    int coordinate[2];
    getCoordinateForSolution(solution, coordinate);
    return set(solution, coordinate[0], coordinate[1]);
}

void HandlerContainer::clearContainer(int x, int y) {
    if (grid.getSizeOf(x, y) > 0)
        activeBuckets.fetch_and_decrement();
    
    disabledBuckets.fetch_and_increment();
    grid.clear(x, y);
}

/**
 * There are three states in gridState/BucketState:
 * 0: No explored.
 * 1: non-Dominated (Pareto front).
 * 2: Dominated.
 */
BucketState HandlerContainer::getStateOf(int x, int y) const {
    return grid.getStateOf(x, y);
}

std::vector<Solution>& HandlerContainer::get(int x, int y) {
    return grid.get(x, y);
}

unsigned int HandlerContainer::getRows() const {
    return grid.getNumberOfRows();
}

unsigned int HandlerContainer::getCols() const {
    return grid.getNumberOfCols();
}

unsigned long HandlerContainer::getSize() const {
    return grid.getSize();
}

unsigned long HandlerContainer::getSizeOf(int x, int y) const {
    return grid.getSizeOf(x, y);
}

unsigned long HandlerContainer::getNumberOfActiveBuckets() const {
    return activeBuckets;
}

unsigned long HandlerContainer::getNumberOfUnexploredBuckets() const {
    return unexploredBuckets;
}

unsigned long HandlerContainer::getNumberOfDisabledBuckets() const {
    return disabledBuckets;
}

void HandlerContainer::setNonDominatedState(int x, int y) {
    grid.setNonDominatedState(x, y);
}

void HandlerContainer::setDominatedState(int x, int y) {
    grid.setDominatedState(x, y);
}

void HandlerContainer::setUnexploredState(int x, int y) {
    grid.setUnexploredState(x, y);
}

void HandlerContainer::print() const {
    grid.print();
}

void HandlerContainer::printGridSize() const {
    for (int nRow = grid.getNumberOfRows() - 1; nRow >= 0; --nRow) {
        printf("[%3d] ", nRow);
        for (int nCol = 0; nCol < grid.getNumberOfCols(); ++nCol)
            if(grid.getStateOf(nCol, nRow) == BucketState::Unexplored)
                printf(" - ");
            else
                printf("%3ld", grid.getSizeOf(nCol, nRow));
        printf("\n");
    }
}

void HandlerContainer::printStates() const {
    BucketState state;
    for (int nRow = grid.getNumberOfRows() - 1; nRow >= 0; --nRow) {
        printf("[%3d] ", nRow);
        for (int nCol = 0; nCol < grid.getNumberOfCols(); ++nCol) {
            state = grid.getStateOf(nCol, nRow);
            if(state == BucketState::Unexplored)
                printf(" - ");
            else
                printf("%3d", state);
        }
        printf("\n");
    }
}

/**
 * Stores a copy of the received solution.
 */
bool HandlerContainer::improvesTheGrid(const Solution &solution) {
    int bucketCoordinate[2];
    bool improves = false;
    getCoordinateForSolution(solution, bucketCoordinate);
    BucketState stateOfBucket = grid.getStateOf(bucketCoordinate[0], bucketCoordinate[1]);
    switch (stateOfBucket) {
        case BucketState::dominated:
            improves = false;
            break;
            
        case BucketState::Unexplored:
            improves = true;
            break;
            
        case BucketState::NonDominated:
            improves = grid.produceImprovementInBucket(solution, bucketCoordinate[0], bucketCoordinate[1]);
            break;
    }
    return improves;
}

bool HandlerContainer::improvesTheBucket(const Solution &solution, int x, int y) {
    return grid.produceImprovementInBucket(solution, x, y);
}

std::vector<Solution>& HandlerContainer::getParetoFront() {
    paretoFront.reserve(getSize());
    for (int bucketY = 0; bucketY < getRows(); ++bucketY)
        for (int bucketX = 0; bucketX < getCols(); ++bucketX) {
            BucketState state = grid.getStateOf(bucketX, bucketY);
            if (state == BucketState::NonDominated) {
                std::vector<Solution> vec = grid.get(bucketX, bucketY);
                std::vector<Solution>::iterator it = vec.begin();
                
                for (it = vec.begin(); it != vec.end(); ++it)
                    paretoFront.push_back(*it);
                
            } else if (state == BucketState::dominated)
                bucketX = grid.getNumberOfCols();
        }
    
    extractParetoFront(paretoFront);
    return paretoFront;
}

double HandlerContainer::getMaxIn(int dimension) {
    if (dimension == 0)
        return maxinx;
    else if (dimension == 1)
        return maxiny;
    else
        return -1;
}
