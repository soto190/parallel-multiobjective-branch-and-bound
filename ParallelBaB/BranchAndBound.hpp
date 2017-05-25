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
#include <queue>
#include <math.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include "Problem.hpp"
#include "ProblemFJSSP.hpp"
#include "Solution.hpp"
#include "myutils.hpp"
#include "GridContainer.hpp"
#include "Dominance.hpp"
#include "IVMTree.hpp"
#include "Interval.hpp"
#include "tbb/atomic.h"
#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/cache_aligned_allocator.h"
#include "tbb/concurrent_queue.h"
#include "assert.h"
//#include <memory.h> /** For the Ehecatl wich uses GCC 4.4.7, this activates the shared_ptr. **/
/**
 *
 * If Data Race in tbb::concurrent_queue<interval>; read the next lines
 * From https://software.intel.com/en-us/forums/intel-threading-building-blocks/topic/295293 :
 *   "There is internal synchronization in TBB that a tool can not recognize as correct. E.g. some common synchronization patterns (for example, test and test and set) might have benigndata races.Alsotools usually can not recognize synchronization that does not use "standard" primitives (such as pthread_mutex) and instead is uses carefully designed protocols based on atomic operations." - Alexey Kukanov (Intel)  Fri, 07/24/2009 - 05:20
 *
 **/

const float to_share = 0.5f;
const float deep_limit_share = 0.90f;

extern ReadySubproblems globalPool;  /** intervals are the pending branches/subproblems/partialSolutions to be explored. **/
extern HandlerContainer paretoContainer;

class BranchAndBound: public tbb::task {

private:
    
    int rank; /** Identifies the number of thread-B&B. **/

    tbb::atomic<unsigned long> totalNodes;
    tbb::atomic<unsigned long> branches;
    tbb::atomic<unsigned long> exploredNodes;
    tbb::atomic<unsigned long> callsToBranch;
    tbb::atomic<unsigned long> reachedLeaves;
    tbb::atomic<unsigned long> unexploredNodes;
    tbb::atomic<unsigned long> prunedNodes;
    tbb::atomic<unsigned long> callsToPrune;
    tbb::atomic<unsigned long> totalUpdatesInLowerBound;
    tbb::atomic<unsigned long> totalLevels; /** Number of tree levels. **/
    unsigned long shared_work;
    
    int currentLevel; /** Active level **/
    
    ProblemFJSSP problem;
    FJSSPdata fjssp_data;
    
    Solution incumbent_s;
    IVMTree ivm_tree;
    Interval interval_to_solve;
    std::vector<Solution> paretoFront; /** paretofFront. **/
    
    char outputFile[255];
    char summarizeFile[255];
    
    int branches_to_move;
    int deep_to_share;
    
    double totalTime;
    std::clock_t start;
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    
public:
    BranchAndBound(const BranchAndBound& branchAndBound);
    BranchAndBound(int rank, const ProblemFJSSP& problem, const Interval & branch /*,GlobalPool& globa_pool, HandlerContainer& pareto_container*/);
    BranchAndBound& operator()(int rank, const ProblemFJSSP& problem, const Interval & branch);
	~BranchAndBound();
    
    int getRank() const;
    int getCurrentLevel() const;
    double getTotalTime();
    
	void solve(Interval & interval);
	void initialize(int starting_level);
	int explore(Solution & solution);
	int branch(Solution & solution, int currentLevel);
	void prune(Solution & solution, int currentLevel);
    
    std::vector<Solution>& getParetoFront();
	void printParetoFront(int withVariables = 0);

    unsigned long getNumberOfNodes() const;
    unsigned long getNumberOfLevels() const;
    unsigned long getNumberOfBranches() const;
    unsigned long getNumberOfExploredNodes() const;
    unsigned long getNumberOfCallsToBranch() const;
    unsigned long getNumberOfReachedLeaves() const;
    unsigned long getNumberOfUnexploredNodes() const;
    unsigned long getNumberOfPrunedNodes() const;
    unsigned long getNumberOfCallsToPrune() const;
    unsigned long getNumberOfUpdatesInLowerBound() const;
    unsigned long getSharedWork() const;
    
    void increaseNumberOfExploredNodes(unsigned long value);
    void increaseNumberOfCallsToBranch(unsigned long value);
    void increaseNumberOfBranches(unsigned long value);
    void increaseNumberOfCallsToPrune(unsigned long value);
    void increaseNumberOfPrunedNodes(unsigned long value);
    void increaseNumberOfReachedLeaves(unsigned long value);
    void increaseNumberOfUpdatesInLowerBound(unsigned long value);

    const IVMTree& getIVMTree() const;
    const Interval& getStartingInterval() const;
    const ProblemFJSSP& getProblem() const;
    const Solution& getIncumbentSolution() const;
    const FJSSPdata& getFJSSPdata() const;
    
    void saveEvery(double timeInSeconds);
    void setParetoFront(const std::vector<Solution>& front);
	int setParetoFrontFile(const char outputFile[255]);
    int setSummarizeFile(const char outputFile[255]);
	int saveParetoFront();
	int saveSummarize();

    void printDebug();
    
private:
	void printCurrentSolution(int withVariables = 0);
	int aLeafHasBeenReached() const;
	int theTreeHasMoreBranches() const;

	unsigned long computeTotalNodes(unsigned long totalVariables) const;
	unsigned long permut(unsigned long n, unsigned long i) const;

    void shareWorkAndSendToGlobalPool(const Interval& interval);
	/** Grid functions. **/
	int improvesTheGrid(const Solution & solution) const;
	int updateParetoGrid(const Solution & solution);
	/** End Grid functions. **/
    void updateBounds(const Solution & solution, FJSSPdata& data);
    void updateBoundsWithSolution(const Solution & solution);
    
    void setPriorityTo(Interval & interval) const;
	/** IVM functions. **/
public:
	void computeLastBranch(Interval & branch);
	void initGlobalPoolWithInterval(Interval & branch);

private:
	int initializeExplorationIntervalSolution(const Solution & branch, IVMTree & tree);
    int intializeIVM_data(Interval& branch, IVMTree & tree);
	/** End IVM functions **/

public:
	task* execute();
	/* void operator()(const Interval& branch) {
		this->solve(branch);
	}; */
};

#endif /* BranchAndBound_hpp */
