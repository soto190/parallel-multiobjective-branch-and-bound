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
    int r = 0, c = 0;
    this->active_nodes = new int[rows];
    this->max_in_level = new int[rows];
    this->ivm = new int * [rows];
    
    for (r = 0; r < rows; r++){
        this->ivm[r] = new int[cols];
        this->active_nodes[r] = 0;
        this->max_in_level[r] = 0;
        for (c = 0; c < cols; c++)
            this->ivm[r][c] = -1;
    }
    
    this->active_level = 0;
    this->limit_exploration = new int[rows];
}

IVMTree::~IVMTree(){
    delete [] active_nodes;
    delete [] max_in_level;
    int r = 0;
    for (r = 0; r < rows; r++)
        delete [] this->ivm[r];
    delete [] ivm;
}

int IVMTree::getNumberOfRows(){
    return this->rows;
}

int IVMTree::getNumberOfCols(){
    return this->cols;
}

void IVMTree::setActiveLevel(int level){
    this->active_level = level;
}

int IVMTree::hasPendingBranches(){
    
    int hasBranches = 1;
    
    int level = 0;
    for (level = 0; level < this->rows; level++)
        if (this->active_nodes[level] < this->limit_exploration[level])
            level = this->rows;
        else
            hasBranches = 0;
    
    return hasBranches;
}

int IVMTree::getCurrentLevel(){
    return this->active_level;
}

void IVMTree::setNode(int level, int value){
    this->ivm[level][value] = value;
}

int IVMTree::moveToNextLevel(){
    return this->active_level++;
}

int IVMTree::moveToNextNode(){
    return this->active_nodes[active_level]++;
}

int IVMTree::getActiveNode(){
    return this->ivm[active_level][active_nodes[active_level]];
}

int IVMTree::getNextNode(){
    
    while (active_nodes[active_level] + 1 >= this->max_in_level[active_level])
        active_level--;
    
    return this->ivm[active_level][active_nodes[active_level]++];
}

int IVMTree::getFatherNode(){
    return this->ivm[active_level - 1][active_nodes[active_level - 1]];
}

int IVMTree::pruneActiveNode(){
    this->ivm[active_level][active_nodes[active_level]] = -1;
    this->getNextNode();
    return 1;
}

void IVMTree::setMaxValueInLevel(int level, int value){
    this->max_in_level[level] = value;
}


