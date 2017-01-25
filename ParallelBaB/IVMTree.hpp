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


class IVMTree{

    int rows;
    int cols;
    
public:
    IVMTree();
    IVMTree(int rows, int cols);
    ~IVMTree();
    int ** ivm;
    int * active_nodes;
    int * start_exploration;
    int * end_exploration;
    int active_level;
    int starting_level;
    int hasBranches = 1;
    
    int * max_nodes_in_level;
    
    int getNumberOfRows();
    int getNumberOfCols();
    void setActiveLevel(int level);
    void setExplorationInterval(int starting_level, int * starts, int * ends);
    int getTreeDeep();
    
    int hasPendingBranches();
    void setNode(int level, int value);
    int getActiveNode();
    int getFatherNode();
    int pruneActiveNode();

    int getCurrentLevel();
    
    int moveToNextLevel();
    int moveToNextNode();
    
    void showIVM();
    
};

#endif /* IVMTree_hpp */
