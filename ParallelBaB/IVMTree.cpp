//
//  IVMTree.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 16/01/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "IVMTree.hpp"

IVMTree::IVMTree():
n_rows(0),
n_cols(0),
i_am(0),
matrix_nodes(nullptr),
vector_pointing_to_col_at_row(nullptr),
start_exploration(nullptr),
end_exploration(nullptr),
integer_pointing_to_row(0),
starting_row(0),
there_are_more_branches(false),
root_row(0),
n_nodes_at_row(nullptr),
pending_nodes(0) {

}

IVMTree::IVMTree(int rows, int cols):
n_rows(rows),
n_cols(cols),
i_am(0),
matrix_nodes(new int * [n_rows]),
vector_pointing_to_col_at_row(new int[n_rows]),
start_exploration(new unsigned int[n_rows]),
end_exploration(new unsigned int[n_rows]),
integer_pointing_to_row(0),
starting_row(0),
there_are_more_branches(true),
root_row(0),
n_nodes_at_row(new unsigned int[n_rows]),
pending_nodes(0) {
    
    for (int r = 0; r < n_rows; ++r) {
        
        vector_pointing_to_col_at_row[r] = -1;
        n_nodes_at_row[r] = 0;
        start_exploration[r] = 0;
        end_exploration[r] = 0;
        
        matrix_nodes[r] = new int[n_cols];
        for (int c = 0; c < n_cols; ++c)
            matrix_nodes[r][c] = 0;
    }
}

IVMTree::IVMTree(const IVMTree& toCopy):
n_rows(toCopy.getNumberOfRows()),
n_cols(toCopy.getNumberOfCols()),
i_am(toCopy.getId()),
matrix_nodes(new int * [n_rows]),
vector_pointing_to_col_at_row(new int[n_rows]),
start_exploration(new unsigned int[n_rows]),
end_exploration(new unsigned int[n_rows]),
integer_pointing_to_row(toCopy.getActiveRow()),
starting_row(toCopy.getStartingRow()),
there_are_more_branches(toCopy.thereAreMoreBranches()),
root_row(toCopy.getRootRow()),
n_nodes_at_row(new unsigned int[n_rows]),
pending_nodes(toCopy.getNumberOfPendingNodes()) {
    
    for (int r = 0; r < n_rows; ++r) {
        vector_pointing_to_col_at_row[r] = toCopy.getActiveColAt(r);
        n_nodes_at_row[r] = toCopy.getNumberOfNodesAt(r);
        start_exploration[r] = toCopy.getStartExploration(r);
        end_exploration[r] = toCopy.getEndExploration(r);

        matrix_nodes[r] = new int[n_cols];
        for (int c = 0; c < n_cols; ++c)
            matrix_nodes[r][c] = toCopy.getNodeValueAt(r, c);
    }
}

IVMTree& IVMTree::operator=(const IVMTree &toCopy) {

    if (matrix_nodes != nullptr) {
        delete[] vector_pointing_to_col_at_row;
        delete[] n_nodes_at_row;
        delete[] start_exploration;
        delete[] end_exploration;

        for (int r = 0; r < n_rows; ++r)
            delete[] matrix_nodes[r];
        delete[] matrix_nodes;
    }

    n_rows = toCopy.getNumberOfRows();
    n_cols = toCopy.getNumberOfCols();
    there_are_more_branches = toCopy.thereAreMoreBranches();
    root_row = toCopy.getRootRow();
    starting_row = toCopy.getStartingRow();
    integer_pointing_to_row = toCopy.getActiveRow();
    i_am = toCopy.getId();
    pending_nodes = toCopy.getNumberOfPendingNodes();
    
    vector_pointing_to_col_at_row = new int[n_rows];
    n_nodes_at_row = new unsigned int[n_rows];
    start_exploration = new unsigned int[n_rows];
    end_exploration = new unsigned int[n_rows];
    matrix_nodes = new int *[n_rows];
    
    for (int r = 0; r < n_rows; ++r) {
        vector_pointing_to_col_at_row[r] = toCopy.getActiveColAt(r);
        n_nodes_at_row[r] = toCopy.getNumberOfNodesAt(r);
        start_exploration[r] = toCopy.getStartExploration(r);
        end_exploration[r] = toCopy.getEndExploration(r);

        matrix_nodes[r] = new int[n_cols];
        for (int c = 0; c < n_cols; ++c)
            matrix_nodes[r][c] = toCopy.getNodeValueAt(r, c);
    }
    return *this;
}

IVMTree& IVMTree::operator()(int rows, int cols) {

    if (matrix_nodes != nullptr) {
        delete[] vector_pointing_to_col_at_row;
        delete[] n_nodes_at_row;
        delete[] start_exploration;
        delete[] end_exploration;

        for (int r = 0; r < n_rows; ++r)
            delete[] matrix_nodes[r];
        delete[] matrix_nodes;
    }
    
    n_rows = rows;
    n_cols = cols;
    there_are_more_branches = true;
    root_row = 0;
    starting_row = 0;
    integer_pointing_to_row = 0;
    pending_nodes = 0;
    
    vector_pointing_to_col_at_row = new int[n_rows];
    n_nodes_at_row = new unsigned int[n_rows];
    start_exploration = new unsigned int[n_rows];
    end_exploration = new unsigned int[n_rows];
    matrix_nodes = new int *[n_rows];
    
    for (int r = 0; r < n_rows; ++r) {
        matrix_nodes[r] = new int[n_cols];
        vector_pointing_to_col_at_row[r] = -1;
        n_nodes_at_row[r] = 0;
        for (int c = 0; c < n_cols; ++c)
            matrix_nodes[r][c] = 0;
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

void IVMTree::setRootRow(int row) {
    root_row = row;
}

/**
 *Sets a new node value at the indicated row and col.*

 - Parameters:

 - row: Row to be affected.

 - col: Column to be affected.

 - value: New value to be set.
 */
void IVMTree::setNodeValueAt(int row, int col, int value) throw(IVMTreeException) {
    try{
        if (row < 0 || row >= n_rows || col < 0 || col >= n_cols)
            throw IVMTreeException(IVMTreeErrorCode::MATRIX_OUT_OF_RANGE, "when calling setNodeValueAt(row:" + std::to_string(static_cast<long long>(row)) + ", col:" + std::to_string(static_cast<long long>(col)) + ")");
        
        matrix_nodes[row][col] = value;
        
    } catch (IVMTreeException& ivmTreeEx) {
        printf("%s\n",  ivmTreeEx.what());
    }
}

void IVMTree::setActiveColAtRow(int row, int value) throw(IVMTreeException) {
    try {
        if (row < 0 || row >= n_rows)
            throw IVMTreeException(VECTOR_OUT_OF_RANGE, "row:" + std::to_string(static_cast<long long>(row)));
        
        vector_pointing_to_col_at_row[row] = value;
        
    } catch (IVMTreeException& ivmTreeEx) {
        printf("%s\n",  ivmTreeEx.what());
    }
}

void IVMTree::setStartingRow(int row) {
    starting_row = row;
}

void IVMTree::setStartExploration(int row, unsigned int value) {
    start_exploration[row] = value;
}

void IVMTree::setEndExploration(int row, unsigned int value) {
    end_exploration[row] = value;
}

/**
 *Set the number of nodes at the row indicated. This also updates the number of pending nodes. *

 - Parameters:

 - row: Row to be affected.

 - value: Number of nodes to set.
 */
void IVMTree::setNumberOfNodesAt(int row, int value) {
    pending_nodes -= n_nodes_at_row[row]; /** Decrease the actual number of nodes at indicated row. **/
    pending_nodes += value; /** Increase the number of nodes with new value. **/
    n_nodes_at_row[row] = value; /** Set the new number of nodes. **/
}

/**
 *Increases the number of nodes at the row by 1.*

 - Parameters row: The row to be affected.
 */
int IVMTree::increaseNumberOfNodesAt(int row) {
    return n_nodes_at_row[row]++;
}

/**
 *Decreases the number of nodes at the row by 1.*

 - Parameters row: The row to be affected.
 */
int IVMTree::decreaseNumberOfNodesAt(int row) {
    return n_nodes_at_row[row]--;
}

/**
 *Sets to 0 the number of nodes at the row indicated. Also updates the number of pending nodes.*

 - Parameters row: The row to be affected.
 */
void IVMTree::resetNumberOfNodesAt(int row) {
    pending_nodes -= n_nodes_at_row[row];
    n_nodes_at_row[row] = 0;
}

/**
 *Updates the flag to indicate that there are more pending branches to be explored.*
 */
void IVMTree::setThereAreMoreBranches() {
    there_are_more_branches = true;
}

/**
 * - Returns: The node value of root.*
 */
int IVMTree::getRootNode() const {
    return getNodeValueAt(root_row, getActiveColAt(root_row));
}
/**
 * - Returns: The row correspoing to root node.
 */
int IVMTree::getRootRow() const {
    return root_row;
}

/**
 - Parameter row: Row to be query.

 - Returns: The number of nodes at the given row.
 */
int IVMTree::getNumberOfNodesAt(int row) const {
    return n_nodes_at_row[row];
}

/**
 *Retrieves the matrix value at the indicated row and col.*

 - Parameters:

 - row: Matrix row to be consulted.

 - col: Matrix colum to be consulted.

 - Returns: Node value at the given row and col.
 */
int IVMTree::getNodeValueAt(int row, int col) const throw(IVMTreeException) {
    try {
        if (row < 0 || row >= n_rows || col < 0 || col >= n_cols)
            throw IVMTreeException(IVMTreeErrorCode::MATRIX_OUT_OF_RANGE, "(row:" + std::to_string(static_cast<long long>(row)) + ", col:" + std::to_string(static_cast<long long>(col)) + ")");
        
        return matrix_nodes[row][col];
        
    } catch (IVMTreeException& ivmTreeEx) {
        std::cerr << ivmTreeEx.what();
        exit(EXIT_FAILURE);
    }
    return -1;
}

/**
 *Returns the current column which is being explored.*

 - Returns: The current column.
 */
int IVMTree::getActiveCol() const {
    return getActiveColAt(getActiveRow());
}

/**
 *Returns the column which is being explored at the indicted row.*

 - Parameter row: The row to be query.

 - Returns: The current column.
 */
int IVMTree::getActiveColAt(int row) const throw(IVMTreeException) {
    try {
        if (row < 0 || row >= n_rows)
            throw IVMTreeException(VECTOR_OUT_OF_RANGE, "row:" + std::to_string(static_cast<long long>(row)));
        
        return vector_pointing_to_col_at_row[row];
        
    } catch (IVMTreeException& ivmTreeEx) {
        std::cerr << ivmTreeEx.what();
        exit(EXIT_FAILURE);
    }
    return -1;
}

/**
 - Returns: The row being pointed by I.
 */
int IVMTree::getActiveRow() const {
    return integer_pointing_to_row;
}

int IVMTree::getStartingRow() const {
    return starting_row;
}

unsigned int IVMTree::getStartExploration(int row) const {
    return start_exploration[row];
}

unsigned int IVMTree::getEndExploration(int row) const {
    return end_exploration[row];
}

unsigned int IVMTree::getNumberOfRows() const {
    return n_rows;
}

unsigned int IVMTree::getNumberOfCols() const {
    return n_cols;
}

int IVMTree::getDeepOfTree() const {
    return n_rows;
}

int IVMTree::getId() const {
    return i_am;
}

/**
 *Removes the last node value at the indicated row. The node value is marked as 0, thus updating end_exploration[row], pending_nodes, and n_nodes_at_row[row].*

*Important*: The last node will be marked as 0 so the node value will be missed.

 - Parameter row: The row to be affected.

 - Returns: The node value corresponding to the last node of indicated row.
 */
int IVMTree::removeLastNodeAtRow(int row) {
    int last_col = end_exploration[row] - 1;
    int node_at_col = matrix_nodes[row][last_col];
    
    setNodeValueAt(row, last_col, -1 * node_at_col);
    decreaseNumberOfNodesAt(row);
    decreaseEndExplorationAtRow(row);
    decreaseNumberOfPendingNodes();
    return node_at_col;
}

/**
*Shares the last node value at the indicated row. The node value is marked as pruned in the row thus updating end_exploration[row], pending_nodes, and n_nodes_at_row[row].*

 - Parameter row: The row to be affected.

 - Returns: The node value corresponding to the last node of indicated row.
 */
int IVMTree::shareLastNodeAtRow(int row) {
    int last_col = end_exploration[row];
    int node_at_col = matrix_nodes[row][last_col];

    setNodeValueAt(row, last_col, -1 * node_at_col); /**Marks the node with the negative value to be able to use the value when copyin the row. **/
    decreaseNumberOfNodesAt(row);
    decreaseEndExplorationAtRow(row);
    decreaseNumberOfPendingNodes();
    return node_at_col;
}

/**
 *Prunes the last node value at the indicated row. The node value is marked as pruned in the row thus updating start_exploration[row], pending_nodes, and n_nodes_at_row[row].*

 - Parameter row: The row to be affected.

 - Returns: The node value corresponding to the last node of indicated row.
 */
int IVMTree::pruneLastNodeAtRow(int row) {

    int last_col = getEndExploration(row) - 1;
    int node_at_col = getNodeValueAt(row, last_col);

    setNodeValueAt(row, last_col, -1 * node_at_col); /**Marks the node with the negative value to be able to use the value when copyin the row. **/
    decreaseNumberOfNodesAt(row);
    decreaseNumberOfPendingNodes();
    increaseStartExplorationAtRow(row);

    return node_at_col;
}

/**
 *Prunes the first node value at the indicated row. The node value is marked as pruned in the row thus updating start_exploration[row], pending_nodes, and n_nodes_at_row[row].*

 - Parameter row: The row to be affected.

 - Returns: The node value corresponding to the first node of indicated row.
 */
int IVMTree::pruneFirstNodeAtRow(int row) {

    int first_col =  getStartExploration(row);
    int node_at_col = getNodeValueAt(row, first_col);

    setNodeValueAt(row, first_col, -1 * node_at_col); /**Marks the node with the negative value to be able to use the value when copyin the row. **/
    decreaseNumberOfNodesAt(row);
    decreaseNumberOfPendingNodes();
    increaseStartExplorationAtRow(row);

    return node_at_col;
}

unsigned int IVMTree::decreaseEndExplorationAtRow(int row) {
    return end_exploration[row]--;
}

unsigned int IVMTree::increaseEndExplorationAtRow(int row) {
    return end_exploration[row]++;
}

unsigned int IVMTree::decreaseStartExplorationAtRow(int row) {
    return start_exploration[row]--;
}

unsigned int IVMTree::increaseStartExplorationAtRow(int row) {
    return start_exploration[row]++;
}

/**
 *Sets to 0 the node values at the given row.*

 - Parameter row: The row to be affected.
 */
void IVMTree::resetRow(int row) {
    for (int col = 0; col < getNumberOfCols(); ++col)
        setNodeValueAt(row, col, 0);
    resetNumberOfNodesAt(row);
    setActiveColAtRow(row, -1);
}

void IVMTree::setActiveRow(int row) throw (IVMTreeException) {
    try{
        if (row < 0 || row >= n_rows)
            throw IVMTreeException(INTEGER_OUT_OF_RANGE, "row: " + std::to_string(static_cast<long long>(row)));
        
        integer_pointing_to_row = row;
        
    }catch(IVMTreeException& ex) {
        std::cerr << ex.what();
        exit(EXIT_FAILURE);
    }
}

void IVMTree::setOwnerId(int id_bb) {
    i_am = id_bb;
}

/**
 *Adds a new node to given row. This function updates: end_exploration[row], pending_nodes, n_nodes_at[row]. The n_nodes_at[row] and pending_nodes are increased by 1. Also the end_exploration[row] is increased by 1.*

 - Parameters:

 - row: Row to be affected.

 - node_value: New node to be added.
 */
void IVMTree::addNodeToRow(int row, int node_value) {
    int next_free_col = getNextFreeColAtRow(row);
    setNodeValueAt(row, next_free_col, node_value);
    increaseNumberOfNodesAt(row);
    increaseNumberOfPendingNodes();
    increaseEndExplorationAtRow(row);
}

int IVMTree::getNextFreeColAtRow(int row) {
    return end_exploration[row];
}

bool IVMTree::thereAreMoreBranches() const {
    return there_are_more_branches;
}

int IVMTree::moveToNextRow() {
    setActiveRow(getActiveRow() + 1);
    setActiveColAtRow(getActiveRow(), getStartExploration(getActiveRow()));

    return getActiveRow();
}

int IVMTree::getActiveNode() const {
    return getNodeValueAt(getActiveRow(), getActiveCol());
}

int IVMTree::getFatherNode() const {
    int father_row = getActiveRow() - 1;
    return root_row == 0 ? getNodeValueAt(father_row, getActiveColAt(father_row)) : 0;
}

/**
 - Returns: The number of pending nodes to explored.
 */
unsigned long IVMTree::getNumberOfPendingNodes() const {
    return pending_nodes;
}

/** Prune the active node and set the active_level pointing to new active node. **/
unsigned long IVMTree::pruneActiveNode() {
    unsigned long pruned_nodes = 0;
    /** If the active node is the root node we cannot remove it.**/
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
        if (!isRootRow()) {
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

bool IVMTree::isRootRow() const {
    return integer_pointing_to_row == root_row;
}

bool IVMTree::isUnderRootRow() const {
    return getActiveRow() > root_row;
}

void IVMTree::removeActiveNode() {
    markActiveNodeAsRemoved();
    decreaseNumberOfNodesAt(integer_pointing_to_row);
    decreaseNumberOfPendingNodes();
}

int IVMTree::thereAreMoreNodes() {
    return getNumberOfNodesAt(getActiveRow()) > 0 ? getNumberOfNodesAt(getActiveRow()) : 0;
}

void IVMTree::moveToNodeAtRight() {
    start_exploration[getActiveRow()]++;
    setActiveColAtRow(getActiveRow(), getActiveColAt(getActiveRow()) + 1);
}

void IVMTree::moveToFatherRow() {
    setActiveRow(getActiveRow() - 1);
}

void IVMTree::removeRow() {
    setNodeValueAt(getActiveRow(), end_exploration[getActiveRow()], 0);
    setNodeValueAt(getActiveRow(), 0, 0);
    setActiveColAtRow(getActiveRow(), -1);

    start_exploration[getActiveRow()] = 0;
    end_exploration[getActiveRow()] = 0;
}

void IVMTree::moveToRootRow() {
    setActiveRow(root_row);
}

void IVMTree::markActiveNodeAsRemoved() {
    setNodeValueAt(getActiveRow(), getActiveCol(), -1 * getActiveNode());
}

unsigned long IVMTree::decreaseNumberOfPendingNodes() {
    return pending_nodes--;
}

unsigned long IVMTree::increaseNumberOfPendingNodes() {
    return pending_nodes++;
}

void IVMTree::setNoMoreBranches() {
    there_are_more_branches = false;
}
/**
 * S: Start exploration
 * I: Index column.
 * N: Node value.
 * #: Number of nodes.
 */
void IVMTree::print() const {
    char sep = '-';
    printf("[Row]    S    V    #\n");
    for (int r = 0; r < getNumberOfRows(); ++r) {

        /** The row. **/
        printf("[%4d]", r);
        /** The solution. **/
        if (vector_pointing_to_col_at_row[r] == -1 || matrix_nodes[r][vector_pointing_to_col_at_row[r]] == 0)
            printf("%4c", sep);
        else
            printf("%4d", matrix_nodes[r][vector_pointing_to_col_at_row[r]]);

        /** The integer vector. **/
        if (vector_pointing_to_col_at_row[r] == -1)
            printf("%4c", sep);
        else
            printf("%4d", vector_pointing_to_col_at_row[r]);
        
        /** Number of nodes at row. **/
        printf("%4d |", n_nodes_at_row[r]);
        
        /** The matrix. **/
        for (int c = 0; c < n_cols; ++c)
            if (matrix_nodes[r][c] == 0)
                printf("%5c", sep);
            else if(vector_pointing_to_col_at_row[r] == c)
                printf(" [%3d]", matrix_nodes[r][c]);
            else
                printf("%5d", matrix_nodes[r][c]);

        /** Begin and end. **/
        printf(" |%4d %4d", start_exploration[r], end_exploration[r]);

        /** The active row. **/
        printf("|");
        if (r == root_row)
            printf("r");
        if (r == integer_pointing_to_row)
            printf("*");
        printf("\n");
    }
}

void IVMTree::saveToFile(const char outputFile[255]) const {
    std::ofstream myfile(outputFile);
    int size_word = 6;
    if (myfile.is_open()) {
        printf("Saving ivm file.\n");
        myfile << getNumberOfRows() << ' ' << getNumberOfCols() << '\n';
        myfile << getActiveRow() << '\n';
        for (int row = 0; row < getNumberOfRows(); ++row) {
            
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
