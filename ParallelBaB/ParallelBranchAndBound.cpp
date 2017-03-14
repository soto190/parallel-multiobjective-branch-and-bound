//
//  ParallelBranchAndBound.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 06/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "ParallelBranchAndBound.hpp"

ParallelBranchAndBound::ParallelBranchAndBound(const ProblemFJSSP& problem, GlobalPool& global_pool):global_pool(global_pool){
    this->problem = problem;
    this->branch_init(this->problem.getNumberOfVariables());
}

tbb::task * ParallelBranchAndBound::execute() {

    int counter_threads = 0;
    BranchAndBound BB_container(0, this->problem, branch_init, global_pool);
    BB_container.setParetoFrontFile(this->outputParetoFile);
    BB_container.setSummarizeFile(this->summarizeFile);
    BB_container.splitInterval(branch_init);
    
	this->set_ref_count(this->number_of_threads + 1);

	tbb::task_list tl; /** When task_list is destroyed it doesn't calls the destructor. **/
    vector<BranchAndBound *> bb_threads;
	while (counter_threads++ < this->number_of_threads) {

		BranchAndBound * BaB_task =
				new (tbb::task::allocate_child()) BranchAndBound(
						counter_threads, this->problem, branch_init, global_pool);

		BaB_task->setParetoContainer(BB_container.getParetoContainer());
        

        bb_threads.push_back(BaB_task);
		tl.push_back(*BaB_task);
	}

    printf("Spawning the swarm...\n Waiting for all...\n");
    tbb::task::spawn_and_wait_for_all(tl);
    printf("Job done...\n");
    
	/** Recollects the data. **/
	BB_container.getTotalTime();
	BranchAndBound* bb_in;
	while (!bb_threads.empty()) {

        bb_in = bb_threads.back();
        bb_threads.pop_back();
    
		BB_container.exploredNodes += bb_in->exploredNodes;
		BB_container.callsToBranch += bb_in->callsToBranch;
		BB_container.branches += bb_in->branches;
		BB_container.callsToPrune += bb_in->callsToPrune;
		BB_container.prunedNodes += bb_in->prunedNodes;
		BB_container.reachedLeaves += bb_in->reachedLeaves;
		BB_container.totalUpdatesInLowerBound += bb_in->totalUpdatesInLowerBound;
	}
	printf("Data recollected.\n");

	BB_container.paretoFront = BB_container.paretoContainer->getParetoFront();
	BB_container.printParetoFront();
	BB_container.saveParetoFront();
	BB_container.saveSummarize();
    
	printf("Parallel Branch And Bound ended.\n");
	return NULL;
}

void ParallelBranchAndBound::setNumberOfThreads(int number_of_threads) {
	this->number_of_threads = number_of_threads;
}

void ParallelBranchAndBound::setProblem(const ProblemFJSSP&  problem) {
	this->problem = problem;
}

void ParallelBranchAndBound::setParetoFrontFile(const char * outputFile) {
	this->outputParetoFile = new char[255];
	std::strcpy(this->outputParetoFile, outputFile);
}

void ParallelBranchAndBound::setSummarizeFile(const char * outputFile) {
	this->summarizeFile = new char[255];
	std::strcpy(this->summarizeFile, outputFile);
}
