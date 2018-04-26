//
//  ParallelBranchAndBound.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 06/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//
/** Dummy commit**/
#include "ParallelBranchAndBound.hpp"

ParallelBranchAndBound::ParallelBranchAndBound(int rank, int n_threads, const ProblemFJSSP& problem):
time_limit(0),
is_grid_enable(false),
is_sorting_enable(false),
is_priority_enable(false),
rank(rank),
number_of_bbs(n_threads),
problem(problem),
branch_init(problem.getNumberOfVariables()) {
    number_of_nodes = 0;
    number_of_nodes_created = 0;
    number_of_nodes_pruned = 0;
     number_of_nodes_explored = 0;
     number_of_nodes_unexplored = 0;
     number_of_calls_to_branch = 0;
     number_of_reached_leaves = 0;
     number_of_calls_to_prune = 0;
     number_of_updates_in_lower_bound = 0;
     number_of_tree_levels = 0;
     number_of_shared_works = 0;

}

ParallelBranchAndBound::~ParallelBranchAndBound() {
    
}

tbb::task * ParallelBranchAndBound::execute() {
    
    sharedPool.setSizeEmptying((unsigned long) (number_of_bbs * 2)); /** If the global pool reach this size then the B&B starts sending part of their work to the global pool. **/

    BranchAndBound BB_container(rank, 0, problem, branch_init);

    if (isGridEnable())
        BB_container.enableGrid();
    
    if (isSortingEnable())
        BB_container.enableSortingNodes();

    if (isPriorityEnable())
        BB_container.enablePriorityQueue();

    BB_container.initialize(0);
    number_of_shared_works += BB_container.initGlobalPoolWithInterval(branch_init);

    BB_container.setTimeLimit(getTimeLimit());

    set_ref_count(number_of_bbs + 1);
    
    tbb::task_list tl; /** When task_list is destroyed it doesn't calls the destructor. **/
    vector<BranchAndBound *> bb_threads;
    int n_bb = 0;
    while (n_bb++ < number_of_bbs) {
        BranchAndBound * BaB_task = new (tbb::task::allocate_child()) BranchAndBound(rank, n_bb,  problem, branch_init);
        if (isGridEnable())
            BaB_task->enableGrid();

        if (isSortingEnable())
            BaB_task->enableSortingNodes();

        if (isPriorityEnable())
            BaB_task->enablePriorityQueue();

        BaB_task->setTimeLimit(getTimeLimit());
        BaB_task->setSummarizeFile(summarizeFile);

        bb_threads.push_back(BaB_task);
        tl.push_back(*BaB_task);
    }
    
    printf("[Worker-%03d] Spawning the swarm...\nWaiting for all...\n", rank);
    tbb::task::spawn_and_wait_for_all(tl);
    printf("[Worker-%03d] Job done...\n", rank);
    
    /** Recollects the data. **/
    BB_container.getElapsedTime();

    BranchAndBound* bb_in;
    while (!bb_threads.empty()) {
        
        bb_in = bb_threads.back();
        bb_threads.pop_back();

//        number_of_nodes_created += bb_in->getNumberOfNodesCreated();
//        number_of_nodes_pruned += bb_in->getNumberOfNodesPruned();
//        number_of_nodes_explored += bb_in->getNumberOfNodesExplored();
//        number_of_calls_to_branch += bb_in->getNumberOfCallsToBranch();
//        number_of_reached_leaves += bb_in->getNumberOfNodesExplored();
//        number_of_calls_to_prune += bb_in->getNumberOfCallsToPrune();
//        number_of_updates_in_lower_bound += bb_in->getNumberOfUpdatesInLowerBound();
//        number_of_shared_works += bb_in->getSharedWork();

        BB_container.increaseNumberOfNodesExplored(bb_in->getNumberOfNodesExplored());
        BB_container.increaseNumberOfCallsToBranch(bb_in->getNumberOfCallsToBranch());
        BB_container.increaseNumberOfNodesCreated(bb_in->getNumberOfNodesCreated());
        BB_container.increaseNumberOfCallsToPrune(bb_in->getNumberOfCallsToPrune());
        BB_container.increaseNumberOfNodesPruned(bb_in->getNumberOfNodesPruned());
        BB_container.increaseNumberOfReachedLeaves(bb_in->getNumberOfReachedLeaves());
        BB_container.increaseNumberOfUpdatesInLowerBound(bb_in->getNumberOfUpdatesInLowerBound());
        BB_container.increaseSharedWork(bb_in->getSharedWork());

    }
    cout << number_of_nodes_created << endl;
    cout << number_of_nodes_pruned << endl;
    cout << number_of_nodes_explored << endl;
    cout << number_of_calls_to_branch << endl;
    cout << number_of_reached_leaves << endl;
    cout << number_of_calls_to_prune << endl;
    cout << number_of_updates_in_lower_bound << endl;
    cout << number_of_shared_works << endl;

    printf("[Worker-%03d] Parallel Branch And Bound front.\n", rank);
    sharedParetoFront.print();

    BB_container.setParetoFrontFile(outputParetoFile);
    BB_container.setSummarizeFile(summarizeFile);
    BB_container.saveParetoFront();
    BB_container.saveSummarize();
    bb_threads.clear();
    printf("[Worker-%03d] Parallel Branch And Bound ended.\n", rank);
    return NULL;
}

double ParallelBranchAndBound::getTimeLimit() const {
    return time_limit;
}

void ParallelBranchAndBound::setTimeLimit(double time_sec) {
    time_limit = time_sec;
}

void ParallelBranchAndBound::enableGrid() {
    is_grid_enable = true;
}

void ParallelBranchAndBound::enableSortingNodes() {
    is_sorting_enable = true;
}

void ParallelBranchAndBound::enablePriorityQueue() {
    is_priority_enable = true;
}

bool ParallelBranchAndBound::isGridEnable() const {
    return is_grid_enable;
}

bool ParallelBranchAndBound::isSortingEnable() const {
    return is_sorting_enable;
}

bool ParallelBranchAndBound::isPriorityEnable() const {
    return is_priority_enable;
}

void ParallelBranchAndBound::setBranchInitPayload(const Payload_interval& payload) {
    branch_init(payload);
}

void ParallelBranchAndBound::setBranchInit(const Interval &interval) {
    branch_init = interval;
}

void ParallelBranchAndBound::setNumberOfThreads(int n_number_of_threads) {
    number_of_bbs = n_number_of_threads;
}

void ParallelBranchAndBound::setParetoFrontFile(const char outputFile[255]) {
    std::strcpy(outputParetoFile, outputFile);
}

void ParallelBranchAndBound::setSummarizeFile(const char outputFile[255]) {
    std::strcpy(summarizeFile, outputFile);
}

int ParallelBranchAndBound::getRank() const {
    return rank;
}
