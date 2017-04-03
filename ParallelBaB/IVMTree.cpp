//
//  IVMTree.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 16/01/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "IVMTree.hpp"

IVMTree::IVMTree() {
    
    whoIam = -1;
    rows = 1;
    cols = 1;
    hasBranches = 0;
    root_node = 0;
    starting_level = 0;
    active_level = 0;

}

IVMTree::IVMTree(int n_rows, int n_cols) {

    rows = n_rows;
    cols = n_cols;
    hasBranches = 1;
    root_node = 0;
    starting_level = 0;
    active_level = 0;
    active_node = new int[rows];
    max_nodes_in_level = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; ++r) {
        
        active_node[r] = -1;
        max_nodes_in_level[r] = 0;
        start_exploration[r] = 0;
        end_exploration[r] = cols;
        
        ivm[r] = new int[cols];
        for (c = 0; c < cols; ++c)
            ivm[r][c] = -1;
    }
}

IVMTree::IVMTree(const IVMTree& toCopy) {
    
    whoIam = toCopy.whoIam;
    rows = toCopy.rows;
    cols = toCopy.cols;
    hasBranches = toCopy.hasBranches;
    root_node = toCopy.root_node;
    starting_level = toCopy.starting_level;
    active_level = toCopy.active_level;
    
    active_node = new int[rows];
    max_nodes_in_level = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    
    ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; ++r) {
        active_node[r] = toCopy.active_node[r];
        max_nodes_in_level[r] = toCopy.max_nodes_in_level[r];
        start_exploration[r] = toCopy.start_exploration[r];
        end_exploration[r] = toCopy.end_exploration[r];
        
        ivm[r] = new int[cols];
        
        for (c = 0; c < cols; ++c)
            ivm[r][c] = toCopy.ivm[r][c];
    }
}

IVMTree& IVMTree::operator=(const IVMTree &toCopy){

    rows = toCopy.rows;
    cols = toCopy.cols;
    hasBranches = toCopy.hasBranches;
    root_node = toCopy.root_node;
    starting_level = toCopy.starting_level;
    active_level = toCopy.active_level;
    whoIam = toCopy.whoIam;
    
    active_node = new int[rows];
    max_nodes_in_level = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    
    ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; ++r) {
        active_node[r] = toCopy.active_node[r];
        max_nodes_in_level[r] = toCopy.max_nodes_in_level[r];
        start_exploration[r] = toCopy.start_exploration[r];
        end_exploration[r] = toCopy.end_exploration[r];
        
        ivm[r] = new int[cols];
        
        for (c = 0; c < cols; ++c)
            ivm[r][c] = toCopy.ivm[r][c];
    }
    
    return *this;
}

IVMTree& IVMTree::operator()(int n_rows, int n_cols) {

    rows = n_rows;
    cols = n_cols;
    hasBranches = 1;
    root_node = 0;
    starting_level = 0;
    active_level = 0;
    active_node = new int[rows];
    max_nodes_in_level = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; ++r) {
        ivm[r] = new int[cols];
        active_node[r] = -1;
        max_nodes_in_level[r] = 0;
        for (c = 0; c < cols; ++c)
            ivm[r][c] = -1;
    }
    
    return *this;
}

IVMTree::~IVMTree() {
        
    delete[] active_node;
    delete[] max_nodes_in_level;
    delete[] start_exploration;
    delete[] end_exploration;
    
    int r = 0;
    for (r = 0; r < rows; ++r)
        delete[] ivm[r];
    
    delete[] ivm;
}

void IVMTree::setRootNode(int node){ root_node = node; }
void IVMTree::setIVMValueAt(int row, int col, int value){ ivm[row][col] = value; }
void IVMTree::setActiveNodeAt(int row, int value){ active_node[row] = value; }
void IVMTree::setStartingLevel(int row){ starting_level = row; }
void IVMTree::setStartExploration(int row, int value){ start_exploration[row] = value; }
void IVMTree::setEndExploration(int row, int value){ end_exploration[row] = value; }
void IVMTree::setNumberOfNodesAt(int row, int value){ max_nodes_in_level[row] = value; }
int IVMTree::increaseNodesAt(int row){ return max_nodes_in_level[row]++; }
int IVMTree::decreaseNodesAt(int row){ return max_nodes_in_level[row]--; }
void IVMTree::resetNumberOfNodesAt(int row) { max_nodes_in_level[row] = 0;}
void IVMTree::setHasBranches(int itHas){ hasBranches = itHas;}
int IVMTree::getRootNode() const{ return root_node; }
int IVMTree::getNumberOfNodesAt(int row) const{ return max_nodes_in_level[row]; }
int IVMTree::getIVMValue(int row, int col) const{ return ivm[row][col]; }
int IVMTree::getActiveNode(int row) const{ return active_node[row]; }
int IVMTree::getActiveLevel() const{ return active_level; }
int IVMTree::getStartingLevel() const{ return starting_level; }
int IVMTree::getStartExploration(int row)const{ return start_exploration[row]; }
int IVMTree::getEndExploration(int row) const{ return end_exploration[row]; }
int IVMTree::getNumberOfRows() const { return rows; }
int IVMTree::getNumberOfCols() const { return cols; }
int IVMTree::getTreeDeep() const { return rows; }
int IVMTree::getOwner() const {return whoIam;}

void IVMTree::setActiveLevel(int level) { active_level = level; }
void IVMTree::setOwner(int idBB) { whoIam = idBB;}

/**
 * Initialize th IVM with the given interval.
 *
 */
void IVMTree::setExplorationInterval(int set_starting_level, int *starts,
                                     int *ends) {
    int level = 0;
    starting_level = set_starting_level;
    active_level = set_starting_level - 1;
    
    for (level = 0; level < rows; ++level) {
        active_node[level] = starts[level];
        start_exploration[level] = starts[level];
        end_exploration[level] = ends[level];
        max_nodes_in_level[level] = 0;
        ivm[level][starts[level]] = starts[level];
    }
    
}

void IVMTree::setNode(int level, int value) {
    ivm[level][max_nodes_in_level[level]] = value;
    max_nodes_in_level[level]++;
}

int IVMTree::hasPendingBranches() const { return hasBranches; }
int IVMTree::getCurrentLevel() const { return active_level; }
int IVMTree::moveToNextLevel() { return active_level++; }
int IVMTree::moveToNextNode() { return ivm[active_level][active_node[active_level]]; }
int IVMTree::getActiveNode() const { return ivm[active_level][active_node[active_level]];}
int IVMTree::getFatherNode() const { return ivm[active_level - 1][active_node[active_level - 1]]; }

/** Prune the active node and set the active_level pointing a new active node. **/
int IVMTree::pruneActiveNode() {
    
    ivm[active_level][active_node[active_level]] = -1; /** Marks the node as removed. **/
    max_nodes_in_level[active_level]--; /** Reduces the number of nodes in the active level. **/
    
    if (max_nodes_in_level[active_level] > 0) { /** If there are more nodes in the active level, then move to the next node. **/
        
        /** TODO: Re-think if we always move to the right or if we can search for another node. **/
        
        active_node[active_level]++; /** Moves to the node of the right. **/
        return ivm[active_level][active_node[active_level]];
    }
    
    /** If the active level doesn't have more nodes then move to the father node while there are pending nodes **/
    while (max_nodes_in_level[active_level] == 0 && active_level > root_node) { /** TODO: Check, why max_nodes_in_level reach -1, the minimun should be 0. **/
        
        active_node[active_level] = -1; /** Mark the level to indicate that there are no more pending nodes. **/
        active_level--; /** Go to father node. **/
        
        ivm[active_level][active_node[active_level]] = -1; /** Prune the father node. **/
        max_nodes_in_level[active_level]--; /** Reduce the number of nodes. **/
        
        if (max_nodes_in_level[active_level] > 0) {
            active_node[active_level]++; /** Move to the next node. **/
            return ivm[active_level][active_node[active_level]]; /** Return the active node. **/
        }
    }
    hasBranches = 0; /** There are no more branches. **/
    return ivm[active_level][active_node[active_level]];
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
    for (r = 0; r < rows; ++r) {
        /** The integer vector. **/
        if (active_node[r] == -1)
            printf("[%3d] %3c ", r, sep);
        else
            printf("[%3d] %3d ", r, active_node[r]);
        
        /** The solution. **/
        if (active_node[r] == -1
            || ivm[r][active_node[r]] == -1)
            printf(" %3c ", sep);
        else
            printf(" %3d ", ivm[r][active_node[r]]);
        
        /** Max nodes in level. **/
        printf(" %3d | ", max_nodes_in_level[r]);
        
        /** The matrix. **/
        for (c = 0; c < cols; ++c)
            if (ivm[r][c] == -1)
                printf("%3c", sep);
            else
                printf("%3d", ivm[r][c]);
        
        /** The active level. **/
        if (active_level == r)
            printf("|*\n");
        else
            printf("|\n");
    }
}

