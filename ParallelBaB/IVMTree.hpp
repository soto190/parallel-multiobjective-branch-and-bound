//
//  IVMTree.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 16/01/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#ifndef IVMTree_hpp
#define IVMTree_hpp

#include <stdio.h>
#include <fstream>
#include <iomanip>
#include "IVMTreeException.hpp"

class IVMTree {

private:
    unsigned int n_rows;
    unsigned int n_cols;
    unsigned int i_am = 0;
    int ** matrix_nodes;
    int * vector_pointing_to_col_at_row;
    unsigned int * start_exploration;
    unsigned int * end_exploration;
    unsigned int * n_nodes_at_row;
    int integer_pointing_to_row;
    int starting_row;
    bool there_are_more_branches = true;
    int root_row;
    unsigned long pending_nodes;
    
public:
    IVMTree();
    IVMTree(const IVMTree& toCopy);
    IVMTree(int rows, int cols);
    ~IVMTree();
    
    IVMTree& operator()(int rows, int cols);
    IVMTree& operator=(const IVMTree& toCopy);
    
    void setOwnerId(int idBB);
    void setRootRow(int node);
    void setNodeValueAt(int row, int col, int value) throw(IVMTreeException);
    void setActiveColAtRow(int row, int value) throw(IVMTreeException);
    void setActiveRow(int row) throw(IVMTreeException);
    void setStartingRow(int row);
    void setNumberOfNodesAt(int row, int value);
    void setStartExploration(int row, unsigned int value);
    void setEndExploration(int row, unsigned int value);
    int increaseNumberOfNodesAt(int row);
    int decreaseNumberOfNodesAt(int row);
    void resetNumberOfNodesAt(int row);
    void setThereAreMoreBranches();
    
    int getRootNode() const;
    int getRootRow() const;
    int getNumberOfNodesAt(int row) const;
    int getNodeValueAt(int row, int col) const throw(IVMTreeException);
    int getActiveColAt(int row) const throw(IVMTreeException);
    int getActiveRow() const;
    int getStartingRow() const;
    unsigned int getNumberOfRows() const;
    unsigned int getNumberOfCols() const;
    int getDeepOfTree() const;
    int getId() const;
    unsigned int getStartExploration(int row) const;
    unsigned int getEndExploration(int row) const;
    unsigned long getNumberOfPendingNodes() const;
    int removeLastNodeAtRow(int row);
    int shareLastNodeAtRow(int row);
    int pruneLastNodeAtRow(int row);
    int pruneFirstNodeAtRow(int row);
    bool thereAreMoreBranches() const;
    void addNodeToRow(int level, int value);
    int getActiveNode() const;
    int getFatherNode() const;
    unsigned long pruneActiveNode();
    int moveToNextRow();
    void resetRow(int row);
    void print() const;
    void saveToFile(const char outputFile[255]) const;
    
private:
    int getNextFreeColAtRow(int row);
    int getActiveCol() const;
    void moveToNodeAtRight();
    int thereAreMoreNodes();
    void removeActiveNode();
    void moveToFatherRow();
    void removeRow();
    void moveToRootRow();
    unsigned long decreaseNumberOfPendingNodes();
    unsigned long increaseNumberOfPendingNodes();
    unsigned int decreaseEndExplorationAtRow(int row);
    unsigned int increaseEndExplorationAtRow(int row);
    unsigned int decreaseStartExplorationAtRow(int row);
    unsigned int increaseStartExplorationAtRow(int row);
    bool isRootRow() const;
    bool isUnderRootRow() const;
    void markActiveNodeAsRemoved();
    void setNoMoreBranches();

};
#endif /* IVMTree_hpp */
