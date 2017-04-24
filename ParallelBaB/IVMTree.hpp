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
    int * active_column;
    int * start_exploration; /** This is not used. **/
    int * end_exploration; /** This is not used. **/
    int active_row;
    int starting_row;
    int hasBranches = 1;
    int root_row = 0; /** Root row. **/
    int * n_nodes_at_row;

public:
	IVMTree();
    IVMTree(const IVMTree& toCopy);
	IVMTree(int rows, int cols);
	~IVMTree();
    
    void setOwner(int idBB);
    void setRootRow(int node);
    void setIVMValueAt(int row, int col, int value);
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
    void setHasBranches(int itHas);
    
    int getRootNode() const;
    int getRootRow() const;
    int getNumberOfNodesAt(int row) const;
    int getIVMValue(int row, int col) const;
    int getActiveColAt(int row) const;
    int getActiveRow() const;
    int getLastNodeAtRow(int row) const;
    int getStartingRow() const;
	int getNumberOfRows() const;
	int getNumberOfCols() const;
	int getTreeDeep() const;
	int getOwner() const;
    int getStartExploration(int row) const;
    int getEndExploration(int row) const;
    
    int removeLastNodeAtRow(int row);
    
	int hasPendingBranches() const;
	void setNode(int level, int value);
	int getActiveNode() const;
	int getFatherNode() const;
	int pruneActiveNode();
    
	int moveToNextRow();
	int moveToNextNode();

	void print();
	IVMTree& operator()(int rows, int cols);
    IVMTree& operator=(const IVMTree& toCopy);

};

#endif /* IVMTree_hpp */
