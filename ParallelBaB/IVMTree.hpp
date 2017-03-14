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
    int * active_node;
    int * start_exploration; /** This is not used. **/
    int * end_exploration; /** This is not used. **/
    int active_level;
    int starting_level;
    int hasBranches = 1;
    int root_node = 0;
    int * max_nodes_in_level;

public:
	IVMTree();
    IVMTree(const IVMTree& toCopy);
	IVMTree(int rows, int cols);
	~IVMTree();
    
    void setOwner(int idBB);
    void setRootNode(int node);
    void setIVMValueAt(int row, int col, int value);
    void setActiveNodeAt(int row, int value);
    void setActiveLevel(int row);
    void setStartingLevel(int row);
    void setNumberOfNodesAt(int row, int value);
    void setExplorationInterval(int starting_level, int * starts, int * ends);
    void setStartExploration(int row, int value);
    void setEndExploration(int row, int value);
    int increaseNodesAt(int row);
    int decreaseNodesAt(int row);
    void resetNumberOfNodesAt(int row);
    void setHasBranches(int itHas);
    
    int getRootNode() const;
    int getNumberOfNodesAt(int row) const;
    int getIVMValue(int row, int col) const;
    int getActiveNode(int row) const;
    int getActiveLevel() const;
    int getStartingLevel() const;
	int getNumberOfRows() const;
	int getNumberOfCols() const;
	int getTreeDeep() const;
	int getOwner() const;
    int getStartExploration(int row) const;
    int getEndExploration(int row) const;
    

	int hasPendingBranches() const;
	void setNode(int level, int value);
	int getActiveNode() const;
	int getFatherNode() const;
	int pruneActiveNode();

	int getCurrentLevel() const;

	int moveToNextLevel();
	int moveToNextNode();

	void showIVM();
	IVMTree& operator()(int rows, int cols);
    IVMTree& operator=(const IVMTree& toCopy);

};

#endif /* IVMTree_hpp */
