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

class IVMTree {
private:
    int rows;
    int cols;
    int whoIam = -1;
    int ** ivm;
    int * active_column_at_row;
    int * start_exploration; /** This is not used. **/
    int * end_exploration; /** This is not used. **/
    int active_row;
    int starting_row;
    int hasBranches = 1;
    int root_row = 0;
    int * n_nodes_at_row;
    unsigned long pending_nodes = 0;
    
public:
    IVMTree();
    IVMTree(const IVMTree& toCopy);
    IVMTree(int rows, int cols);
    ~IVMTree();
    
    void setOwner(int idBB);
    void setRootRow(int node);
    void setValueAt(int row, int col, int value);
    void setActiveNodeAt(int row, int value);
    void setActiveRow(int row);
    void setStartingRow(int row);
    void setNumberOfNodesAt(int row, int value);
    void setExplorationInterval(int starting_level, int * starts, int * ends);
    void setStartExploration(int row, int value);
    void setEndExploration(int row, int value);
    int increaseNodesAt(int row);
    int decreaseNodesAt(int row);
    void resetNumberOfNodesAt(int row);
    void setHasBranches();
    
    int getRootNode() const;
    int getRootRow() const;
    int getNumberOfNodesAt(int row) const;
    int getNodeValue(int row, int col) const;
    int getActiveColAt(int row) const;
    int getActiveRow() const;
    int getLastNodeAtRow(int row) const;
    int getStartingRow() const;
    int getNumberOfRows() const;
    int getNumberOfCols() const;
    int getDeepOfTree() const;
    int getOwner() const;
    int getStartExploration(int row) const;
    int getEndExploration(int row) const;
    unsigned long getNumberOfPendingNodes() const;
    int removeLastNodeAtRow(int row);
    int hasPendingBranches() const;
    void setNodeAtRow(int level, int value);
    int getActiveNode() const;
    int getFatherNode() const;
    int pruneActiveNode();
    
    int moveToNextRow();
    int moveToNextNode();
    
    void print();
    IVMTree& operator()(int rows, int cols);
    IVMTree& operator=(const IVMTree& toCopy);

private:
    void setNodeAtNextFreeNodeAtRow(int row, int node_value);
    int getNextFreeNodeAtRow(int row);
    void moveToNodeAtRight();
    int thereAreMoreNodes();
    void removeActiveNode();
    void moveToFatherRow();
    void removeRow();
    int isUnderRootRow();
    void moveToRootRow();
    unsigned long decreaseNumberOfPendingNodes();
    unsigned long increaseNumberOfPendingNodes();
    int decreaseEndExplorationAtRow(int row);
    int isRootRow();

    void markActiveNodeAsRemoved();
    void setNoMoreBranches();
};

/*class IVMException: public std::exception{
    virtual const char* what() const throw(){
        return "IVM Exception";
    }
} IVMex;*/

#endif /* IVMTree_hpp */
