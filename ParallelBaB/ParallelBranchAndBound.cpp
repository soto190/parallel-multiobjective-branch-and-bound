//
//  ParallelBranchAndBound.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 06/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "ParallelBranchAndBound.hpp"

tbb::task * ParallelBranchAndBound::execute() {

    int counter_threads = 0;
	Interval branch_init(problem->getNumberOfVariables());
    
    printf("From parallel B&B %d\n", this->problem->getType());
    
    BranchAndBound BB_container (0, problem, branch_init);
	BB_container.setParetoFrontFile(this->outputParetoFile);
	BB_container.setSummarizeFile(this->summarizeFile);
	BB_container.splitInterval(branch_init);

	this->set_ref_count(this->number_of_threads + 1);

	tbb::task_list tl; /** When task_list is destroyed it doesn't calls the destructor. **/
    vector<BranchAndBound *> bb_threads;
	while (counter_threads++ < this->number_of_threads) {

		BranchAndBound * BaB_task =
				new (tbb::task::allocate_child()) BranchAndBound(
						counter_threads, this->problem, branch_init);
		BaB_task->setParetoContainer(BB_container.getParetoContainer());
		BaB_task->setGlobalPool(BB_container.globalPool);
        

        bb_threads.push_back(BaB_task);
		tl.push_back(*BaB_task);
	}

	printf("Spawning the swarm...\n");
    tbb::task::spawn(tl);
    printf("Waiting for all...\n");
    tbb::task::wait_for_all();
    printf("Job done...\n");

    //tbb::task::spawn_and_wait_for_all(tl);
	
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

    
    //bb_threads.clear();
	printf("Parallel Branch And Bound ended.\n");
	return NULL;
}

void ParallelBranchAndBound::setNumberOfThreads(int number_of_threads) {
	this->number_of_threads = number_of_threads;
}

void ParallelBranchAndBound::setInstanceFile(char *path[]) {
	this->path[0] = new char[255];
	this->path[1] = new char[255];

	std::strcpy(this->path[0], path[0]);
	std::strcpy(this->path[1], path[1]);
}

void ParallelBranchAndBound::setProblem(std::shared_ptr<Problem> problem) {
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
