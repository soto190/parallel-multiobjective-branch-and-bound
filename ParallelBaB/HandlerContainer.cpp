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
grid(0,0){
};

HandlerContainer::HandlerContainer(const HandlerContainer& toCopy):
maxinx(toCopy.maxinx),
maxiny(toCopy.maxiny),
numberOfElements(toCopy.numberOfElements),
activeBuckets(toCopy.getNumberOfActiveBuckets()),
unexploredBuckets(toCopy.getNumberOfUnexploredBuckets()),
disabledBuckets(toCopy.getNumberOfDisabledBuckets()),
grid(toCopy.grid){
    
    rangeinx = new double[toCopy.getCols()];
    rangeiny = new double[toCopy.getRows()];
    
    int indexx = 0, indexy = 0;
    for (indexy = 0; indexy < toCopy.getRows(); indexy++)
        rangeiny[indexy] = toCopy.rangeiny[indexy];
    
    for (indexx = 0; indexx < toCopy.getCols(); indexx++)
        rangeinx[indexx] = toCopy.rangeinx[indexx];
    
    for (int obj = 0; obj < 2; ++obj)
        min_value_found_in_obj[obj].fetch_and_store(toCopy.getBestValueFoundIn(obj));
}

HandlerContainer::HandlerContainer(unsigned int rows, unsigned int cols, double maxValX, double maxValY):
grid(cols, rows){
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

HandlerContainer& HandlerContainer::operator()(unsigned int rows, unsigned int cols, double maxValX, double maxValY, int minValX, int minValY ){
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
int HandlerContainer::set(const Solution & solution, int x, int y) {
    int updated = 0;
    BucketState state = grid.getStateOf(x, y);
    switch (state) {
        case BucketState::Unexplored:
            clearContainersDominatedBy(x, y);
            grid.addTo(solution, x, y);
            grid.setNonDominatedState(x, y);
            activeBuckets.fetch_and_increment();
            unexploredBuckets.fetch_and_decrement();
            updated = 1;
            break;
            
        case BucketState::NonDominated:
            updated = grid.addTo(solution, x, y);
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

void HandlerContainer::clearContainersDominatedBy(const int x, const int y){
    for (int nRow = y + 1; nRow < grid.getNumberOfRows(); nRow++)
        for (int nCol = x + 1; nCol < grid.getNumberOfCols(); nCol++)
            if (grid.getStateOf(nCol, nRow) == BucketState::dominated)
                nCol = grid.getNumberOfCols(); /** If the bucket in (nCol, nRow) is dominated then the exploration continues in next row. **/
            else
                clearContainer(nCol, nRow);
}


void HandlerContainer::updateMinValueFound(const Solution &solution){
    for (int objective = 0; objective < 2; ++objective)
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

unsigned long HandlerContainer::getNumberOfActiveBuckets() const{
    return activeBuckets;
}

unsigned long HandlerContainer::getNumberOfUnexploredBuckets() const{
    return unexploredBuckets;
}

unsigned long HandlerContainer::getNumberOfDisabledBuckets() const{
    return disabledBuckets;
}

void HandlerContainer::setNonDominatedState(int x, int y){
    grid.setNonDominatedState(x, y);
}

void HandlerContainer::setDominatedState(int x, int y){
    grid.setDominatedState(x, y);
}

void HandlerContainer::setUnexploredState(int x, int y){
    grid.setUnexploredState(x, y);
}

void HandlerContainer::print() const{
    grid.print();
}

void HandlerContainer::printGridSize() const{
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

void HandlerContainer::printStates() const{
    BucketState state;
    for (int nRow = grid.getNumberOfRows() - 1; nRow >= 0; --nRow) {
        printf("[%3d] ", nRow);
        for (int nCol = 0; nCol < grid.getNumberOfCols(); ++nCol){
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
int HandlerContainer::improvesTheGrid(const Solution &solution) {
    int bucketCoordinate[2];
    int improves = 0;
    getCoordinateForSolution(solution, bucketCoordinate);
    BucketState stateOfBucket = grid.getStateOf(bucketCoordinate[0], bucketCoordinate[1]);
    switch (stateOfBucket) {
        case BucketState::dominated:
            improves = 0;
            break;
            
        case BucketState::Unexplored:
            improves = 1;
            break;
            
        case BucketState::NonDominated:
            improves = grid.produceImprovementInBucket(solution, bucketCoordinate[0], bucketCoordinate[1]);
            break;
    }
    return improves;
}

int HandlerContainer::improvesTheBucket(const Solution &solution, int x, int y) {
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

/********************************************/
/*
 * Handler container for 3 Dimensions.
 *
 */
/********************************************/
/*
 HandlerContainer3D::HandlerContainer3D() {
 int rows = 1, cols = 1, depth = 1, maxValX = 10, maxValY = 10, maxValZ = 10;
 
 totalElements = 0;
 unexploredBuckets = rows * cols * depth;
 activeBuckets = 0;
 disabledBuckets = 0;
 
 grid = GridContainer3D<Solution>(rows, cols, depth);
 gridState = new BucketState[rows * cols * depth];
 
 rangeinx = new double[rows];
 rangeiny = new double[cols];
 rangeinz = new double[depth];
 
 maxinx = maxValX;
 maxiny = maxValY;
 maxinz = maxValZ;
 
 int divs = 0;
 double rx = maxValX / cols;
 double ry = maxValY / rows;
 double rz = maxValZ / depth;
 
 rangeinx[divs] = 0;
 rangeiny[divs] = 0;
 rangeinz[divs] = 0;
 
 for (divs = 1; divs < cols; divs++)
 rangeinx[divs] = rangeinx[divs - 1] + rx;
 
 for (divs = 1; divs < rows; divs++)
 rangeiny[divs] = rangeiny[divs - 1] + ry;
 
 for (divs = 1; divs < depth; divs++)
 rangeinz[divs] = rangeinz[divs - 1] + rz;
 
 int r = 0;
 for (r = 0; r < rows * cols * depth; r++)
 gridState[r] = BucketState::unexplored;
 
 dimensionSize = new int[3];
 
 dimensionSize[0] = rows;
 dimensionSize[1] = cols;
 dimensionSize[2] = depth;
 
 maxin = new double[3];
 
 maxin[0] = maxValX;
 maxin[1] = maxValY;
 maxin[2] = maxValZ;
 }
 
 HandlerContainer3D::~HandlerContainer3D() {
 delete[] rangeinx;
 delete[] rangeiny;
 delete[] rangeinz;
 delete[] gridState;
 delete[] dimensionSize;
 delete[] maxin;
 }
 
 HandlerContainer3D::HandlerContainer3D(int rows, int cols, int depth,
 double maxValX, double maxValY, double maxValZ) {
 
 totalElements = 0;
 unexploredBuckets = rows * cols * depth;
 activeBuckets = 0;
 disabledBuckets = 0;
 
 grid = GridContainer3D<Solution>(rows, cols, depth);
 gridState = new BucketState[rows * cols * depth];
 
 rangeinx = new double[rows];
 rangeiny = new double[cols];
 rangeinz = new double[depth];
 
 maxinx = maxValX;
 maxiny = maxValY;
 maxinz = maxValZ;
 
 int divs = 0;
 double rx = maxValX / cols;
 double ry = maxValY / rows;
 double rz = maxValZ / depth;
 
 rangeinx[divs] = 0;
 rangeiny[divs] = 0;
 rangeinz[divs] = 0;
 
 for (divs = 1; divs < cols; divs++)
 rangeinx[divs] = rangeinx[divs - 1] + rx;
 
 for (divs = 1; divs < rows; divs++)
 rangeiny[divs] = rangeiny[divs - 1] + ry;
 
 for (divs = 1; divs < depth; divs++)
 rangeinz[divs] = rangeinz[divs - 1] + rz;
 
 int r = 0;
 for (r = 0; r < rows * cols * depth; r++)
 gridState[r] = BucketState::unexplored;
 
 dimensionSize = new int[3];
 
 dimensionSize[0] = rows;
 dimensionSize[1] = cols;
 dimensionSize[2] = depth;
 
 maxin = new double[3];
 
 maxin[0] = maxValX;
 maxin[1] = maxValY;
 maxin[2] = maxValZ;
 
 }
 
 int * HandlerContainer3D::getCandidateBucket(Solution &solution) {
 
 int * coordinate = new int[getNumberOfDimension()];
 
 coordinate[0] = binarySearch(solution.getObjective(0), rangeinx,
 getCols());
 
 coordinate[1] = binarySearch(solution.getObjective(1), rangeiny,
 getRows());
 
 coordinate[2] = binarySearch(solution.getObjective(2), rangeinz,
 getDepth());
 
 return coordinate;
 
 }
 
 int HandlerContainer3D::set(Solution & solution, int x, int y, int z) {
 
 if (getStateOf(x, y, z) == BucketState::unexplored) {
 
 int nCol = 0;
 int nRow = 0;
 int nDeep = 0;
 for (nDeep = z + 1; nDeep < grid.getDepth(); nDeep++)
 for (nRow = y + 1; nRow < grid.getRows(); nRow++)
 for (nCol = x + 1; nCol < grid.getCols(); nCol++)
 if (getStateOf(nCol, nRow, nDeep)
 == BucketState::dominated)
 nCol = grid.getDepth();
 else
 clearContainer(nCol, nRow, nDeep);
 
 updateBucket(new Solution(solution), x, y, z);
 setStateOf(BucketState::nondominated, x, y, z);
 totalElements++;
 activeBuckets++;
 unexploredBuckets--;
 
 return 1;
 
 } else if (getStateOf(x, y, z) == BucketState::nondominated)
 return updateBucket(new Solution(solution), x, y, z);
 
 return 0;
 }
 
 int * HandlerContainer3D::add(Solution & solution) {
 
 int * coordinate = getCandidateBucket(solution);
 
 set(solution, coordinate[0], coordinate[1], coordinate[2]);
 
 return coordinate;
 }
 
 void HandlerContainer3D::clearContainer(int x, int y, int z) {
 setStateOf(dominated, x, y, z);
 
 if (grid.getSizeOf(x, y, z) > 0) {
 totalElements -= grid.getSizeOf(x, y, z);
 disabledBuckets++;
 activeBuckets--;
 grid.clear(x, y, z);
 }
 }
 
 std::vector<Solution>& HandlerContainer3D::get(int x, int y, int z) {
 return grid(x, y, z);
 }
 
 unsigned int HandlerContainer3D::getRows() {
 return grid.getRows();
 }
 
 unsigned int HandlerContainer3D::getCols() {
 return grid.getCols();
 }
 
 unsigned int HandlerContainer3D::getDepth() {
 return grid.getDepth();
 }
 
 unsigned long HandlerContainer3D::getSize() {
 return totalElements;
 }
 
 unsigned long HandlerContainer3D::getSizeOf(int x, int y, int z) {
 return grid.getSizeOf(x, y, z);
 }
 */
/**
 *
 * TODO: print z dimension.
 *
 **/
/*
 void HandlerContainer3D::printGridSize() {
 int nCol = 0;
 int nRow = 0;
 int nDeep = 0;
 for (nRow = grid.getRows() - 1; nRow >= 0; nRow--) {
 printf("[%3d] ", nRow);
 for (nCol = 0; nCol < grid.getCols(); nCol++)
 printf("%3ld ", grid.getSizeOf(nCol, nRow, nDeep));
 printf("\n");
 }
 }
 */
/**
 *
 * TODO: print z dimension.
 *
 
 void HandlerContainer3D::printStates() {
 int nCol = 0;
 int nRow = 0;
 int nDeep = 0;
 for (nRow = grid.getRows() - 1; nRow >= 0; nRow--) {
 printf("[%3d] ", nRow);
 for (nCol = 0; nCol < grid.getCols(); nCol++)
 printf("%3d", getStateOf(nCol, nRow, nDeep));
 printf("\n");
 }
 }
 
 
 BucketState HandlerContainer3D::getStateOf(int x, int y, int z) {
 
 return gridState[(x * getCols()) + y
 + (getCols() * getRows() * z)];
 }
 
 void HandlerContainer3D::setStateOf(BucketState state, int x, int y, int z) {
 gridState[(x * getCols()) + y
 + (getCols() * getRows() * z)] = state;
 }
 
 int HandlerContainer3D::updateBucket(Solution * solution, int x, int y, int z) {
 
 std::vector<Solution> paretoFront = grid.get(x, y, z);
 unsigned long sizeBeforeUpdate = paretoFront.size();
 
 int updated = updateFront(*solution, paretoFront);
 if (updated == 1) {
 if (paretoFront.size() < sizeBeforeUpdate) {
 unsigned long int removedElements = sizeBeforeUpdate
 - (paretoFront.size() - 1);
 totalElements -= removedElements;
 totalElements++;
 }
 else if (paretoFront.size() == sizeBeforeUpdate + 1)
 totalElements++;
 }
 return updated;
 }
 
 std::vector<Solution> HandlerContainer3D::getParetoFront() {
 
 std::vector<Solution> paretoFront;
 paretoFront.reserve(totalElements);
 
 int bucketX = 0;
 int bucketY = 0;
 int bucketZ = 0;
 int index = 0;
 
 for (bucketY = 0; bucketY < getRows(); bucketY++)
 for (bucketX = 0; bucketX < getCols(); bucketX++)
 for (bucketZ = 0; bucketZ < getDepth(); bucketZ++) {
 
 BucketState state = getStateOf(bucketX, bucketY, bucketZ);
 if (state == BucketState::nondominated) {
 std::vector<Solution> bucket = get(bucketX, bucketY,
 bucketZ);
 //                    paretoFront.insert(paretoFront.begin(), bucket.begin() , bucket.end());
 for (index = 0; index < bucket.size(); index++) {
 paretoFront.push_back(bucket.at(index));
 }
 } else if (state == BucketState::dominated) {
 bucketZ = getDepth();
 }
 }
 
 extractParetoFront(paretoFront);
 return paretoFront;
 }
 
 double HandlerContainer3D::getMaxIn(int dimension) {
 return maxin[dimension];
 }
 
 unsigned int HandlerContainer3D::getSizeOfDimension(int dimension) {
 return dimensionSize[dimension];
 }
 
 unsigned int HandlerContainer3D::getNumberOfDimension() {
 return grid.getNumberOfDimensions();
 }
 */
