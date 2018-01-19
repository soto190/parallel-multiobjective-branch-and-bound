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
rangeinz(nullptr),
maxinx(0),
maxiny(0),
numberOfElements(0),
grid(1, 1, 1){

};

HandlerContainer::HandlerContainer(const HandlerContainer& toCopy):
maxinx(toCopy.maxinx),
maxiny(toCopy.maxiny),
maxinz(toCopy.maxinz),
numberOfElements(toCopy.numberOfElements),
activeBuckets(toCopy.getNumberOfActiveBuckets()),
unexploredBuckets(toCopy.getNumberOfUnexploredBuckets()),
disabledBuckets(toCopy.getNumberOfDisabledBuckets()),
grid(toCopy.grid){
    
    rangeinx = new double[toCopy.getCols()];
    rangeiny = new double[toCopy.getRows()];
    rangeinz = new double[toCopy.getDeep()];

    for (int indexy = 0; indexy < toCopy.getRows(); ++indexy)
        rangeiny[indexy] = toCopy.rangeiny[indexy];
    
    for (int indexx = 0; indexx < toCopy.getCols(); ++indexx)
        rangeinx[indexx] = toCopy.rangeinx[indexx];
    
    for (int indexz = 0; indexz < toCopy.getDeep(); ++indexz)
        rangeinz[indexz] = toCopy.rangeinz[indexz];
    
    for (int obj = 0; obj < 3; ++obj)
        min_value_found_in_obj[obj].fetch_and_store(toCopy.getBestValueFoundIn(obj));
}

HandlerContainer::HandlerContainer(unsigned int rows, unsigned int cols, double maxValX, double maxValY):
grid(cols, rows, 1){
    //    grid(maxValX < cols?maxValX:cols, maxValY < rows?maxValY:rows) {
    /*
     if (maxValX < cols)
     cols = maxValX;
     if (maxValY < rows)
     rows = maxValY;
     */
    double maxValZ = 1;
    unsigned int deep = grid.getNumberOfDeep();
    numberOfElements = 0;
    unexploredBuckets = rows * cols * deep;
    activeBuckets = 0;
    disabledBuckets = 0;
    
    rangeinx = new double[cols];
    rangeiny = new double[rows];
    rangeinz = new double[getDeep()];

    maxinx = maxValX;
    maxiny = maxValY;
    maxinz = 1;

    int divs = 0;
    
    double rx = maxValX / cols;
    double ry = maxValY / rows;
    double rz = maxValZ / deep;

    rangeinx[divs] = 0;
    rangeiny[divs] = 0;
    rangeinz[divs] = 0;
    
    for (divs = 1; divs < cols; ++divs)
        rangeinx[divs] = rangeinx[divs - 1] + rx;
    
    for (divs = 1; divs < rows; ++divs)
        rangeiny[divs] = rangeiny[divs - 1] + ry;
    
    for (divs = 1; divs < deep; ++divs)
        rangeinz[divs] = rangeinz[divs - 1] + rz;
    
    for (int obj = 0; obj < 3; ++obj)
        min_value_found_in_obj[obj].fetch_and_store(BIG_VALUE);
}

HandlerContainer::~HandlerContainer() {
    delete[] rangeinx;
    delete[] rangeiny;
    delete[] rangeinz;
    paretoFront.clear();
}

HandlerContainer& HandlerContainer::operator()(unsigned int cols, unsigned int rows, unsigned int deep, double maxValX, double maxValY, double maxValZ, double minValX, double minValY, double minValZ){
    /*
     if (maxValX < cols)
     cols = maxValX;
     if (maxValY < rows)
     rows = maxValY;
     */
    if(deep == 0) /** Third dimension can not be 0. **/
        deep = 1;
    grid(cols, rows, deep);
    
    //grid(maxValX < cols?maxValX:cols, maxValY < rows?maxValY:rows);
    
    numberOfElements = 0;
    unexploredBuckets = rows * cols * deep;
    activeBuckets = 0;
    disabledBuckets = 0;
    
    if(rangeinx != nullptr)
        delete rangeinx;
    if(rangeiny != nullptr)
        delete rangeiny;
    if(rangeinz != nullptr)
        delete rangeinz;
    
    paretoFront.clear();
    
    rangeinx = new double[cols];
    rangeiny = new double[rows];
    rangeinz = new double[deep];

    maxinx = maxValX;
    maxiny = maxValY;
    maxinz = maxValZ;

    int divs = 0;
    
    double rx = maxValX / cols;
    double ry = maxValY / rows;
    double rz = maxValZ / deep;

    rangeinx[divs] = minValX;
    rangeiny[divs] = minValY;
    rangeinz[divs] = minValZ;

    for (divs = 1; divs < cols; ++divs)
        rangeinx[divs] = rangeinx[divs - 1] + rx;
    
    for (divs = 1; divs < rows; ++divs)
        rangeiny[divs] = rangeiny[divs - 1] + ry;
    
    for (divs = 1; divs < deep; ++divs)
        rangeinz[deep] = rangeinz[divs - 1] + rz;
    
    for (int obj = 0; obj < 3; ++obj)
        min_value_found_in_obj[obj].fetch_and_store(BIG_VALUE);
    
    return *this;
}

void HandlerContainer::getCoordinateForSolution(const Solution &solution, int * coordinate) const {
    coordinate[0] = binarySearch(solution.getObjective(0), rangeinx, getCols());
    coordinate[1] = binarySearch(solution.getObjective(1), rangeiny, getRows());
    coordinate[2] = binarySearch(solution.getObjective(2), rangeinz, getDeep());
}

/**
 *
 * Sets a copy of the solution.
 * This could requiere a Mutex.
 * Returns 1 if the solution was added 0 in other case.
 *
 */
int HandlerContainer::set(const Solution & solution, int x, int y, int z) {
    int updated = 0;
    BucketState state = grid.getStateOf(x, y, z);
    switch (state) {
        case BucketState::Unexplored:
            clearContainersDominatedBy(x, y, z);
            grid.addTo(solution, x, y, z);
            grid.setNonDominatedState(x, y, z);
            activeBuckets.fetch_and_increment();
            unexploredBuckets.fetch_and_decrement();
            updated = 1;
            break;
            
        case BucketState::NonDominated:
            updated = grid.addTo(solution, x, y, z);
            break;
            
        case BucketState::dominated:
            updated = 0;
            /** If the bucket is dominated (State = 2) the element is not added. Then do nothing**/
            break;
    }
    
    if(updated)
        updateMinValueFound(solution);
    
    return updated;
}

void HandlerContainer::clearContainersDominatedBy(const int x, const int y, const int z){
    for (int nRow = y + 1; nRow < grid.getNumberOfRows(); nRow++)
        for (int nCol = x + 1; nCol < grid.getNumberOfCols(); nCol++)
            for (int nDeep = z + 1; nCol < grid.getNumberOfDeep(); nDeep++)
                if (grid.getStateOf(nCol, nRow, nDeep) == BucketState::dominated)
                    nCol = grid.getNumberOfCols(); /** If the bucket in (nCol, nRow) is dominated then the exploration continues in next row. **/
                else
                    clearContainer(nCol, nRow, nDeep);
}


void HandlerContainer::updateMinValueFound(const Solution &solution){
    int n_obj = solution.getNumberOfObjectives();
    for (int objective = 0; objective < n_obj; ++objective)
        if (solution.getObjective(objective) < min_value_found_in_obj[objective])
            min_value_found_in_obj[objective].fetch_and_store(solution.getObjective(objective));
}

int HandlerContainer::getBestValueFoundIn(int obj) const{
    return min_value_found_in_obj[obj];
}

/**
 * It uses a binary search tree to locate the bucket which will contain the new solution.
 **/
int HandlerContainer::add(const Solution & solution) {
    int coordinate[3];
    getCoordinateForSolution(solution, coordinate);
    return set(solution, coordinate[0], coordinate[1], coordinate[2]);
}

void HandlerContainer::clearContainer(int x, int y, int z) {
    if (grid.getSizeOf(x, y, z) > 0)
        activeBuckets.fetch_and_decrement();
    
    disabledBuckets.fetch_and_increment();
    grid.clear(x, y, z);
}

/**
 * There are three states in gridState/BucketState:
 * 0: No explored.
 * 1: non-Dominated (Pareto front).
 * 2: Dominated.
 */
BucketState HandlerContainer::getStateOf(int x, int y, int z) const {
    return grid.getStateOf(x, y, z);
}

std::vector<Solution>& HandlerContainer::get(int x, int y, int z) {
    return grid.get(x, y, z);
}

unsigned int HandlerContainer::getRows() const {
    return grid.getNumberOfRows();
}

unsigned int HandlerContainer::getCols() const {
    return grid.getNumberOfCols();
}


unsigned int HandlerContainer::getDeep() const {
    return grid.getNumberOfDeep();
}

unsigned long HandlerContainer::getSize() const {
    return grid.getSize();
}

unsigned long HandlerContainer::getSizeOf(int x, int y, int z) const {
    return grid.getSizeOf(x, y, z);
}

unsigned long HandlerContainer::getNumberOfActiveBuckets() const{
    return activeBuckets;
}

unsigned long HandlerContainer::getNumberOfUnexploredBuckets() const{
    return unexploredBuckets;
}

unsigned long HandlerContainer::getNumberOfDisabledBuckets() const{
    return disabledBuckets;
}

void HandlerContainer::setNonDominatedState(int x, int y, int z){
    grid.setNonDominatedState(x, y, z);
}

void HandlerContainer::setDominatedState(int x, int y, int z){
    grid.setDominatedState(x, y, z);
}

void HandlerContainer::setUnexploredState(int x, int y, int z){
    grid.setUnexploredState(x, y, z);
}

void HandlerContainer::print() const{
    grid.print();
}

void HandlerContainer::printGridSize() const{
    for (int nRow = grid.getNumberOfRows() - 1; nRow >= 0; --nRow) {
        printf("[%3d] ", nRow);
        for (int nCol = 0; nCol < grid.getNumberOfCols(); ++nCol)
            if(grid.getStateOf(nCol, nRow, 0) == BucketState::Unexplored)
                printf(" - ");
            else
                printf("%3ld", grid.getSizeOf(nCol, nRow, 0));
        printf("\n");
    }
}

void HandlerContainer::printStates() const{
    BucketState state;
    for (int nRow = grid.getNumberOfRows() - 1; nRow >= 0; --nRow) {
        printf("[%3d] ", nRow);
        for (int nCol = 0; nCol < grid.getNumberOfCols(); ++nCol){
            state = grid.getStateOf(nCol, nRow, 0);
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
int HandlerContainer::isImprovingTheGrid(const Solution &solution) {
    int bucketCoordinate[3];
    int improves = 0;
    getCoordinateForSolution(solution, bucketCoordinate);
    BucketState stateOfBucket = grid.getStateOf(bucketCoordinate[0], bucketCoordinate[1], bucketCoordinate[2]);
    switch (stateOfBucket) {
        case BucketState::dominated:
            improves = 0;
            break;
            
        case BucketState::Unexplored:
            improves = 1;
            break;
            
        case BucketState::NonDominated:
            improves = grid.isImprovingTheBucket(solution, bucketCoordinate[0], bucketCoordinate[1], bucketCoordinate[2]);
            break;
    }
    return improves;
}

int HandlerContainer::improvesTheBucket(const Solution &solution, int x, int y, int z) {
    return grid.isImprovingTheBucket(solution, x, y, z);
}

std::vector<Solution>& HandlerContainer::getParetoFront() {
    paretoFront.reserve(getSize());
    for (int bucketY = 0; bucketY < getRows(); ++bucketY)
        for (int bucketX = 0; bucketX < getCols(); ++bucketX)
            for (int bucketZ = 0; bucketZ < getDeep(); ++bucketZ) {
                BucketState state = grid.getStateOf(bucketX, bucketY, bucketZ);
                if (state == BucketState::NonDominated) {
                    std::vector<Solution> vec = grid.get(bucketX, bucketY, bucketZ);
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
    else if (dimension == 2)
        return maxinz;
    else
        return -1;
}
