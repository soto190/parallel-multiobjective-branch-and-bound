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
#include <vector>
#include <queue>
#include <math.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include "NSGA_II.hpp"
#include "MOSA.hpp"
#include "Problem.hpp"
#include "ProblemFJSSP.hpp"
#include "Solution.hpp"
#include "ConcurrentHandlerContainer.hpp"
#include "HandlerContainer.hpp"
#include "Dominance.hpp"
#include "SortedVector.hpp"
#include "IVMTree.hpp"
#include "Interval.hpp"
#include "SubproblemsPool.hpp"
#include "tbb/atomic.h"
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

const float size_to_share = 0.2f; /** We share the half of the row. **/
const float deep_limit_share = 0.80f;

extern SubproblemsPool globalPool;  /** intervals are the pending branches/subproblems/partialSolutions to be explored. **/
//extern ConcurrentHandlerContainer paretoContainer;
extern ParetoBucket globalParetoFront;
extern tbb::atomic<int> sleeping_bb;
extern tbb::atomic<int> there_is_more_work;

class BranchAndBound: public tbb::task {
    
private:
    int node_rank;
    int bb_rank;

    tbb::atomic<unsigned long> number_of_nodes;
    tbb::atomic<unsigned long> number_of_nodes_created;
    tbb::atomic<unsigned long> number_of_nodes_pruned;
    tbb::atomic<unsigned long> number_of_nodes_explored;
    tbb::atomic<unsigned long> number_of_nodes_unexplored;
    tbb::atomic<unsigned long> number_of_calls_to_branch;
    tbb::atomic<unsigned long> number_of_reached_leaves;
    tbb::atomic<unsigned long> number_of_calls_to_prune;
    tbb::atomic<unsigned long> number_of_updates_in_lower_bound;
    tbb::atomic<unsigned long> number_of_tree_levels;
    tbb::atomic<unsigned long> number_of_shared_works;
    
    int currentLevel; /** Active level **/

    HandlerContainer paretoContainer;
    ProblemFJSSP problem;
    FJSSPdata fjssp_data;
    Solution incumbent_s;
    IVMTree ivm_tree;
    Interval interval_to_solve;
    ParetoFront pareto_front; /** paretoFront. **/
    
    char pareto_file[255];
    char summarize_file[255];
    char pool_file[255];
    char ivm_file[255];
    
    int branches_to_move;
    int limit_level_to_share;
    
    double elapsed_time;
    std::clock_t start;
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    
public:
    BranchAndBound(const BranchAndBound& branchAndBound);
    BranchAndBound(int node_rank, int rank, const ProblemFJSSP& problem, const Interval & branch);
    BranchAndBound& operator()(int node_rank, int rank, const ProblemFJSSP& problem, const Interval & branch);
    ~BranchAndBound();
    
    int getNodeRank() const;
    int getBBRank() const;
    int getCurrentLevel() const;
    double getElapsedTime();
    
    void solve(Interval & interval);
    void initialize(int starting_level);
    int explore(Solution & solution);
    int branch(Solution & solution, int currentLevel);
    void prune(Solution & solution, int currentLevel);

    void printParetoFront(int withVariables = 0);
    
    unsigned long getNumberOfNodes() const;
    unsigned long getNumberOfLevels() const;
    unsigned long getNumberOfNodesCreated() const;
    unsigned long getNumberOfNodesExplored() const;
    unsigned long getNumberOfCallsToBranch() const;
    unsigned long getNumberOfReachedLeaves() const;
    unsigned long getNumberOfNodesUnexplored() const;
    unsigned long getNumberOfNodesPruned() const;
    unsigned long getNumberOfCallsToPrune() const;
    unsigned long getNumberOfUpdatesInLowerBound() const;
    unsigned long getSharedWork() const;
    
    void increaseNumberOfNodesExplored(unsigned long value);
    void increaseNumberOfCallsToBranch(unsigned long value);
    void increaseNumberOfNodesCreated(unsigned long value);
    void increaseNumberOfCallsToPrune(unsigned long value);
    void increaseNumberOfNodesPruned(unsigned long value);
    void increaseNumberOfReachedLeaves(unsigned long value);
    void increaseNumberOfUpdatesInLowerBound(unsigned long value);
    void increaseSharedWork(unsigned long value);
    
    const IVMTree& getIVMTree() const;
    const Interval& getStartingInterval() const;
    const ProblemFJSSP& getProblem() const;
    const Solution& getIncumbentSolution() const;
    const FJSSPdata& getFJSSPdata() const;
    const HandlerContainer& getParetoContainer() const;
    const ParetoFront& getParetoFront() const;

    float getDeepLimitToShare() const;
    float getSizeToShare() const;
    
    void saveEvery(double timeInSeconds);
    void setParetoFront(const std::vector<Solution>& front);
    void setParetoFrontFile(const char outputFile[255]);
    void setSummarizeFile(const char outputFile[255]);
    void setPoolFile(const char outputFile[255]);
    int saveParetoFront();
    int saveSummarize();
    void saveGlobalPool() const;
    void saveCurrentState() const;

    void print() const;
    void printDebug();
    int initGlobalPoolWithInterval(const Interval & branch);
    static float distanceToObjective(int value, int objective);
    task* execute();
    
private:

    bool aLeafHasBeenReached() const;
    bool theTreeHasMoreNodes() const;
    bool thereIsMoreWork() const;
    unsigned long computeTotalNodes(unsigned long totalVariables) const;
    unsigned long permut(unsigned long n, unsigned long i) const;
    
    void shareWorkAndSendToGlobalPool(const Interval& interval);
    bool improvesTheGrid(const Solution & solution);
    bool updateParetoGrid(const Solution & solution);
    void updateBounds(const Solution & solution, FJSSPdata& data);
    void updateBoundsWithSolution(const Solution & solution);
    void setPriorityTo(Interval & interval) const;

    int getLimitLevelToShare() const;
    void computeLastBranch(Interval & branch);
    
    int initializeExplorationIntervalSolution(const Solution & branch, IVMTree & tree);
    int intializeIVM_data(Interval& branch, IVMTree & tree);

    void updateLocalAndGlobalPFBounds();
    void updateGlobalPF(const Solution& new_solution);
    void updateLocalPF();
    void saveIVM() const;
    void buildOutputFiles();
    void increaseExploredNodes();
    void increasePrunedNodes();
    void increaseEvaluatedNodes();
    void increaseNodesCreated();
    void increaseReachedLeaves();
    void increaseUpdatesInLowerBound();
    void increaseSharedWorks();
    void printCurrentSolution(int withVariables = 0);
};
#endif /* BranchAndBound_hpp */
