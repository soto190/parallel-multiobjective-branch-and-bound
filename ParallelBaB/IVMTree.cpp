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
    root_row = 0;
    starting_row = 0;
    active_row = 0;

}

IVMTree::IVMTree(int n_rows, int n_cols) {

    rows = n_rows;
    cols = n_cols;
    hasBranches = 1;
    root_row = 0;
    starting_row = 0;
    active_row = 0;
    active_column = new int[rows];
    n_nodes_at_row = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; ++r) {
        
        active_column[r] = -1;
        n_nodes_at_row[r] = 0;
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
    root_row = toCopy.root_row;
    starting_row = toCopy.starting_row;
    active_row = toCopy.active_row;
    
    active_column = new int[rows];
    n_nodes_at_row = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    
    ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; ++r) {
        active_column[r] = toCopy.active_column[r];
        n_nodes_at_row[r] = toCopy.n_nodes_at_row[r];
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
    root_row = toCopy.root_row;
    starting_row = toCopy.starting_row;
    active_row = toCopy.active_row;
    whoIam = toCopy.whoIam;
    
    active_column = new int[rows];
    n_nodes_at_row = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    
    ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; ++r) {
        active_column[r] = toCopy.active_column[r];
        n_nodes_at_row[r] = toCopy.n_nodes_at_row[r];
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
    root_row = 0;
    starting_row = 0;
    active_row = 0;
    active_column = new int[rows];
    n_nodes_at_row = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    ivm = new int *[rows];
    
    int r = 0, c = 0;
    
    for (r = 0; r < rows; ++r) {
        ivm[r] = new int[cols];
        active_column[r] = -1;
        n_nodes_at_row[r] = 0;
        for (c = 0; c < cols; ++c)
            ivm[r][c] = -1;
    }
    
    return *this;
}

IVMTree::~IVMTree() {
        
    delete[] active_column;
    delete[] n_nodes_at_row;
    delete[] start_exploration;
    delete[] end_exploration;
    
    for (int r = 0; r < rows; ++r)
        delete[] ivm[r];
    delete[] ivm;
}

void IVMTree::setRootRow(int row){ root_row = row; }
void IVMTree::setIVMValueAt(int row, int col, int value){ ivm[row][col] = value; }
void IVMTree::setActiveNodeAt(int row, int value){ active_column[row] = value; }
void IVMTree::setStartingRow(int row){ starting_row = row; }
void IVMTree::setStartExploration(int row, int value){ start_exploration[row] = value; }
void IVMTree::setEndExploration(int row, int value){ end_exploration[row] = value; }
void IVMTree::setNumberOfNodesAt(int row, int value){ n_nodes_at_row[row] = value; }
int IVMTree::increaseNodesAt(int row){ return n_nodes_at_row[row]++; }
int IVMTree::decreaseNodesAt(int row){ return n_nodes_at_row[row]--; }
void IVMTree::resetNumberOfNodesAt(int row) { n_nodes_at_row[row] = 0;}
void IVMTree::setHasBranches(int itHas){ hasBranches = itHas;}
int IVMTree::getRootNode() const{ return ivm[root_row][active_column[root_row]];}
int IVMTree::getRootRow() const { return root_row;}
int IVMTree::getNumberOfNodesAt(int row) const{ return n_nodes_at_row[row]; }
int IVMTree::getIVMValue(int row, int col) const{ return ivm[row][col]; }
int IVMTree::getActiveColAt(int row) const{ return active_column[row]; }
int IVMTree::getActiveRow() const{ return active_row; }
int IVMTree::getStartingRow() const{ return starting_row; }
int IVMTree::getStartExploration(int row)const{ return start_exploration[row]; }
int IVMTree::getEndExploration(int row) const{ return end_exploration[row]; }
int IVMTree::getNumberOfRows() const { return rows; }
int IVMTree::getNumberOfCols() const { return cols; }
int IVMTree::getTreeDeep() const { return rows; }
int IVMTree::getOwner() const {return whoIam;}

/** TODO: Delete unused function. **/
int IVMTree::getLastNodeAtRow(int row) const{ return ivm[row][n_nodes_at_row[row] - 1];}
int IVMTree::removeLastNodeAtRow(int row){
    int last_col = active_column[row] + n_nodes_at_row[row] - 1;
    if (ivm[row][last_col] == -1) {
        printf("DEBUG: %d\n",last_col);
    }
    
    if (active_column[row] == -1)
        last_col = n_nodes_at_row[row];
    
    int node_at_col = ivm[row][last_col];
    ivm[row][last_col] = -1;
    n_nodes_at_row[row]--;
    end_exploration[row]--;
    return node_at_col;
}

void IVMTree::setActiveRow(int level) { active_row = level; }
void IVMTree::setOwner(int idBB) { whoIam = idBB;}

/**
 * Initialize th IVM with the given interval.
 * TODO: Delete unused function.
 */
void IVMTree::setExplorationInterval(int set_starting_level, int *starts,
                                     int *ends) {
    int level = 0;
    starting_row = set_starting_level;
    active_row = set_starting_level - 1;
    
    for (level = 0; level < rows; ++level) {
        active_column[level] = starts[level];
        start_exploration[level] = starts[level];
        end_exploration[level] = ends[level];
        n_nodes_at_row[level] = 0;
        ivm[level][starts[level]] = starts[level];
    }
    
}

/** TODO: Check this function. **/
void IVMTree::setNode(int row, int value) {
    ivm[row][active_column[row] + n_nodes_at_row[row] + 1] = value;
    n_nodes_at_row[row]++;
}

int IVMTree::hasPendingBranches() const { return hasBranches; }
int IVMTree::moveToNextRow() { return active_row++; }
int IVMTree::moveToNextNode() { return ivm[active_row][active_column[active_row]]; }
int IVMTree::getActiveNode() const { return ivm[active_row][active_column[active_row]];}
int IVMTree::getFatherNode() const { return root_row == 0 ? ivm[active_row - 1][active_column[active_row - 1]] : -1; }

/** Prune the active node and set the active_level pointing a new active node. **/
int IVMTree::pruneActiveNode() {

    ivm[active_row][active_column[active_row]] = -1; /** Marks the node as removed. **/
    n_nodes_at_row[active_row]--; /** Reduces the number of nodes in the active level. **/
    
    if (n_nodes_at_row[active_row] > 0) { /** If there are more nodes in the active level, then move to the next node. **/
        
        /** TODO: Re-think if we always move to the right or if we can search for another node. **/
        
        active_column[active_row]++; /** Moves to the node of the right. **/
        return ivm[active_row][active_column[active_row]];
    }
    
    /** If the active level doesn't have more nodes then move to the father node while there are pending nodes **/
    while (n_nodes_at_row[active_row] == 0 && active_row - 1 > root_row) {
        
        active_column[active_row] = -1; /** Mark the level to indicate that there are no more pending nodes. **/
        active_row--; /** Move to father node. **/
        ivm[active_row][active_column[active_row]] = -1; /** Prune the node because it doesnt has more child. **/
        n_nodes_at_row[active_row]--; /** Reduce the number of nodes. **/
        
        if (n_nodes_at_row[active_row] > 0) { /** If there are more nodes then move to the next node. **/
            active_column[active_row]++; /** Move to the next node. **/
            return ivm[active_row][active_column[active_row]]; /** Return the active node. **/
        }
    }
    active_column[active_row] = -1;
    active_row = root_row;
    hasBranches = 0; /** There are no more branches. **/
    return ivm[active_row][active_column[active_row]];
}

void IVMTree::print() {
    
    int r = 0, c = 0;
    
    char sep = '-';
    /**
     * I: Index column.
     * N: Active node.
     * #: Number of nodes.
     */
    printf("[Row]\tS\tI\t N\t  #\n");
    for (r = 0; r < rows; ++r) {
        /** The integer vector. **/
        if (active_column[r] == -1 && start_exploration[r] == -1)
            printf("[%3d] %3c %3c ", r, sep, sep);
        else if (active_column[r] == -1 && start_exploration[r] > -1)
            printf("[%3d] %3d %3c ", r, start_exploration[r], sep);
        else if (active_column[r] > -1 && start_exploration[r] == -1)
            printf("[%3d] %3c %3d ", r, sep, active_column[r]);
        else
            printf("[%3d] %3d %3d ", r, start_exploration[r], active_column[r]);
        
        /** The solution. **/
        if (active_column[r] == -1
            || ivm[r][active_column[r]] == -1)
            printf(" %3c ", sep);
        else
            printf(" %3d ", ivm[r][active_column[r]]);
        
        /** Max nodes in row. **/
        printf(" %3d | ", n_nodes_at_row[r]);
        
        /** The matrix. **/
        for (c = 0; c < cols; ++c)
            if (ivm[r][c] == -1)
                printf("%3c", sep);
            else
                printf("%3d", ivm[r][c]);
        
        /** The active row. **/
        printf("|");
        if (r == root_row)
            printf("r");
        if (r == active_row)
            printf("*");
        printf("\n");
    }
}

