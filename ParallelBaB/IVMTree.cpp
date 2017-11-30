//
//  IVMTree.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 16/01/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "IVMTree.hpp"

IVMTree::IVMTree() {
    n_rows = 0;
    n_cols = 0;
    i_am = -1;
    matrix_nodes = nullptr;
    vector_pointing_to_col_at_row = nullptr;
    start_exploration = nullptr; /** This is not used. **/
    end_exploration = nullptr; /** This is not used. **/
    integer_pointing_to_row = 0;
    starting_row = 0;
    there_are_more_ranches = 0;
    root_row = 0; /** Root row. **/
    n_nodes_at_row = nullptr;
    pending_nodes = 0;
}

IVMTree::IVMTree(int rows, int cols) {
    n_rows = rows;
    n_cols = cols;
    there_are_more_ranches = 1;
    root_row = 0;
    starting_row = 0;
    integer_pointing_to_row = 0;
    pending_nodes = 0;
    vector_pointing_to_col_at_row = new int[n_rows];
    n_nodes_at_row = new int[n_rows];
    start_exploration = new int[n_rows];
    end_exploration = new int[n_rows];
    matrix_nodes = new int *[n_rows];
    
    for (int r = 0; r < n_rows; ++r) {
        
        vector_pointing_to_col_at_row[r] = -1;
        n_nodes_at_row[r] = 0;
        start_exploration[r] = 0;
        end_exploration[r] = n_cols;
        
        matrix_nodes[r] = new int[n_cols];
        for (int c = 0; c < n_cols; ++c)
            matrix_nodes[r][c] = -1;
    }
}

IVMTree::IVMTree(const IVMTree& toCopy) {
    i_am = toCopy.getId();
    n_rows = toCopy.getNumberOfRows();
    n_cols = toCopy.getNumberOfCols();
    there_are_more_ranches = toCopy.thereAreMoreBranches();
    root_row = toCopy.getRootRow();
    starting_row = toCopy.getStartingRow();
    integer_pointing_to_row = toCopy.getActiveRow();
    pending_nodes = toCopy.getNumberOfPendingNodes();
    
    vector_pointing_to_col_at_row = new int[n_rows];
    n_nodes_at_row = new int[n_rows];
    start_exploration = new int[n_rows];
    end_exploration = new int[n_rows];
    matrix_nodes = new int *[n_rows];
    
    for (int r = 0; r < n_rows; ++r) {
        vector_pointing_to_col_at_row[r] = toCopy.getActiveColAt(r);
        n_nodes_at_row[r] = toCopy.getNumberOfNodesAt(r);
        start_exploration[r] = toCopy.getStartExploration(r);
        end_exploration[r] = toCopy.getEndExploration(r);
        pending_nodes += n_nodes_at_row[r];
        matrix_nodes[r] = new int[n_cols];
        
        for (int c = 0; c < n_cols; ++c)
            matrix_nodes[r][c] = toCopy.getNodeValueAt(r, c);
    }
}

IVMTree& IVMTree::operator=(const IVMTree &toCopy){
    n_rows = toCopy.getNumberOfRows();
    n_cols = toCopy.getNumberOfCols();
    there_are_more_ranches = toCopy.thereAreMoreBranches();
    root_row = toCopy.getRootRow();
    starting_row = toCopy.getStartingRow();
    integer_pointing_to_row = toCopy.getActiveRow();
    i_am = toCopy.getId();
    pending_nodes = toCopy.getNumberOfPendingNodes();
    
    vector_pointing_to_col_at_row = new int[n_rows];
    n_nodes_at_row = new int[n_rows];
    start_exploration = new int[n_rows];
    end_exploration = new int[n_rows];
    matrix_nodes = new int *[n_rows];
    
    for (int r = 0; r < n_rows; ++r) {
        vector_pointing_to_col_at_row[r] = toCopy.getActiveColAt(r);
        n_nodes_at_row[r] = toCopy.getNumberOfNodesAt(r);
        start_exploration[r] = toCopy.getStartExploration(r);
        end_exploration[r] = toCopy.getEndExploration(r);
        pending_nodes += n_nodes_at_row[r];
        matrix_nodes[r] = new int[n_cols];
        
        for (int c = 0; c < n_cols; ++c)
            matrix_nodes[r][c] = toCopy.getNodeValueAt(r, c);
    }
    return *this;
}

IVMTree& IVMTree::operator()(int rows, int cols) {
    n_rows = rows;
    n_cols = cols;
    there_are_more_ranches = 1;
    root_row = 0;
    starting_row = 0;
    integer_pointing_to_row = 0;
    pending_nodes = 0;
    
    vector_pointing_to_col_at_row = new int[n_rows];
    n_nodes_at_row = new int[n_rows];
    start_exploration = new int[n_rows];
    end_exploration = new int[n_rows];
    matrix_nodes = new int *[n_rows];
    
    for (int r = 0; r < n_rows; ++r) {
        matrix_nodes[r] = new int[n_cols];
        vector_pointing_to_col_at_row[r] = -1;
        n_nodes_at_row[r] = 0;
        for (int c = 0; c < n_cols; ++c)
            matrix_nodes[r][c] = -1;
    }
    return *this;
}

IVMTree::~IVMTree() {
    delete[] vector_pointing_to_col_at_row;
    delete[] n_nodes_at_row;
    delete[] start_exploration;
    delete[] end_exploration;
    
    for (int r = 0; r < n_rows; ++r)
        delete[] matrix_nodes[r];
    delete[] matrix_nodes;
}

void IVMTree::setRootRow(int row){
    root_row = row;
}

void IVMTree::setValueAt(int row, int col, int value){
    matrix_nodes[row][col] = value;
}

void IVMTree::setActiveColAtRow(int row, int value){
    vector_pointing_to_col_at_row[row] = value;
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

int IVMTree::increaseNumberOfNodesAt(int row){
    return n_nodes_at_row[row]++;
}

int IVMTree::decreaseNumberOfNodesAt(int row){
    return n_nodes_at_row[row]--;
}

void IVMTree::resetNumberOfNodesAt(int row) {
    pending_nodes -= n_nodes_at_row[row];
    n_nodes_at_row[row] = 0;
}

void IVMTree::setThereAreMoreBranches(){
    there_are_more_ranches = 1;
}

int IVMTree::getRootNode() const{
    return matrix_nodes[root_row][vector_pointing_to_col_at_row[root_row]];
}

int IVMTree::getRootRow() const {
    return root_row;
}

int IVMTree::getNumberOfNodesAt(int row) const{
    return n_nodes_at_row[row];
}

int IVMTree::getNodeValueAt(int row, int col) const{
    return matrix_nodes[row][col];
}

int IVMTree::getActiveCol() const{
    return getActiveColAt(getActiveRow());
}

int IVMTree::getActiveColAt(int row) const{
    return vector_pointing_to_col_at_row[row];
}

int IVMTree::getActiveRow() const{
    return integer_pointing_to_row;
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
    return n_rows;
}

int IVMTree::getNumberOfCols() const {
    return n_cols;
}

int IVMTree::getDeepOfTree() const {
    return n_rows;
}

int IVMTree::getId() const {
    return i_am;
}

int IVMTree::removeLastNodeAtRow(int row){
    int last_col = getActiveColAt(row) + getNumberOfNodesAt(row) - 1;
    int node_at_col = matrix_nodes[row][last_col];
    
    setValueAt(row, last_col, -1);
    decreaseNumberOfNodesAt(row);
    decreaseEndExplorationAtRow(row);
    decreaseNumberOfPendingNodes();
    return node_at_col;
}

int IVMTree::decreaseEndExplorationAtRow(int row){
    return end_exploration[row]--;
}

void IVMTree::resetRow(int row){
    for (int col = 0; col < getNumberOfCols(); ++col)
        setValueAt(row, col, -1);
    resetNumberOfNodesAt(row);
    setActiveColAtRow(row, -1);
}

void IVMTree::setActiveRow(int row) {
    integer_pointing_to_row = row;
}

void IVMTree::setOwnerId(int id_bb) {
    i_am = id_bb;
}

/**
 * Initialize th IVM with the given interval.
 * TODO: Delete unused function.
 */
void IVMTree::setExplorationInterval(int starting_level, int *starts, int *ends) {
    starting_row = starting_level;
    integer_pointing_to_row = starting_level - 1;
    for (int level = 0; level < n_rows; ++level) {
        vector_pointing_to_col_at_row[level] = starts[level];
        start_exploration[level] = starts[level];
        end_exploration[level] = ends[level];
        n_nodes_at_row[level] = 0;
        matrix_nodes[level][starts[level]] = starts[level];
    }
}

void IVMTree::addNodeToRow(int row, int node_value) {
    int next_free_col = getNextFreeColAtRow(row);
    setValueAt(row, next_free_col, node_value);
    increaseNumberOfNodesAt(row);
    increaseNumberOfPendingNodes();
}

int IVMTree::getNextFreeColAtRow(int row){
    return getActiveColAt(row) + getNumberOfNodesAt(row) + 1;
}

int IVMTree::thereAreMoreBranches() const {
    return there_are_more_ranches;
}

int IVMTree::moveToNextRow() {
    return integer_pointing_to_row++;
}

int IVMTree::getActiveNode() const {
    return getNodeValueAt(getActiveRow(), getActiveCol());
}

int IVMTree::getFatherNode() const {
    int father_row = getActiveRow() - 1;
    return root_row == 0 ? getNodeValueAt(father_row, getActiveColAt(father_row)) : -1;
}

unsigned long IVMTree::getNumberOfPendingNodes() const{
    return pending_nodes;
}

/** Prune the active node and set the active_level pointing to new active node. **/
int IVMTree::pruneActiveNode() {
    int pruned_nodes = 0;
    /** TODO if the active node is the root node we cannot remove it.**/
    if (isRootRow())
        setNoMoreBranches();

    removeActiveNode();
    pruned_nodes++;
    if (thereAreMoreNodes()) {
        moveToNodeAtRight();
        return pruned_nodes;
    }
    
    /** If the active level doesn't have more nodes then move to the father node while there are pending nodes **/
    while (isUnderRootRow()) {
        removeRow();
        moveToFatherRow();
        if (!isRootRow()){
            removeActiveNode();
            pruned_nodes++;
            if (thereAreMoreNodes()) {
                moveToNodeAtRight();
                return pruned_nodes;
            }
        }
    }

    if (isRootRow())
        setNoMoreBranches();
    return pruned_nodes;
}

int IVMTree::isRootRow() const{
    return integer_pointing_to_row == root_row;
}

int IVMTree::isUnderRootRow() const{
    return getActiveRow() > root_row ? 1 : 0;
}

void IVMTree::removeActiveNode(){
    markActiveNodeAsRemoved();
    decreaseNumberOfNodesAt(integer_pointing_to_row);
    decreaseNumberOfPendingNodes();
}

int IVMTree::thereAreMoreNodes(){
    return getNumberOfNodesAt(getActiveRow()) > 0? getNumberOfNodesAt(getActiveRow()) : 0;
}

void IVMTree::moveToNodeAtRight(){
    vector_pointing_to_col_at_row[getActiveRow()]++;
}

void IVMTree::moveToFatherRow(){
    integer_pointing_to_row--;
}

void IVMTree::removeRow(){
    vector_pointing_to_col_at_row[getActiveRow()] = -1;
}

void IVMTree::moveToRootRow(){
    integer_pointing_to_row = root_row;
}

void IVMTree::markActiveNodeAsRemoved(){
    setValueAt(getActiveRow(), getActiveCol(), -1);
}

unsigned long IVMTree::decreaseNumberOfPendingNodes(){
    return pending_nodes--;
}

unsigned long IVMTree::increaseNumberOfPendingNodes(){
    return pending_nodes++;
}

void IVMTree::setNoMoreBranches(){
    there_are_more_ranches = 0;
}
/**
 * S: Start exploration
 * I: Index column.
 * N: Node value.
 * #: Number of nodes.
 */
void IVMTree::print() const {
    char sep = '-';
    printf("[Row]\tS\tI\t V\t  #\n");
    for (int r = 0; r < getNumberOfRows(); ++r) {
        /** The integer vector. **/
        if (vector_pointing_to_col_at_row[r] == -1 && start_exploration[r] == -1)
            printf("[%4d] %4c %4c ", r, sep, sep);
        else if (vector_pointing_to_col_at_row[r] == -1 && start_exploration[r] > -1)
            printf("[%4d] %4d %4c ", r, start_exploration[r], sep);
        else if (vector_pointing_to_col_at_row[r] > -1 && start_exploration[r] == -1)
            printf("[%4d] %4c %4d ", r, sep, vector_pointing_to_col_at_row[r]);
        else
            printf("[%4d] %4d %4d ", r, start_exploration[r], vector_pointing_to_col_at_row[r]);
        
        /** The solution. **/
        if (vector_pointing_to_col_at_row[r] == -1 || matrix_nodes[r][vector_pointing_to_col_at_row[r]] == -1)
            printf(" %4c ", sep);
        else
            printf(" %4d ", matrix_nodes[r][vector_pointing_to_col_at_row[r]]);
        
        /** Max nodes in row. **/
        printf(" %4d | ", n_nodes_at_row[r]);
        
        /** The matrix. **/
        for (int c = 0; c < n_cols; ++c)
            if (matrix_nodes[r][c] == -1)
                printf("%4c", sep);
            else
                printf("%4d", matrix_nodes[r][c]);
        
        /** The active row. **/
        printf("|");
        if (r == root_row)
            printf("r");
        if (r == integer_pointing_to_row)
            printf("*");
        printf("\n");
    }
}

void IVMTree::saveToFile(const char outputFile[255]) const{
    std::ofstream myfile(outputFile);
    int size_word = 6;
    if (myfile.is_open()) {
        printf("Saving ivm file.\n");
        myfile << getNumberOfRows() << ' ' << getNumberOfCols() << '\n';
        myfile << getActiveRow() << '\n';
        for (int row = 0; row < getNumberOfRows(); ++row){
            
            if (getStartExploration(row) > -1)
                myfile << std::setw(size_word) << getStartExploration(row);
            else
                myfile << std::setw(size_word) << '-';
        
            if (getActiveColAt(row) > -1)
                myfile << std::setw(size_word) << getActiveColAt(row);
            else
                myfile << std::setw(size_word) << '-';
        
            if (getNumberOfNodesAt(row) > -1)
                myfile << std::setw(size_word) << getNumberOfNodesAt(row);
            else
                myfile << std::setw(size_word) << '-';
        
            for (int col = 0; col < getNumberOfCols(); ++col)
                if (getNodeValueAt(row, col) > -1)
                    myfile << std::setw(size_word) << getNodeValueAt(row, col);
                else
                    myfile << std::setw(size_word) << '-';
            
            myfile << '\n';
        }
        myfile.close();
    } else
        printf("Unable to open file to save IVM tree pool...\n");
}
