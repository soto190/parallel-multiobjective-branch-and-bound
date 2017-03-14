//
//  IVMTree.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 16/01/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "IVMTree.hpp"

IVMTree::IVMTree() {
    
    this->whoIam = -1;
    this->rows = 1;
    this->cols = 1;
    this->hasBranches = 0;
    this->root_node = 0;
    this->starting_level = 0;
    this->active_level = 0;
    this->active_node = new int[rows];
    this->max_nodes_in_level = new int[rows];
    this->start_exploration = new int[rows];
    this->end_exploration = new int[rows];

    this->ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; r++) {
        this->active_node[r] = -1;
        this->max_nodes_in_level[r] = 0;
        this->start_exploration[r] = 0;
        this->end_exploration[r] = this->cols;
        
        this->ivm[r] = new int[cols];
        
        for (c = 0; c < cols; c++)
            this->ivm[r][c] = -1;
    }
}

IVMTree::IVMTree(int rows, int cols) {

    this->rows = rows;
    this->cols = cols;
    this->hasBranches = 1;
    this->root_node = 0;
    this->starting_level = 0;
    this->active_level = 0;
    this->active_node = new int[rows];
    this->max_nodes_in_level = new int[rows];
    this->start_exploration = new int[rows];
    this->end_exploration = new int[rows];
    this->ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; r++) {
        
        this->active_node[r] = -1;
        this->max_nodes_in_level[r] = 0;
        this->start_exploration[r] = 0;
        this->end_exploration[r] = this->cols;
        
        this->ivm[r] = new int[cols];
        for (c = 0; c < cols; c++)
            this->ivm[r][c] = -1;
    }
    
}



IVMTree::IVMTree(const IVMTree& toCopy) {
    
    this->whoIam = toCopy.whoIam;
    this->rows = toCopy.rows;
    this->cols = toCopy.cols;
    this->hasBranches = toCopy.hasBranches;
    this->root_node = toCopy.root_node;
    this->starting_level = toCopy.starting_level;
    this->active_level = toCopy.active_level;
    
    this->active_node = new int[rows];
    this->max_nodes_in_level = new int[rows];
    this->start_exploration = new int[rows];
    this->end_exploration = new int[rows];
    
    this->ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; r++) {
        this->active_node[r] = toCopy.active_node[r];
        this->max_nodes_in_level[r] = toCopy.max_nodes_in_level[r];
        this->start_exploration[r] = toCopy.start_exploration[r];
        this->end_exploration[r] = toCopy.end_exploration[r];
        
        this->ivm[r] = new int[cols];
        
        for (c = 0; c < cols; c++)
            this->ivm[r][c] = toCopy.ivm[r][c];
    }
}

IVMTree& IVMTree::operator=(const IVMTree &toCopy){

    this->rows = toCopy.rows;
    this->cols = toCopy.cols;
    this->hasBranches = toCopy.hasBranches;
    this->root_node = toCopy.root_node;
    this->starting_level = toCopy.starting_level;
    this->active_level = toCopy.active_level;
    this->whoIam = toCopy.whoIam;
    
    this->active_node = new int[rows];
    this->max_nodes_in_level = new int[rows];
    this->start_exploration = new int[rows];
    this->end_exploration = new int[rows];
    
    this->ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; r++) {
        this->active_node[r] = toCopy.active_node[r];
        this->max_nodes_in_level[r] = toCopy.max_nodes_in_level[r];
        this->start_exploration[r] = toCopy.start_exploration[r];
        this->end_exploration[r] = toCopy.end_exploration[r];
        
        this->ivm[r] = new int[cols];
        
        for (c = 0; c < cols; c++)
            this->ivm[r][c] = toCopy.ivm[r][c];
    }
    
    return *this;
}

IVMTree& IVMTree::operator()(int rows, int cols) {

    this->rows = rows;
    this->cols = cols;
    this->hasBranches = 1;
    this->root_node = 0;
    this->starting_level = 0;
    this->active_level = 0;
    this->active_node = new int[rows];
    this->max_nodes_in_level = new int[rows];
    this->start_exploration = new int[rows];
    this->end_exploration = new int[rows];
    this->ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; r++) {
        this->ivm[r] = new int[cols];
        this->active_node[r] = -1;
        this->max_nodes_in_level[r] = 0;
        for (c = 0; c < cols; c++)
            this->ivm[r][c] = -1;
    }
    
    return *this;
}

IVMTree::~IVMTree() {
        
    delete[] active_node;
    delete[] max_nodes_in_level;
    delete[] start_exploration;
    delete[] end_exploration;
    
    int r = 0;
    for (r = 0; r < rows; r++)
        delete[] this->ivm[r];
    
    delete[] ivm;
}

void IVMTree::setRootNode(int node){ this->root_node = node; }
void IVMTree::setIVMValueAt(int row, int col, int value){ this->ivm[row][col] = value; }
void IVMTree::setActiveNodeAt(int row, int value){ this->active_node[row] = value; }
void IVMTree::setStartingLevel(int row){ this->starting_level = row; }
void IVMTree::setStartExploration(int row, int value){ this->start_exploration[row] = value; }
void IVMTree::setEndExploration(int row, int value){ this->end_exploration[row] = value; }
void IVMTree::setNumberOfNodesAt(int row, int value){ this->max_nodes_in_level[row] = value; }
int IVMTree::increaseNodesAt(int row){ return this->max_nodes_in_level[row]++; }
int IVMTree::decreaseNodesAt(int row){ return this->max_nodes_in_level[row]--; }
void IVMTree::resetNumberOfNodesAt(int row) { this->max_nodes_in_level[row] = 0;}
void IVMTree::setHasBranches(int itHas){ this->hasBranches = itHas;}
int IVMTree::getRootNode() const{ return this->root_node; }
int IVMTree::getNumberOfNodesAt(int row) const{ return this->max_nodes_in_level[row]; }
int IVMTree::getIVMValue(int row, int col) const{ return this->ivm[row][col]; }
int IVMTree::getActiveNode(int row) const{ return this->active_node[row]; }
int IVMTree::getActiveLevel() const{ return this->active_level; }
int IVMTree::getStartingLevel() const{ return this->starting_level; }
int IVMTree::getStartExploration(int row)const{ return this->start_exploration[row]; }
int IVMTree::getEndExploration(int row) const{ return this->end_exploration[row]; }


int IVMTree::getNumberOfRows() const {
    return this->rows;
}

int IVMTree::getNumberOfCols() const {
    return this->cols;
}

int IVMTree::getTreeDeep() const {
    return this->rows;
}

void IVMTree::setOwner(int idBB) {
    this->whoIam = idBB;
}

int IVMTree::getOwner() const {
    return this->whoIam;
}

void IVMTree::setActiveLevel(int level) {
    this->active_level = level;
}

/**
 * Initialize th IVM with the given interval.
 *
 */
void IVMTree::setExplorationInterval(int starting_level, int *starts,
                                     int *ends) {
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

int IVMTree::hasPendingBranches() const {
    return this->hasBranches;
}

int IVMTree::getCurrentLevel() const {
    return this->active_level;
}

void IVMTree::setNode(int level, int value) {
    this->ivm[level][this->max_nodes_in_level[level]] = value;
    this->max_nodes_in_level[level]++;
}

int IVMTree::moveToNextLevel() {
    return this->active_level++;
}

int IVMTree::moveToNextNode() {
    return this->ivm[active_level][active_node[active_level]];
}

int IVMTree::getActiveNode() const {
    return this->ivm[active_level][active_node[active_level]];
}

int IVMTree::getFatherNode() const {
    return this->ivm[active_level - 1][active_node[active_level - 1]];
}

/** Prune the active node and set the active_level pointing a new active node. **/
int IVMTree::pruneActiveNode() {
    
    
    this->ivm[active_level][active_node[active_level]] = -1; /** Marks the node as removed. **/
    this->max_nodes_in_level[active_level]--; /** Reduces the number of nodes in the active level. **/
    
    if (max_nodes_in_level[active_level] > 0) { /** If there are more nodes in the active level, then move to the next node. **/
        
        /** TODO: Re-think if we always move to the right or if we can search for another node. **/
        
        this->active_node[active_level]++; /** Moves to the node of the right. **/
        return this->ivm[active_level][active_node[active_level]];
    }
    
    /** If the active level doesn't have more nodes then move to the father node while there are pending nodes **/
    while (max_nodes_in_level[active_level] == 0 && active_level > root_node) { /** TODO: Check, why max_nodes_in_level reach -1, the minimun should be 0. **/
        
        this->active_node[active_level] = -1; /** Mark the level to indicate that there are no more pending nodes. **/
        this->active_level--; /** Go to father node. **/
        
        this->ivm[active_level][active_node[active_level]] = -1; /** Prune the father node. **/
        this->max_nodes_in_level[active_level]--; /** Reduce the number of nodes. **/
        
        if (max_nodes_in_level[active_level] > 0) {
            this->active_node[active_level]++; /** Move to the next node. **/
            return this->ivm[active_level][active_node[active_level]]; /** Return the active node. **/
        }
    }
    this->hasBranches = 0; /** There are no more branches. **/
    return this->ivm[active_level][active_node[active_level]];
}

void IVMTree::showIVM() {
    
    int r = 0, c = 0;
    
    char sep = '-';
    /**
     * AC: Active column.
     * AN: Active node.
     * NN: Number of nodes.
     */
    printf("[Row]\tAC\tAN\tNN\n");
    for (r = 0; r < this->rows; r++) {
        /** The integer vector. **/
        if (this->active_node[r] == -1)
            printf("[%3d] %3c ", r, sep);
        else
            printf("[%3d] %3d ", r, this->active_node[r]);
        
        /** The solution. **/
        if (this->active_node[r] == -1
            || this->ivm[r][this->active_node[r]] == -1)
            printf(" %3c ", sep);
        else
            printf(" %3d ", this->ivm[r][this->active_node[r]]);
        
        /** Max nodes in level. **/
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

