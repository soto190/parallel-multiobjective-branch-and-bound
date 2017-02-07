//
//  ParallelBranchAndBound.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 06/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "ParallelBranchAndBound.hpp"

tbb::task * ParallelBranchAndBound::execute(){
    
    int numberOfBB = 0;
    Interval branch_init (problem->getNumberOfVariables());
    
    BranchAndBound * BaB_master = new(tbb::task::allocate_root()) BranchAndBound(numberOfBB++, problem, branch_init);
    
    BaB_master->splitInterval(branch_init);
    
    Interval branch (0);
    int refCount = (int) BaB_master->intervals.size();
    this->set_ref_count(refCount + 1);

    
    tbb::task_list tl;
    while (BaB_master->intervals.size() > 0) {
        
        
        branch = BaB_master->intervals.back();
        BaB_master->intervals.pop_back();
        
//        mutexPareto.lock();
        
        std::shared_ptr<Problem> copyProblem = this->problem;
        BranchAndBound * BaB_task = new(tbb::task::allocate_child()) BranchAndBound(numberOfBB++, copyProblem, branch);
        BaB_task->setParetoContainer(BaB_master->getParetoContainer());
        
        tl.push_back(*BaB_task);
        //tbb::task::spawn(*BaB_task);
//        mutexPareto.unlock();

    }
    
    tbb::task::spawn_and_wait_for_all(tl);
    
    
    
    BaB_master->paretoFront = BaB_master->paretoContainer->getParetoFront();
    BaB_master->saveParetoFront();
    BaB_master->saveSummarize();
    
    return NULL;
}


void ParallelBranchAndBound::setInstanceFile(char *path []){
    this->path[0] = new char [255];
    this->path[1] = new char [255];
    std::strcpy(this->path[0], path[0]);
    std::strcpy(this->path[1], path[1]);
}

void ParallelBranchAndBound::setProblem(std::shared_ptr<Problem> problem){
    this->problem = problem;
}

void ParallelBranchAndBound::setParetoFrontFile(const char * outputFile){
    this->outputFile = new char[255];
    std::strcpy(this->outputFile, outputFile);
}

void ParallelBranchAndBound::setSummarizeFile(const char * outputFile){
    this->summarizeFile = new char[255];
    std::strcpy(this->summarizeFile, outputFile);
}
