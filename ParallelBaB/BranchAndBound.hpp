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
#include "Interval.hpp"
#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/mutex.h"


class BranchAndBound:public tbb::task{
    
public:
    
    /**
     * The constructor should receives the parameters to start at some point of the tree.
     **/
    BranchAndBound();
    BranchAndBound(int rank, std::shared_ptr<Problem> problem);
    BranchAndBound(int rank, std::shared_ptr<Problem> problem, const Interval & branch);
    ~BranchAndBound();
    
    std::shared_ptr<Problem> problem;
    
    tbb::mutex MutexToUpdateGrid;
    
    Solution * currentSolution;
    Solution * bestObjectivesFound;
    
    /** intervals are the pending branches to be explored. **/
    std::vector<Interval> intervals;
    /** paretofFront needs to be a vector because omp works better with it anothre way is to use the intel vector version. **/
    std::vector<Solution *> paretoFront;
    
    std::shared_ptr<HandlerContainer> paretoContainer;
    
    IVMTree * ivm_tree;
    Interval * starting_interval;
    
    int levels_completed;
    
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
    
    void solve(const Interval & interval);
    void initialize(int starting_level);
    int explore(Solution * solution);
    void branch(Solution * solution, int currentLevel);
    void prune(Solution * solution, int currentLevel);
    void printParetoFront(int withVariables = 0);
    
    int setParetoFrontFile(const char * outputFile);
    int saveParetoFront();
    int setSummarizeFile(const char * outputFile);
    int saveSummarize();
    void saveEvery(double timeInSeconds);
    
    void setParetoContainer(std::shared_ptr<HandlerContainer> paretoContainer);
    std::shared_ptr<HandlerContainer> getParetoContainer();

    
private:
    int rank;
    void printCurrentSolution(int withVariables = 0);
    int aLeafHasBeenReached();
    int theTreeHasMoreBranches();
    int improvesTheLowerBound(Solution * solution);
    int updateLowerBound(Solution * solution);
    int getLowerBoundInObj(int nObj);

    unsigned long computeTotalNodes(int totalVariables);
    long permut(int n, int i);
    int getUpperBound(int objective);
    
    /** Grid functions. **/
    int improvesTheGrid(Solution * solution);
    int updateParetoGrid(Solution * solution);
    int improvesTheBucket(Solution * solution, vector<Solution *>& bucketFront);
    /** End Grid functions. **/
    
    /** IVM functions. **/
public:
    void computeLastBranch(Interval * branch);
    void setStartingInterval(Interval * branch);
    void splitInterval(const Interval & branch);
private:
    int initializeExplorationInterval(const Interval & branch, IVMTree * tree);
    /** End IVM functions **/

public:
    task* execute(); 
    
    void operator()(const Interval& branch){
        this->solve(branch);
    };
    
};


#endif /* BranchAndBound_hpp */
