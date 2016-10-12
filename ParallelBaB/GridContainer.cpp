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

    /**If the bucket is dominated then the element is not added**/
    if(this->getStateOf(x, y) == 0){
        this->totalElements++;
        
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
    else if(this->getStateOf(x, y) == 1){
        
        int updated = this->updateBucket(solution, x, y);
        if(updated == 1)
            this->totalElements++;
    }
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

int HandlerContainer::dominanceTest(Solution * solutionA, Solution * solutionB){
    int objective = 0;
    int solAIsBetterIn = 0;
    int solBIsBetterIn = 0;
    int equals = 1;
    
    /**
     * For more objectives consider
     * if (solAIsBetterIn > 0 and solBIsBetterIn > 0) break the FOR because the solutions are non-dominated.
     **/
    for (objective = 0; objective < solutionA->totalObjectives; objective++) {
        double objA = solutionA->getObjective(objective);
        double objB = solutionB->getObjective(objective);
        
        if(objA < objB){
            solAIsBetterIn++;
            equals = 0;
        }
        else if(objB < objA){
            solBIsBetterIn++;
            equals = 0;
        }
    }
    
    if(equals == 1)
        return 11;
    else if (solAIsBetterIn > 0 && solBIsBetterIn == 0)
        return 1;
    else if (solBIsBetterIn > 0 && solAIsBetterIn == 0)
        return -1;
    else
        return 0;
}

int HandlerContainer::updateBucket(Solution * solution, int x, int y){
    
    
    unsigned int * status = new unsigned int[4];
    status[0] = 0;
    status[1] = 0;
    status[2] = 0;
    status[3] = 0;
    
    std::vector<Solution *> paretoFront = grid.get(x, y);
    std::vector<Solution *>::iterator begin = paretoFront.begin();
    
    unsigned int nSol = 0;
    int domination = 0;
    
    for(nSol = 0; nSol < paretoFront.size(); nSol++){
            
        domination = dominanceTest(solution, paretoFront.at(nSol));
            
        if(domination == 1){
            paretoFront.erase(begin + nSol);
            status[0]++;
            nSol--;
            this->totalElements--;
        }
        else if(domination == 0)
            status[1]++;
        else if(domination == -1){
            status[2]++;
            nSol = (int) paretoFront.size();
        }
        else if(domination == 11)
            status[3] = 1;
    }
    
    /**
     * status[3] is to avoid to add solutions with the same objective values in the front, remove it if repeated objective values are requiered.
     */
    //if(status[0] > 0 || status[1] == this->paretoFront.size() || status[2] == 0){
    if((status[3] == 0) && (paretoFront.size() == 0 || status[0] > 0 || status[1] == paretoFront.size() || status[2] == 0)){
        
        Solution * copyOfSolution = new Solution(solution->totalObjectives, solution->totalVariables);
        
        this->grid.set(copyOfSolution, x, y);
        delete[] status;
        return 1;
    }
    
    delete [] status;
    return  0;
}

