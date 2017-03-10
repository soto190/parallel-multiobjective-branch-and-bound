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
#include "Solution.hpp"
#include "myutils.hpp"
#include "GridContainer.hpp"
#include "Dominance.hpp"
#include "IVMTree.hpp"
#include "Interval.hpp"
#include "tbb/parallel_for.h"
#include "tbb/task.h"
#include "tbb/mutex.h"
#include "tbb/cache_aligned_allocator.h"
#include "tbb/concurrent_queue.h"
//#include <memory.h> /** For the Ehecatl wich uses GCC 4.4.7, this activates the shared_ptr. **/

class BranchAndBound: public tbb::task {

public:

	/**
	 * The constructor should receives the parameters to start at some point of the tree.
	 **/
	BranchAndBound();
    BranchAndBound(const BranchAndBound& branchAndBound);
    BranchAndBound(int rank, tbb::atomic<Problem *> problem);
    BranchAndBound(int rank, tbb::atomic<Problem *> problem,
			const Interval & branch);
	~BranchAndBound();

//	std::shared_ptr<Problem> problem;
    tbb::atomic<Problem *> problem;
	tbb::mutex MutexToUpdateGlobalPool;

	Solution currentSolution;
	Solution bestObjectivesFound;

	tbb::concurrent_queue<Interval>* globalPool;
	std::queue<Interval> localPool; /** intervals are the pending branches/subproblems/partialSolutions to be explored. **/

	std::vector<Solution> paretoFront; /** paretofFront. **/

	std::shared_ptr<HandlerContainer> paretoContainer; /** Global Pareto container. **/

	IVMTree ivm_tree;
	Interval starting_interval;

	int levels_completed;

	int currentLevel; /** Active level **/
	int totalLevels; /** Number of tree levels **/

	unsigned long totalNodes;
	unsigned long branches;
	unsigned long exploredNodes;
	unsigned long callsToBranch;
	unsigned long reachedLeaves;
	unsigned long unexploredNodes;
	unsigned long prunedNodes;
	unsigned long callsToPrune;

	unsigned long totalUpdatesInLowerBound;

	char * outputFile;
	char * summarizeFile;

	double totalTime;
	std::clock_t start;
	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
	double getTotalTime();

	void solve(const Interval & interval);
	void initialize(int starting_level);
	int explore(Solution & solution);
	void branch(Solution & solution, int currentLevel);
	void prune(Solution & solution, int currentLevel);
	void printParetoFront(int withVariables = 0);

	int setParetoFrontFile(const char * outputFile);
	int saveParetoFront();
	int setSummarizeFile(const char * outputFile);
	int saveSummarize();
	void saveEvery(double timeInSeconds);

	void setParetoContainer(std::shared_ptr<HandlerContainer> paretoContainer);
	std::shared_ptr<HandlerContainer> getParetoContainer();

private:
	int rank; /** identifies the number of thread-B&B**/
	void printCurrentSolution(int withVariables = 0);
	int aLeafHasBeenReached() const;
	int theTreeHasMoreBranches();
	int getLowerBoundInObj(int nObj);

	unsigned long computeTotalNodes(int totalVariables);
	long permut(int n, int i);
	int getUpperBound(int objective);

	/** Grid functions. **/
	int improvesTheGrid(Solution & solution) const;
	int updateParetoGrid(Solution & solution);
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
	void setGlobalPool(tbb::concurrent_queue<Interval>* globalPool);
	void operator()(const Interval& branch) {
		this->solve(branch);
	}
	;

};

#endif /* BranchAndBound_hpp */
