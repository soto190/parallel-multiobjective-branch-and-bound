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

	BranchAndBound * BaB_master =
			new (tbb::task::allocate_root()) BranchAndBound(counter_threads,
					problem, branch_init);
	BaB_master->setParetoFrontFile(this->outputParetoFile);
	BaB_master->setSummarizeFile(this->summarizeFile);
	//BaB_master->setGlobalPool(BaB_master->globalPool);
	BaB_master->splitInterval(branch_init);

	this->set_ref_count(this->number_of_threads + 1);

	tbb::task_list tl;
    vector<BranchAndBound *> bb_threads;
	while (counter_threads++ < this->number_of_threads) {

		BranchAndBound * BaB_task =
				new (tbb::task::allocate_child()) BranchAndBound(
						counter_threads, this->problem, branch_init);
		BaB_task->setParetoContainer(BaB_master->getParetoContainer());
		BaB_task->setGlobalPool(BaB_master->globalPool);
        

        bb_threads.push_back(BaB_task);
		tl.push_back(*BaB_task);
	}

	printf("Spawning the swarm...\n");
	tbb::task::spawn_and_wait_for_all(tl);
	printf("Job done...\n");

	/** Recollects the data. **/
	BaB_master->getTotalTime();
	BranchAndBound* bb_in;
	while (!bb_threads.empty()) {

        bb_in = bb_threads.back();
        bb_threads.pop_back();
    
		BaB_master->exploredNodes += bb_in->exploredNodes;
		BaB_master->callsToBranch += bb_in->callsToBranch;
		BaB_master->branches += bb_in->branches;
		BaB_master->callsToPrune += bb_in->callsToPrune;
		BaB_master->prunedNodes += bb_in->prunedNodes;
		BaB_master->reachedLeaves += bb_in->reachedLeaves;
		BaB_master->totalUpdatesInLowerBound += bb_in->totalUpdatesInLowerBound;

	}
	printf("Data recollected.\n");

	BaB_master->paretoFront = BaB_master->paretoContainer->getParetoFront();
	BaB_master->printParetoFront();
	BaB_master->saveParetoFront();
	BaB_master->saveSummarize();

    
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
