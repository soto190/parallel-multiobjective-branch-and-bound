//
//  IVMTree.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 16/01/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "IVMTree.hpp"


IVMTree::IVMTree(){
}

IVMTree::IVMTree(int rows, int cols){

    this->rows = rows;
    this->cols = cols;
    this->hasBranches = 1;
    this->active_node = new int[rows];
    this->max_nodes_in_level = new int[rows];
    this->ivm = new int * [rows];
    
    int r = 0, c = 0;

    for (r = 0; r < rows; r++){
        this->ivm[r] = new int[cols];
        this->active_node[r] = -1;
        this->max_nodes_in_level[r] = 0;
        for (c = 0; c < cols; c++)
            this->ivm[r][c] = -1;
    }
    
    this->starting_level = 0;
    this->active_level = 0;
    this->start_exploration = new int[rows];
    this->end_exploration = new int[rows];
}

IVMTree::~IVMTree(){
    
    int r = 0;
    for (r = 0; r < rows; r++)
        delete [] this->ivm[r];
    
    delete [] active_node;
    delete [] max_nodes_in_level;
    delete [] ivm;
    delete [] start_exploration;
    delete [] end_exploration;
}

int IVMTree::getNumberOfRows(){
    return this->rows;
}

int IVMTree::getNumberOfCols(){
    return this->cols;
}

int IVMTree::getTreeDeep(){
    return this->rows;
}

void IVMTree::setActiveLevel(int level){
    this->active_level = level;
}

/**
 * Initialize th IVM with the given interval.
 *
 */
void IVMTree::setExplorationInterval(int starting_level, int *starts, int *ends){
    int level = 0;
    this->starting_level = starting_level;
    this->active_level = starting_level - 1;

    for (level = 0; level < this->rows; level++) {
        this->active_node[level] = starts[level];
        this->start_exploration[level] = starts[level];
        this->end_exploration[level] = ends[level];
        this->max_nodes_in_level[level] = 0;
        this->ivm[level][starts[level]] = starts[level];
    }
    
}

int IVMTree::hasPendingBranches(){
    return this->hasBranches;
}

int IVMTree::getCurrentLevel(){
    return this->active_level;
}

void IVMTree::setNode(int level, int value){
    this->ivm[level][this->max_nodes_in_level[level]] = value;
    this->max_nodes_in_level[level]++;
}

int IVMTree::moveToNextLevel(){
    return this->active_level++;
}

int IVMTree::moveToNextNode(){
    return this->ivm[active_level][active_node[active_level]];
}

int IVMTree::getActiveNode(){
    return this->ivm[active_level][active_node[active_level]];
}

int IVMTree::getFatherNode(){
    return this->ivm[active_level - 1][active_node[active_level - 1]];
}

/** Prune the active node and set the active_level pointing a new active node. **/
int IVMTree::pruneActiveNode(){
    
    this->ivm[active_level][active_node[active_level]] = -1;
    this->max_nodes_in_level[active_level]--;
    
    /** If there are more nodes in the active_level, then move to the next node. **/
    if(max_nodes_in_level[active_level] > 0){
        this->active_node[active_level]++;
        return this->ivm[active_level][active_node[active_level]];
    }
    
    /** while the active level doesnt have nodes **/
    while (max_nodes_in_level[active_level] == 0) {
        
        this->active_node[active_level] = -1;
        
        /** Go to father node. **/
        this->active_level--;
        
        /** If it is the first level of the tree. */
        if (active_level <= starting_level - 1) {
            active_level = 0;
            this->max_nodes_in_level[active_level]--;
            this->hasBranches = 0;
            return this->ivm[active_level][active_node[0]];
        }
        
        /** Prune father node. **/
        this->ivm[active_level][active_node[active_level]] = -1;
        this->max_nodes_in_level[active_level]--;
        
        /** Move to next node. **/
        this->active_node[active_level]++;
        
    }

    return this->ivm[active_level][active_node[active_level]];
}

void IVMTree::showIVM(){
    
    int r = 0, c = 0;
    
    char sep = '-';
    for (r = 0; r < this->rows; r++) {
        /** The integer verctor. **/
        if (this->active_node[r] == -1)
            printf("[%3d] %3c ", r, sep);
        else
            printf("[%3d] %3d ", r, this->active_node[r]);
        
        /** The solution. **/
        if (this->active_node[r] == -1 || this->ivm[r][this->active_node[r]] == -1)
            printf(" %3c ", sep);
        else
            printf(" %3d ", this->ivm[r][this->active_node[r]]);
        
        /** Max nodes in level. **/
        //if (this->active_nodes[r] == -1 || this->ivm[r][this->active_nodes[r]] == -1)
        //  printf(" %3c | ", sep);
        //else
            printf(" %3d | ", this->max_nodes_in_level[r]);
        
        /** The matrix. **/
        for (c = 0; c < this->cols; c++)
            if (this->ivm[r][c] == -1)
                printf("%3c", sep);
            else
                printf("%3d", this->ivm[r][c]);
        
        /** The active level. **/
        if (this->active_level == r)
            printf("|*\n");
        else
            printf("|\n");
    }
}



