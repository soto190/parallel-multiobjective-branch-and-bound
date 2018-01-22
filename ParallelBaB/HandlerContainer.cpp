//
//  HandlerContainer.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/6/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "HandlerContainer.hpp"

HandlerContainer::HandlerContainer():
range_dim_x(nullptr),
range_dim_y(nullptr),
range_dim_z(nullptr),
max_val_in_x(0),
max_val_in_y(0),
max_val_in_z(0),
numberOfElements(0),
grid(1, 1, 1){
    
};

HandlerContainer::HandlerContainer(const HandlerContainer& toCopy):
max_val_in_x(toCopy.max_val_in_x),
max_val_in_y(toCopy.max_val_in_y),
max_val_in_z(toCopy.max_val_in_z),
numberOfElements(toCopy.numberOfElements),
activeBuckets(toCopy.getNumberOfActiveBuckets()),
unexploredBuckets(toCopy.getNumberOfUnexploredBuckets()),
disabledBuckets(toCopy.getNumberOfDisabledBuckets()),
grid(toCopy.grid){
    
    range_dim_x = new double[toCopy.getCols()];
    range_dim_y = new double[toCopy.getRows()];
    range_dim_z = new double[toCopy.getDeep()];
    
    for (int indexy = 0; indexy < toCopy.getRows(); ++indexy)
        range_dim_y[indexy] = toCopy.range_dim_y[indexy];
    
    for (int indexx = 0; indexx < toCopy.getCols(); ++indexx)
        range_dim_x[indexx] = toCopy.range_dim_x[indexx];
    
    for (int indexz = 0; indexz < toCopy.getDeep(); ++indexz)
        range_dim_z[indexz] = toCopy.range_dim_z[indexz];
    
    for (int obj = 0; obj < DIMENSIONS; ++obj)
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
    
    range_dim_x = new double[cols];
    range_dim_y = new double[rows];
    range_dim_z = new double[getDeep()];
    
    max_val_in_x = maxValX;
    max_val_in_y = maxValY;
    max_val_in_z = 1;
    
    int divs = 0;
    
    double rx = maxValX / cols;
    double ry = maxValY / rows;
    double rz = maxValZ / deep;
    
    range_dim_x[divs] = 0;
    range_dim_y[divs] = 0;
    range_dim_z[divs] = 0;
    
    for (divs = 1; divs < cols; ++divs)
        range_dim_x[divs] = range_dim_x[divs - 1] + rx;
    
    for (divs = 1; divs < rows; ++divs)
        range_dim_y[divs] = range_dim_y[divs - 1] + ry;
    
    for (divs = 1; divs < deep; ++divs)
        range_dim_z[divs] = range_dim_z[divs - 1] + rz;
    
    for (int obj = 0; obj < DIMENSIONS; ++obj)
        min_value_found_in_obj[obj].fetch_and_store(BIG_VALUE);
}

HandlerContainer::~HandlerContainer() {
    delete[] range_dim_x;
    delete[] range_dim_y;
    delete[] range_dim_z;
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
    
    if(range_dim_x != nullptr)
        delete range_dim_x;
    if(range_dim_y != nullptr)
        delete range_dim_y;
    if(range_dim_z != nullptr)
        delete range_dim_z;
    
    paretoFront.clear();
    
    range_dim_x = new double[cols];
    range_dim_y = new double[rows];
    range_dim_z = new double[deep];
    
    max_val_in_x = maxValX;
    max_val_in_y = maxValY;
    max_val_in_z = maxValZ;
    
    int divs = 0;
    
    double rx = maxValX / cols;
    double ry = maxValY / rows;
    double rz = maxValZ / deep;
    
    range_dim_x[divs] = minValX;
    range_dim_y[divs] = minValY;
    range_dim_z[divs] = minValZ;
    
    for (divs = 1; divs < cols; ++divs)
        range_dim_x[divs] = range_dim_x[divs - 1] + rx;
    
    for (divs = 1; divs < rows; ++divs)
        range_dim_y[divs] = range_dim_y[divs - 1] + ry;
    
    for (divs = 1; divs < deep; ++divs)
        range_dim_z[deep] = range_dim_z[divs - 1] + rz;
    
    for (int obj = 0; obj < DIMENSIONS; ++obj)
        min_value_found_in_obj[obj].fetch_and_store(BIG_VALUE);
    
    return *this;
}

void HandlerContainer::getCoordinateForSolution(const Solution &solution, int * coordinate) const {
    coordinate[0] = binarySearch(solution.getObjective(0), range_dim_x, getCols());
    coordinate[1] = binarySearch(solution.getObjective(1), range_dim_y, getRows());
    coordinate[2] = binarySearch(solution.getObjective(2), range_dim_z, getDeep());
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
    for (int n_deep = z + 1; n_deep < grid.getNumberOfDeep(); ++n_deep)
        for (int n_row = y + 1; n_row < grid.getNumberOfRows(); ++n_row)
            for (int n_col = x + 1; n_col < grid.getNumberOfCols(); ++n_col)
                if (grid.getStateOf(n_col, n_row, n_deep) == BucketState::dominated)
                    n_col = grid.getNumberOfCols(); /** If the bucket in (nCol, nRow) is dominated then the exploration continues in next row. **/
                else
                    clearContainer(n_col, n_row, n_deep);
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
    int coordinate[DIMENSIONS];
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
    int bucketCoordinate[DIMENSIONS];
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
    for (int bucket_z = 0; bucket_z < getDeep(); ++bucket_z)
        for (int bucket_y = 0; bucket_y < getRows(); ++bucket_y)
            for (int bucket_x = 0; bucket_x < getCols(); ++bucket_x){
                BucketState state = grid.getStateOf(bucket_x, bucket_y, bucket_z);
                if (state == BucketState::NonDominated) {
                    std::vector<Solution> vec = grid.get(bucket_x, bucket_y, bucket_z);
                    std::vector<Solution>::iterator it = vec.begin();
                    
                    for (it = vec.begin(); it != vec.end(); ++it)
                        paretoFront.push_back(*it);
                    
                } else if (state == BucketState::dominated)
                    bucket_x = grid.getNumberOfCols();
            }
    
    extractParetoFront(paretoFront);
    return paretoFront;
}

double HandlerContainer::getMaxIn(int dimension) {
    if (dimension == 0)
        return max_val_in_x;
    else if (dimension == 1)
        return max_val_in_y;
    else if (dimension == 2)
        return max_val_in_z;
    else
        return -1;
}
