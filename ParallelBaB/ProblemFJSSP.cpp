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
        delete [] jobMachineToMap[job];
    }
    
    for (job = 0; job < this->totalJobs * this->totalMachines; job++)
        delete [] mapToJobMachine[job];
    
    delete [] jobMachineToMap;
    delete [] mapToJobMachine;
    delete [] processingTime;
    delete [] jobHasNoperations;
    delete [] releaseTime;
    delete [] operationInJobIsNumber;
    delete [] operationIsFromJob;
    delete [] assignationMinPij;
    delete [] minWorkload;
    delete [] assignationBestWorkload;
    delete [] bestWorkloads;
    
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
    evaluatePartialTest4(solution, levelEvaluation);
    return 0.0;
}

/**
 This representation is a contigous allocation of job and machine, :
  [J, M, J, M, J, M, J, M]
 **/
double ProblemFJSSP::evaluatePartialTest3(Solution * solution, int levelEvaluation){
    
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int operationInPosition = 0;
    int operation = 0;
    int job = 0;
    int machine = 0;
    int numberOp = 0;
    
    int minPij = this->sumOfMinPij;
    int bestWL [this->totalMachines];

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
        bestWL[machine] = this->bestWorkloads[machine];
    }
    
    for (operationInPosition = 0; operationInPosition <= levelEvaluation; operationInPosition++) {
        
        job = solution->getVariable(operationInPosition * 2);
        machine = solution->getVariable((operationInPosition * 2) + 1);
        
        numberOp = this->operationInJobIsNumber[job][operationOfJob[job]];
        
        /** The minimun total workload is reduced. **/
        minPij -= this->processingTime[numberOp][this->assignationMinPij[machine]];
        
        /** With the number of operation and the machine we can continue. **/
        workload[machine] += this->processingTime[numberOp][machine];
        totalWorkload += this->processingTime[numberOp][machine];
        
        bestWL[this->assignationBestWorkload[numberOp]] -= this->processingTime[numberOp][this->assignationBestWorkload[numberOp]];
    
        if (operationOfJob[job] == 0) { /** If it is the first operation of the job.**/
            if(timeInMachine[machine] >= this->releaseTime[job]){
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            }else{ /** If the job has to wait for the release time.**/
                startingTime[numberOp] = this->releaseTime[job];
                timeInMachine[machine] = this->releaseTime[job] + this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            }
            
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

    for (machine = 0; machine < this->totalMachines; machine++)
        if(workload[machine] + bestWL[machine] > maxWorkload)
            maxWorkload = workload[machine] + bestWL[machine];
    
    solution->setObjective(0, makespan);
    solution->setObjective(1, maxWorkload);
    //solution->setObjective(2, totalWorkload + minPij);
    
    if(maxWorkload < this->bestWorkloadFound){
        this->bestWorkloadFound = maxWorkload;

        for (machine = 0; machine < this->totalMachines; machine++)
            this->bestWorkloads[machine] = workload[machine];
        
        for (operation = 0; operation < this->totalOperations; operation++)
            this->assignationBestWorkload[operation] = solution->getVariable((operation * 2) + 1);
    }
    
    return 0.0;
}


/**
 This representation use a mapping:
 [ 0, 0, 0, 6, 6, 6, 3, 4, 5]
 map job machine
 0    0   0
 1    0   1
 2    0   2
 3    1   0
 4    1   1
 5    1   2
 6    2   0
 7    2   1
 8    2   2
 **/
double ProblemFJSSP::evaluatePartialTest4(Solution * solution, int levelEvaluation){
    
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int operationInPosition = 0;
    int operation = 0;
    int job = 0;
    int machine = 0;
    int numberOp = 0;
    
    int minPij = this->sumOfMinPij;
    int bestWL [this->totalMachines];
    
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
        bestWL[machine] = this->bestWorkloads[machine];
    }
    int map = 0;
    for (operationInPosition = 0; operationInPosition <= levelEvaluation; operationInPosition++) {
        map = solution->getVariable(operationInPosition);
        job = this->mapToJobMachine[map][0];
        machine = this->mapToJobMachine[map][1];
        
        numberOp = this->operationInJobIsNumber[job][operationOfJob[job]];
        
        /** The minimun total workload is reduced. **/
        minPij -= this->processingTime[numberOp][this->assignationMinPij[machine]];
        
        /** With the number of operation and the machine we can continue. **/
        workload[machine] += this->processingTime[numberOp][machine];
        totalWorkload += this->processingTime[numberOp][machine];
        
        bestWL[this->assignationBestWorkload[numberOp]] -= this->processingTime[numberOp][this->assignationBestWorkload[numberOp]];
        
        if (operationOfJob[job] == 0) { /** If it is the first operation of the job.**/
            if(timeInMachine[machine] >= this->releaseTime[job]){
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            }else{ /** If the job has to wait for the release time.**/
                startingTime[numberOp] = this->releaseTime[job];
                timeInMachine[machine] = this->releaseTime[job] + this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            }
            
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
    
    for (machine = 0; machine < this->totalMachines; machine++)
        if(workload[machine] + bestWL[machine] > maxWorkload)
            maxWorkload = workload[machine] + bestWL[machine];
    
    solution->setObjective(0, makespan);
    solution->setObjective(1, maxWorkload);
    //solution->setObjective(2, totalWorkload + minPij);
    
    if(maxWorkload < this->bestWorkloadFound){
        this->bestWorkloadFound = maxWorkload;
        
        for (machine = 0; machine < this->totalMachines; machine++)
            this->bestWorkloads[machine] = workload[machine];
        /*
        for (operation = 0; operation < this->totalOperations; operation++)
            this->assignationBestWorkload[operation] = solution->getVariable(operation);
         */
    }
    
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
    int countOperations = 0;
    int map = 0;
    
    for (job = 0; job < this->totalJobs; job++)
        for (operation = 0; operation < this->jobHasNoperations[job]; operation++)
            solution->setVariable(countOperations++, map++);
    
    this->evaluate(solution);
}

Solution * ProblemFJSSP::getSolutionWithLowerBoundInObj(int nObj){
    Solution * solution = new Solution(this->getNumberOfObjectives(), this->getNumberOfVariables());
    
    if(nObj == 0){
        this->createDefaultSolution(solution);
    }else if(nObj == 1){
        solution = this->goodSolutionWithMaxWorkload;
    }
    else if(nObj == 2){
        int operation = 0;
        for (operation = 0; operation < this->totalOperations; operation++)
            solution->setVariable(operation, this->jobMachineToMap[this->operationIsFromJob[operation]][this->assignationMinPij[operation]]);
    }
    this->evaluate(solution);
    return solution;
}

void ProblemFJSSP::buildSolutionWithGoodMaxWorkload(Solution * solution){

    /**
     *
     * Creates a good solution with max workload based on the best solution for total workload.
     *
     **/
    this->buildSolutionWithGoodMaxWorkloadv2(solution);

}

void ProblemFJSSP::buildSolutionWithGoodMaxWorkloadv2(Solution *solution){
   
    int nJob = 0;
    int nOperation = 0;
    int nMachine = 0;
    int procTiOp = 0;
    int maxWorkloadIsReduced = 1;
    
    int maxWorkload = 0;
    int minWorkload = INT_MAX;
    int maxWorkloadObj = 0;
    int totalWorkload = 0;
    
    int operationOfJob [this->totalJobs];
    int workload [this->totalMachines];
    int maxWorkloadedMachine = 0;
    
    for (nMachine = 0; nMachine < this->totalMachines; nMachine++)
        workload[nMachine] = 0;
    
    /** Assign the operations to machines which generates the min TotalWorkload and computes the machines workloads. **/
    int counterOperations = 0;
    for (nJob = 0; nJob < this->totalJobs; nJob++){
        operationOfJob[nJob] = 0;
        for (nOperation = 0; nOperation < this->jobHasNoperations[nJob]; nOperation++){
            
            nMachine = this->assignationMinPij[counterOperations];
            procTiOp = this->processingTime[counterOperations][nMachine];
            this->assignationBestWorkload[counterOperations] = nMachine;
            
            workload[nMachine] += procTiOp;
            totalWorkload += procTiOp;
            
            solution->setVariable(counterOperations, this->jobMachineToMap[nJob][nMachine]);
            counterOperations++;
            
            if(workload[nMachine] > maxWorkload) {
                maxWorkload = workload[nMachine];
                maxWorkloadedMachine = nMachine;
                maxWorkloadObj = workload[nMachine];
            }
        }
    }
    
    int bestOperation = 0;
    int bestMachine = 0;
    
    /** Searches for the min operation's processing with the less increment. **/
    while (maxWorkloadIsReduced == 1) {
        
        bestOperation = 0;
        bestMachine = 0;
        minWorkload = INT_MAX;
        
        for (nOperation = 0; nOperation < this->totalOperations; nOperation++)
            if(this->mapToJobMachine[solution->getVariable(nOperation)][1] == maxWorkloadedMachine)
                for (nMachine = 0; nMachine < this->totalMachines; nMachine++)
                    if(nMachine != maxWorkloadedMachine
                       && (workload[nMachine] + this->processingTime[nOperation][nMachine]) < minWorkload) {
                        bestOperation = nOperation;
                        bestMachine = nMachine;
                        minWorkload = workload[nMachine] + this->processingTime[nOperation][nMachine];
                    }
        
        /** Applies the change. **/
        totalWorkload -= this->processingTime[bestOperation][maxWorkloadedMachine];
        workload[maxWorkloadedMachine] -= this->processingTime[bestOperation][maxWorkloadedMachine];
      
        totalWorkload += this->processingTime[bestOperation][bestMachine];
        workload[bestMachine] += this->processingTime[bestOperation][bestMachine];
        
        solution->setVariable(bestOperation, this->jobMachineToMap[this->operationIsFromJob[bestOperation]][bestMachine]);

//        solution->setVariable((bestOperation * 2) + 1, bestMachine);
        
        this->assignationBestWorkload[bestOperation] = bestMachine;
        
        /** Recalculates the maxWorkload and minWorkload for the next iteration. **/
        maxWorkload = 0;
        int lastMaxWorkloadedMachine = maxWorkloadedMachine;
        int lastBestWorkloadMachine = bestMachine;
        for (nMachine = 0; nMachine < this->totalMachines; nMachine++)
            if(workload[nMachine] > maxWorkload) {
                maxWorkload = workload[nMachine];
                maxWorkloadedMachine = nMachine;
            }
        
        solution->setObjective(1, workload[maxWorkloadedMachine]);

        if(maxWorkload < maxWorkloadObj)
            maxWorkloadObj = maxWorkload;
        else{
            /** Removes the change. **/
            workload[lastMaxWorkloadedMachine] += this->processingTime[bestOperation][lastMaxWorkloadedMachine];
            workload[lastBestWorkloadMachine] -= this->processingTime[bestOperation][lastBestWorkloadMachine];
            
            totalWorkload += this->processingTime[bestOperation][lastMaxWorkloadedMachine];
            totalWorkload -= this->processingTime[bestOperation][lastBestWorkloadMachine];
            
            
            solution->setVariable(bestOperation, this->jobMachineToMap[this->operationIsFromJob[bestOperation]][lastMaxWorkloadedMachine]);
            
//            solution->setVariable((bestOperation * 2) + 1, lastMaxWorkloadedMachine);
            solution->setObjective(1, workload[lastMaxWorkloadedMachine]);
            
            this->assignationBestWorkload[bestOperation] = lastMaxWorkloadedMachine;

            maxWorkloadIsReduced = 0;
        }
    }
    
    this->bestWorkloadFound = maxWorkloadObj;
    for (nMachine = 0; nMachine < this->totalMachines; nMachine++)
        this->bestWorkloads[nMachine] = workload[nMachine];
}

/** For all the variables the lower bound is 0. **/
int ProblemFJSSP::getLowerBound(int indexVar){
    return 0;
}

/** 
 1)
    For the variables which are part of the permutation the upper bound is the number of Jobs.
    For the variables which are part of the machine allocations the upper bound is the number of Machines.
 
 2) The Range of variables is the number of maps.
 **/
int ProblemFJSSP::getUpperBound(int indexVar){
    /*
    if (indexVar == 0 || indexVar % 2 == 0)
        return this->totalJobs - 1;
    else
        return this->totalMachines - 1;
    */
    
    return (this->totalJobs * this->totalMachines) - 1;
    
}

int ProblemFJSSP::getLowerBoundInObj(int nObj){
    if(nObj == 1)
        return this->sumOfMinPij;
    return INT_MAX;
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

int ProblemFJSSP::getMapping(int map, int position){
    return this->mapToJobMachine[map][position];
}

int ProblemFJSSP::getMappingOf(int job, int machine){
    return this->jobMachineToMap[job][machine];
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
    this->releaseTime = new int[this->totalJobs];
    
    
    std::getline(infile, line);
    std::getline(infile, line);
    elemens = split(line, ' ');
    int job = 0;
    this->totalOperations = 0;
    for(job = 0; job < this->totalJobs; job++){
        this->jobHasNoperations[job] = std::stoi(elemens.at(job));
        this->totalOperations += this->jobHasNoperations[job];
    }
    
    std::getline(infile, line);
    std::getline(infile, line);
    elemens = split(line, ' ');
    for (job = 0; job < this->totalJobs; job++)
        this->releaseTime[job] = std::stoi(elemens.at(job));
    
    
    this->operationIsFromJob = new int[this->totalOperations];
    this->sumOfMinPij = 0;
    this->bestWorkloadFound = INT_MAX;
    std::getline(infile, line);
    this->processingTime = new int * [this->totalOperations];
    this->assignationMinPij = new int [this->totalOperations];
    this->minWorkload = new int[this->totalMachines];
    this->assignationBestWorkload = new int [this->totalOperations];
    this->bestWorkloads = new int[this->totalMachines];
    
    this->jobMachineToMap = new int * [this->totalJobs];
    this->mapToJobMachine = new int * [this->totalJobs * this->totalMachines];
    
    int operation = 0;
    int machine = 0;
    int minPij = INT_MAX;
    int minMachine = 0;
    int map = 0;
    
    for (job = 0; job < this->totalJobs; job++) {
        this->jobMachineToMap[job] = new int[this->totalMachines];
        for (machine = 0; machine < this->totalMachines; machine++) {
            this->mapToJobMachine[map] = new int[2];
            this->mapToJobMachine[map][0] = job;
            this->mapToJobMachine[map][1] = machine;
            this->jobMachineToMap[job][machine] = map;
            map++;
        }
    }
    
    for (machine = 0; machine < this->totalMachines; machine++)
        minWorkload[machine] = 0;
    
    for (operation = 0; operation < this->totalOperations; operation++) {
        this->processingTime[operation] = new int[this->totalMachines];
        std::getline(infile, line);
        elemens = split(line, ' ');
        minPij = INT_MAX;
        minMachine = 0;
        for (machine = 0; machine < this->totalMachines; machine++){
            this->processingTime[operation][machine] = std::stoi(elemens.at(machine));
            if (processingTime[operation][machine] < minPij){
                minPij = processingTime[operation][machine];
                minMachine = machine;
            }
        }
        this->sumOfMinPij += minPij;
        this->minWorkload[minMachine] += processingTime[operation][minMachine];
        this->assignationMinPij[operation] = minMachine;
    }
    
    this->totalVariables = this->totalOperations;
    
    int operationCounter = 0;
    this->operationInJobIsNumber = new int * [this->totalJobs];
    for (job = 0; job < this->totalJobs; job++) {
        this->operationInJobIsNumber[job] = new int[this->jobHasNoperations[job]];
        for(operation = 0; operation < this->jobHasNoperations[job]; operation++){
            this->operationInJobIsNumber[job][operation] = operationCounter;
            this->operationIsFromJob[operationCounter++] = job;
        }
    }
    goodSolutionWithMaxWorkload = new Solution(this->getNumberOfObjectives(), this->getNumberOfVariables());
    buildSolutionWithGoodMaxWorkload(goodSolutionWithMaxWorkload);

}

void ProblemFJSSP::printInstance(){}

void ProblemFJSSP::printProblemInfo(){
    printf("Total jobs: %d\n", this->totalJobs);
    printf("Total machines: %d\n", this->totalMachines);
    printf("Total operations: %d\n", this->totalOperations);

    printf("Operations in each job:\n");
    
    int job = 0, machine = 0, operation = 0;
    for (job = 0; job < this->totalJobs; job++)
        printf("%2d ", this->jobHasNoperations[job]);
    printf("\n");
    
    printf("Release time for each job:\n");
    for (job = 0; job < this->totalJobs; job++)
        printf("%2d ", this->releaseTime[job]);
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

void ProblemFJSSP::printSolutionInfo(Solution *solution){
    printSchedule(solution);
}

void ProblemFJSSP::printSchedule(Solution * solution){
    
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
    
    char gantt[this->totalMachines][255];
    int time = 0;
    
    for (operation = 0; operation < this->totalOperations; operation++) {
        startingTime[operation] = 0;
        endingTime[operation] = 0;
    }
    
    for (job = 0; job < this->totalJobs; job++)
        operationOfJob[job] = 0;
    
    for (machine = 0; machine < this->totalMachines; machine++){
        timeInMachine[machine] = 0;
        workload[machine] = 0;
        
        /**creates an empty gantt**/
        for (time = 0; time < 255; time++) {
            gantt[machine][time] = ' ';
        }
    }
    
    for (operationInPosition = 0; operationInPosition < this->totalOperations; operationInPosition++) {
        
        job = solution->getVariable(operationInPosition * 2);
        machine = solution->getVariable((operationInPosition * 2) + 1);
        
        numberOp = this->operationInJobIsNumber[job][operationOfJob[job]];
        
        /** With the number of operation and the machine we can continue. **/
        workload[machine] += this->processingTime[numberOp][machine];
        totalWorkload += this->processingTime[numberOp][machine];
        
        if (operationOfJob[job] == 0) { /** If it is the first operation of the job.**/
            if(timeInMachine[machine] >= this->releaseTime[job]){
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            }else{ /** If the job has to wait for the release time.**/
                startingTime[numberOp] = this->releaseTime[job];
                timeInMachine[machine] = this->releaseTime[job] + this->processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            }
            
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
        
        for (time = startingTime[numberOp]; time < endingTime[numberOp]; time++)
            gantt[machine][time] = 'a' + job;
        
        operationOfJob[job]++;
        
        if (timeInMachine[machine] > makespan)
            makespan = timeInMachine[machine];
        
        if(workload[machine] > maxWorkload)
            maxWorkload = workload[machine];
    }
    
    solution->setObjective(0, makespan);
    solution->setObjective(1, maxWorkload);
    
    printf("Op :  M  ti -  tf\n");
    for (operation = 0; operation < this->totalOperations; operation++)
        printf("%3d: %2d %3d - %3d \n", operation, solution->getVariable((operation * 2) + 1), startingTime[operation], endingTime[operation]);
    
    printf("makespan: %d\nmaxWorkLoad: %d\ntotalWorkload: %d \n", makespan, maxWorkload, totalWorkload);

    for (machine = 0; machine < this->totalMachines; machine++) {
        printf("M%d  |", machine);
        for (time = 0; time < makespan; time++)
            printf("%3c", gantt[machine][time]);
        printf("| %3d\n", workload[machine]);
    }
     printf("----");
    for (time = 0; time < makespan; time++)
        printf("---");
    printf("--\n");

    printf("Time:");
    for (time = 0; time < makespan; time++)
        printf("%3d", (time));
    printf("\n");
}

void ProblemFJSSP::printSolution(Solution * solution){
    printPartialSolution(solution, this->totalOperations - 1);
}

void ProblemFJSSP::printPartialSolution(Solution * solution, int level){
    
    int indexVar = 0;
    int withVariables = 1;
    int job = 0;
    int machine = 0;
    
    for (indexVar = 0; indexVar < this->getNumberOfObjectives(); indexVar++)
        printf("%7.0f ", solution->getObjective(indexVar));
    
    if (withVariables == 1) {
        
        printf(" | ");
        
        for (indexVar = 0; indexVar <= level; indexVar++){
            job = this->getMapping(solution->getVariable(indexVar), 0);
            printf("%3d ", job);
        }
        for (indexVar = level + 1; indexVar < this->totalOperations; indexVar ++)
                printf("  - ");
        
        printf("|\n\t\t\t\t | ");

        for (indexVar = 0; indexVar <= level; indexVar++){
            machine = this->getMapping(solution->getVariable(indexVar), 1);
            printf("%3d ", machine);
        }
        for (indexVar = level + 1; indexVar < this->totalOperations; indexVar ++)
            printf("  - ");
        
        printf("|");
    }
}
