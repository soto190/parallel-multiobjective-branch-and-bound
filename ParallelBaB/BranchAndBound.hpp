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
//#include <memory.h> /** For the Ehecatl wich uses GCC 4.4.7, this activates the shared_ptr. **/

struct GlobalPool{
    tbb::concurrent_queue<Interval> Queue;
    
    bool pop_if_present(Interval& interval){
        if(!Queue.try_pop(interval)) return false;
        return true;
    }
    
    unsigned long unsafe_size(){ return Queue.unsafe_size(); }
    void push(const Interval & interval){ Queue.push(interval);}
    bool try_pop(Interval& interval){ return Queue.try_pop(interval); }
    bool empty(){ return Queue.empty(); }
};

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
    
    int currentLevel; /** Active level **/
    
    ProblemFJSSP problem;
    Solution currentSolution;
    Solution bestObjectivesFound;
    IVMTree ivm_tree;
    Interval starting_interval;
    GlobalPool& globalPool;
    HandlerContainer& paretoContainer;
    std::vector<Solution> paretoFront; /** paretofFront. **/
    std::queue<Interval> localPool; /** intervals are the pending branches/subproblems/partialSolutions to be explored. **/
    
    char * outputFile;
    char * summarizeFile;
    
    double totalTime;
    std::clock_t start;
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    
public:
    BranchAndBound(const BranchAndBound& branchAndBound);
    BranchAndBound(int rank, const ProblemFJSSP& problem, const Interval & branch, GlobalPool& globa_pool, HandlerContainer& pareto_container);
    BranchAndBound& operator()(int rank, const ProblemFJSSP& problem, const Interval & branch);
	~BranchAndBound();
    
    int getRank() const;
    int getCurrentLevel() const;
    double getTotalTime();
    
	void solve(const Interval & interval);
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
    
    void increaseNumberOfExploredNodes(unsigned long value);
    void increaseNumberOfCallsToBranch(unsigned long value);
    void increaseNumberOfBranches(unsigned long value);
    void increaseNumberOfCallsToPrune(unsigned long value);
    void increaseNumberOfPrunedNodes(unsigned long value);
    void increaseNumberOfReachedLeaves(unsigned long value);
    void increaseNumberOfUpdatesInLowerBound(unsigned long value);

    const IVMTree& getIVMTree() const;
    const Interval& getStartingInterval() const;
    GlobalPool& getGlobalPool() const;
    const ProblemFJSSP& getProblem() const;
    HandlerContainer& getParetoGrid() const;
    const Solution& getIncumbentSolution() const;
    
    void setParetoFront(const std::vector<Solution>& front);
	int setParetoFrontFile(const char * outputFile);
	int saveParetoFront();
	int setSummarizeFile(const char * outputFile);
	int saveSummarize();
	void saveEvery(double timeInSeconds);

    void setParetoContainer(HandlerContainer & paretoContainer);
    HandlerContainer& getParetoContainer();

private:
	void printCurrentSolution(int withVariables = 0);
	int aLeafHasBeenReached() const;
	int theTreeHasMoreBranches();
	int getLowerBoundInObj(int nObj);

	unsigned long computeTotalNodes(unsigned long totalVariables);
	unsigned long permut(unsigned long n, unsigned long i);
	int getUpperBound(int objective);

	/** Grid functions. **/
	int improvesTheGrid(const Solution & solution) const;
	int updateParetoGrid(const Solution & solution);
	/** End Grid functions. **/

	/** IVM functions. **/
public:
	void computeLastBranch(Interval & branch);
	void splitInterval(const Interval & branch);
	void splitIntervalSolution(const Solution & solution);

private:
	int initializeExplorationInterval(const Interval & branch, IVMTree & tree);
	int initializeExplorationIntervalSolution(const Solution & branch,
			IVMTree & tree);
	/** End IVM functions **/

public:
	task* execute();
	void setGlobalPool(tbb::concurrent_queue<Interval>& globalPool);
	void operator()(const Interval& branch) {
		this->solve(branch);
	};

};

#endif /* BranchAndBound_hpp */
