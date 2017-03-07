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

	int rows;
	int cols;
	int whoIam = -1;

public:
	IVMTree();
	IVMTree(int rows, int cols);

	~IVMTree();

	int ** ivm;
	int * active_node;
	int * start_exploration; /** This is not used. **/
	int * end_exploration; /** This is not used. **/
	int active_level;
	int starting_level;
	int hasBranches = 1;
	int root_node = 0;
	int * max_nodes_in_level;

	int getNumberOfRows() const;
	int getNumberOfCols() const;
	void setActiveLevel(int level);
	void setExplorationInterval(int starting_level, int * starts, int * ends);
	int getTreeDeep() const;
	void setOwner(int idBB);
	int getOwner() const;

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

};

#endif /* IVMTree_hpp */
