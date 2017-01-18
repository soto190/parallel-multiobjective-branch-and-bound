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
    int * limit_exploration;
    int active_level;
    
    int * max_in_level;
    
    int getNumberOfRows();
    int getNumberOfCols();
    void setActiveLevel(int level);
    
    int hasPendingBranches();
    void setNode(int level, int value);
    void setMaxValueInLevel(int level, int value);
    int getActiveNode();
    int getNextNode();
    int getFatherNode();
    int pruneActiveNode();

    int getCurrentLevel();
    
    int moveToNextLevel();
    int moveToNextNode();
    
};

#endif /* IVMTree_hpp */
