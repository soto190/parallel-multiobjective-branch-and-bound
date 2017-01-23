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
    int r = 0, c = 0;
    this->active_nodes = new int[rows];
    this->max_nodes_in_level = new int[rows];
    this->ivm = new int * [rows];
    
    for (r = 0; r < rows; r++){
        this->ivm[r] = new int[cols];
        this->active_nodes[r] = -1;
        this->max_nodes_in_level[r] = 0;
        for (c = 0; c < cols; c++)
            this->ivm[r][c] = -1;
    }
    
    this->active_level = 0;
    this->active_nodes[0] = 0;
    this->start_exploration = new int[rows];
    this->end_exploration = new int[rows];
}

IVMTree::~IVMTree(){
    delete [] active_nodes;
    delete [] max_nodes_in_level;
    int r = 0;
    for (r = 0; r < rows; r++)
        delete [] this->ivm[r];
    
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

void IVMTree::setExplorationInterval(int *starts, int *ends){
    int level = 0;
    for (level = 0; level < this->rows; level++) {
        this->active_nodes[level] = starts[level];
        this->end_exploration[level] = ends[level];
    }
}

int IVMTree::hasPendingBranches(){
    
    /*
    for (level = 0; level < this->rows; level++)
        if (this->active_nodes[level] < this->end_exploration[level])
            level = this->rows;
        else
            hasBranches = 0;
    */
    
    if (this->active_nodes[0] == -1)
        this->hasBranches = 0;
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
    return this->ivm[active_level][active_nodes[active_level]];
}

int IVMTree::getActiveNode(){
    return this->ivm[active_level][active_nodes[active_level]];
}

int IVMTree::getFatherNode(){
    return this->ivm[active_level - 1][active_nodes[active_level - 1]];
}

/** Prune the active node. **/
int IVMTree::pruneActiveNode(){
    this->ivm[active_level][active_nodes[active_level]] = -1;
    this->max_nodes_in_level[active_level]--;
    
    if(max_nodes_in_level[active_level] > 0){
        this->active_nodes[active_level]++;
        return this->ivm[active_level][active_nodes[active_level]];
    }
    
    while (max_nodes_in_level[active_level] == 0) {
        this->active_nodes[active_level] = -1;
        /** Go to father node. **/
        this->active_level--;
        
        /** If is the first level of the tree*/
        if (active_level == -1) {
            active_level = 0;
            this->max_nodes_in_level[active_level]--;
            this->hasBranches = 0;
            return this->ivm[active_level][active_nodes[0]];
        }
        
        /** Prune father node. **/
        this->ivm[active_level][active_nodes[active_level]] = -1;
        this->max_nodes_in_level[active_level]--;
        /** Move to next node. **/
        this->active_nodes[active_level]++;
        
    }

    return this->ivm[active_level][active_nodes[active_level]];
}

void IVMTree::showIVM(){
    
    int r = 0, c = 0;
    
    for (r = 0; r < this->rows; r++) {
        printf("[%3d] %3d | ", r, this->active_nodes[r]);
        for (c = 0; c < this->cols; c++)
            printf("%3d", this->ivm[r][c]);
        if (this->active_level == r)
            printf("|*\n");
        else
            printf("|\n");
    }
}



