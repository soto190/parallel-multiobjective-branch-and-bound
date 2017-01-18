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
#include <ctime>
#include <fstream>
#include <iomanip>
#include "Problem.hpp"
#include "Solution.hpp"
#include "myutils.hpp"
#include "GridContainer.hpp"
#include "Dominance.hpp"
#include "IVMTree.hpp"


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
    Solution* bestObjectivesFound;
    
    //std::vector<int> treeOnAStack; /** Remove **/
    //std::vector<int> levelOfTree ; /** Remove **/
    
    IVMTree * ivm_tree;
    
    int currentLevel; /** Active level **/
    int totalLevels;  /** Number of tree levels **/
    
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
    std::clock_t start;
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    
    void solve();
    void initialize();
    int explore(Solution * solution);
    void branch(Solution * solution, int currentLevel);
    void prune(Solution * solution, int currentLevel);
    void printParetoFront(int withVariables = 0);
    
    int setParetoFrontFile(const char * outputFile);
    int saveParetoFront();
    int setSummarizeFile(const char * outputFile);
    int saveSummarize();
    void saveEvery(double timeInSeconds);
    
private:
    void printCurrentSolution(int withVariables = 0);
    int aLeafHasBeenReached();
    int theTreeHasMoreBranches();
    int improvesTheLowerBound(Solution * solution);
    int updateLowerBound(Solution * solution);
    int getLowerBoundInObj(int nObj);

    unsigned long computeTotalNodes(int totalVariables);
    long permut(int n, int i);
    int getUpperBound(int objective);
    
    /**Grid functions**/
    int improvesTheGrid(Solution * solution);
    int updateParetoGrid(Solution * solution);
    int improvesTheBucket(Solution * solution, vector<Solution *>& bucketFront);
    
    
};

#endif /* BranchAndBound_hpp */
