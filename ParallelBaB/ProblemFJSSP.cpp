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
        delete [] operationInJobIsNumber[job];
    }
    
    delete [] processingTime;
    delete [] jobHasNoperations;
    delete [] operationInJobIsNumber;
    delete [] operationIsFromJob;

}
/**
 * The first part of the array is the order of execution.
 * The second part of the array is the machine assignations.
 */
double ProblemFJSSP::evaluate(Solution * solution){
    
    evaluatePartial(solution, this->getFinalLevel());
    
    return 0.0;
}

double ProblemFJSSP::evaluatePartial(Solution * solution, int levelEvaluation){
    evaluatePartialTest3(solution, levelEvaluation);
    return 0.0;
}

double ProblemFJSSP::evaluatePartialTest1(Solution * solution, int levelEvaluation){
    
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int operationInPosition = 0;
    int currentOperation = 0;
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
    
    for (operationInPosition = 0; operationInPosition <= levelEvaluation; operationInPosition++) {
        
        currentOperation = solution->getVariable(operationInPosition);
        job = this->operationIsFromJob[currentOperation];
        machine = solution->getVariable(operationInPosition + this->totalOperations);
        
        numberOp = this->operationInJobIsNumber[job][operationOfJob[job]];
        
        /** With the number of operation and the machine we can continue. **/
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

double ProblemFJSSP::evaluatePartialTest2(Solution * solution, int levelEvaluation){
    
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int operationInPosition = 0;
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

    int orginalLevelEvaluation = levelEvaluation;
    orginalLevelEvaluation *= 1;
    
    if(levelEvaluation > this->getFinalLevel() / 2)
        levelEvaluation = levelEvaluation / 2;

    
    for (operationInPosition = 0; operationInPosition <= levelEvaluation; operationInPosition++) {

        job = solution->getVariable(operationInPosition);
        machine = solution->getVariable(operationInPosition + this->totalOperations);
        
        numberOp = this->operationInJobIsNumber[job][operationOfJob[job]];
        
        /** With the number of operation and the machine we can continue. **/
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

double ProblemFJSSP::evaluatePartialTest3(Solution * solution, int levelEvaluation){
    
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int operationInPosition = 0;
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
    
    for (operationInPosition = 0; operationInPosition <= levelEvaluation; operationInPosition++) {
        
        job = solution->getVariable(operationInPosition * 2);
        machine = solution->getVariable((operationInPosition * 2) + 1);
        
        numberOp = this->operationInJobIsNumber[job][operationOfJob[job]];
        
        /** With the number of operation and the machine we can continue. **/
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
    
    int job = 0;
    int operation = 0;
    int machine = 0;
    int machAssig = 1;
    int countOperations = 0;
    
    /** permutation of jobs **/
    /*for (job = 0; job < this->totalJobs; job++)
        for (operation = 0; operation < this->jobHasNoperations[job]; operation++)
            solution->setVariable(countOperations++, job);
    */
    /** permutation of operations.
     
    for (operation = 0; operation < this->totalOperations; operation++)
        solution->setVariable(operation, operation);
     **/
    /*
    for (operation = 0; operation < this->totalOperations; operation++){
        
        solution->setVariable(operation + this->totalOperations, machine++);
        if(machine == this->totalMachines)
            machine = 0;
    }
     */
    
    for (job = 0; job < this->totalJobs; job++)
        for (operation = 0; operation < this->jobHasNoperations[job]; operation++){
            solution->setVariable(countOperations, job);
            countOperations += 2;
        }
    
    for (operation = 0; operation < this->totalOperations; operation++){
        
        solution->setVariable(machAssig, machine++);
        if(machine == this->totalMachines)
            machine = 0;
        machAssig += 2;
    }
    
}

/** For all the variables the lower bound is 0. **/
int ProblemFJSSP::getLowerBound(int indexVar){
    return 0;
}

/** 
    For the variables which are part of the permutation the upper bound is the number of Jobs.
    
    For the variables which are part of the machine allocations the upper bound is the number of Machines.
 **/
int ProblemFJSSP::getUpperBound(int indexVar){
    if (indexVar == 0 || indexVar % 2 == 0)
        return this->totalJobs - 1;
    else
        return this->totalMachines - 1;
}

ProblemType ProblemFJSSP::getType(){
    return ProblemType::permutation_with_repetition_and_combination;
}

int ProblemFJSSP::getStartingLevel(){
    return 0;
}

int ProblemFJSSP::getFinalLevel(){
    return this->totalOperations - 1;
}

int ProblemFJSSP::getTotalElements(){
    return this->totalJobs;
}

int * ProblemFJSSP::getElemensToRepeat(){
    return this->jobHasNoperations;
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
    this->jobHasNoperations = new int[this->totalJobs];
    
    std::getline(infile, line);
    std::getline(infile, line);
    elemens = split(line, ' ');
    int job = 0;
    this->totalOperations = 0;
    for(job = 0; job < this->totalJobs; job++){
        this->jobHasNoperations[job] = std::stoi(elemens.at(job));
        this->totalOperations += this->jobHasNoperations[job];
    }
    this->operationIsFromJob = new int[this->totalOperations];
    
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
    
    this->totalVariables = this->totalOperations * 2;
    this->totalObjectives = 2;
    
    int operationCounter = 0;
    this->operationInJobIsNumber = new int * [this->totalJobs];
    for (job = 0; job < this->totalJobs; job++) {
        this->operationInJobIsNumber[job] = new int[this->jobHasNoperations[job]];
        for(operation = 0; operation < this->jobHasNoperations[job]; operation++){
            this->operationInJobIsNumber[job][operation] = operationCounter;
            this->operationIsFromJob[operationCounter++] = job;
        }
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
        printf("%2d ", this->jobHasNoperations[job]);
    printf("\n");
    
    printf("Processing times: \n");
    printf("\t\t  ");
    for (machine = 0; machine < this->totalMachines; machine++)
        printf("M%-2d ", machine);
    printf("\n");
    for (operation = 0; operation < this->totalOperations; operation++) {
        
        printf("[J%-2d] %2d:", this->operationIsFromJob[operation], operation);
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
        numberOp = this->operationInJobIsNumber[job][operationOfJob[job]];
        
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

void ProblemFJSSP::printSolution(Solution * solution){
    printPartialSolution(solution, this->totalOperations - 1);
}

void ProblemFJSSP::printPartialSolution(Solution * solution, int level){
    
    int indexVar = 0;
    int withVariables = 1;
    int counter = 0;
    
    for (indexVar = 0; indexVar < this->getNumberOfObjectives(); indexVar++)
        printf("%7.0f ", solution->getObjective(indexVar));
    
    if (withVariables == 1) {
        
        printf(" | ");
        
        for (indexVar = 0; indexVar <= level; indexVar++){
            printf("%3d ", solution->getVariable(counter));
            counter += 2;
        }
        for (indexVar = level + 1; indexVar < this->totalOperations; indexVar ++)
                printf("  - ");
        
        printf("|\n\t\t\t\t | ");
        counter = 0;
        for (indexVar = 0; indexVar <= level; indexVar++){
            printf("%3d ", solution->getVariable(counter + 1));
            counter += 2;
        }
        for (indexVar = level + 1; indexVar < this->totalOperations; indexVar ++)
            printf("  - ");
        
        /*
        printf(" | ");

        for (indexVar = this->totalOperations; indexVar <= level + this->totalOperations; indexVar++)
            printf("%3d ", solution->getVariable(indexVar));
        
        for (indexVar = level + this->totalOperations + 1; indexVar < this->getNumberOfVariables(); indexVar++)
            printf("  - ");
*/
        
        printf("|");
    }

    
    
}
