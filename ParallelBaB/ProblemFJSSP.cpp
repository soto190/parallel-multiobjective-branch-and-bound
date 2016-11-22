//
//  ProblemFJSSP.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 14/11/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "ProblemFJSSP.hpp"


ProblemFJSSP::~ProblemFJSSP(){
    int job = 0;
    for(job = 0; job < this->totalJobs; job++){
        delete [] processingTime[job];
        delete [] jobOperations[job];
    }
    
    delete [] processingTime;
    delete [] operationsInJob;
    delete [] jobOperations;

}
/**
 * The first part of the array is the order of execution.
 * The second part of the array is the machine assignations.
 */
double ProblemFJSSP::evaluate(Solution * solution){
    
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int currentJob = 0;
    int operation = 0;
    int job = 0;
    int machine = 0;
    int numberOp = 0;
    
    int operationOfJob [this->totalJobs];
    int startingTime [this->totalOperations];
    int endingTime [this->totalOperations];
    int timeInMachine [this->totalMachines];
    int workload [this->totalMachines];
    
    for (operation = 0; operation < this->totalOperations; operation++) {
        startingTime[operation] = 0;
        endingTime[operation] = 0;
    }
    
    for (job = 0; job < this->totalJobs; job++)
        operationOfJob[job] = 0;
    
    for (machine = 0; machine < this->totalMachines; machine++){
        timeInMachine[machine] = 0;
        workload[machine] = 0;
    }
    
    for (currentJob = 0; currentJob < this->totalOperations; currentJob++) {
        job = solution->getVariable(currentJob);
        machine = solution->getVariable(currentJob + this->totalOperations);
        numberOp = this->jobOperations[job][operationOfJob[job]];
        
        workload[machine] += this->processingTime[numberOp][machine];
        totalWorkload += this->processingTime[numberOp][machine];
        
        if (operationOfJob[job] == 0) { /** If it is the first operation of the job.**/
           
            startingTime[numberOp] = timeInMachine[machine];
            timeInMachine[machine] += this->processingTime[numberOp][machine];
            endingTime[numberOp] = timeInMachine[machine];
        
        }else{
            if(endingTime[numberOp - 1] > timeInMachine[machine]){ /**The operation is waiting for their dependency operation.**/
                
                startingTime[numberOp] = endingTime[numberOp - 1];
                timeInMachine[machine] = endingTime[numberOp - 1] + this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
                
            }else{ /**The operation starts when the machine is avaliable.**/
                
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            
            }
        }
        
        operationOfJob[job]++;
        
        if (timeInMachine[machine] > makespan)
            makespan = timeInMachine[machine];
        
        if(workload[machine] > maxWorkload)
            maxWorkload = workload[machine];
    }
    
    solution->setObjective(0, makespan);
    solution->setObjective(1, maxWorkload);
    
   /**
    for (operation = 0; operation < this->totalOperations; operation++) {
        printf("%d: %d %d - %d \n", operation, solution->getVariable(operation + this->totalOperations), startingTime[operation], endingTime[operation]);
    }
    
    printf("makespan: %d\nmaxWorkLoad: %d\ntotalWorkload: %d \n", makespan, maxWorkload, totalWorkload);
    **/
    return 0.0;
}

double ProblemFJSSP::evaluatePartial(Solution * solution, int levelEvaluation){
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int currentJob = 0;
    int operation = 0;
    int job = 0;
    int machine = 0;
    int numberOp = 0;
    
    int operationOfJob [this->totalJobs];
    int startingTime [this->totalOperations];
    int endingTime [this->totalOperations];
    int timeInMachine [this->totalMachines];
    int workload [this->totalMachines];
    
    for (operation = 0; operation < this->totalOperations; operation++) {
        startingTime[operation] = 0;
        endingTime[operation] = 0;
    }
    
    for (job = 0; job < this->totalJobs; job++)
        operationOfJob[job] = 0;
    
    for (machine = 0; machine < this->totalMachines; machine++){
        timeInMachine[machine] = 0;
        workload[machine] = 0;
    }
    
    for (currentJob = 0; currentJob < levelEvaluation; currentJob++) {
        job = solution->getVariable(currentJob);
        machine = solution->getVariable(currentJob + this->totalOperations);
        numberOp = this->jobOperations[job][operationOfJob[job]];
        
        workload[machine] += this->processingTime[numberOp][machine];
        totalWorkload += this->processingTime[numberOp][machine];
        
        if (operationOfJob[job] == 0) { /** If it is the first operation of the job.**/
            
            startingTime[numberOp] = timeInMachine[machine];
            timeInMachine[machine] += this->processingTime[numberOp][machine];
            endingTime[numberOp] = timeInMachine[machine];
            
        }else{
            if(endingTime[numberOp - 1] > timeInMachine[machine]){ /**The operation is waiting for their dependency operation.**/
                
                startingTime[numberOp] = endingTime[numberOp - 1];
                timeInMachine[machine] = endingTime[numberOp - 1] + this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
                
            }else{ /**The operation starts when the machine is avaliable.**/
                
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
                
            }
        }
        
        operationOfJob[job]++;
        
        if (timeInMachine[machine] > makespan)
            makespan = timeInMachine[machine];
        
        if(workload[machine] > maxWorkload)
            maxWorkload = workload[machine];
    }
    
    solution->setObjective(0, makespan);
    solution->setObjective(1, maxWorkload);

    return 0.0;
}

double ProblemFJSSP::evaluateLastLevel(Solution * solution){
    return 0.0;
}

double ProblemFJSSP::removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel){
    return 0.0;
}

double ProblemFJSSP::removeLastLevelEvaluation(Solution * solution, int newLevel){
    return 0.0;
}

void ProblemFJSSP::createDefaultSolution(Solution * solution){
}

int ProblemFJSSP::getLowerBound(int indexVar){
    return 0;
}

int ProblemFJSSP::getUpperBound(int indexVar){
    if(indexVar > this->totalOperations)
        return this->totalMachines;
    else
        return this->totalJobs;
}

ProblemType ProblemFJSSP::getType(){
    return ProblemType::permutation_with_repetition_and_combination;
}

int ProblemFJSSP::getStartingLevel(){
    return 0;
}

int ProblemFJSSP::getFinalLevel(){
    return this->totalVariables;
}

int * ProblemFJSSP::getElemensToRepeat(){
    return this->operationsInJob;
}

Solution* ProblemFJSSP::createSolution(){
    Solution* solution = new Solution(this->getNumberOfObjectives(), this->getNumberOfVariables());
    return solution;

}

void ProblemFJSSP::loadInstance(char* filePath[]){
    std::ifstream infile(filePath[0]);
    std::string line;
    std::vector<std::string> elemens;
    
    std::getline(infile, line);
    std::getline(infile, line);
    elemens = split(line, ' ');
    this->totalJobs = std::stoi(elemens.at(0));
    this->totalMachines = std::stoi(elemens.at(1));
    this->operationsInJob = new int[this->totalJobs];
    
    std::getline(infile, line);
    std::getline(infile, line);
    elemens = split(line, ' ');
    int job = 0;
    this->totalOperations = 0;
    for(job = 0; job < this->totalJobs; job++){
        this->operationsInJob[job] = std::stoi(elemens.at(job));
        this->totalOperations += this->operationsInJob[job];
    }
    
    std::getline(infile, line);
    this->processingTime = new int * [this->totalOperations];
    int operation = 0;
    int machine = 0;
    for (operation = 0; operation < this->totalOperations; operation++) {
        this->processingTime[operation] = new int[this->totalMachines];
        std::getline(infile, line);
        elemens = split(line, ' ');
        
        for (machine = 0; machine < this->totalMachines; machine++)
            this->processingTime[operation][machine] = std::stoi(elemens.at(machine));
    }
    
    this->totalVariables = this->totalOperations;
    this->totalObjectives = 2;
    
    int operationCounter = 0;
    this->jobOperations = new int * [this->totalJobs];
    for (job = 0; job < this->totalJobs; job++) {
        this->jobOperations[job] = new int[this->operationsInJob[job]];
        for(operation = 0; operation < this->operationsInJob[job]; operation++)
            this->jobOperations[job][operation] = operationCounter++;
    }
}

void ProblemFJSSP::printInstance(){}

void ProblemFJSSP::printProblemInfo(){
    printf("Total jobs: %d\n", this->totalJobs);
    printf("Total machines: %d\n", this->totalMachines);
    printf("Total operations: %d\n", this->totalOperations);

    printf("Operations in each job: ");
    
    int job = 0, machine = 0, operation = 0;
    for (job = 0; job < this->totalJobs; job++)
        printf("%2d ", this->operationsInJob[job]);
    printf("\n");
    
    printf("Processing times: \n");
    printf("    ");
    for (machine = 0; machine < this->totalMachines; machine++)
        printf("M%-2d ", machine);
    printf("\n");
    for (operation = 0; operation < this->totalOperations; operation++) {
        printf("%2d:", operation);
        for (machine = 0; machine < this->totalMachines; machine++)
            printf("%3d ", this->processingTime[operation][machine]);
        printf("\n");
    }
}


double ProblemFJSSP::printSchedule(Solution * solution){
    
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int currentJob = 0;
    int operation = 0;
    int job = 0;
    int machine = 0;
    int numberOp = 0;
    
    int operationOfJob [this->totalJobs];
    int startingTime [this->totalOperations];
    int endingTime [this->totalOperations];
    int timeInMachine [this->totalMachines];
    int workload [this->totalMachines];
    
    for (operation = 0; operation < this->totalOperations; operation++) {
        startingTime[operation] = 0;
        endingTime[operation] = 0;
    }
    
    for (job = 0; job < this->totalJobs; job++)
        operationOfJob[job] = 0;
    
    for (machine = 0; machine < this->totalMachines; machine++){
        timeInMachine[machine] = 0;
        workload[machine] = 0;
    }
    
    for (currentJob = 0; currentJob < this->totalOperations; currentJob++) {
        job = solution->getVariable(currentJob);
        machine = solution->getVariable(currentJob + this->totalOperations);
        numberOp = this->jobOperations[job][operationOfJob[job]];
        
        workload[machine] += this->processingTime[numberOp][machine];
        totalWorkload += this->processingTime[numberOp][machine];
        
        if (operationOfJob[job] == 0) { /** If it is the first operation of the job.**/
            
            startingTime[numberOp] = timeInMachine[machine];
            timeInMachine[machine] += this->processingTime[numberOp][machine];
            endingTime[numberOp] = timeInMachine[machine];
            
        }else{
            if(endingTime[numberOp - 1] > timeInMachine[machine]){ /**The operation is waiting for their dependency operation.**/
                
                startingTime[numberOp] = endingTime[numberOp - 1];
                timeInMachine[machine] = endingTime[numberOp - 1] + this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
                
            }else{ /**The operation starts when the machine is avaliable.**/
                
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
                
            }
        }
        
        operationOfJob[job]++;
        
        if (timeInMachine[machine] > makespan)
            makespan = timeInMachine[machine];
        
        if(workload[machine] > maxWorkload)
            maxWorkload = workload[machine];
    }
    
    solution->setObjective(0, makespan);
    solution->setObjective(1, maxWorkload);
    
    for (operation = 0; operation < this->totalOperations; operation++)
        printf("%d: %d %d - %d \n", operation, solution->getVariable(operation + this->totalOperations), startingTime[operation], endingTime[operation]);
    
    printf("makespan: %d\nmaxWorkLoad: %d\ntotalWorkload: %d \n", makespan, maxWorkload, totalWorkload);
    
    return 0.0;
}
