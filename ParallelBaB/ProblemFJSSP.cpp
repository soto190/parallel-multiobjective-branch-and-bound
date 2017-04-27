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
    
    n_jobs = 0;
    n_operations = 0;
    n_machines = 0;
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
n_jobs(toCopy.getNumberOfJobs()),
n_operations(toCopy.getNumberOfOperations()),
n_machines(toCopy.getNumberOfMachines()),
goodSolutionWithMaxWorkload(toCopy.goodSolutionWithMaxWorkload),
sumOfMinPij(toCopy.getSumOfMinPij()),
bestWorkloadFound(toCopy.getBestWorkloadFound()){

        lowerBound = new int[totalVariables];
        upperBound = new int[totalVariables];

    std::strcpy(name, toCopy.name);
    
        
    jobMachineToMap = new int * [n_jobs];
    mapToJobMachine = new int * [n_jobs * n_machines];
    processingTime = new int * [n_operations];
    jobOperationHasNumber = new int * [n_jobs];
    
    releaseTime = new int[n_jobs];
    numberOfOperationsInJob = new int[n_jobs];
    operationIsFromJob = new int[n_operations];
    assignationBestWorkload = new int[n_operations];
    assignationMinPij = new int[n_operations];
    bestWorkloads = new int[n_machines];
    minWorkload = new int[n_machines];
    
    int job = 0, map = 0, machine = 0, operation = 0, operationCounter = 0;
    
    for (job = 0; job < n_jobs; ++job) {
        numberOfOperationsInJob[job] = toCopy.getNumberOfOperationsInJob(job);
        releaseTime[job] = toCopy.getReleaseTimeOfJob(job);
        
        jobMachineToMap[job] = new int[toCopy.getNumberOfMachines()];
        
        for (machine = 0; machine < n_machines; ++machine) {
            mapToJobMachine[map] = new int[2];
            mapToJobMachine[map][0] = toCopy.getMapOfJobMachine(map, 0);
            mapToJobMachine[map][1] = toCopy.getMapOfJobMachine(map, 1);
            jobMachineToMap[job][machine] = toCopy.getJobMachineToMap(job, machine);
            map++;
        }
    }
    
    for (machine = 0; machine < n_machines; ++machine) {
        minWorkload[machine] = toCopy.getMinWorkload(machine);
        bestWorkloads[machine] = toCopy.getBestWorkload(machine);
    }
    
    for (operation = 0; operation < n_operations; ++operation){
        lowerBound[operation] = toCopy.getLowerBound(operation);
        
        processingTime[operation] = new int[toCopy.getNumberOfMachines()];
        for (machine = 0; machine < n_machines; ++machine)
            processingTime[operation][machine] = toCopy.getProccessingTime(operation, machine);
        
        assignationMinPij[operation] = toCopy.getAssignationMinPij(operation);
        assignationBestWorkload[operation] = toCopy.getAssignatioBestWorkload(operation);
    }
    
    for (job = 0; job < n_jobs; ++job) {
        jobOperationHasNumber[job] = new int[toCopy.getNumberOfOperationsInJob(job)];
        
        for(operation = 0; operation < numberOfOperationsInJob[job]; ++operation){
            jobOperationHasNumber[job][operation] = toCopy.getOperationInJobIsNumber(job, operation);
            operationIsFromJob[operationCounter] = toCopy.getOperationIsFromJob(operationCounter);
            operationCounter++;
        }
    }
}

ProblemFJSSP& ProblemFJSSP::operator=(const ProblemFJSSP &toCopy){
    
    if (this == &toCopy) return *this;
    
    
    n_jobs = toCopy.getNumberOfJobs();
    n_operations = toCopy.getNumberOfOperations();
    n_machines = toCopy.getNumberOfMachines();
    totalVariables = toCopy.getNumberOfVariables();
    totalObjectives = toCopy.getNumberOfObjectives();
    totalConstraints = toCopy.getNumberOfConstraints();
    
    if (processingTime != nullptr) {
        int job = 0, operation = 0;
        for(job = 0; job < n_jobs; ++job){
            delete [] jobMachineToMap[job];
            delete [] jobOperationHasNumber[job];
        }
        
        for (job = 0; job < n_jobs * n_machines; ++job)
            delete [] mapToJobMachine[job];
        
        for (operation = 0; operation < n_operations; ++operation)
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
    }
    
    lowerBound = new int[totalVariables];
    upperBound = new int[totalVariables];
    
    std::strcpy(name, toCopy.name);
    
    goodSolutionWithMaxWorkload = toCopy.goodSolutionWithMaxWorkload;
    
    sumOfMinPij = toCopy.getSumOfMinPij();
    bestWorkloadFound = toCopy.getBestWorkloadFound();
    
    jobMachineToMap = new int * [n_jobs];
    mapToJobMachine = new int * [n_jobs * n_machines];
    
    numberOfOperationsInJob = new int[n_jobs];
    releaseTime = new int[n_jobs];
    operationIsFromJob = new int[n_operations];
    assignationMinPij = new int[n_operations];
    assignationBestWorkload = new int[n_operations];
    bestWorkloads = new int[n_machines];
    minWorkload = new int[n_machines];
    
    int job = 0, map = 0, machine = 0, operation = 0, operationCounter = 0;
    
    for (job = 0; job < n_jobs; ++job) {
        numberOfOperationsInJob[job] = toCopy.getNumberOfOperationsInJob(job); //jobHasNoperations[job];
        releaseTime[job] = toCopy.getReleaseTimeOfJob(job);//releaseTime[job];
        
        jobMachineToMap[job] = new int[n_machines];
        
        for (machine = 0; machine < n_machines; ++machine) {
            mapToJobMachine[map] = new int[2];
            mapToJobMachine[map][0] = toCopy.getMapOfJobMachine(map, 0); //toCopy.mapToJobMachine[map][0];
            mapToJobMachine[map][1] = toCopy.getMapOfJobMachine(map, 1);
            jobMachineToMap[job][machine] = toCopy.getJobMachineToMap(job, machine);// jobMachineToMap[job][machine];
            map++;
        }
    }
    
    for (machine = 0; machine < n_machines; ++machine) {
        minWorkload[machine] = toCopy.getMinWorkload(machine);// minWorkload[machine];
        bestWorkloads[machine] = toCopy.getBestWorkload(machine);// bestWorkloads[machine];
    }
    
    jobOperationHasNumber = new int * [n_jobs];
    processingTime = new int * [n_operations];
    
    for (operation = 0; operation < n_operations; ++operation){
        
        processingTime[operation] = new int[n_machines];
        for (machine = 0; machine < n_machines; ++machine)
            processingTime[operation][machine] = toCopy.getProccessingTime(operation, machine);// processingTime[operation][machine];
        
        assignationMinPij[operation] = toCopy.getAssignationMinPij(operation);// assignationMinPij[operation];
        assignationBestWorkload[operation] = toCopy.getAssignatioBestWorkload(operation);//assignationBestWorkload[operation];
    }
    
    for (job = 0; job < n_jobs; ++job) {
        jobOperationHasNumber[job] = new int[numberOfOperationsInJob[job]];
        for(operation = 0; operation < numberOfOperationsInJob[job]; ++operation){
            jobOperationHasNumber[job][operation] = toCopy.getOperationInJobIsNumber(job, operation);//operationInJobIsNumber[job][operation];
            operationIsFromJob[operationCounter] = toCopy.getOperationIsFromJob(operationCounter);// operationIsFromJob[operationCounter];
            operationCounter++;
        }
    }
    
    return *this;
}

ProblemFJSSP::~ProblemFJSSP(){
    int job = 0, operation = 0;
    for(job = 0; job < n_jobs; ++job){
        delete [] jobOperationHasNumber[job];
        delete [] jobMachineToMap[job];
    }
    
    for (job = 0; job < n_jobs * n_machines; ++job)
        delete [] mapToJobMachine[job];

    for (operation = 0; operation < n_operations; ++operation)
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
    int bestWL [n_machines];
    
    int operationOfJob [n_jobs];
    int startingTime [n_operations];
    int endingTime [n_operations];
    int timeInMachine [n_machines];
    int workload [n_machines];
    
    for (operation = 0; operation < n_operations; ++operation) {
        startingTime[operation] = 0;
        endingTime[operation] = 0;
    }
    
    for (job = 0; job < n_jobs; ++job)
        operationOfJob[job] = 0;
    
    for (machine = 0; machine < n_machines; ++machine){
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
        
        operationOfJob[job]++; /** This counts the number of operations from an allocated job. **/
        
        if (timeInMachine[machine] > makespan)
            makespan = timeInMachine[machine];
        
        if(workload[machine] > maxWorkload)
            maxWorkload = workload[machine];
        
    }
    
    for (machine = 0; machine < n_machines; ++machine)
        if(workload[machine] + bestWL[machine] > maxWorkload)
            maxWorkload = workload[machine] + bestWL[machine];
    
    solution.setObjective(0, makespan);
    solution.setObjective(1, maxWorkload);
//    solution.setObjective(1, totalWorkload + minPij);
    
    return 0.0;
}

void ProblemFJSSP::evaluateDynamic(Solution &solution, FJSSPdata &data, int level){
    
    int makespan = 0;
    int max_workload = 0;
    int temp = 0;
    int map = solution.getVariable(level);
    int job = getDecodeMap(map, 0);
    int machine = getDecodeMap(map, 1);
    int numberOp = getOperationInJobIsNumber(job, data.getNumberOfOperationsAllocatedInJob(job));
    
    
    /** With the operation number and the machine we can continue. **/
    int proccessingTime = getProccessingTime(numberOp, machine);
    data.decreaseToltalWorkload(getProccessingTime(numberOp, getAssignationMinPij(numberOp)));
    
    data.decreaseTempWorkloadIn(getAssignatioBestWorkload(numberOp), getProccessingTime(numberOp, getAssignatioBestWorkload(numberOp)));
    data.increaseTempWorkloadIn(machine, proccessingTime);
    
    data.setOperationAllocation(job, numberOp, machine, proccessingTime);

    if (data.getLastOperationAllocatedInJob(job) == 0) { /** If it is the first operation of the job.**/
        if(data.getTimeOnMachine(machine) >= getReleaseTimeOfJob(job)){
            
            data.setStartingTime(numberOp, data.getTimeOnMachine(machine));
            data.increaseTimeOnMachine(machine, proccessingTime);
            data.setEndingTime(numberOp, data.getTimeOnMachine(machine));
            
        }else{ /** If the job has to wait for the release time.**/
            
            data.setStartingTime(numberOp, getReleaseTimeOfJob(job));
            data.setTimeOnMachine(machine, getReleaseTimeOfJob(job) + proccessingTime);
            data.setEndingTime(numberOp, data.getTimeOnMachine(machine));
        }
    }else{
        if(data.getEndingTime(numberOp - 1) > data.getTimeOnMachine(machine)){
            
            data.setStartingTime(numberOp, data.getEndingTime(numberOp - 1));
            data.setTimeOnMachine(machine, data.getEndingTime(numberOp - 1) + proccessingTime);
            data.setEndingTime(numberOp, data.getTimeOnMachine(machine));
            
        }else{ /**The operation starts when the machine is avaliable.**/
            
            data.setStartingTime(numberOp, data.getTimeOnMachine(machine));
            data.increaseTimeOnMachine(machine, proccessingTime);
            data.setEndingTime(numberOp, data.getTimeOnMachine(machine));
        }
    }
    
    for (machine = 0; machine < n_machines; ++machine){

        if (data.getTimeOnMachine(machine) > makespan)
            makespan = data.getTimeOnMachine(machine);
        
        if(data.getWorkloadOnMachine(machine) > max_workload)
            max_workload = data.getWorkloadOnMachine(machine);
    
        if(data.getTempBestWorkloadInMachine(machine) > temp)
            temp = data.getTempBestWorkloadInMachine(machine);
    }
    
    if (temp < max_workload)
        max_workload = temp;
    
    data.setMakespan(makespan);
    data.setMaxWorkload(max_workload);
    
    solution.setObjective(0, makespan);
    solution.setObjective(1, max_workload);
//    solution.setObjective(1, data.getTotalWorkload());
}

void ProblemFJSSP::evaluateRemoveDynamic(Solution & solution, FJSSPdata& data, int level){
    int makespan = 0;
    int max_workload = 0;
    int temp = 0;
    
    int map = solution.getVariable(level);
    int job = getDecodeMap(map, 0);
    int machine = 0;
    int numberOp = getOperationInJobIsNumber(job, data.getLastOperationAllocatedInJob(job));
    
    /** With the operation number and the machine we can continue. **/
    data.increaseToltalWorkload(getProccessingTime(numberOp, getAssignationMinPij(numberOp))); /** Using the best total_workload. **/
    
    data.increaseTempWorkloadIn(getAssignatioBestWorkload(numberOp), getProccessingTime(numberOp, getAssignatioBestWorkload(numberOp)));
    data.decreaseTempWorkloadIn(machine, getProccessingTime(numberOp, machine));

    data.deallocateOperation(job, numberOp);
    
    for (machine = 0; machine < n_machines; ++machine){
        
        if (data.getTimeOnMachine(machine) > makespan)
            makespan = data.getTimeOnMachine(machine);
        
        if(data.getWorkloadOnMachine(machine) > max_workload)
            max_workload = data.getWorkloadOnMachine(machine);
        
        if(data.getTempBestWorkloadInMachine(machine) > temp)
            temp = data.getTempBestWorkloadInMachine(machine);
        
        
    }
    
    if (temp < max_workload)
        max_workload = temp;
    
    data.setMakespan(makespan);
    data.setMaxWorkload(max_workload);
    
    solution.setVariable(level, -1);
    solution.setObjective(0, makespan);
    solution.setObjective(1, max_workload);
//    solution.setObjective(1, data.getTotalWorkload());
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
    
    FJSSPdata fjsspd(n_jobs, n_operations, n_machines);
    
    for (job = 0; job < n_jobs; ++job)
        for (operation = 0; operation < numberOfOperationsInJob[job]; ++operation){
            map = jobMachineToMap[job][machine];
            solution.setVariable(countOperations++, map);
            machine++;
            if (machine == n_machines)
                machine = 0;
        }
    
    evaluate(solution);
}

void ProblemFJSSP::getSolutionWithLowerBoundInObj(int nObj, Solution& solution){
    
    if(nObj == 0)
        createDefaultSolution(solution);
    else if(nObj == 1)
        solution = goodSolutionWithMaxWorkload;
    else if(nObj == 2){
        int operation = 0;
        for (operation = 0; operation < n_operations; ++operation)
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
    
    int operationOfJob [n_jobs];
    int workload [n_machines];
    int maxWorkloadedMachine = 0;
    
    for (nMachine = 0; nMachine < n_machines; ++nMachine)
        workload[nMachine] = 0;
    
    /** Assign the operations to machines which generates the min TotalWorkload and computes the machines workloads. **/
    int counterOperations = 0;
    for (nJob = 0; nJob < getNumberOfJobs(); ++nJob){
        operationOfJob[nJob] = 0;
        for (nOperation = 0; nOperation < numberOfOperationsInJob[nJob]; ++nOperation){
            
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
        
        for (nOperation = 0; nOperation < n_operations; ++nOperation)
            if(mapToJobMachine[solution.getVariable(nOperation)][1] == maxWorkloadedMachine)
                for (nMachine = 0; nMachine < getNumberOfMachines(); ++nMachine)
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
        for (nMachine = 0; nMachine < getNumberOfMachines(); ++nMachine)
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
    
    getBestWorkload(maxWorkloadObj);// bestWorkloadFound = maxWorkloadObj;
    for (nMachine = 0; nMachine < getNumberOfMachines(); ++nMachine)
        bestWorkloads[nMachine] = workload[nMachine];

}

/** For all the variables the lower bound is 0. **/
int ProblemFJSSP::getLowerBound(int indexVar) const{ return 0; }

/** 
 *
 * The Range of variables is the number of maps.
 *
 **/
int ProblemFJSSP::getUpperBound(int indexVar) const{ return (n_jobs * n_machines) - 1; }

int ProblemFJSSP::getLowerBoundInObj(int nObj) const{
    if(nObj == 1)
        return sumOfMinPij;
    return INT_MAX;
}

void ProblemFJSSP::loadInstance(char filePath[2][255]){
    
    std::ifstream infile(filePath[0]);
    if(infile.is_open()){
        
        if(processingTime != nullptr){
            int job = 0, operation = 0;
            for(job = 0; job < n_jobs; ++job){
                delete [] jobOperationHasNumber[job];
                delete [] jobMachineToMap[job];
            }
            
            for (job = 0; job < n_jobs * n_machines; ++job)
                delete [] mapToJobMachine[job];
            
            for (operation = 0; operation < n_operations; ++operation)
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
        
        std::string line;
        std::vector<std::string> elemens;
        
        std::getline(infile, line);
        std::getline(infile, line);
        elemens = split(line, ' ');
        n_jobs = std::stoi(elemens.at(0));
        n_machines = std::stoi(elemens.at(1));
        numberOfOperationsInJob = new int[n_jobs];
        releaseTime = new int[n_jobs];
        
        std::getline(infile, line);
        std::getline(infile, line);
        elemens = split(line, ' ');
        int job = 0;
        n_operations = 0;

        for(job = 0; job < getNumberOfJobs(); ++job){
            numberOfOperationsInJob[job] = std::stoi(elemens.at(job));
            n_operations += numberOfOperationsInJob[job];
        }
        
        std::getline(infile, line);
        std::getline(infile, line);
        elemens = split(line, ' ');
        for (job = 0; job < getNumberOfJobs(); ++job)
            releaseTime[job] = std::stoi(elemens.at(job));
        
        
        operationIsFromJob = new int[n_operations];
        sumOfMinPij = 0;
        bestWorkloadFound = INT_MAX;
        std::getline(infile, line);
        
        assignationMinPij = new int [n_operations];
        minWorkload = new int[n_machines];
        assignationBestWorkload = new int [n_operations];
        bestWorkloads = new int[n_machines];
        jobOperationHasNumber = new int * [n_jobs];
        processingTime = new int * [n_operations];
        jobMachineToMap = new int * [n_jobs];
        mapToJobMachine = new int * [n_jobs * n_machines];
        
        int operation = 0;
        int machine = 0;
        int minPij = INT_MAX;
        int minMachine = 0;
        int map = 0;
        
        for (job = 0; job < getNumberOfJobs(); ++job) {
            jobMachineToMap[job] = new int[n_machines];
            for (machine = 0; machine < n_machines; ++machine) {
                mapToJobMachine[map] = new int[2];
                mapToJobMachine[map][0] = job;
                mapToJobMachine[map][1] = machine;
                jobMachineToMap[job][machine] = map;
                map++;
            }
        }
        
        for (machine = 0; machine < getNumberOfMachines(); ++machine)
            minWorkload[machine] = 0;
        
        for (operation = 0; operation < getNumberOfOperations(); ++operation) {
            processingTime[operation] = new int[n_machines];
            std::getline(infile, line);
            elemens = split(line, ' ');
            minPij = INT_MAX;
            minMachine = 0;
            for (machine = 0; machine < getNumberOfMachines(); ++machine){
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

        infile.close();

        totalVariables = n_operations;
        
        int operationCounter = 0;
        for (job = 0; job < getNumberOfJobs(); ++job) {
            jobOperationHasNumber[job] = new int[numberOfOperationsInJob[job]];
            for(operation = 0; operation < numberOfOperationsInJob[job]; ++operation){
                jobOperationHasNumber[job][operation] = operationCounter;
                operationIsFromJob[operationCounter++] = job;
            }
        }
        goodSolutionWithMaxWorkload (getNumberOfObjectives(), getNumberOfVariables());
        buildSolutionWithGoodMaxWorkload(goodSolutionWithMaxWorkload);
    }
}

ProblemType ProblemFJSSP::getType() const { return ProblemType::permutation_with_repetition_and_combination; }
int ProblemFJSSP::getStartingRow(){ return 0;}
int ProblemFJSSP::getFinalLevel(){return n_operations - 1;}
int ProblemFJSSP::getTotalElements(){return n_jobs;}
int * ProblemFJSSP::getElemensToRepeat(){return numberOfOperationsInJob;}

/** If position = 0 returns the job, if position = 1 returns the machine. (Decodes the map in job or machine). **/
int ProblemFJSSP::getDecodeMap(int map, int position){return mapToJobMachine[map][position];}

/** Returns the map corresponding to the configuration of job and machine. (Codes the job and machine in a map). **/
int ProblemFJSSP::getCodeMap(int job, int machine){return jobMachineToMap[job][machine];}
int ProblemFJSSP::getTimesValueIsRepeated(int value){return numberOfOperationsInJob[value];}
int ProblemFJSSP::getNumberOfJobs() const{ return n_jobs;}
int ProblemFJSSP::getNumberOfOperations() const{ return n_operations;}
int ProblemFJSSP::getNumberOfMachines() const{ return n_machines;}
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
    printf("Total jobs: %d\n", n_jobs);
    printf("Total machines: %d\n", n_machines);
    printf("Total operations: %d\n", n_operations);

    printf("Operations in each job:\n");
    
    int job = 0, machine = 0, operation = 0;
    for (job = 0; job < n_jobs; ++job)
        printf("%2d ", numberOfOperationsInJob[job]);
    printf("\n");
    
    printf("Release time for each job:\n");
    for (job = 0; job < n_jobs; ++job)
        printf("%2d ", releaseTime[job]);
    printf("\n");

    printf("Processing times: \n");
    printf("\t\t  ");
    for (machine = 0; machine < n_machines; ++machine)
        printf("M%-2d ", machine);
    printf("\n");
    for (operation = 0; operation < n_operations; ++operation) {
        
        printf("[J%-2d] %2d:", operationIsFromJob[operation], operation);
        for (machine = 0; machine < n_machines; ++machine)
            printf("%3d ", processingTime[operation][machine]);
        printf("\n");
    }
}

void ProblemFJSSP::printSolutionInfo(const Solution &solution) const{ printSchedule(solution);}

void ProblemFJSSP::printSchedule(const Solution & solution) const {
    
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int operationInPosition = 0;
    int operation = 0;
    int job = 0;
    int machine = 0;
    int numberOp = 0;
    
    int operation_in_machine[n_operations];
    int operationOfJob [n_jobs];
    int startingTime [n_operations];
    int endingTime [n_operations];
    int timeInMachine [n_machines];
    int workload [n_machines];
    
    char gantt[n_machines][255];
    int time = 0;
    
    for (operation = 0; operation < n_operations; ++operation) {
        startingTime[operation] = 0;
        endingTime[operation] = 0;
        operation_in_machine[operation] = 0;
    }
    
    for (job = 0; job < n_jobs; ++job)
        operationOfJob[job] = 0;
    
    for (machine = 0; machine < n_machines; ++machine){
        timeInMachine[machine] = 0;
        workload[machine] = 0;
        
        /**creates an empty gantt**/
        for (time = 0; time < 255; time++) {
            gantt[machine][time] = ' ';
        }
    }
    int map = 0;
    for (operationInPosition = 0; operationInPosition < n_operations; operationInPosition++) {
        
        map = solution.getVariable(operationInPosition);
        job = mapToJobMachine[map][0];
        machine = mapToJobMachine[map][1];
        
        numberOp = jobOperationHasNumber[job][operationOfJob[job]];
        operation_in_machine[numberOp] = machine;
        
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
            gantt[machine][time] = 'a' + numberOp;
        
        operationOfJob[job]++;
        
        if (timeInMachine[machine] > makespan)
            makespan = timeInMachine[machine];
        
        if(workload[machine] > maxWorkload)
            maxWorkload = workload[machine];
    }
    
    printf("\tOp :  M  ti -  tf\n");
    for (operation = 0; operation < n_operations; ++operation)
        printf("%3c %3d: %2d %3d - %3d \n", 'a' + operation, operation, operation_in_machine[operation], startingTime[operation], endingTime[operation]);
    
    for (machine = 0; machine < n_machines; ++machine) {
        printf("M%d  | ", machine);
        for (time = 0; time <= makespan; ++time)
            printf("%3c", gantt[machine][time]);
        printf("| %3d\n", workload[machine]);
    }
     printf("----");
    for (time = 0; time <= makespan; ++time)
        printf("---");
    printf("--\n");

    printf("Time:");
    for (time = 0; time <= makespan; ++time)
        printf("%3d", (time));
    printf("\n");
    printf("makespan: %d\nmaxWorkLoad: %d\ntotalWorkload: %d \n", makespan, maxWorkload, totalWorkload);

}

void ProblemFJSSP::printSolution(const Solution & solution) const{
    printPartialSolution(solution, n_operations - 1);
}

void ProblemFJSSP::printPartialSolution(const Solution & solution, int level) const{
    
    int indexVar = 0;
    int withVariables = 1;
    
    for (indexVar = 0; indexVar < getNumberOfObjectives(); indexVar++)
        printf("%7.0f ", solution.getObjective(indexVar));
    
    if (withVariables == 1) {
        
        printf(" | ");
        
        for (indexVar = 0; indexVar <= level; indexVar++)
            printf("%3d ", solution.getVariable(indexVar));
        
        for (indexVar = level + 1; indexVar < n_operations; indexVar ++)
            printf("  - ");
        
        printf("|");
    }
}


