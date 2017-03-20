//
//  ParallelBranchAndBound.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 06/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "ParallelBranchAndBound.hpp"

ParallelBranchAndBound::ParallelBranchAndBound(const ProblemFJSSP& problem, GlobalPool& global_pool, HandlerContainer& global_grid):
    problem(problem),
    global_pool(global_pool),
    global_grid(global_grid),
    branch_init(problem.getNumberOfVariables()){
        this->outputParetoFile = new char[255];
        this->summarizeFile = new char[255];
}

ParallelBranchAndBound::~ParallelBranchAndBound(){
    delete [] outputParetoFile;
    delete [] summarizeFile;
}

tbb::task * ParallelBranchAndBound::execute() {

    int counter_threads = 0;
    BranchAndBound BB_container(0, this->problem, branch_init, global_pool, global_grid);
    BB_container.setParetoFrontFile(this->outputParetoFile);
    BB_container.setSummarizeFile(this->summarizeFile);
    BB_container.splitInterval(branch_init);
    
	this->set_ref_count(this->number_of_threads + 1);

	tbb::task_list tl; /** When task_list is destroyed it doesn't calls the destructor. **/
    vector<BranchAndBound *> bb_threads;
	while (counter_threads++ < this->number_of_threads) {

		BranchAndBound * BaB_task =
				new (tbb::task::allocate_child()) BranchAndBound(
						counter_threads, this->problem, branch_init, global_pool, global_grid);

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
        BB_container.increaseNumberOfCallsToBranch(bb_in->getNumberOfCallsToBranch());// callsToBranch += bb_in->getNumberOfCallsToBranch();
        BB_container.increaseNumberOfBranches(bb_in->getNumberOfBranches()); // branches += bb_in->getNumberOfBranches();
        BB_container.increaseNumberOfCallsToPrune(bb_in->getNumberOfCallsToPrune());// callsToPrune += bb_in->getNumberOfCallsToPrune();
        BB_container.increaseNumberOfPrunedNodes(bb_in->getNumberOfPrunedNodes());//prunedNodes += bb_in->getNumberOfPrunedNodes();
        BB_container.increaseNumberOfReachedLeaves(bb_in->getNumberOfReachedLeaves());// reachedLeaves += bb_in->getNumberOfReachedLeaves();
        BB_container.increaseNumberOfUpdatesInLowerBound(bb_in->getNumberOfUpdatesInLowerBound());// totalUpdatesInLowerBound += ;
	}
    
	printf("Data recollected.\n");

	BB_container.getParetoFront();
	BB_container.printParetoFront();
	BB_container.saveParetoFront();
	BB_container.saveSummarize();
    
    bb_threads.clear();
	printf("Parallel Branch And Bound ended.\n");
	return NULL;
}

void ParallelBranchAndBound::setNumberOfThreads(int number_of_threads) {
	this->number_of_threads = number_of_threads;
}

void ParallelBranchAndBound::setParetoFrontFile(const char * outputFile) {
	std::strcpy(this->outputParetoFile, outputFile);
}

void ParallelBranchAndBound::setSummarizeFile(const char * outputFile) {
	std::strcpy(this->summarizeFile, outputFile);
}
