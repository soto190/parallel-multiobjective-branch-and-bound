//
//  ParallelBranchAndBound.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 06/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "ParallelBranchAndBound.hpp"

ParallelBranchAndBound::ParallelBranchAndBound(int rank, int n_threads, const ProblemFJSSP& problem):
rank(rank),
number_of_bbs(n_threads),
problem(problem),
branch_init(problem.getNumberOfVariables()){
}

ParallelBranchAndBound::~ParallelBranchAndBound(){}

tbb::task * ParallelBranchAndBound::execute() {
    
    globalPool.setSizeEmptying((unsigned long) (number_of_bbs * 2)); /** If the global pool reach this size then the B&B starts sending part of their work to the global pool. **/
   
    Solution solution (problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.createDefaultSolution(solution);

    paretoContainer(25, 25, solution.getObjective(0), solution.getObjective(1), problem.getLowerBoundInObj(0), problem.getLowerBoundInObj(1));
    
    BranchAndBound BB_container(rank, 0, problem, branch_init);
    BB_container.initGlobalPoolWithInterval(branch_init);
    
    set_ref_count(number_of_bbs + 1);
    
    tbb::task_list tl; /** When task_list is destroyed it doesn't calls the destructor. **/
    vector<BranchAndBound *> bb_threads;
    int n_bb = 0;
    while (n_bb++ < number_of_bbs) {
        
        BranchAndBound * BaB_task = new (tbb::task::allocate_child()) BranchAndBound(rank, n_bb, problem, branch_init);
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
        
        BB_container.increaseNumberOfExploredNodes(bb_in->getNumberOfExploredNodes());
        BB_container.increaseNumberOfCallsToBranch(bb_in->getNumberOfCallsToBranch());
        BB_container.increaseNumberOfBranches(bb_in->getNumberOfBranches());
        BB_container.increaseNumberOfCallsToPrune(bb_in->getNumberOfCallsToPrune());
        BB_container.increaseNumberOfPrunedNodes(bb_in->getNumberOfPrunedNodes());
        BB_container.increaseNumberOfReachedLeaves(bb_in->getNumberOfReachedLeaves());
        BB_container.increaseNumberOfUpdatesInLowerBound(bb_in->getNumberOfUpdatesInLowerBound());
        BB_container.increaseSharedWork(bb_in->getSharedWork());
    }
    
    BB_container.setParetoFrontFile(outputParetoFile);
    BB_container.setSummarizeFile(summarizeFile);
    
    BB_container.getParetoFront();
    BB_container.printParetoFront();
    BB_container.saveParetoFront();
    BB_container.saveSummarize();
    bb_threads.clear();
    //printf("[Worker-%03d] Data swarm recollected and saved.\n", rank);
    printf("[Worker-%03d] Parallel Branch And Bound ended.\n", rank);
    return NULL;
}

std::vector<Solution>& ParallelBranchAndBound::getParetoFront(){
    return paretoContainer.getParetoFront();
}

void ParallelBranchAndBound::setBranchInitPayload(const Payload_interval& payload){
    branch_init(payload);
}

void ParallelBranchAndBound::setBranchInit(const Interval &interval){
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

int ParallelBranchAndBound::getRank() const{
    return rank;
}
