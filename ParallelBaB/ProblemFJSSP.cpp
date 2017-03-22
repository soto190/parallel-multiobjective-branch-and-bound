//
//  ProblemFJSSP.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 14/11/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "ProblemFJSSP.hpp"
/*
ProblemFJSSP::ProblemFJSSP():Problem(){
    
    jobMachineToMap = nullptr;
    mapToJobMachine = nullptr;
    numberOfOperationsInJob = nullptr;
    releaseTime = nullptr;
    operationIsFromJob = nullptr;
    assignationMinPij = nullptr;
    assignationBestWorkload = nullptr;
    bestWorkloads = nullptr;
    minWorkload = nullptr;
    jobOperationHasNumber = nullptr;
    processingTime = nullptr;

}
**/
ProblemFJSSP::ProblemFJSSP(int totalObjectives, int totalVariables):Problem(totalObjectives, totalVariables){
    
    totalJobs = 0;
    totalOperations = 0;
    totalMachines = 0;
    sumOfMinPij = 0;
    
    jobMachineToMap = nullptr;
    mapToJobMachine = nullptr;
    numberOfOperationsInJob = nullptr;
    releaseTime = nullptr;
    operationIsFromJob = nullptr;
    assignationMinPij = nullptr;
    assignationBestWorkload = nullptr;
    bestWorkloads = nullptr;
    minWorkload = nullptr;
    jobOperationHasNumber = nullptr;
    processingTime = nullptr;
    
    lowerBound = nullptr;
    upperBound = nullptr;
}

ProblemFJSSP::ProblemFJSSP(const ProblemFJSSP& toCopy): Problem(toCopy),
    totalJobs(toCopy.getNumberOfJobs()),
    totalOperations(toCopy.getNumberOfOperations()),
    totalMachines(toCopy.getNumberOfMachines()),
    goodSolutionWithMaxWorkload(toCopy.goodSolutionWithMaxWorkload),
    sumOfMinPij(toCopy.getSumOfMinPij()),
    bestWorkloadFound(toCopy.getBestWorkloadFound()){

        lowerBound = new int[totalVariables];
        upperBound = new int[totalVariables];

    std::strcpy(name, toCopy.name);
    
        
    jobMachineToMap = new int * [totalJobs];
    mapToJobMachine = new int * [totalJobs * totalMachines];
    processingTime = new int * [totalOperations];
    jobOperationHasNumber = new int * [totalJobs];
    
    releaseTime = new int[totalJobs];
    numberOfOperationsInJob = new int[totalJobs];
    operationIsFromJob = new int[totalOperations];
    assignationBestWorkload = new int[totalOperations];
    assignationMinPij = new int[totalOperations];
    bestWorkloads = new int[totalMachines];
    minWorkload = new int[totalMachines];
    
    int job = 0, map = 0, machine = 0, operation = 0, operationCounter = 0;
    
    for (job = 0; job < totalJobs; job++) {
        numberOfOperationsInJob[job] = toCopy.getNumberOfOperationsInJob(job);
        releaseTime[job] = toCopy.getReleaseTimeOfJob(job);
        
        jobMachineToMap[job] = new int[toCopy.getNumberOfMachines()];
        
        for (machine = 0; machine < totalMachines; machine++) {
            mapToJobMachine[map] = new int[2];
            mapToJobMachine[map][0] = toCopy.getMapOfJobMachine(map, 0);
            mapToJobMachine[map][1] = toCopy.getMapOfJobMachine(map, 1);
            jobMachineToMap[job][machine] = toCopy.getJobMachineToMap(job, machine);
            map++;
        }
    }
    
    for (machine = 0; machine < totalMachines; machine++) {
        minWorkload[machine] = toCopy.getMinWorkload(machine);
        bestWorkloads[machine] = toCopy.getBestWorkload(machine);
    }
    
    for (operation = 0; operation < totalOperations; operation++){
        lowerBound[operation] = toCopy.getLowerBound(operation);
        
        processingTime[operation] = new int[toCopy.getNumberOfMachines()];
        for (machine = 0; machine < totalMachines; machine++)
            processingTime[operation][machine] = toCopy.getProccessingTime(operation, machine);
        
        assignationMinPij[operation] = toCopy.getAssignationMinPij(operation);
        assignationBestWorkload[operation] = toCopy.getAssignatioBestWorkload(operation);
    }
    
    for (job = 0; job < totalJobs; job++) {
        jobOperationHasNumber[job] = new int[toCopy.getNumberOfOperationsInJob(job)];
        
        for(operation = 0; operation < numberOfOperationsInJob[job]; operation++){
            jobOperationHasNumber[job][operation] = toCopy.getOperationInJobIsNumber(job, operation);
            operationIsFromJob[operationCounter] = toCopy.getOperationIsFromJob(operationCounter);
            operationCounter++;
        }
    }
}

ProblemFJSSP& ProblemFJSSP::operator=(const ProblemFJSSP &toCopy){
    
    if (this == &toCopy) return *this;
    
    
    totalJobs = toCopy.getNumberOfJobs();
    totalOperations = toCopy.getNumberOfOperations();
    totalMachines = toCopy.getNumberOfMachines();
    totalVariables = toCopy.getNumberOfVariables();
    totalObjectives = toCopy.getNumberOfObjectives();
    totalConstraints = toCopy.getNumberOfConstraints();
    
    if (processingTime != nullptr) {
        int job = 0, operation = 0;
        for(job = 0; job < totalJobs; job++){
            delete [] jobMachineToMap[job];
            delete [] jobOperationHasNumber[job];
        }
        
        for (job = 0; job < totalJobs * totalMachines; job++)
            delete [] mapToJobMachine[job];
        
        for (operation = 0; operation < totalOperations; operation++)
            delete [] processingTime[operation];
        
        delete [] jobMachineToMap;
        delete [] mapToJobMachine;
        delete [] processingTime;
        delete [] jobOperationHasNumber;
        delete [] numberOfOperationsInJob;
        delete [] releaseTime;
        delete [] operationIsFromJob;
        delete [] assignationMinPij;
        delete [] minWorkload;
        delete [] assignationBestWorkload;
        delete [] bestWorkloads;
        
        delete [] upperBound;
        delete [] lowerBound;
        delete [] name;
    }
    
    lowerBound = new int[totalVariables];
    upperBound = new int[totalVariables];
    name = new char[255];
    
    std::strcpy(name, toCopy.name);
    
    goodSolutionWithMaxWorkload = toCopy.goodSolutionWithMaxWorkload;
    
    sumOfMinPij = toCopy.getSumOfMinPij();
    bestWorkloadFound = toCopy.getBestWorkloadFound();
    
    jobMachineToMap = new int * [totalJobs];
    mapToJobMachine = new int * [totalJobs * totalMachines];
    
    numberOfOperationsInJob = new int[totalJobs];
    releaseTime = new int[totalJobs];
    operationIsFromJob = new int[totalOperations];
    assignationMinPij = new int[totalOperations];
    assignationBestWorkload = new int[totalOperations];
    bestWorkloads = new int[totalMachines];
    minWorkload = new int[totalMachines];
    
    int job = 0, map = 0, machine = 0, operation = 0, operationCounter = 0;
    
    for (job = 0; job < totalJobs; job++) {
        numberOfOperationsInJob[job] = toCopy.getNumberOfOperationsInJob(job); //jobHasNoperations[job];
        releaseTime[job] = toCopy.getReleaseTimeOfJob(job);//releaseTime[job];
        
        jobMachineToMap[job] = new int[totalMachines];
        
        for (machine = 0; machine < totalMachines; machine++) {
            mapToJobMachine[map] = new int[2];
            mapToJobMachine[map][0] = toCopy.getMapOfJobMachine(map, 0); //toCopy.mapToJobMachine[map][0];
            mapToJobMachine[map][1] = toCopy.getMapOfJobMachine(map, 1);
            jobMachineToMap[job][machine] = toCopy.getJobMachineToMap(job, machine);// jobMachineToMap[job][machine];
            map++;
        }
    }
    
    for (machine = 0; machine < totalMachines; machine++) {
        minWorkload[machine] = toCopy.getMinWorkload(machine);// minWorkload[machine];
        bestWorkloads[machine] = toCopy.getBestWorkload(machine);// bestWorkloads[machine];
    }
    
    jobOperationHasNumber = new int * [totalJobs];
    processingTime = new int * [totalOperations];
    
    for (operation = 0; operation < totalOperations; operation++){
        
        processingTime[operation] = new int[totalMachines];
        for (machine = 0; machine < totalMachines; machine++)
            processingTime[operation][machine] = toCopy.getProccessingTime(operation, machine);// processingTime[operation][machine];
        
        assignationMinPij[operation] = toCopy.getAssignationMinPij(operation);// assignationMinPij[operation];
        assignationBestWorkload[operation] = toCopy.getAssignatioBestWorkload(operation);//assignationBestWorkload[operation];
    }
    
    for (job = 0; job < totalJobs; job++) {
        jobOperationHasNumber[job] = new int[numberOfOperationsInJob[job]];
        for(operation = 0; operation < numberOfOperationsInJob[job]; operation++){
            jobOperationHasNumber[job][operation] = toCopy.getOperationInJobIsNumber(job, operation);//operationInJobIsNumber[job][operation];
            operationIsFromJob[operationCounter] = toCopy.getOperationIsFromJob(operationCounter);// operationIsFromJob[operationCounter];
            operationCounter++;
        }
    }
    
    return *this;
}

ProblemFJSSP::~ProblemFJSSP(){
    int job = 0, operation = 0;
    for(job = 0; job < totalJobs; job++){
        delete [] jobOperationHasNumber[job];
        delete [] jobMachineToMap[job];
    }
    
    for (job = 0; job < totalJobs * totalMachines; job++)
        delete [] mapToJobMachine[job];

    for (operation = 0; operation < totalOperations; operation++)
        delete [] processingTime[operation];
    
    delete [] jobMachineToMap;
    delete [] mapToJobMachine;
    delete [] processingTime;
    delete [] numberOfOperationsInJob;
    delete [] releaseTime;
    delete [] jobOperationHasNumber;
    delete [] operationIsFromJob;
    delete [] assignationMinPij;
    delete [] minWorkload;
    delete [] assignationBestWorkload;
    delete [] bestWorkloads;
    
    delete [] upperBound;
    delete [] lowerBound;
    //delete [] name;
}

/**
 *
 * Evaluates a given solution.
 *
 */
double ProblemFJSSP::evaluate(Solution & solution){
    evaluatePartial(solution, getFinalLevel());
    return 0.0;
}

double ProblemFJSSP::evaluatePartial(Solution & solution, int levelEvaluation){
    evaluatePartialTest4(solution, levelEvaluation);
    return 0.0;
}

/**
 *
 * TODO: improve the evaluation function.
 *
 * This representation use a mapping:
 * solution [ 0, 0, 0, 6, 7, 6, 3, 4, 5]
 * job      [ 0, 0, 0, 2, 2, 2, 1, 1, 1]
 * machine  [ 0, 0, 0, 0, 1, 1, 0, 1, 2]
 * map | job | machine
 * 0  ->  0     0
 * 1  ->  0     1
 * 2  ->  0     2
 * 3  ->  1     0
 * 4  ->  1     1
 * 5  ->  1     2
 * 6  ->  2     0
 * 7  ->  2     1
 * 8  ->  2     2
 **/
double ProblemFJSSP::evaluatePartialTest4(Solution & solution, int levelEvaluation){
    
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int operationInPosition = 0;
    int operation = 0;
    int job = 0;
    int machine = 0;
    int numberOp = 0;
    
    int minPij = sumOfMinPij;
    int bestWL [totalMachines];
    
    int operationOfJob [totalJobs];
    int startingTime [totalOperations];
    int endingTime [totalOperations];
    int timeInMachine [totalMachines];
    int workload [totalMachines];
    
    for (operation = 0; operation < totalOperations; operation++) {
        startingTime[operation] = 0;
        endingTime[operation] = 0;
    }
    
    for (job = 0; job < totalJobs; job++)
        operationOfJob[job] = 0;
    
    for (machine = 0; machine < totalMachines; machine++){
        timeInMachine[machine] = 0;
        workload[machine] = 0;
        bestWL[machine] = bestWorkloads[machine];
    }
    
    int map = 0;
    for (operationInPosition = 0; operationInPosition <= levelEvaluation; operationInPosition++) {
        map = solution.getVariable(operationInPosition);
        job = getMapOfJobMachine(map, 0);
        machine = getMapOfJobMachine(map, 1);
        
        numberOp = getOperationInJobIsNumber(job, operationOfJob[job]);
        /** The minimun total workload is reduced. **/
        minPij -= getProccessingTime(numberOp, getAssignationMinPij(numberOp));
        
        /** With the number of operation and the machine we can continue. **/
        int proccesingTime = getProccessingTime(numberOp, machine);
        workload[machine] += proccesingTime;
        totalWorkload += proccesingTime;
        
        bestWL[getAssignatioBestWorkload(numberOp)] -= getProccessingTime(numberOp, getAssignatioBestWorkload(numberOp));
        
        if (operationOfJob[job] == 0) { /** If it is the first operation of the job.**/
            if(timeInMachine[machine] >= getReleaseTimeOfJob(job)){
             
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += proccesingTime;
                endingTime[numberOp] = timeInMachine[machine];
            
            }else{ /** If the job has to wait for the release time.**/
                
                startingTime[numberOp] = getReleaseTimeOfJob(job);
                timeInMachine[machine] = getReleaseTimeOfJob(job) + proccesingTime;
                endingTime[numberOp] = timeInMachine[machine];
            
            }
        }else{
            if(endingTime[numberOp - 1] > timeInMachine[machine]){ /** The operation starts inmediatly after their precedent operation.**/
                
                startingTime[numberOp] = endingTime[numberOp - 1];
                timeInMachine[machine] = endingTime[numberOp - 1] + proccesingTime;
                endingTime[numberOp] = timeInMachine[machine];
                
            }else{ /**The operation starts when the machine is avaliable.**/
                
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += proccesingTime;
                endingTime[numberOp] = timeInMachine[machine];
                
            }
        }
        
        operationOfJob[job]++; /** This counts the number of operations from a job allocated. **/
        
        if (timeInMachine[machine] > makespan){
            makespan = timeInMachine[machine];
            solution.setPartialObjective(operationInPosition, 0, makespan);
        }
        if(workload[machine] > maxWorkload){
            maxWorkload = workload[machine];
            solution.setPartialObjective(operationInPosition, 1, maxWorkload);
        }
    }
    
    for (machine = 0; machine < totalMachines; machine++)
        if(workload[machine] + bestWL[machine] > maxWorkload)
            maxWorkload = workload[machine] + bestWL[machine];
    
    solution.setObjective(0, makespan);
    solution.setObjective(1, maxWorkload);
    //solution->setObjective(2, totalWorkload + minPij);
    
    /** Updates the best workloads and the assignation. **/
    /*
    if(maxWorkload < bestWorkloadFound){
        MutexToUpdate.lock();
        bestWorkloadFound = maxWorkload;
        
        for (machine = 0; machine < totalMachines; machine++)
            bestWorkloads[machine] = workload[machine];
        
        for (operation = 0; operation < totalOperations; operation++)
            assignationBestWorkload[operation] = mapToJobMachine[solution.getVariable(operation)][1];
        MutexToUpdate.unlock();
    }
    */
    return 0.0;
}

double ProblemFJSSP::evaluateLastLevel(Solution * solution){ return 0.0;}
double ProblemFJSSP::removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel){ return 0.0;}
double ProblemFJSSP::removeLastLevelEvaluation(Solution * solution, int newLevel){ return 0.0;}

void ProblemFJSSP::createDefaultSolution(Solution & solution){
    
    int job = 0;
    int operation = 0;
    int countOperations = 0;
    int map = 0;
    int machine = 0;
    
    for (job = 0; job < totalJobs; job++)
        for (operation = 0; operation < numberOfOperationsInJob[job]; operation++){
            map = jobMachineToMap[job][machine];
            solution.setVariable(countOperations++, map);
            machine++;
            if (machine == totalMachines)
                machine = 0;
        }
    
    evaluate(solution);
}

void ProblemFJSSP::getSolutionWithLowerBoundInObj(int nObj, Solution& solution){
    
    if(nObj == 0){
        createDefaultSolution(solution);
    }else if(nObj == 1){
        solution = goodSolutionWithMaxWorkload;
    }
    else if(nObj == 2){
        int operation = 0;
        for (operation = 0; operation < totalOperations; operation++)
            solution.setVariable(operation, jobMachineToMap[operationIsFromJob[operation]][assignationMinPij[operation]]);
    }
    evaluate(solution);
}

/**
 *
 * Creates a good solution with max workload based on the best solution for total workload.
 *
 **/
void ProblemFJSSP::buildSolutionWithGoodMaxWorkload(Solution & solution){
    buildSolutionWithGoodMaxWorkloadv2(solution);
}

void ProblemFJSSP::buildSolutionWithGoodMaxWorkloadv2(Solution & solution){
   
    int nJob = 0;
    int nOperation = 0;
    int nMachine = 0;
    int procTiOp = 0;
    int maxWorkloadIsReduced = 1;
    
    int maxWorkload = 0;
    int minWorkload = INT_MAX;
    int maxWorkloadObj = 0;
    int totalWorkload = 0;
    
    int operationOfJob [totalJobs];
    int workload [totalMachines];
    int maxWorkloadedMachine = 0;
    
    for (nMachine = 0; nMachine < totalMachines; nMachine++)
        workload[nMachine] = 0;
    
    /** Assign the operations to machines which generates the min TotalWorkload and computes the machines workloads. **/
    int counterOperations = 0;
    for (nJob = 0; nJob < totalJobs; nJob++){
        operationOfJob[nJob] = 0;
        for (nOperation = 0; nOperation < numberOfOperationsInJob[nJob]; nOperation++){
            
            nMachine = assignationMinPij[counterOperations];
            procTiOp = processingTime[counterOperations][nMachine];
            assignationBestWorkload[counterOperations] = nMachine;
            
            workload[nMachine] += procTiOp;
            totalWorkload += procTiOp;
            
            solution.setVariable(counterOperations, jobMachineToMap[nJob][nMachine]);
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
        
        for (nOperation = 0; nOperation < totalOperations; nOperation++)
            if(mapToJobMachine[solution.getVariable(nOperation)][1] == maxWorkloadedMachine)
                for (nMachine = 0; nMachine < totalMachines; nMachine++)
                    if(nMachine != maxWorkloadedMachine
                       && (workload[nMachine] + processingTime[nOperation][nMachine]) < minWorkload) {
                        bestOperation = nOperation;
                        bestMachine = nMachine;
                        minWorkload = workload[nMachine] + processingTime[nOperation][nMachine];
                    }
        
        /** Applies the change. **/
        totalWorkload -= processingTime[bestOperation][maxWorkloadedMachine];
        workload[maxWorkloadedMachine] -= processingTime[bestOperation][maxWorkloadedMachine];
      
        totalWorkload += processingTime[bestOperation][bestMachine];
        workload[bestMachine] += processingTime[bestOperation][bestMachine];
        
        solution.setVariable(bestOperation, jobMachineToMap[operationIsFromJob[bestOperation]][bestMachine]);
        assignationBestWorkload[bestOperation] = bestMachine;
        
        /** Recalculates the maxWorkload and minWorkload for the next iteration. **/
        maxWorkload = 0;
        int lastMaxWorkloadedMachine = maxWorkloadedMachine;
        int lastBestWorkloadMachine = bestMachine;
        for (nMachine = 0; nMachine < totalMachines; nMachine++)
            if(workload[nMachine] > maxWorkload) {
                maxWorkload = workload[nMachine];
                maxWorkloadedMachine = nMachine;
            }
        
        solution.setObjective(1, workload[maxWorkloadedMachine]);

        if(maxWorkload < maxWorkloadObj)
            maxWorkloadObj = maxWorkload;
        else{
            /** Removes the change. **/
            workload[lastMaxWorkloadedMachine] += processingTime[bestOperation][lastMaxWorkloadedMachine];
            workload[lastBestWorkloadMachine] -= processingTime[bestOperation][lastBestWorkloadMachine];
            
            totalWorkload += processingTime[bestOperation][lastMaxWorkloadedMachine];
            totalWorkload -= processingTime[bestOperation][lastBestWorkloadMachine];
            
            
            solution.setVariable(bestOperation, jobMachineToMap[operationIsFromJob[bestOperation]][lastMaxWorkloadedMachine]);
            solution.setObjective(1, workload[lastMaxWorkloadedMachine]);
            
            assignationBestWorkload[bestOperation] = lastMaxWorkloadedMachine;

            maxWorkloadIsReduced = 0;
        }
    }
    
    bestWorkloadFound = maxWorkloadObj;
    for (nMachine = 0; nMachine < totalMachines; nMachine++)
        bestWorkloads[nMachine] = workload[nMachine];
}

/** For all the variables the lower bound is 0. **/
int ProblemFJSSP::getLowerBound(int indexVar) const{
    return 0;
}

/** 
 *
 * The Range of variables is the number of maps.
 *
 **/
int ProblemFJSSP::getUpperBound(int indexVar) const{
    return (totalJobs * totalMachines) - 1;
}

int ProblemFJSSP::getLowerBoundInObj(int nObj){
    if(nObj == 1)
        return sumOfMinPij;
    return INT_MAX;
}

void ProblemFJSSP::loadInstance(char** filePath){
    
    if(processingTime != nullptr){
        int job = 0, operation = 0;
        for(job = 0; job < totalJobs; job++){
            delete [] jobOperationHasNumber[job];
            delete [] jobMachineToMap[job];
        }
        
        for (job = 0; job < totalJobs * totalMachines; job++)
            delete [] mapToJobMachine[job];
        
        for (operation = 0; operation < totalOperations; operation++)
            delete[] processingTime[operation];
        
        delete [] jobMachineToMap;
        delete [] mapToJobMachine;
        delete [] processingTime;
        delete [] numberOfOperationsInJob;
        delete [] releaseTime;
        delete [] jobOperationHasNumber;
        delete [] operationIsFromJob;
        delete [] assignationMinPij;
        delete [] minWorkload;
        delete [] assignationBestWorkload;
        delete [] bestWorkloads;
    }
    
    std::ifstream infile(filePath[0]);
    std::string line;
    std::vector<std::string> elemens;
    
    std::getline(infile, line);
    std::getline(infile, line);
    elemens = split(line, ' ');
    totalJobs = std::stoi(elemens.at(0));
    totalMachines = std::stoi(elemens.at(1));
    numberOfOperationsInJob = new int[totalJobs];
    releaseTime = new int[totalJobs];
    
    
    std::getline(infile, line);
    std::getline(infile, line);
    elemens = split(line, ' ');
    int job = 0;
    totalOperations = 0;
    for(job = 0; job < totalJobs; job++){
        numberOfOperationsInJob[job] = std::stoi(elemens.at(job));
        totalOperations += numberOfOperationsInJob[job];
    }
    
    std::getline(infile, line);
    std::getline(infile, line);
    elemens = split(line, ' ');
    for (job = 0; job < totalJobs; job++)
        releaseTime[job] = std::stoi(elemens.at(job));
    
    
    operationIsFromJob = new int[totalOperations];
    sumOfMinPij = 0;
    bestWorkloadFound = INT_MAX;
    std::getline(infile, line);
    
    assignationMinPij = new int [totalOperations];
    minWorkload = new int[totalMachines];
    assignationBestWorkload = new int [totalOperations];
    bestWorkloads = new int[totalMachines];
    jobOperationHasNumber = new int * [totalJobs];
    processingTime = new int * [totalOperations];
    jobMachineToMap = new int * [totalJobs];
    mapToJobMachine = new int * [totalJobs * totalMachines];
    
    int operation = 0;
    int machine = 0;
    int minPij = INT_MAX;
    int minMachine = 0;
    int map = 0;
    
    for (job = 0; job < totalJobs; job++) {
        jobMachineToMap[job] = new int[totalMachines];
        for (machine = 0; machine < totalMachines; machine++) {
            mapToJobMachine[map] = new int[2];
            mapToJobMachine[map][0] = job;
            mapToJobMachine[map][1] = machine;
            jobMachineToMap[job][machine] = map;
            map++;
        }
    }
    
    for (machine = 0; machine < totalMachines; machine++)
        minWorkload[machine] = 0;
    
    for (operation = 0; operation < totalOperations; operation++) {
        processingTime[operation] = new int[totalMachines];
        std::getline(infile, line);
        elemens = split(line, ' ');
        minPij = INT_MAX;
        minMachine = 0;
        for (machine = 0; machine < totalMachines; machine++){
            processingTime[operation][machine] = std::stoi(elemens.at(machine));
            if (processingTime[operation][machine] < minPij){
                minPij = processingTime[operation][machine];
                minMachine = machine;
            }
        }
        sumOfMinPij += minPij;
        minWorkload[minMachine] += processingTime[operation][minMachine];
        assignationMinPij[operation] = minMachine;
    }
    
    totalVariables = totalOperations;
    
    int operationCounter = 0;
    for (job = 0; job < totalJobs; job++) {
        jobOperationHasNumber[job] = new int[numberOfOperationsInJob[job]];
        for(operation = 0; operation < numberOfOperationsInJob[job]; operation++){
            jobOperationHasNumber[job][operation] = operationCounter;
            operationIsFromJob[operationCounter++] = job;
        }
    }
    goodSolutionWithMaxWorkload (getNumberOfObjectives(), getNumberOfVariables());
    buildSolutionWithGoodMaxWorkload(goodSolutionWithMaxWorkload);
    
    infile.close();

}

ProblemType ProblemFJSSP::getType() const { return ProblemType::permutation_with_repetition_and_combination; }
int ProblemFJSSP::getStartingLevel(){ return 0;}
int ProblemFJSSP::getFinalLevel(){return totalOperations - 1;}
int ProblemFJSSP::getTotalElements(){return totalJobs;}
int * ProblemFJSSP::getElemensToRepeat(){return numberOfOperationsInJob;}
int ProblemFJSSP::getMapping(int map, int position){return mapToJobMachine[map][position];}
int ProblemFJSSP::getMappingOf(int job, int machine){return jobMachineToMap[job][machine];}
int ProblemFJSSP::getTimesValueIsRepeated(int value){return totalMachines;}
int ProblemFJSSP::getNumberOfJobs() const{ return totalJobs;}
int ProblemFJSSP::getNumberOfOperations() const{ return totalOperations;}
int ProblemFJSSP::getNumberOfMachines() const{ return totalMachines;}
int ProblemFJSSP::getSumOfMinPij() const{ return sumOfMinPij;}
int ProblemFJSSP::getBestWorkloadFound() const{ return bestWorkloadFound;}
int ProblemFJSSP::getAssignationMinPij(int n_operation) const{ return assignationMinPij[n_operation];}
int ProblemFJSSP::getAssignatioBestWorkload(int n_operation) const{ return assignationBestWorkload[n_operation];}
int ProblemFJSSP::getBestWorkload(int n_machine) const{ return bestWorkloads[n_machine];}
int ProblemFJSSP::getMinWorkload(int n_machine) const{ return minWorkload[n_machine];}
int ProblemFJSSP::getMapOfJobMachine(int map, int machine_or_job) const{ return mapToJobMachine[map][machine_or_job];}
int ProblemFJSSP::getJobMachineToMap(int job, int machine) const{ return jobMachineToMap[job][machine];}
int ProblemFJSSP::getOperationInJobIsNumber(int job, int operation) const{ return jobOperationHasNumber[job][operation];}
int ProblemFJSSP::getOperationIsFromJob(int n_operation) const{ return operationIsFromJob[n_operation];}
int ProblemFJSSP::getProccessingTime(int operation, int machine) const{ return processingTime[operation][machine];}
int ProblemFJSSP::getNumberOfOperationsInJob(int job) const{return numberOfOperationsInJob[job];}
int ProblemFJSSP::getReleaseTimeOfJob(int job) const{ return releaseTime[job];}

void ProblemFJSSP::printInstance(){}

void ProblemFJSSP::printProblemInfo(){
    printf("Total jobs: %d\n", totalJobs);
    printf("Total machines: %d\n", totalMachines);
    printf("Total operations: %d\n", totalOperations);

    printf("Operations in each job:\n");
    
    int job = 0, machine = 0, operation = 0;
    for (job = 0; job < totalJobs; job++)
        printf("%2d ", numberOfOperationsInJob[job]);
    printf("\n");
    
    printf("Release time for each job:\n");
    for (job = 0; job < totalJobs; job++)
        printf("%2d ", releaseTime[job]);
    printf("\n");

    printf("Processing times: \n");
    printf("\t\t  ");
    for (machine = 0; machine < totalMachines; machine++)
        printf("M%-2d ", machine);
    printf("\n");
    for (operation = 0; operation < totalOperations; operation++) {
        
        printf("[J%-2d] %2d:", operationIsFromJob[operation], operation);
        for (machine = 0; machine < totalMachines; machine++)
            printf("%3d ", processingTime[operation][machine]);
        printf("\n");
    }
}

void ProblemFJSSP::printSolutionInfo(Solution &solution){ printSchedule(solution);}

void ProblemFJSSP::printSchedule(Solution & solution){
    
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int operationInPosition = 0;
    int operation = 0;
    int job = 0;
    int machine = 0;
    int numberOp = 0;
    
    int operationOfJob [totalJobs];
    int startingTime [totalOperations];
    int endingTime [totalOperations];
    int timeInMachine [totalMachines];
    int workload [totalMachines];
    
    char gantt[totalMachines][255];
    int time = 0;
    
    for (operation = 0; operation < totalOperations; operation++) {
        startingTime[operation] = 0;
        endingTime[operation] = 0;
    }
    
    for (job = 0; job < totalJobs; job++)
        operationOfJob[job] = 0;
    
    for (machine = 0; machine < totalMachines; machine++){
        timeInMachine[machine] = 0;
        workload[machine] = 0;
        
        /**creates an empty gantt**/
        for (time = 0; time < 255; time++) {
            gantt[machine][time] = ' ';
        }
    }
    int map = 0;
    for (operationInPosition = 0; operationInPosition < totalOperations; operationInPosition++) {
        
        map = solution.getVariable(operationInPosition);
        job = mapToJobMachine[map][0];
        machine = mapToJobMachine[map][1];
        
        numberOp = jobOperationHasNumber[job][operationOfJob[job]];
        
        /** With the number of operation and the machine we can continue. **/
        workload[machine] += processingTime[numberOp][machine];
        totalWorkload += processingTime[numberOp][machine];
        
        if (operationOfJob[job] == 0) { /** If it is the first operation of the job.**/
            if(timeInMachine[machine] >= releaseTime[job]){
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            }else{ /** If the job has to wait for the release time.**/
                startingTime[numberOp] = releaseTime[job];
                timeInMachine[machine] = releaseTime[job] + processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            }
            
        }else{
            if(endingTime[numberOp - 1] > timeInMachine[machine]){ /**The operation is waiting for their dependency operation.**/
                
                startingTime[numberOp] = endingTime[numberOp - 1];
                timeInMachine[machine] = endingTime[numberOp - 1] + processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
                
            }else{ /**The operation starts when the machine is avaliable.**/
                
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += processingTime[numberOp][machine];
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
    
    solution.setObjective(0, makespan);
    solution.setObjective(1, maxWorkload);
    
    printf("Op :  M  ti -  tf\n");
    for (operation = 0; operation < totalOperations; operation++)
        printf("%3d: %2d %3d - %3d \n", operation, mapToJobMachine[solution.getVariable(operation)][1], startingTime[operation], endingTime[operation]);
    
    printf("makespan: %d\nmaxWorkLoad: %d\ntotalWorkload: %d \n", makespan, maxWorkload, totalWorkload);

    for (machine = 0; machine < totalMachines; machine++) {
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

void ProblemFJSSP::printSolution(Solution & solution){
    printPartialSolution(solution, totalOperations - 1);
}

void ProblemFJSSP::printPartialSolution(Solution & solution, int level){
    
    int indexVar = 0;
    int withVariables = 1;
    
    for (indexVar = 0; indexVar < getNumberOfObjectives(); indexVar++)
        printf("%7.0f ", solution.getObjective(indexVar));
    
    if (withVariables == 1) {
        
        printf(" | ");
        
        for (indexVar = 0; indexVar <= level; indexVar++)
            printf("%3d ", solution.getVariable(indexVar));
        
        for (indexVar = level + 1; indexVar < totalOperations; indexVar ++)
            printf("  - ");
        /*
        printf("|\n\t\t\t\t | ");
        for (indexVar = 0; indexVar <= level; indexVar++)
            printf("%3d ", getMapping(solution->getVariable(indexVar), 0));
        
        for (indexVar = level + 1; indexVar < totalOperations; indexVar ++)
                printf("  - ");
        
        printf("|\n\t\t\t\t | ");

        for (indexVar = 0; indexVar <= level; indexVar++)
            printf("%3d ", getMapping(solution->getVariable(indexVar), 1));
        
        for (indexVar = level + 1; indexVar < totalOperations; indexVar ++)
            printf("  - ");
        */
        printf("|");
    }
}


