//
//  Grid.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 22/09/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "GridContainer.hpp"

template <class T> GridContainer<T>::GridContainer(){
}

template <class T> GridContainer3D<T>::GridContainer3D(){
}

HandlerContainer::HandlerContainer(){
}

HandlerContainer::~HandlerContainer(){
    delete [] rangeinx;
    delete [] rangeiny;
    delete [] gridState;
}

HandlerContainer::HandlerContainer(int rows, int cols, double maxValX, double maxValY){
    
    if(maxValX < cols)
        cols = maxValX;
    if(maxValY < rows)
        rows = maxValY;
   
    
    totalElements = 0;
    unexploredBuckets = rows * cols;
    activeBuckets = 0;
    disabledBuckets = 0;
    
    grid = GridContainer<Solution * >(cols, rows);
    gridState = new BucketState[cols * rows];
    
    rangeinx = new double [cols];
    rangeiny = new double [rows];
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
    
    int r = 0;
    for (r = 0; r < rows * cols; r++)
        gridState[r] = BucketState::unexplored;
    
}

void HandlerContainer::checkCoordinate(Solution &solution, int * coordinate){
    
    coordinate[0] = binarySearch(solution.getObjective(0), this->rangeinx, this->getCols());
    coordinate[1] = binarySearch(solution.getObjective(1), this->rangeiny, this->getRows());
}


/**
 *
 * Sets a copy of the solution.
 */
 
int HandlerContainer::set(Solution & solution, int x, int y){
    int nCol = 0;
    int nRow = 0;
    int updated = 0;
    BucketState state = this->getStateOf(x, y);
    
    switch (state) {
            
        case BucketState::unexplored:
            
            /**
             Empty the dominated containers.
             Calls to this segment of code on worst case O((cols - 1) * (rows - 1))
             **/
            
            for (nRow = y + 1; nRow < this->grid.getRows(); nRow++)
                for (nCol = x + 1; nCol < this->grid.getCols(); nCol++)
                    if(this->getStateOf(nCol, nRow) == BucketState::dominated)
                    /** If the bucket in (nCol, nRow) is dominated the exploration continue to the next row**/
                        nCol = this->grid.getCols();
                    else
                        this->clearContainer(nCol, nRow);
            
            this->updateBucket(solution, x, y);
            this->setStateOf(BucketState::nondominated, x, y);
            this->totalElements++;
            this->activeBuckets++;
            this->unexploredBuckets--;
            
            updated = 1;
            break;
            
        case BucketState::nondominated:
            updated  = this->updateBucket(solution, x, y);
            break;
            
        case BucketState::dominated:
            /**If the bucket is dominated (State = 2) the element is not added. Then do nothing**/
            break;
    }
    
    return updated;
}

/**
 * It uses a binary search tree locate the bucket which will contain the new solution.
 **/
void HandlerContainer::add(Solution & solution){

    
    int coordinate [2];
    checkCoordinate(solution, coordinate);
    
    this->set(solution, coordinate[0], coordinate[1]);
}

void HandlerContainer::clearContainer(int x, int y){
    this->setStateOf(BucketState::dominated, x, y);

    if (this->grid.getSizeOf(x, y) > 0){
        this->totalElements -= this->grid.getSizeOf(x, y);
        this->disabledBuckets++;
        this->activeBuckets--;
        this->grid.clear(x, y);
    }
}

std::vector<Solution *> HandlerContainer::get(int x, int y){
    return grid(x, y);
}

unsigned int HandlerContainer::getRows(){
    return grid.getRows();
}

unsigned int HandlerContainer::getCols(){
    return grid.getCols();
}

unsigned long HandlerContainer::getSize(){
    return totalElements;
}

unsigned long HandlerContainer::getSizeOf(int x, int y){
    return this->grid.getSizeOf(x, y);
}

void HandlerContainer::printGridSize(){
    int nCol = 0;
    int nRow = 0;
    for (nRow = this->grid.getRows() - 1; nRow >= 0 ; nRow--){
        printf("[%3d] ", nRow);
        for (nCol = 0; nCol < this->grid.getCols(); nCol++)
            printf("%3ld ", this->grid.getSizeOf(nCol, nRow));
        printf("\n");
    }
}

void HandlerContainer::printStates(){
    int nCol = 0;
    int nRow = 0;
    for (nRow = this->grid.getRows() - 1; nRow >= 0 ; nRow--){
        printf("[%3d] ", nRow);
        for (nCol = 0; nCol < this->grid.getCols(); nCol++)
            printf("%3d", this->getStateOf(nCol, nRow));
        printf("\n");
    }
}

/**
 * There are three states in gridState:
 * 0: No explored.
 * 1: non-Dominated (Pareto front).
 * 2: Dominated.
 */
BucketState HandlerContainer::getStateOf(int x, int y){
    return this->gridState[y * this->getCols() + x];
}

void HandlerContainer::setStateOf(BucketState state, int x, int y){
    this->gridState[y * this->getCols() + x] = state;
}

int HandlerContainer::updateBucket(Solution & solution, int x, int y){
    
    std::vector<Solution *> *paretoFront = &grid.get(x, y);
    unsigned long sizeBeforeUpdate = paretoFront->size();
    
    int updated = updateFront(solution, *paretoFront);
    if(updated == 1){
        if(paretoFront->size() < sizeBeforeUpdate){ /** Some solutions were removed. **/
            unsigned long int removedElements = sizeBeforeUpdate - (paretoFront->size() - 1);
            this->totalElements -= removedElements;
            this->totalElements++;
        }
        else if(paretoFront->size() == sizeBeforeUpdate + 1) /** No solutions were removed and the new solution was added. **/
            this->totalElements++;
        
        /** else the size doesnt change**/
    }
    
    return updated;
}

std::vector<Solution *> HandlerContainer::getParetoFront(){
    
    std::vector<Solution *> paretoFront;
    paretoFront.reserve(this->totalElements);
    
    int bucketX = 0;
    int bucketY = 0;
    
    for (bucketY = 0; bucketY < this->getRows(); bucketY++)
        for (bucketX = 0; bucketX < this->getCols(); bucketX++) {
            BucketState state = this->getStateOf(bucketX, bucketY);
            if (state == BucketState::nondominated) {
                std::vector<Solution * > bucket = this->get(bucketX, bucketY);
                paretoFront.insert(paretoFront.begin(), bucket.begin() , bucket.end());
            }
            else if (state == BucketState::dominated)
                bucketX = this->getCols();
        }
    
    extractParetoFront(paretoFront);
    return paretoFront;
}

double HandlerContainer::getMaxIn(int dimension){
    if(dimension == 0)
        return this->maxinx;
    else if(dimension == 1)
        return this->maxiny;
    else
        return -1;
}
/********************************************/
/*
 * Handler container for 3 Dimensions.
 *
 */
/********************************************/

HandlerContainer3D::HandlerContainer3D(){
}

HandlerContainer3D::~HandlerContainer3D(){
    delete [] rangeinx;
    delete [] rangeiny;
    delete [] rangeinz;
    delete [] gridState;
    delete [] dimensionSize;
    delete [] maxin;
}

HandlerContainer3D::HandlerContainer3D(int rows, int cols, int depth, double maxValX, double maxValY, double maxValZ){
    
    totalElements = 0;
    unexploredBuckets = rows * cols * depth;
    activeBuckets = 0;
    disabledBuckets = 0;
    
    grid = GridContainer3D<Solution * >(rows, cols, depth);
    gridState = new BucketState[rows * cols * depth];
    
    rangeinx = new double [rows];
    rangeiny = new double [cols];
    rangeinz = new double [depth];

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

int * HandlerContainer3D::getCandidateBucket(Solution &solution){
    
    int * coordinate = new int[this->getNumberOfDimension()];
    
    coordinate[0] = binarySearch(solution.getObjective(0), this->rangeinx, this->getCols());
    
    coordinate[1] = binarySearch(solution.getObjective(1), this->rangeiny, this->getRows());
    
    coordinate[2] = binarySearch(solution.getObjective(2), this->rangeinz, this->getDepth());
    
    return coordinate;
    
}

int HandlerContainer3D::set(Solution & solution, int x, int y, int z){
    
    if(this->getStateOf(x, y, z) == BucketState::unexplored){
        
        /**
         Empty the dominated containers.
         Calls to this segment of code on worst case O((cols - 1) * (rows - 1))
         **/
        
        int nCol = 0;
        int nRow = 0;
        int nDeep = 0;
        for (nDeep = z + 1; nDeep < this->grid.getDepth(); nDeep++)
            for (nRow = y + 1; nRow < this->grid.getRows(); nRow++)
                for (nCol = x + 1; nCol < this->grid.getCols(); nCol++)
                    if(this->getStateOf(nCol, nRow, nDeep) == BucketState::dominated) /** If the bucket in (nCol, nRow) is dominated the exploration continue to the next row**/
                        nCol = this->grid.getDepth();
                    else
                        this->clearContainer(nCol, nRow, nDeep);
        
        this->updateBucket(new Solution(solution), x, y, z); /** Sends a copy. **/
        this->setStateOf(BucketState::nondominated, x, y, z);
        this->totalElements++;
        this->activeBuckets++;
        this->unexploredBuckets--;
        
        return 1;
        
    }
    else if(this->getStateOf(x, y, z) == BucketState::nondominated)
        return this->updateBucket(new Solution(solution), x, y, z); /** Sends a copy. **/
    
    /**If the bucket is dominated (State = 2) the element is not added.**/
    return 0;
}

int * HandlerContainer3D::add(Solution & solution){
    
    int * coordinate = getCandidateBucket(solution);
    
    this->set(solution, coordinate[0], coordinate[1], coordinate[2]);
    
    return coordinate;
}

void HandlerContainer3D::clearContainer(int x, int y, int z){
    this->setStateOf(dominated, x, y, z);
    
    if (this->grid.getSizeOf(x, y, z) > 0){
        this->totalElements -= this->grid.getSizeOf(x, y, z);
        this->disabledBuckets++;
        this->activeBuckets--;
        this->grid.clear(x, y, z);
    }
}

std::vector<Solution *>& HandlerContainer3D::get(int x, int y, int z){
    return grid(x, y, z);
}

unsigned int HandlerContainer3D::getRows(){
    return grid.getRows();
}

unsigned int HandlerContainer3D::getCols(){
    return grid.getCols();
}

unsigned int HandlerContainer3D::getDepth(){
    return grid.getDepth();
}

unsigned long HandlerContainer3D::getSize(){
    return totalElements;
}

unsigned long HandlerContainer3D::getSizeOf(int x, int y, int z){
    return this->grid.getSizeOf(x, y, z);
}

/**
 *
 * TODO: print z dimension.
 *
 **/
void HandlerContainer3D::printGridSize(){
    int nCol = 0;
    int nRow = 0;
    int nDeep = 0;
    for (nRow = this->grid.getRows() - 1; nRow >= 0 ; nRow--){
        printf("[%3d] ", nRow);
        for (nCol = 0; nCol < this->grid.getCols(); nCol++)
            printf("%3ld ", this->grid.getSizeOf(nCol, nRow, nDeep));
        printf("\n");
    }
}

/**
 *
 * TODO: print z dimension.
 *
 **/
void HandlerContainer3D::printStates(){
    int nCol = 0;
    int nRow = 0;
    int nDeep = 0;
    for (nRow = this->grid.getRows() - 1; nRow >= 0 ; nRow--){
        printf("[%3d] ", nRow);
        for (nCol = 0; nCol < this->grid.getCols(); nCol++)
            printf("%3d", this->getStateOf(nCol, nRow, nDeep));
        printf("\n");
    }
}

/**
 * There are three states in gridState:
 * 0: No explored.
 * 1: non-Dominated (Pareto front).
 * 2: Dominated.
 */
BucketState HandlerContainer3D::getStateOf(int x, int y, int z){
    
    return this->gridState[(x * this->getCols()) + y + (this->getCols() * this->getRows() * z)];
}

void HandlerContainer3D::setStateOf(BucketState state, int x, int y, int z){
    this->gridState[(x * this->getCols()) + y + (this->getCols() * this->getRows() * z)] = state;
}

int HandlerContainer3D::updateBucket(Solution * solution, int x, int y, int z){
    
    std::vector<Solution *> paretoFront = grid.get(x, y, z);
    unsigned long sizeBeforeUpdate = paretoFront.size();
    
    int updated = updateFront(*solution, paretoFront);
    if(updated == 1){
        /**Some solutions were removed **/
        if(paretoFront.size() < sizeBeforeUpdate){
            unsigned long int removedElements = sizeBeforeUpdate - (paretoFront.size() - 1);
            this->totalElements -= removedElements;
            this->totalElements++;
        }
        /** No solutions were removed and the new solution was added**/
        else if(paretoFront.size() == sizeBeforeUpdate + 1)
            this->totalElements++;
        /**else the size doesnt change**/
    }
    return updated;
}

std::vector<Solution *> HandlerContainer3D::getParetoFront(){
    
    std::vector<Solution *> paretoFront;
    paretoFront.reserve(this->totalElements);
    
    int bucketX = 0;
    int bucketY = 0;
    int bucketZ = 0;
    
    for (bucketY = 0; bucketY < this->getRows(); bucketY++)
        for (bucketX = 0; bucketX < this->getCols(); bucketX++)
            for (bucketZ = 0; bucketZ < this->getDepth(); bucketZ++) {
                
                BucketState state = this->getStateOf(bucketX, bucketY, bucketZ);
                if (state == BucketState::nondominated) {
                    std::vector<Solution * > bucket = this->get(bucketX, bucketY, bucketZ);
                    paretoFront.insert(paretoFront.begin(), bucket.begin() , bucket.end());
                }
                else if (state == BucketState::dominated){
                    bucketZ = this->getDepth();
                }
    }
    
    extractParetoFront(paretoFront);
    return paretoFront;
}

double HandlerContainer3D::getMaxIn(int dimension){
    return maxin[dimension];
}

unsigned int HandlerContainer3D::getSizeOfDimension(int dimension){
    return dimensionSize[dimension];
}

unsigned int HandlerContainer3D::getNumberOfDimension(){
    return this->grid.getNumberOfDimensions();
}
