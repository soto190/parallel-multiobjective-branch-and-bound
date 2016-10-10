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

HandlerContainer::HandlerContainer(int rows, int cols, double maxValX, double maxValY){
    totalElements = 0;
    grid = GridContainer<Solution * >(rows, cols);
    
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
    
}
/**
 NOTES TODO: Improve it by using a binary tree to search the bucket which will contain the new solution.
 **/
int * HandlerContainer::add(Solution *solution){
    int y = 0;
    int x = 0;
    
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
    
    grid.set(solution, x, y);
    this->totalElements++;
    
    /**
     Empty the dominated containers.
     **/
    
    int nCol = 0;
    int nRow = 0;
    for (nRow = y + 1; nRow < this->grid.getRows(); nRow++)
        for (nCol = x + 1; nCol < this->grid.getCols(); nCol++)
            this->clearContainer(nRow, nCol);
    

    coordinate[0] = x;
    coordinate[1] = y;
    
    return coordinate;
}

void HandlerContainer::clearContainer(int x, int y){
    
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
    for (nRow = 0; nRow < this->grid.getRows(); nRow++){
        printf("[%3d] ", nRow);
        for (nCol = 0; nCol < this->grid.getCols(); nCol++)
            printf("%3ld ", this->grid.getSizeOf(nRow, nCol));
        printf("\n");
    }
}

