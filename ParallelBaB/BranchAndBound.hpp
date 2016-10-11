//
//  BranchAndBound.hpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#ifndef BranchAndBound_hpp
#define BranchAndBound_hpp

#include <stdio.h>
//#include <libiomp/omp.h>
#include <vector>
#include <math.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include "Problem.hpp"
#include "Solution.hpp"
#include "myutils.hpp"
#include "GridContainer.hpp"


class BranchAndBound{
    
public:
    
    /**
     * The constructor should receives the parameters to start at some point of the tree.
     **/
    BranchAndBound();
    BranchAndBound(Problem * problem);
    ~BranchAndBound();
    
    Problem* problem;
    /**
     * paretofFront needs to be a vector because omp works better with it or use the intel vector version.
     */
    
    HandlerContainer * paretoContainer;
    
    std::vector<Solution * > paretoFront;
    Solution* currentSolution;
    
    std::vector<int> treeOnAStack;
    std::vector<int> levelOfTree ;
    
    int currentLevel;
    int totalLevels;
    
    unsigned long totalNodes;
    unsigned long branches;
    unsigned long exploredNodes;
    unsigned long callsToBranch;
    unsigned long leaves;
    
    unsigned long unexploredNodes;
    unsigned long prunedNodes;
    unsigned long prunedBranches;
    
    unsigned long totalUpdatesInLowerBound;
    
    char * outputFile;
    char * summarizeFile;
    
    double totalTime;
    
    void start();
    void initialize();
    int explore(Solution * solution);
    void branch(Solution * solution, int currentLevel);
    void prune(Solution * solution, int currentLevel);
    void printParetoFront(int withVariables = 0);
    
    int setParetoFrontFile(const char * outputFile);
    int saveParetoFront();
    int setSummarizeFile(const char * outputFile);
    int saveSummarize();
    
private:
    void printCurrentSolution(int withVariables = 0);
    int aLeafHasBeenReached();
    int theTreeHasMoreBranches();
    int improvesTheLowerBound(Solution * solution);
    unsigned int* getDominationStatus(Solution * solution);
    int dominanceTest(Solution * solutionA, Solution * solutionB);
    int updateLowerBound(Solution * solution);
    unsigned long computeTotalNodes(int totalVariables);
    long permut(int n, int i);
    
    /**Grid functions**/
    int improvesTheGrid(Solution * solution);
    int updateParetoGrid(Solution * solution);
    
};

#endif /* BranchAndBound_hpp */
