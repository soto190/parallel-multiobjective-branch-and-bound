//
//  IVMTree.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 16/01/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "IVMTree.hpp"

IVMTree::IVMTree() {
    rows = 0;
    cols = 0;
    whoIam = -1;
    ivm = nullptr;
    active_column_at_row = nullptr;
    start_exploration = nullptr; /** This is not used. **/
    end_exploration = nullptr; /** This is not used. **/
    active_row = 0;
    starting_row = 0;
    hasBranches = 0;
    root_row = 0; /** Root row. **/
    n_nodes_at_row = nullptr;
    pending_nodes = 0;
}

IVMTree::IVMTree(int n_rows, int n_cols) {
    rows = n_rows;
    cols = n_cols;
    hasBranches = 1;
    root_row = 0;
    starting_row = 0;
    active_row = 0;
    pending_nodes = 0;
    active_column_at_row = new int[rows];
    n_nodes_at_row = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    ivm = new int *[rows];
    
    for (int r = 0; r < rows; ++r) {
        
        active_column_at_row[r] = -1;
        n_nodes_at_row[r] = 0;
        start_exploration[r] = 0;
        end_exploration[r] = cols;
        
        ivm[r] = new int[cols];
        for (int c = 0; c < cols; ++c)
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
    pending_nodes = toCopy.getNumberOfPendingNodes();
    
    active_column_at_row = new int[rows];
    n_nodes_at_row = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    ivm = new int *[rows];
    
    for (int r = 0; r < rows; ++r) {
        active_column_at_row[r] = toCopy.active_column_at_row[r];
        n_nodes_at_row[r] = toCopy.n_nodes_at_row[r];
        start_exploration[r] = toCopy.start_exploration[r];
        end_exploration[r] = toCopy.end_exploration[r];
        pending_nodes += n_nodes_at_row[r];
        ivm[r] = new int[cols];
        
        for (int c = 0; c < cols; ++c)
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
    whoIam = toCopy.getOwner();
    pending_nodes = toCopy.getNumberOfPendingNodes();
    
    active_column_at_row = new int[rows];
    n_nodes_at_row = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    ivm = new int *[rows];
    
    for (int r = 0; r < rows; ++r) {
        active_column_at_row[r] = toCopy.active_column_at_row[r];
        n_nodes_at_row[r] = toCopy.n_nodes_at_row[r];
        start_exploration[r] = toCopy.start_exploration[r];
        end_exploration[r] = toCopy.end_exploration[r];
        pending_nodes += n_nodes_at_row[r];
        ivm[r] = new int[cols];
        
        for (int c = 0; c < cols; ++c)
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
    pending_nodes = 0;
    
    active_column_at_row = new int[rows];
    n_nodes_at_row = new int[rows];
    start_exploration = new int[rows];
    end_exploration = new int[rows];
    ivm = new int *[rows];
    
    for (int r = 0; r < rows; ++r) {
        ivm[r] = new int[cols];
        active_column_at_row[r] = -1;
        n_nodes_at_row[r] = 0;
        for (int c = 0; c < cols; ++c)
            ivm[r][c] = -1;
    }
    return *this;
}

IVMTree::~IVMTree() {
    delete[] active_column_at_row;
    delete[] n_nodes_at_row;
    delete[] start_exploration;
    delete[] end_exploration;
    
    for (int r = 0; r < rows; ++r)
        delete[] ivm[r];
    delete[] ivm;
}

void IVMTree::setRootRow(int row){
    root_row = row;
}

void IVMTree::setValueAt(int row, int col, int value){
    ivm[row][col] = value;
}

void IVMTree::setActiveNodeAt(int row, int value){
    active_column_at_row[row] = value;
}

void IVMTree::setStartingRow(int row){
    starting_row = row;
}

void IVMTree::setStartExploration(int row, int value){
    start_exploration[row] = value;
}

void IVMTree::setEndExploration(int row, int value){
    end_exploration[row] = value;
}

void IVMTree::setNumberOfNodesAt(int row, int value){
    pending_nodes -= n_nodes_at_row[row];
    pending_nodes += value;
    n_nodes_at_row[row] = value;
}

int IVMTree::increaseNodesAt(int row){
    return n_nodes_at_row[row]++;
}

int IVMTree::decreaseNodesAt(int row){
    return n_nodes_at_row[row]--;
}

void IVMTree::resetNumberOfNodesAt(int row) {
    pending_nodes -= n_nodes_at_row[row];
    n_nodes_at_row[row] = 0;
}

void IVMTree::setHasBranches(){
    hasBranches = 1;
}

int IVMTree::getRootNode() const{
    return ivm[root_row][active_column_at_row[root_row]];
}

int IVMTree::getRootRow() const {
    return root_row;
}

int IVMTree::getNumberOfNodesAt(int row) const{
    return n_nodes_at_row[row];
}

int IVMTree::getNodeValue(int row, int col) const{
    return ivm[row][col];
}

int IVMTree::getActiveColAt(int row) const{
    return active_column_at_row[row];
}

int IVMTree::getActiveRow() const{
    return active_row;
}

int IVMTree::getStartingRow() const{
    return starting_row;
}

int IVMTree::getStartExploration(int row)const{
    return start_exploration[row];
}

int IVMTree::getEndExploration(int row) const{
    return end_exploration[row];
}

int IVMTree::getNumberOfRows() const {
    return rows;
}

int IVMTree::getNumberOfCols() const {
    return cols;
}

int IVMTree::getDeepOfTree() const {
    return rows;
}

int IVMTree::getOwner() const {
    return whoIam;
}

/** TODO: Delete unused function. **/
int IVMTree::getLastNodeAtRow(int row) const{
    return ivm[row][n_nodes_at_row[row] - 1];
}

int IVMTree::removeLastNodeAtRow(int row){
    int last_col = getActiveColAt(row) + getNumberOfNodesAt(row) - 1;
    int node_at_col = ivm[row][last_col];
    
    ivm[row][last_col] = -1;
    decreaseNodesAt(row);
    decreaseEndExplorationAtRow(row);
    decreaseNumberOfPendingNodes();
    return node_at_col;
}

int IVMTree::decreaseEndExplorationAtRow(int row){
    return end_exploration[row]--;
}

void IVMTree::setActiveRow(int level) {
    active_row = level;
}

void IVMTree::setOwner(int idBB) {
    whoIam = idBB;
}

/**
 * Initialize th IVM with the given interval.
 * TODO: Delete unused function.
 */
void IVMTree::setExplorationInterval(int starting_level, int *starts, int *ends) {
    starting_row = starting_level;
    active_row = starting_level - 1;
    for (int level = 0; level < rows; ++level) {
        active_column_at_row[level] = starts[level];
        start_exploration[level] = starts[level];
        end_exploration[level] = ends[level];
        n_nodes_at_row[level] = 0;
        ivm[level][starts[level]] = starts[level];
    }
}

/** TODO: Check this function.
 * Because active_column[row] is initialized with -1 it needs the +1 to store the next value.
 **/
void IVMTree::setNodeAtRow(int row, int node_value) {
    setNodeAtNextFreeNodeAtRow(row, node_value);
    increaseNodesAt(row);
    increaseNumberOfPendingNodes();
}

void IVMTree::setNodeAtNextFreeNodeAtRow(int row, int node_value){
    int next_free_node = getNextFreeNodeAtRow(row);
    ivm[row][next_free_node] = node_value;
}

int IVMTree::getNextFreeNodeAtRow(int row){
    return getActiveColAt(row) + getNumberOfNodesAt(row) + 1;
}

int IVMTree::hasPendingBranches() const {
    return hasBranches;
}

int IVMTree::moveToNextRow() {
    return active_row++;
}

int IVMTree::moveToNextNode() {
    return ivm[active_row][active_column_at_row[active_row]];
}

int IVMTree::getActiveNode() const {
    return ivm[active_row][active_column_at_row[active_row]];
}

int IVMTree::getFatherNode() const {
    return root_row == 0 ? ivm[active_row - 1][active_column_at_row[active_row - 1]] : -1;
}

unsigned long IVMTree::getNumberOfPendingNodes() const{
    return pending_nodes;
}

/** Prune the active node and set the active_level pointing to new active node. **/
int IVMTree::pruneActiveNode() {
    /** TODO if the active node is the root node we cannot remove it.**/
    if (isRootRow())
        setNoMoreBranches();

    removeActiveNode();
    if (thereAreMoreNodes()) {
        moveToNodeAtRight();
        return getActiveNode();
    }
    
    /** If the active level doesn't have more nodes then move to the father node while there are pending nodes **/
    while (isUnderRootRow()) {
        removeRow();
        moveToFatherRow();
        if (!isRootRow()){
            removeActiveNode();
            if (thereAreMoreNodes()) {
                moveToNodeAtRight();
                return getActiveNode();
            }
        }
    }

    if (isRootRow())
        setNoMoreBranches();
    return getActiveNode();
}

int IVMTree::isRootRow(){
    return active_row == root_row;
}

void IVMTree::removeActiveNode(){
    markActiveNodeAsRemoved();
    decreaseNodesAt(active_row);
    decreaseNumberOfPendingNodes();
}

int IVMTree::thereAreMoreNodes(){
    return n_nodes_at_row[active_row] > 0? n_nodes_at_row[active_row] : 0;
}

void IVMTree::moveToNodeAtRight(){
    active_column_at_row[active_row]++;
}

void IVMTree::moveToFatherRow(){
    active_row--;
}

void IVMTree::removeRow(){
    active_column_at_row[active_row] = -1;
}

int IVMTree::isUnderRootRow(){
    return active_row > root_row ? 1 : 0;
}

void IVMTree::moveToRootRow(){
    active_row = root_row;
}

void IVMTree::markActiveNodeAsRemoved(){
    ivm[active_row][active_column_at_row[active_row]] = -1;
}

unsigned long IVMTree::decreaseNumberOfPendingNodes(){
    return pending_nodes--;
}

unsigned long IVMTree::increaseNumberOfPendingNodes(){
    return pending_nodes++;
}

void IVMTree::setNoMoreBranches(){
    hasBranches = 0;
}
/**
 * S: Start exploration
 * I: Index column.
 * N: Node value.
 * #: Number of nodes.
 */
void IVMTree::print() {
    char sep = '-';
    printf("[Row]\tS\tI\t N\t  #\n");
    for (int r = 0; r < rows; ++r) {
        /** The integer vector. **/
        if (active_column_at_row[r] == -1 && start_exploration[r] == -1)
            printf("[%3d] %3c %3c ", r, sep, sep);
        else if (active_column_at_row[r] == -1 && start_exploration[r] > -1)
            printf("[%3d] %3d %3c ", r, start_exploration[r], sep);
        else if (active_column_at_row[r] > -1 && start_exploration[r] == -1)
            printf("[%3d] %3c %3d ", r, sep, active_column_at_row[r]);
        else
            printf("[%3d] %3d %3d ", r, start_exploration[r], active_column_at_row[r]);
        
        /** The solution. **/
        if (active_column_at_row[r] == -1 || ivm[r][active_column_at_row[r]] == -1)
            printf(" %3c ", sep);
        else
            printf(" %3d ", ivm[r][active_column_at_row[r]]);
        
        /** Max nodes in row. **/
        printf(" %3d | ", n_nodes_at_row[r]);
        
        /** The matrix. **/
        for (int c = 0; c < cols; ++c)
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
