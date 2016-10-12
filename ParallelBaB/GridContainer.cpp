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

HandlerContainer::HandlerContainer(){
}

HandlerContainer::HandlerContainer(int rows, int cols, double maxValX, double maxValY){
    totalElements = 0;
    grid = GridContainer<Solution * >(rows, cols);
    gridState = new int[rows * cols];
    
    rangeinx = new double [rows];
    rangeiny = new double [cols];
    maxinx = maxValX;
    maxiny = maxValY;
    int divs = 0;
    double rx = maxValX / cols;
    double ry = maxValY / rows;
        
    rangeinx[divs] = rx;
    rangeiny[divs] = ry;
    
    for (divs = 1; divs < cols; divs++)
        rangeinx[divs] = rangeinx[divs - 1] + rx;
        
    for (divs = 1; divs < rows; divs++)
        rangeiny[divs] = rangeiny[divs - 1] + ry;
    
    int r = 0;
    for (r = 0; r < rows * cols; r++){
        gridState[r] = 0;
    }
    
}

int * HandlerContainer::checkCoordinate(Solution *solution){

    int x = grid.getCols() - 1;
    int y = grid.getRows() - 1;
    
    int col = 0;
    int row = 0;
    
    int *coordinate = new int[2];
    
    for (col = 0; col < grid.getCols(); col++)
        if(solution->getObjective(0) <= rangeinx[col]){
            x = col;
            col = grid.getCols();
        }
    
    for (row = 0; row < grid.getRows(); row++)
        if(solution->getObjective(1) <= rangeiny[row]){
            y = row;
            row = grid.getRows();
        }
    
    
    coordinate[0] = x;
    coordinate[1] = y;

    return coordinate;

}

void HandlerContainer::set(Solution * solution, int x, int y){

    if(this->getStateOf(x, y) == 0){
        
        /**
         Empty the dominated containers.
         **/
        
        int nCol = 0;
        int nRow = 0;
        for (nRow = y + 1; nRow < this->grid.getRows(); nRow++)
            for (nCol = x + 1; nCol < this->grid.getCols(); nCol++)
                if(this->getStateOf(nCol, nRow) == 2) /** If the bucket in (nCol, nRow) is dominated the exploration continue to the next row**/
                    nCol = this->grid.getCols();
                else
                    this->clearContainer(nCol, nRow);
        
        this->grid.set(solution, x, y);
        this->totalElements++;
        this->setStateOf(1, x, y);

    }
    else if(this->getStateOf(x, y) == 1)
        this->updateBucket(solution, x, y);
    
    /**If the bucket is dominated (State = 2) the element is not added.**/

    
}

/**
 NOTES TODO: Improve it by using a binary tree to search the bucket which will contain the new solution.
 **/
int * HandlerContainer::add(Solution * solution){
    int x = grid.getCols() - 1;
    int y = grid.getRows() - 1;
    
    int col = 0;
    int row = 0;
    
    int *coordinate = new int[2];
    
    for (col = 0; col < grid.getCols(); col++)
        if(solution->getObjective(0) <= rangeinx[col]){
            x = col;
            col = grid.getCols();
        }
    
    for (row = 0; row < grid.getRows(); row++)
        if(solution->getObjective(1) <= rangeiny[row]){
            y = row;
            row = grid.getRows();
        }
    
    coordinate[0] = x;
    coordinate[1] = y;
    
    this->set(solution, x, y);
    
    return coordinate;
}

void HandlerContainer::clearContainer(int x, int y){
    this->setStateOf(2, x, y);

    if (this->grid.getSizeOf(x, y) > 0){
        this->totalElements -= this->grid.getSizeOf(x, y);
        this->grid.get(x, y).clear();
    }
}

std::vector<Solution *>& HandlerContainer::get(int x, int y){
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
            printf("%3ld ", this->grid.getSizeOf(nRow, nCol));
        printf("\n");
    }
}

void HandlerContainer::printStates(){
    int nCol = 0;
    int nRow = 0;
    for (nRow = this->grid.getRows() - 1; nRow >= 0 ; nRow--){
        printf("[%3d] ", nRow);
        for (nCol = 0; nCol < this->grid.getCols(); nCol++)
            printf("%3d", this->getStateOf(nRow, nCol));
        printf("\n");
    }
}

/**
 * There are three states in gridState:
 * 0: No explored.
 * 1: non-Dominated (Pareto front).
 * 2: Dominated.
 */
int HandlerContainer::getStateOf(int x, int y){
    return this->gridState[x * this->getCols() + y];
}

void HandlerContainer::setStateOf(int state, int x, int y){
    this->gridState[x * this->getCols() + y] = state;
}

int HandlerContainer::updateBucket(Solution * solution, int x, int y){
    
    std::vector<Solution *> paretoFront = grid.get(x, y);
    unsigned long sizeBeforeUpdate = paretoFront.size();
    
    int updated = updateFront(solution, paretoFront);
    if(updated == 1){
        if(paretoFront.size() < sizeBeforeUpdate){
            this->totalElements -= sizeBeforeUpdate - paretoFront.size() - 1;
            this->totalElements++;
        }
        else if(paretoFront.size() == sizeBeforeUpdate + 1)
            this->totalElements++;
    }
    return updated;
}

