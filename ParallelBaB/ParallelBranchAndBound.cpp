//
//  ParallelBranchAndBound.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 06/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "ParallelBranchAndBound.hpp"

ParallelBranchAndBound::ParallelBranchAndBound(const ProblemFJSSP& problem):
problem(problem),
branch_init(problem.getNumberOfVariables()){}

ParallelBranchAndBound::~ParallelBranchAndBound(){}

tbb::task * ParallelBranchAndBound::execute() {

    unsigned long  shared_work = 0;
    globalPool.setSizeEmptying((unsigned long) (number_of_threads * 2)); /** If the global pool reach this size then the B&B starts sending part of their work to the global pool. **/
    
    BranchAndBound BB_container(0, problem, branch_init);
    BB_container.initGlobalPoolWithInterval(branch_init);
    
	set_ref_count(number_of_threads + 1);

	tbb::task_list tl; /** When task_list is destroyed it doesn't calls the destructor. **/
    vector<BranchAndBound *> bb_threads;
    int counter_threads = 0;
	while (counter_threads++ < number_of_threads) {

		BranchAndBound * BaB_task = new (tbb::task::allocate_child()) BranchAndBound(counter_threads, problem, branch_init);

        bb_threads.push_back(BaB_task);
		tl.push_back(*BaB_task);
	}

    printf("Spawning the swarm...\nWaiting for all...\n");
    tbb::task::spawn_and_wait_for_all(tl);
    printf("Job done...\n");

    /** Recollects the data. **/
	BB_container.getTotalTime();
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
        shared_work += bb_in->getSharedWork();
	}
    
    BB_container.setParetoFrontFile(outputParetoFile);
    BB_container.setSummarizeFile(summarizeFile);
    
	BB_container.getParetoFront();
	BB_container.printParetoFront();
	BB_container.saveParetoFront();
	BB_container.saveSummarize();
    bb_threads.clear();
    printf("Shared work: %lu\n", shared_work);
    printf("Data swarm recollected and saved.\n");
	printf("Parallel Branch And Bound ended.\n");
	return NULL;
}

void ParallelBranchAndBound::setNumberOfThreads(int n_number_of_threads) {
	number_of_threads = n_number_of_threads;
}

void ParallelBranchAndBound::setParetoFrontFile(const char outputFile[255]) {
	std::strcpy(outputParetoFile, outputFile);
}

void ParallelBranchAndBound::setSummarizeFile(const char outputFile[255]) {
	std::strcpy(summarizeFile, outputFile);
}
