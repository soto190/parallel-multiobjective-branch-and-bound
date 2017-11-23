//
//  ProblemFJSSP.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 14/11/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "ProblemFJSSP.hpp"

ProblemFJSSP::ProblemFJSSP() :
Problem() {
    n_jobs = 0;
    n_operations = 0;
    n_machines = 0;
    sumOfMinPij = 0;
    avg_op_per_machine = 0;
    bestBound_maxWorkload = 0;
    bestBound_makespan = 0;
    sum_M_smallest_est = 0;
    min_sum_shortest_proc_times = 0;
    bestWorkloadFound = 0;
    bestMakespanFound = INT_MAX;
    max_eet_of_jobs = 0;
    
    jobMachineToMap = nullptr;
    mapToJobMachine = nullptr;
    n_operations_in_job = nullptr;
    releaseTime = nullptr;
    operationIsFromJob = nullptr;
    assignationMinPij = nullptr;
    assignationBestWorkload = nullptr;
    assignationBestMakespan = nullptr;
    bestWorkloads = nullptr;
    minWorkload = nullptr;
    jobOperationHasNumber = nullptr;
    processingTime = nullptr;
    
    lowerBound = nullptr;
    upperBound = nullptr;
    
    earliest_starting_time = nullptr; /** Length equals to number of operations. **/
    earliest_ending_time = nullptr; /** Length equals to number of operations. **/
    eet_of_job = nullptr; /** Length equals to number of jobs. **/
    sum_shortest_proc_times = nullptr; /** D^{k}_{Ñ}. Length equals to number of machines.**/
    
}
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
ProblemFJSSP::ProblemFJSSP(int totalObjectives, int totalVariables) :
Problem(totalObjectives, totalVariables) {
    
    n_jobs = 0;
    n_operations = 0;
    n_machines = 0;
    sumOfMinPij = 0;
    avg_op_per_machine = 0;
    bestBound_maxWorkload = 0;
    bestBound_makespan = 0;
    sum_M_smallest_est = 0;
    min_sum_shortest_proc_times = 0;
    bestWorkloadFound = 0;
    bestMakespanFound = INT_MAX;
    max_eet_of_jobs = 0;
    jobMachineToMap = nullptr;
    mapToJobMachine = nullptr;
    n_operations_in_job = nullptr;
    releaseTime = nullptr;
    operationIsFromJob = nullptr;
    assignationMinPij = nullptr;
    assignationBestWorkload = nullptr;
    assignationBestMakespan = nullptr;
    bestWorkloads = nullptr;
    minWorkload = nullptr;
    jobOperationHasNumber = nullptr;
    processingTime = nullptr;
    
    lowerBound = nullptr;
    upperBound = nullptr;
    
    earliest_starting_time = nullptr; /** Length equals to number of operations. **/
    earliest_ending_time = nullptr; /** Length equals to number of operations. **/
    eet_of_job = nullptr; /** Length equals to number of jobs. **/
    sum_shortest_proc_times = nullptr; /** D^{k}_{Ñ}. Length equals to number of machines.**/
}

ProblemFJSSP::ProblemFJSSP(const ProblemFJSSP& toCopy) :
Problem(toCopy),
n_jobs(toCopy.getNumberOfJobs()),
n_operations(toCopy.getNumberOfOperations()),
n_machines(toCopy.getNumberOfMachines()),
sumOfMinPij(toCopy.getSumOfMinPij()),
bestWorkloadFound(toCopy.getBestWorkloadFound()),
bestMakespanFound(toCopy.getBestMakespanFound()),
goodSolutionWithMaxWorkload(toCopy.goodSolutionWithMaxWorkload) {
    
    lowerBound = new int[totalVariables];
    upperBound = new int[totalVariables];
    
    avg_op_per_machine = toCopy.getAvgOperationPerMachine();
    min_sum_shortest_proc_times = toCopy.getMinSumShortestProcTime();
    sum_M_smallest_est = toCopy.getSumOf_M_smallestEST();
    
    bestBound_makespan = toCopy.getBestBoundMakespan();
    bestBound_maxWorkload = toCopy.getBestBoundMaxWorkload();
    
    std::strcpy(name, toCopy.name);
    
    jobMachineToMap = new int *[n_jobs];
    mapToJobMachine = new int *[n_jobs * n_machines];
    processingTime = new int *[n_operations];
    jobOperationHasNumber = new int *[n_jobs];
    
    releaseTime = new int[n_jobs];
    n_operations_in_job = new int[n_jobs];
    operationIsFromJob = new int[n_operations];
    assignationBestWorkload = new int[n_operations];
    assignationMinPij = new int[n_operations];
    assignationBestMakespan = new int[n_operations];
    bestWorkloads = new int[n_machines];
    minWorkload = new int[n_machines];
    
    earliest_starting_time = new int[n_operations];/** Length equals to number of operations. **/
    earliest_ending_time = new int[n_operations]; /** Length equals to number of operations. **/
    eet_of_job = new int[n_jobs]; /** Length equals to number of jobs. **/
    sum_shortest_proc_times = new int[n_machines]; /** D^{k}_{Ñ}. Length equals to number of machines **/
    max_eet_of_jobs = toCopy.getMaxEarliestEndingTime(); /** TODO: Fix this.**/
    
    int job = 0, map = 0, machine = 0, operation = 0, operationCounter = 0;
    
    for (job = 0; job < n_jobs; ++job) {
        n_operations_in_job[job] = toCopy.getNumberOfOperationsInJob(job);
        releaseTime[job] = toCopy.getReleaseTimeOfJob(job);
        eet_of_job[job] = toCopy.getEarliestEndingJobTime(job);
        
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
        sum_shortest_proc_times[machine] = toCopy.getSumShortestProcTimeInMachine(machine);
    }
    
    for (operation = 0; operation < n_operations; ++operation) {
        lowerBound[operation] = toCopy.getLowerBound(operation);
        upperBound[operation] = toCopy.getUpperBound(operation);
        earliest_starting_time[operation] = toCopy.getEarliestStartingTime(operation);
        earliest_ending_time[operation] = toCopy.getEarliestEndingTime(operation);
        
        processingTime[operation] = new int[toCopy.getNumberOfMachines()];
        for (machine = 0; machine < n_machines; ++machine)
            processingTime[operation][machine] = toCopy.getProccessingTime(operation, machine);
        
        assignationMinPij[operation] = toCopy.getAssignationMinPij(operation);
        assignationBestWorkload[operation] = toCopy.getAssignationBestWorkload(operation);
        assignationBestMakespan[operation] = toCopy.getAssignationBestMakespan(operation);
    }
    
    for (job = 0; job < n_jobs; ++job) {
        jobOperationHasNumber[job] = new int[toCopy.getNumberOfOperationsInJob(job)];
        
        for (operation = 0; operation < n_operations_in_job[job]; ++operation) {
            jobOperationHasNumber[job][operation] = toCopy.getOperationInJobIsNumber(job, operation);
            operationIsFromJob[operationCounter] = toCopy.getOperationIsFromJob(operationCounter);
            operationCounter++;
        }
    }
}

ProblemFJSSP::ProblemFJSSP(const Payload_problem_fjssp& problem) :
Problem(problem.n_objectives, problem.n_operations) {
    totalObjectives = problem.n_objectives;
    totalVariables = problem.n_operations;
    n_jobs = problem.n_jobs;
    n_machines = problem.n_machines;
    n_operations = problem.n_operations;
    
    sumOfMinPij = 0;
    bestWorkloadFound = INT_MAX;
    
    n_operations_in_job = new int[n_jobs];
    releaseTime = new int[n_jobs];
    
    operationIsFromJob = new int[n_operations];
    assignationMinPij = new int[n_operations];
    minWorkload = new int[n_machines];
    assignationBestWorkload = new int[n_operations];
    assignationBestMakespan = new int[n_operations];
    bestWorkloads = new int[n_machines];
    jobOperationHasNumber = new int *[n_jobs];
    processingTime = new int *[n_operations];
    jobMachineToMap = new int *[n_jobs];
    mapToJobMachine = new int *[n_jobs * n_machines];
    
    earliest_starting_time = new int[n_operations];/** Length equals to number of operations. **/
    earliest_ending_time = new int[n_operations]; /** Length equals to number of operations. **/
    eet_of_job = new int[n_jobs]; /** Length equals to number of jobs. **/
    sum_shortest_proc_times = new int[n_machines]; /** D^{k}_{Ñ}. Length equals to number of machines.**/
    
    avg_op_per_machine = e_function(n_operations / n_machines); /** Ñ parameter (N tilde).  **/
    min_sum_shortest_proc_times = 0; /** D_{{k_0}, Ñ}. **/
    max_eet_of_jobs = 0;
    sum_M_smallest_est = 0;
    
    int minPij = INT_MAX;
    int minMachine = 0;
    int map = 0;
    
    for (int n_job = 0; n_job < n_jobs; ++n_job) {
        n_operations_in_job[n_job] = problem.n_operations_in_job[n_job];
        releaseTime[n_job] = problem.release_times[n_job];
    }
    
    /** Creates the mapping. **/
    for (int job = 0; job < n_jobs; ++job) {
        jobMachineToMap[job] = new int[n_machines];
        for (int machine = 0; machine < n_machines; ++machine) {
            mapToJobMachine[map] = new int[2];
            mapToJobMachine[map][0] = job;
            mapToJobMachine[map][1] = machine;
            jobMachineToMap[job][machine] = map;
            map++;
        }
    }
    
    int sorted_processing[n_machines][n_operations]; //new int * [n_machines];/** Stores the processing times from min to max for each machine. **/
    int sorted_est[n_operations];
    for (int machine = 0; machine < n_machines; ++machine)
        minWorkload[machine] = 0;
    
    for (int n_op = 0; n_op < n_operations; ++n_op) {
        processingTime[n_op] = new int[n_machines];
        for (int n_mach = 0; n_mach < n_machines; ++n_machines)
            processingTime[n_op][n_mach] = problem.processing_times[n_op * problem.n_machines + n_mach];
    }
    
    for (int operation = 0; operation < n_operations; ++operation) {
        minPij = INT_MAX;
        minMachine = 0;
        for (int machine = 0; machine < n_machines; ++machine) {
            sorted_processing[machine][operation] = processingTime[operation][machine];
            if (processingTime[operation][machine] < minPij) {
                minPij = processingTime[operation][machine];
                minMachine = machine;
            }
        }
        sumOfMinPij += minPij;
        minWorkload[minMachine] += processingTime[operation][minMachine];
        assignationMinPij[operation] = minMachine;
    }
    
    earliest_starting_time[0] = releaseTime[0];
    int op_allocated = 0;
    int next_op = 0;
    /** Computes the earliest starting time and the earliest ending time for each job. **/
    for (int job = 0; job < n_jobs; ++job) {
        earliest_starting_time[op_allocated] = releaseTime[job];
        earliest_ending_time[op_allocated] = earliest_starting_time[op_allocated] + processingTime[op_allocated][assignationMinPij[op_allocated]];
        sorted_est[op_allocated] = earliest_starting_time[op_allocated];
        
        for (int operation = 1; operation < n_operations_in_job[job]; ++operation) {
            next_op = op_allocated + operation;
            earliest_starting_time[next_op] = earliest_ending_time[next_op - 1];
            earliest_ending_time[next_op] = earliest_starting_time[next_op] + processingTime[next_op][assignationMinPij[next_op]];
            sorted_est[next_op] = earliest_starting_time[next_op];
        }
        op_allocated += n_operations_in_job[job];
        eet_of_job[job] = earliest_ending_time[op_allocated - 1];
        
        if (eet_of_job[job] > max_eet_of_jobs)
            max_eet_of_jobs = eet_of_job[job];
    }
    
    std::sort(sorted_est, sorted_est + n_operations);
    for (int machine = 0; machine < n_machines; ++machine)
        std::sort(sorted_processing[machine], sorted_processing[machine] + n_operations);
    
    sum_M_smallest_est = 0;
    min_sum_shortest_proc_times = INT_MAX;
    for (int machine = 0; machine < n_machines; ++machine) {
        
        sum_M_smallest_est += sorted_est[machine];
        sum_shortest_proc_times[machine] = 0;
        
        for (int n = 0; n < avg_op_per_machine && sorted_processing[machine][n] != INF_PROC_TIME; ++n) /** Some times the machines can compute less jobs than the average. **/
            sum_shortest_proc_times[machine] += sorted_processing[machine][n];
        
        if (sum_shortest_proc_times[machine] < min_sum_shortest_proc_times)
            min_sum_shortest_proc_times = sum_shortest_proc_times[machine];
    }
    
    int operationCounter = 0;
    for (int job = 0; job < n_jobs; ++job) {
        jobOperationHasNumber[job] = new int[n_operations_in_job[job]];
        for (int operation = 0; operation < n_operations_in_job[job]; ++operation) {
            jobOperationHasNumber[job][operation] = operationCounter;
            operationIsFromJob[operationCounter++] = job;
        }
    }
    
    int temp_f2 = e_function(sumOfMinPij / n_machines);
    int temp_f1 = e_function((sum_M_smallest_est + sumOfMinPij) / n_machines);
    
    bestBound_maxWorkload = temp_f2 >= min_sum_shortest_proc_times ? temp_f2 : min_sum_shortest_proc_times;
    bestBound_makespan = max_eet_of_jobs >= temp_f1 ? max_eet_of_jobs : temp_f1;
    
    goodSolutionWithMaxWorkload(getNumberOfObjectives(), getNumberOfVariables());
    buildSolutionWithGoodMaxWorkload(goodSolutionWithMaxWorkload);
    
}

void ProblemFJSSP::loadInstancePayload(const Payload_problem_fjssp& problem) {
    totalObjectives = problem.n_objectives;
    totalVariables = problem.n_operations;
    n_jobs = problem.n_jobs;
    n_machines = problem.n_machines;
    n_operations = problem.n_operations;
    
    sumOfMinPij = 0;
    bestWorkloadFound = INT_MAX;
    
    upperBound = new int[n_operations];
    lowerBound = new int[n_operations];
    
    n_operations_in_job = new int[n_jobs];
    releaseTime = new int[n_jobs];
    
    operationIsFromJob = new int[n_operations];
    assignationMinPij = new int[n_operations];
    minWorkload = new int[n_machines];
    assignationBestWorkload = new int[n_operations];
    assignationBestMakespan = new int[n_operations];
    bestWorkloads = new int[n_machines];
    jobOperationHasNumber = new int *[n_jobs];
    processingTime = new int *[n_operations];
    jobMachineToMap = new int *[n_jobs];
    mapToJobMachine = new int *[n_jobs * n_machines];
    
    earliest_starting_time = new int[n_operations];/** Length equals to number of operations. **/
    earliest_ending_time = new int[n_operations]; /** Length equals to number of operations. **/
    eet_of_job = new int[n_jobs]; /** Length equals to number of jobs. **/
    sum_shortest_proc_times = new int[n_machines]; /** D^{k}_{Ñ}. Length equals to number of machines.**/
    
    avg_op_per_machine = e_function(n_operations / n_machines); /** Ñ parameter (N tilde).  **/
    min_sum_shortest_proc_times = 0; /** D_{{k_0}, Ñ}. **/
    max_eet_of_jobs = 0;
    sum_M_smallest_est = 0;
    
    int map = 0;
    
    for (int n_job = 0; n_job < n_jobs; ++n_job) {
        n_operations_in_job[n_job] = problem.n_operations_in_job[n_job];
        releaseTime[n_job] = problem.release_times[n_job];
    }
    
    for (int n_op = 0; n_op < n_operations; ++n_op) {
        processingTime[n_op] = new int[n_machines];
        for (int n_mach = 0; n_mach < n_machines; ++n_mach)
            processingTime[n_op][n_mach] = problem.processing_times[n_op * problem.n_machines + n_mach];
        lowerBound[n_op] = 0;
        upperBound[n_op] = n_jobs * n_machines;
    }
    
    /** Creates the mapping. **/
    for (int job = 0; job < n_jobs; ++job) {
        jobMachineToMap[job] = new int[n_machines];
        for (int machine = 0; machine < n_machines; ++machine) {
            mapToJobMachine[map] = new int[2];
            mapToJobMachine[map][0] = job;
            mapToJobMachine[map][1] = machine;
            jobMachineToMap[job][machine] = map;
            map++;
        }
    }
    
    int sorted_processing[n_machines][n_operations];/** Stores the processing times from min to max for each machine. **/
    int sorted_est[n_operations];
    for (int machine = 0; machine < n_machines; ++machine)
        minWorkload[machine] = 0;
    
    int minPij = INT_MAX;
    int minMachine = 0;
    for (int operation = 0; operation < n_operations; ++operation) {
        minPij = INT_MAX;
        minMachine = 0;
        for (int machine = 0; machine < n_machines; ++machine) {
            sorted_processing[machine][operation] = processingTime[operation][machine];
            if (processingTime[operation][machine] < minPij) {
                minPij = processingTime[operation][machine];
                minMachine = machine;
            }
        }
        sumOfMinPij += minPij;
        minWorkload[minMachine] += processingTime[operation][minMachine];
        assignationMinPij[operation] = minMachine;
    }
    
    earliest_starting_time[0] = releaseTime[0];
    int op_allocated = 0;
    int next_op = 0;
    /** Computes the earlier starting time and the earlier ending time for each job. **/
    for (int job = 0; job < n_jobs; ++job) {
        earliest_starting_time[op_allocated] = releaseTime[job];
        earliest_ending_time[op_allocated] = earliest_starting_time[op_allocated] + processingTime[op_allocated][assignationMinPij[op_allocated]];
        sorted_est[op_allocated] = earliest_starting_time[op_allocated];
        for (int operation = 1; operation < n_operations_in_job[job]; ++operation) {
            next_op = op_allocated + operation;
            earliest_starting_time[next_op] = earliest_ending_time[next_op - 1];
            earliest_ending_time[next_op] = earliest_starting_time[next_op] + processingTime[next_op][assignationMinPij[next_op]];
            sorted_est[next_op] = earliest_starting_time[next_op];
        }
        op_allocated += n_operations_in_job[job];
        eet_of_job[job] = earliest_ending_time[op_allocated - 1];
        
        if (eet_of_job[job] > max_eet_of_jobs)
            max_eet_of_jobs = eet_of_job[job];
    }
    
    std::sort(sorted_est, sorted_est + n_operations);
    for (int machine = 0; machine < n_machines; ++machine)
        std::sort(sorted_processing[machine], sorted_processing[machine] + n_operations);
    
    sum_M_smallest_est = 0;
    min_sum_shortest_proc_times = INT_MAX;
    for (int machine = 0; machine < n_machines; ++machine) {
        
        sum_M_smallest_est += sorted_est[machine];
        sum_shortest_proc_times[machine] = 0;
        
        for (int n = 0; n < avg_op_per_machine && sorted_processing[machine][n] != INF_PROC_TIME; ++n) /** Some times the machines can compute less jobs than the average. **/
            sum_shortest_proc_times[machine] += sorted_processing[machine][n];
        
        if (sum_shortest_proc_times[machine] < min_sum_shortest_proc_times)
            min_sum_shortest_proc_times = sum_shortest_proc_times[machine];
    }
    
    int operationCounter = 0;
    for (int job = 0; job < n_jobs; ++job) {
        jobOperationHasNumber[job] = new int[n_operations_in_job[job]];
        for (int operation = 0; operation < n_operations_in_job[job]; ++operation) {
            jobOperationHasNumber[job][operation] = operationCounter;
            operationIsFromJob[operationCounter++] = job;
        }
    }
    
    int temp_f2 = e_function(sumOfMinPij / n_machines);
    int temp_f1 = e_function((sum_M_smallest_est + sumOfMinPij) / n_machines);
    
    bestBound_maxWorkload = temp_f2 >= min_sum_shortest_proc_times ? temp_f2 : min_sum_shortest_proc_times;
    bestBound_makespan = max_eet_of_jobs >= temp_f1 ? max_eet_of_jobs : temp_f1;
    
    goodSolutionWithMaxWorkload(getNumberOfObjectives(), getNumberOfVariables());
    buildSolutionWithGoodMaxWorkload(goodSolutionWithMaxWorkload);
}

ProblemFJSSP& ProblemFJSSP::operator=(const ProblemFJSSP &toCopy) {
    
    if (this == &toCopy)
        return *this;
    
    n_jobs = toCopy.getNumberOfJobs();
    n_operations = toCopy.getNumberOfOperations();
    n_machines = toCopy.getNumberOfMachines();
    totalVariables = toCopy.getNumberOfVariables();
    totalObjectives = toCopy.getNumberOfObjectives();
    totalConstraints = toCopy.getNumberOfConstraints();
    
    sum_M_smallest_est = toCopy.getSumOf_M_smallestEST();
    avg_op_per_machine = toCopy.getAvgOperationPerMachine();
    min_sum_shortest_proc_times = toCopy.getMinSumShortestProcTime();
    
    bestBound_makespan = toCopy.getBestBoundMakespan();
    bestBound_maxWorkload = toCopy.getBestBoundMaxWorkload();
    
    if (processingTime != nullptr) {
        int job = 0, operation = 0;
        for (job = 0; job < n_jobs; ++job) {
            delete[] jobMachineToMap[job];
            delete[] jobOperationHasNumber[job];
        }
        
        for (job = 0; job < n_jobs * n_machines; ++job)
            delete[] mapToJobMachine[job];
        
        for (operation = 0; operation < n_operations; ++operation)
            delete[] processingTime[operation];
        
        delete[] jobMachineToMap;
        delete[] mapToJobMachine;
        delete[] processingTime;
        delete[] jobOperationHasNumber;
        delete[] n_operations_in_job;
        delete[] releaseTime;
        delete[] operationIsFromJob;
        delete[] assignationMinPij;
        delete[] minWorkload;
        delete[] assignationBestWorkload;
        delete[] assignationBestMakespan;
        delete[] bestWorkloads;
        
        delete[] upperBound;
        delete[] lowerBound;
        
        delete[] earliest_starting_time;
        delete[] earliest_ending_time;
        delete[] eet_of_job;
        delete[] sum_shortest_proc_times;
    }
    
    lowerBound = new int[totalVariables];
    upperBound = new int[totalVariables];
    
    std::strcpy(name, toCopy.name);
    
    goodSolutionWithMaxWorkload = toCopy.goodSolutionWithMaxWorkload;
    
    sumOfMinPij = toCopy.getSumOfMinPij();
    bestWorkloadFound = toCopy.getBestWorkloadFound();
    bestMakespanFound = toCopy.getBestMakespanFound();
    
    jobMachineToMap = new int *[n_jobs];
    mapToJobMachine = new int *[n_jobs * n_machines];
    
    n_operations_in_job = new int[n_jobs];
    releaseTime = new int[n_jobs];
    operationIsFromJob = new int[n_operations];
    assignationMinPij = new int[n_operations];
    assignationBestWorkload = new int[n_operations];
    assignationBestMakespan = new int[n_operations];
    bestWorkloads = new int[n_machines];
    minWorkload = new int[n_machines];
    
    earliest_starting_time = new int[n_operations];/** Length equals to number of operations. **/
    earliest_ending_time = new int[n_operations]; /** Length equals to number of operations. **/
    eet_of_job = new int[n_jobs]; /** Length equals to number of jobs. **/
    sum_shortest_proc_times = new int[n_machines]; /** D^{k}_{Ñ}. Length equals to number of machines **/
    
    int job = 0, map = 0, machine = 0, operation = 0, operationCounter = 0;
    
    for (job = 0; job < n_jobs; ++job) {
        n_operations_in_job[job] = toCopy.getNumberOfOperationsInJob(job);
        releaseTime[job] = toCopy.getReleaseTimeOfJob(job);
        eet_of_job[job] = toCopy.getEarliestEndingJobTime(job);
        
        jobMachineToMap[job] = new int[n_machines];
        
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
        sum_shortest_proc_times[machine] = toCopy.getSumShortestProcTimeInMachine(machine);
    }
    
    jobOperationHasNumber = new int *[n_jobs];
    processingTime = new int *[n_operations];
    
    for (operation = 0; operation < n_operations; ++operation) {
        lowerBound[operation] = toCopy.getLowerBound(operation);
        upperBound[operation] = toCopy.getUpperBound(operation);
        
        earliest_starting_time[operation] = toCopy.getEarliestStartingTime(operation);
        earliest_ending_time[operation] = toCopy.getEarliestEndingTime(operation);
        
        processingTime[operation] = new int[n_machines];
        for (machine = 0; machine < n_machines; ++machine)
            processingTime[operation][machine] = toCopy.getProccessingTime(operation, machine);
        
        assignationMinPij[operation] = toCopy.getAssignationMinPij(operation);
        assignationBestWorkload[operation] = toCopy.getAssignationBestWorkload(operation);
        assignationBestMakespan[operation] = toCopy.getAssignationBestMakespan(operation);
        
    }
    
    for (job = 0; job < n_jobs; ++job) {
        jobOperationHasNumber[job] = new int[n_operations_in_job[job]];
        for (operation = 0; operation < n_operations_in_job[job]; ++operation) {
            jobOperationHasNumber[job][operation] = toCopy.getOperationInJobIsNumber(job, operation);
            operationIsFromJob[operationCounter] = toCopy.getOperationIsFromJob(operationCounter);
            operationCounter++;
        }
    }
    
    return *this;
}

ProblemFJSSP::~ProblemFJSSP() {
    int job = 0, operation = 0;
    for (job = 0; job < n_jobs; ++job) {
        delete[] jobOperationHasNumber[job];
        delete[] jobMachineToMap[job];
    }
    
    for (job = 0; job < n_jobs * n_machines; ++job)
        delete[] mapToJobMachine[job];
    
    for (operation = 0; operation < n_operations; ++operation)
        delete[] processingTime[operation];
    
    delete[] jobMachineToMap;
    delete[] mapToJobMachine;
    delete[] processingTime;
    delete[] n_operations_in_job;
    delete[] releaseTime;
    delete[] jobOperationHasNumber;
    delete[] operationIsFromJob;
    delete[] assignationMinPij;
    delete[] minWorkload;
    delete[] assignationBestWorkload;
    delete[] assignationBestMakespan;
    delete[] bestWorkloads;
    
    delete[] upperBound;
    delete[] lowerBound;
    
    delete[] earliest_starting_time;
    delete[] earliest_ending_time;
    delete[] eet_of_job;
    delete[] sum_shortest_proc_times;
    
    //delete [] name;
}

/**
 *
 * Evaluates a given solution.
 *
 */
double ProblemFJSSP::evaluate(Solution & solution) {
    evaluatePartial(solution, getFinalLevel());
    return 0.0;
}

double ProblemFJSSP::evaluatePartial(Solution & solution, int levelEvaluation) {
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
double ProblemFJSSP::evaluatePartialTest4(Solution & solution, int levelEvaluation) {
    
    int makespan = 0;
    int maxWorkload = 0;
    int totalWorkload = 0;
    
    int operationInPosition = 0;
    int operation = 0;
    int job = 0;
    int machine = 0;
    int numberOp = 0;
    
    int minPij = sumOfMinPij;
    int bestWL[n_machines];
    
    int operationOfJob[n_jobs];
    int startingTime[n_operations];
    int endingTime[n_operations];
    int timeInMachine[n_machines];
    int workload[n_machines];
    
    for (operation = 0; operation < n_operations; ++operation) {
        startingTime[operation] = 0;
        endingTime[operation] = 0;
    }
    
    for (job = 0; job < n_jobs; ++job)
        operationOfJob[job] = 0;
    
    for (machine = 0; machine < n_machines; ++machine) {
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
        
        bestWL[getAssignationBestWorkload(numberOp)] -= getProccessingTime(numberOp, getAssignationBestWorkload(numberOp));
        
        if (operationOfJob[job] == 0) { /** If it is the first operation of the job.**/
            if (timeInMachine[machine] >= getReleaseTimeOfJob(job)) {
                
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += proccesingTime;
                endingTime[numberOp] = timeInMachine[machine];
                
            } else { /** If the job has to wait for the release time.**/
                
                startingTime[numberOp] = getReleaseTimeOfJob(job);
                timeInMachine[machine] = getReleaseTimeOfJob(job) + proccesingTime;
                endingTime[numberOp] = timeInMachine[machine];
                
            }
        } else {
            if (endingTime[numberOp - 1] > timeInMachine[machine]) { /** The operation starts inmediatly after their precedent operation.**/
                
                startingTime[numberOp] = endingTime[numberOp - 1];
                timeInMachine[machine] = endingTime[numberOp - 1] + proccesingTime;
                endingTime[numberOp] = timeInMachine[machine];
                
            } else { /**The operation starts when the machine is avaliable.**/
                
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += proccesingTime;
                endingTime[numberOp] = timeInMachine[machine];
                
            }
        }
        
        operationOfJob[job]++; /** This counts the number of operations from an allocated job. **/
        
        if (timeInMachine[machine] > makespan)
            makespan = timeInMachine[machine];
        
        if (workload[machine] > maxWorkload)
            maxWorkload = workload[machine];
        
    }
    
    /*for (machine = 0; machine < n_machines; ++machine)
     if(workload[machine] + bestWL[machine] > maxWorkload)
     maxWorkload = workload[machine] + bestWL[machine];
     */
    solution.setObjective(0, makespan);
    solution.setObjective(1, maxWorkload);
    //    solution.setObjective(1, totalWorkload + minPij);
    
    return 0.0;
}

void ProblemFJSSP::evaluateDynamic(Solution &solution, FJSSPdata &data, int level) {
    
    int makespan = 0;
    int max_workload = 0;
    int map = solution.getVariable(level);
    int job = getDecodeMap(map, 0);
    int machine = getDecodeMap(map, 1);
    int numberOp = getOperationInJobIsNumber(job, data.getNumberOfOperationsAllocatedFromJob(job));
    
    /** With the operation number and the machine we can continue. **/
    int proccessingTime = getProccessingTime(numberOp, machine);
    data.decreaseToltalWorkload(getProccessingTime(numberOp, getAssignationMinPij(numberOp)));
    
    data.decreaseTempWorkloadIn(getAssignationBestWorkload(numberOp), getProccessingTime(numberOp, getAssignationBestWorkload(numberOp)));
    data.increaseTempWorkloadIn(machine, proccessingTime);
    data.setOperationAllocation(job, numberOp, machine, proccessingTime);
    
    if (data.getLastOperationAllocatedInJob(job) == 0) { /** If it is the first operation of the job.**/
        if (data.getTimeOnMachine(machine) >= getReleaseTimeOfJob(job)) {
            
            data.setStartingTime(numberOp, data.getTimeOnMachine(machine));
            data.increaseTimeOnMachine(machine, proccessingTime);
            data.setEndingTime(numberOp, data.getTimeOnMachine(machine));
            
        } else { /** If the job has to wait for the release time.**/
            
            data.setStartingTime(numberOp, getReleaseTimeOfJob(job));
            data.setTimeOnMachine(machine, getReleaseTimeOfJob(job) + proccessingTime);
            data.setEndingTime(numberOp, data.getTimeOnMachine(machine));
        }
    } else {
        if (data.getEndingTime(numberOp - 1) > data.getTimeOnMachine(machine)) {
            
            data.setStartingTime(numberOp, data.getEndingTime(numberOp - 1));
            data.setTimeOnMachine(machine, data.getEndingTime(numberOp - 1) + proccessingTime);
            data.setEndingTime(numberOp, data.getTimeOnMachine(machine));
            
        } else { /**The operation starts when the machine is avaliable.**/
            
            data.setStartingTime(numberOp, data.getTimeOnMachine(machine));
            data.increaseTimeOnMachine(machine, proccessingTime);
            data.setEndingTime(numberOp, data.getTimeOnMachine(machine));
        }
    }
    
    for (machine = 0; machine < n_machines; ++machine) {
        
        if (data.getTimeOnMachine(machine) > makespan)
            makespan = data.getTimeOnMachine(machine);
        
        if (data.getWorkloadOnMachine(machine) > max_workload)
            max_workload = data.getWorkloadOnMachine(machine);
        
        //        if(data.getTempBestWorkloadInMachine(machine) > max_workload)
        //            max_workload = data.getTempBestWorkloadInMachine(machine);
    }
    
    data.setMakespan(makespan);
    data.setMaxWorkload(max_workload);
    
    solution.setObjective(0, makespan);
    solution.setObjective(1, max_workload);
    //    solution.setObjective(1, data.getTotalWorkload());
}

void ProblemFJSSP::evaluateRemoveDynamic(Solution & solution, FJSSPdata& data, int level) {
    int makespan = 0;
    int max_workload = 0;
    
    int map = solution.getVariable(level);
    int job = getDecodeMap(map, 0);
    int machine = getDecodeMap(map, 1);
    int numberOp = getOperationInJobIsNumber(job, data.getLastOperationAllocatedInJob(job));
    
    /** With the operation number and the machine we can continue. **/
    data.increaseToltalWorkload(getProccessingTime(numberOp, getAssignationMinPij(numberOp))); /** Using the best total_workload. **/
    
    data.increaseTempWorkloadIn(getAssignationBestWorkload(numberOp), getProccessingTime(numberOp, getAssignationBestWorkload(numberOp)));
    data.decreaseTempWorkloadIn(machine, getProccessingTime(numberOp, machine));
    
    data.deallocateOperation(job, numberOp);
    
    for (machine = 0; machine < n_machines; ++machine) {
        
        if (data.getTimeOnMachine(machine) > makespan)
            makespan = data.getTimeOnMachine(machine);
        
        if (data.getWorkloadOnMachine(machine) > max_workload)
            max_workload = data.getWorkloadOnMachine(machine);
        
        //        if(data.getTempBestWorkloadInMachine(machine) > max_workload)
        //            max_workload = data.getTempBestWorkloadInMachine(machine);
    }
    
    data.setMakespan(makespan);
    data.setMaxWorkload(max_workload);
    
    solution.setVariable(level, -1);
    solution.setObjective(0, makespan);
    solution.setObjective(1, max_workload);
    //    solution.setObjective(1, data.getTotalWorkload());
}

double ProblemFJSSP::evaluateLastLevel(Solution * solution) {
    return 0.0;
}
double ProblemFJSSP::removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel) {
    return 0.0;
}
double ProblemFJSSP::removeLastLevelEvaluation(Solution * solution, int newLevel) {
    return 0.0;
}

void ProblemFJSSP::createDefaultSolution(Solution & solution) {
    
    int job = 0;
    int operation = 0;
    int countOperations = 0;
    int map = 0;
    int machine = 0;
    
    FJSSPdata fjsspd(n_jobs, n_operations, n_machines);
    
    for (job = 0; job < n_jobs; ++job)
        for (operation = 0; operation < n_operations_in_job[job]; ++operation) {
            
            while (processingTime[countOperations][machine] == INF_PROC_TIME)
                if (machine++ == n_machines - 1)
                    machine = 0;
            
            map = jobMachineToMap[job][machine];
            solution.setVariable(countOperations, map);
            
            if (machine++ == n_machines - 1)
                machine = 0;
            
            countOperations++;
        }
    
    evaluate(solution);
}

void ProblemFJSSP::updateBestMaxWorkloadSolution(FJSSPdata& data) {
    bestWorkloadFound = data.getMaxWorkload();
    
    if (bestWorkloadFound < bestBound_maxWorkload)
        bestBound_maxWorkload = bestWorkloadFound;
    
    for (int n_op = 0; n_op < n_operations; ++n_op) {
        assignationBestWorkload[n_op] = data.getOperationAllocation(n_op);
        goodSolutionWithMaxWorkload.setVariable(n_op, assignationBestWorkload[n_op]);
    }
    for (int m_mach = 0; m_mach < n_machines; ++m_mach)
        data.setBestWorkloadInMachine(m_mach, data.getWorkloadOnMachine(m_mach));
}

void ProblemFJSSP::updateBestMakespanSolution(FJSSPdata& data) {
    
    bestMakespanFound = data.getMakespan();
    
    if (bestMakespanFound < bestBound_makespan)
        bestBound_makespan = bestMakespanFound;
    
    for (int n_op = 0; n_op < n_operations; ++n_op)
        assignationBestMakespan[n_op] = data.getOperationAllocation(n_op);
}

void ProblemFJSSP::updateBestMakespanSolutionWith(const Solution& solution) {
    bestMakespanFound = solution.getObjective(0);
    
    if (bestMakespanFound < bestBound_makespan)
        bestBound_makespan = bestMakespanFound;
    
    for (int n_op = 0; n_op < n_operations; ++n_op)
        assignationBestMakespan[n_op] = getDecodeMap(solution.getVariable(n_op), 1);
}

void ProblemFJSSP::updateBestMaxWorkloadSolutionWith(const Solution& solution) {
    bestWorkloadFound = solution.getObjective(1);
    
    if (bestWorkloadFound < bestBound_maxWorkload)
        bestBound_maxWorkload = bestWorkloadFound;
    
    for (int n_op = 0; n_op < n_operations; ++n_op) {
        assignationBestWorkload[n_op] = getDecodeMap(solution.getVariable(n_op), 1);
        goodSolutionWithMaxWorkload.setVariable(n_op, assignationBestWorkload[n_op]);
    }
}

void ProblemFJSSP::getSolutionWithLowerBoundInObj(int nObj, Solution& solution) {
    
    if (nObj == 0)
        createDefaultSolution(solution);
    else if (nObj == 1)
        solution = goodSolutionWithMaxWorkload;
    else if (nObj == 2) {
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
void ProblemFJSSP::buildSolutionWithGoodMaxWorkload(Solution & solution) {
    buildSolutionWithGoodMaxWorkloadv2(solution);
}

void ProblemFJSSP::buildSolutionWithGoodMaxWorkloadv2(Solution & solution) {
    
    int procTiOp = 0;
    int maxWorkloadIsReduced = 1;
    
    int maxWorkload = 0;
    int minWorkload = INT_MAX;
    int maxWorkloadObj = 0;
    int totalWorkload = 0;
    
    int operationOfJob[n_jobs];
    int workload[n_machines];
    int maxWorkloadedMachine = 0;
    
    for (int nMachine = 0; nMachine < n_machines; ++nMachine)
        workload[nMachine] = 0;
    
    /** Assign the operations to machines which generates the min TotalWorkload and computes the machines workloads. **/
    int counterOperations = 0;
    int machine = 0;
    for (int nJob = 0; nJob < getNumberOfJobs(); ++nJob) {
        operationOfJob[nJob] = 0;
        for (int nOperation = 0; nOperation < n_operations_in_job[nJob]; ++nOperation) {
            
            machine = assignationMinPij[counterOperations];
            procTiOp = processingTime[counterOperations][machine];
            assignationBestWorkload[counterOperations] = machine;
            
            workload[machine] += procTiOp;
            totalWorkload += procTiOp;
            
            solution.setVariable(counterOperations, jobMachineToMap[nJob][machine]);
            counterOperations++;
            
            if (workload[machine] > maxWorkload) {
                maxWorkload = workload[machine];
                maxWorkloadedMachine = machine;
                maxWorkloadObj = workload[machine];
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
        
        for (int nOperation = 0; nOperation < n_operations; ++nOperation)
            if (mapToJobMachine[solution.getVariable(nOperation)][1] == maxWorkloadedMachine)
                for (int nMachine = 0; nMachine < getNumberOfMachines(); ++nMachine)
                    if (nMachine != maxWorkloadedMachine
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
        for (int nMachine = 0; nMachine < getNumberOfMachines(); ++nMachine)
            if (workload[nMachine] > maxWorkload) {
                maxWorkload = workload[nMachine];
                maxWorkloadedMachine = nMachine;
            }
        
        solution.setObjective(1, workload[maxWorkloadedMachine]);
        
        if (maxWorkload < maxWorkloadObj)
            maxWorkloadObj = maxWorkload;
        else {
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
    bestWorkloadFound = 0;
    for (int nMachine = 0; nMachine < getNumberOfMachines(); ++nMachine) {
        bestWorkloads[nMachine] = workload[nMachine];
        if (bestWorkloads[nMachine] > bestWorkloadFound)
            bestWorkloadFound = bestWorkloads[nMachine];
    }
    
}

/** For all the variables the lower bound is 0. **/
int ProblemFJSSP::getLowerBound(int indexVar) const {
    return 0;
}

/**
 *
 * The Range of variables is the number of maps.
 *
 **/
int ProblemFJSSP::getUpperBound(int indexVar) const {
    return (n_jobs * n_machines) - 1;
}

int ProblemFJSSP::getBestBoundMaxWorkload() const {
    return bestBound_maxWorkload;
}

int ProblemFJSSP::getBestBoundMakespan() const {
    return bestBound_makespan;
}

int ProblemFJSSP::getLowerBoundInObj(int nObj) const {
    
    switch (nObj) {
        case 0:
            return bestBound_makespan;
            break;
        case 1:
            return bestBound_maxWorkload;
            break;
        case 2:
            return sumOfMinPij;
            break;
    }
    return -1;
}

void ProblemFJSSP::loadInstance(char filePath[2][255], char file_extension[10]) {
    if (strcmp(file_extension, "txt") == 0) {
        loadInstanceTXT(filePath, file_extension);
    } else if (strcmp(file_extension, "fjs") == 0) {
        loadInstanceFJS(filePath, file_extension);
    }
}

void ProblemFJSSP::loadInstanceFJS(char filePath[2][255], char file_extension[10]) {
    std::ifstream infile(filePath[0]);
    if (infile.is_open()) {
        int job = 0, operation = 0;
        
        if (processingTime != nullptr) {
            for (operation = 0; operation < n_operations; ++operation)
                delete[] processingTime[operation];
            delete[] processingTime;
        }
        if (jobOperationHasNumber != nullptr) {
            for (job = 0; job < n_jobs; ++job)
                delete[] jobOperationHasNumber[job];
            delete[] jobOperationHasNumber;
        }
        if (jobMachineToMap != nullptr) {
            for (job = 0; job < n_jobs; ++job)
                delete[] jobMachineToMap[job];
            delete[] jobMachineToMap;
        }
        if (mapToJobMachine != nullptr) {
            for (job = 0; job < n_jobs * n_machines; ++job)
                delete[] mapToJobMachine[job];
            delete[] mapToJobMachine;
        }
        if (n_operations_in_job != nullptr)
            delete[] n_operations_in_job;
        if (releaseTime != nullptr)
            delete[] releaseTime;
        if (operationIsFromJob != nullptr)
            delete[] operationIsFromJob;
        if (assignationMinPij != nullptr)
            delete[] assignationMinPij;
        if (minWorkload != nullptr)
            delete[] minWorkload;
        if (assignationBestWorkload != nullptr)
            delete[] assignationBestWorkload;
        if (assignationBestMakespan != nullptr)
            delete[] assignationBestMakespan;
        if (bestWorkloads != nullptr)
            delete[] bestWorkloads;
        if (earliest_starting_time != nullptr)
            delete[] earliest_starting_time;
        if (earliest_ending_time != nullptr)
            delete[] earliest_ending_time;
        if (eet_of_job != nullptr)
            delete[] eet_of_job;
        if (sum_shortest_proc_times != nullptr)
            delete[] sum_shortest_proc_times;
        
        sumOfMinPij = 0;
        bestWorkloadFound = INT_MAX;
        n_operations = 0;
        std::string line;
        std::vector<std::string> elemens;
        std::getline(infile, line);
        elemens = split(line, ' ');
        n_jobs = std::stoi(elemens.at(0));
        n_machines = std::stoi(elemens.at(1));
        
        char extension[4];
        int instance_with_release_time = 1; /** This is used because the Kacem's instances have release times and the other sets dont. **/
        
        /** Get name of the instance. **/
        std::vector<std::string> name_file_ext;
        
        elemens = split(filePath[0], '/');
        
        unsigned long int sizeOfElems = elemens.size();
        name_file_ext = split(elemens[sizeOfElems - 1], '.');
        printf("[Master] Name: %s extension: %s\n", name_file_ext[0].c_str(), name_file_ext[1].c_str());
        std::strcpy(extension, name_file_ext[1].c_str());
        
        /** If the instance is Kacem then it has the release time in the first value of each job. TODO: Re-think if its a good idea to update all the instances to include release time at 0. **/
        const int kacem_legnth = 5;
        char kacem[kacem_legnth] {'K', 'a', 'c', 'e', 'm'};
        for (int character = 0; character < kacem_legnth && instance_with_release_time == 1; ++character)
            instance_with_release_time = (kacem[character] == name_file_ext[0][character]) ? 1 : 0;
        
        if (n_jobs > 0 && n_machines > 0) {
            
            n_operations_in_job = new int[n_jobs];
            releaseTime = new int[n_jobs];
            
            std::string * job_line = new std::string[n_jobs];
            for (int n_job = 0; n_job < n_jobs; ++n_job) {
                std::getline(infile, job_line[n_job]);
                split(job_line[n_job], ' ', elemens); /** Stores the text corresponding to each job processing times. **/
                n_operations_in_job[n_job] = (instance_with_release_time == 1) ? std::stoi(elemens.at(1)): std::stoi(elemens.at(0));
                n_operations += n_operations_in_job[n_job];
                releaseTime[n_job] = ((instance_with_release_time == 1) ? std::stoi(elemens.at(0)) : 0);
            }
            
            operationIsFromJob = new int[n_operations];
            assignationMinPij = new int[n_operations];
            minWorkload = new int[n_machines];
            assignationBestWorkload = new int[n_operations];
            assignationBestMakespan = new int[n_operations];
            bestWorkloads = new int[n_machines];
            jobOperationHasNumber = new int *[n_jobs];
            processingTime = new int *[n_operations];
            jobMachineToMap = new int *[n_jobs];
            mapToJobMachine = new int *[n_jobs * n_machines];
            
            earliest_starting_time = new int[n_operations];/** Length equals to number of operations. **/
            earliest_ending_time = new int[n_operations]; /** Length equals to number of operations. **/
            eet_of_job = new int[n_jobs]; /** Length equals to number of jobs. **/
            sum_shortest_proc_times = new int[n_machines]; /** D^{k}_{Ñ}. Length equals to number of machines.**/
            
            avg_op_per_machine = e_function(n_operations / n_machines); /** Ñ parameter (N tilde).  **/
            min_sum_shortest_proc_times = 0; /** D_{{k_0}, Ñ}. **/
            max_eet_of_jobs = 0;
            sum_M_smallest_est = 0;
            
            int minPij = INT_MAX;
            int minMachine = 0;
            int map = 0;
            
            for (int job = 0; job < n_jobs; ++job) {
                jobMachineToMap[job] = new int[n_machines];
                for (int machine = 0; machine < n_machines; ++machine) {
                    mapToJobMachine[map] = new int[2];
                    mapToJobMachine[map][0] = job;
                    mapToJobMachine[map][1] = machine;
                    jobMachineToMap[job][machine] = map;
                    map++;
                }
            }
            int sorted_processing[n_machines][n_operations]; //new int * [n_machines];/** Stores the processing times from min to max for each machine. **/
            int sorted_est[n_operations]; // new int [n_operations];
            
            for (int machine = 0; machine < n_machines; ++machine)
                minWorkload[machine] = 0;
            int op_counter = 0;
            for (int n_job = 0; n_job < n_jobs; ++n_job) {
                int token = (instance_with_release_time == 1)?2:1;
                split(job_line[n_job], ' ', elemens);
                for (int n_op_in_job = 0; n_op_in_job < n_operations_in_job[n_job]; ++n_op_in_job) {
                    int op_can_be_proc_in_n_mach = std::stoi(elemens.at(token++));
                    processingTime[op_counter] = new int[n_machines];
                    
                    for (int n_machine = 0; n_machine < n_machines; ++n_machine)
                        processingTime[op_counter][n_machine] = INF_PROC_TIME;
                    
                    for (int n_mach = 0; n_mach < op_can_be_proc_in_n_mach; ++n_mach) {
                        int machine = std::stoi(elemens.at(token++));
                        int proc_ti = std::stoi(elemens.at(token++));
                        processingTime[op_counter][machine - 1] = proc_ti;
                    }
                    op_counter++;
                }
            }
            
            for (int operation = 0; operation < n_operations; ++operation) {
                minPij = INT_MAX;
                minMachine = 0;
                for (int machine = 0; machine < n_machines; ++machine) {
                    sorted_processing[machine][operation] = processingTime[operation][machine];
                    if (processingTime[operation][machine] < minPij) {
                        minPij = processingTime[operation][machine];
                        minMachine = machine;
                    }
                }
                sumOfMinPij += minPij;
                minWorkload[minMachine] += processingTime[operation][minMachine];
                assignationMinPij[operation] = minMachine;
            }
            
            infile.close();
            earliest_starting_time[0] = releaseTime[0];
            int op_allocated = 0;
            int next_op = 0;
            for (int job = 0; job < n_jobs; ++job) {
                earliest_starting_time[op_allocated] = releaseTime[job];
                earliest_ending_time[op_allocated] = earliest_starting_time[op_allocated] + processingTime[op_allocated][assignationMinPij[op_allocated]];
                sorted_est[op_allocated] = earliest_starting_time[op_allocated];
                for (int operation = 1; operation < n_operations_in_job[job]; ++operation) {
                    next_op = op_allocated + operation;
                    earliest_starting_time[next_op] = earliest_ending_time[next_op - 1];
                    earliest_ending_time[next_op] = earliest_starting_time[next_op] + processingTime[next_op][assignationMinPij[next_op]];
                    sorted_est[next_op] = earliest_starting_time[next_op];
                }
                op_allocated += n_operations_in_job[job];
                eet_of_job[job] = earliest_ending_time[op_allocated - 1];
                
                if (eet_of_job[job] > max_eet_of_jobs)
                    max_eet_of_jobs = eet_of_job[job];
            }
            
            std::sort(sorted_est, sorted_est + n_operations);
            for (int machine = 0; machine < n_machines; ++machine)
                std::sort(sorted_processing[machine], sorted_processing[machine] + n_operations);
            
            sum_M_smallest_est = 0;
            min_sum_shortest_proc_times = INT_MAX;
            for (int machine = 0; machine < n_machines; ++machine) {
                
                sum_M_smallest_est += sorted_est[machine];
                sum_shortest_proc_times[machine] = 0;
                
                for (int n = 0; n < avg_op_per_machine && sorted_processing[machine][n] != INF_PROC_TIME; ++n) /** Some times the machines can compute less jobs than the average. **/
                    sum_shortest_proc_times[machine] += sorted_processing[machine][n];
                
                if (sum_shortest_proc_times[machine] < min_sum_shortest_proc_times)
                    min_sum_shortest_proc_times = sum_shortest_proc_times[machine];
            }
            
            totalVariables = n_operations;
            
            int operationCounter = 0;
            for (int job = 0; job < n_jobs; ++job) {
                jobOperationHasNumber[job] = new int[n_operations_in_job[job]];
                for (int operation = 0; operation < n_operations_in_job[job]; ++operation) {
                    jobOperationHasNumber[job][operation] = operationCounter;
                    operationIsFromJob[operationCounter++] = job;
                }
            }
            
            int temp_f2 = e_function(sumOfMinPij / n_machines);
            int temp_f1 = e_function((sum_M_smallest_est + sumOfMinPij) / n_machines);
            
            bestBound_maxWorkload = temp_f2 >= min_sum_shortest_proc_times ? temp_f2 : min_sum_shortest_proc_times;
            bestBound_makespan = max_eet_of_jobs >= temp_f1 ? max_eet_of_jobs : temp_f1;
            
            goodSolutionWithMaxWorkload(getNumberOfObjectives(), getNumberOfVariables());
            buildSolutionWithGoodMaxWorkload(goodSolutionWithMaxWorkload);
            
            delete[] job_line;
        }
    } else {
        printf("File not found\n");
        exit(1);
    }
}

void ProblemFJSSP::loadInstanceTXT(char filePath[2][255], char file_extension[10]) {
    std::ifstream infile(filePath[0]);
    if (infile.is_open()) {
        
        if (processingTime != nullptr) {
            int job = 0, operation = 0;
            for (job = 0; job < n_jobs; ++job) {
                delete[] jobOperationHasNumber[job];
                delete[] jobMachineToMap[job];
            }
            
            for (job = 0; job < n_jobs * n_machines; ++job)
                delete[] mapToJobMachine[job];
            
            for (operation = 0; operation < n_operations; ++operation)
                delete[] processingTime[operation];
            
            delete[] jobMachineToMap;
            delete[] mapToJobMachine;
            delete[] processingTime;
            delete[] n_operations_in_job;
            delete[] releaseTime;
            delete[] jobOperationHasNumber;
            delete[] operationIsFromJob;
            delete[] assignationMinPij;
            delete[] minWorkload;
            delete[] assignationBestWorkload;
            delete[] assignationBestMakespan;
            delete[] bestWorkloads;
            
            delete[] earliest_starting_time;
            delete[] earliest_ending_time;
            delete[] eet_of_job;
            delete[] sum_shortest_proc_times;
        }
        
        std::string line;
        std::vector<std::string> elemens;
        
        std::getline(infile, line);
        std::getline(infile, line);
        elemens = split(line, ' ');
        n_jobs = std::stoi(elemens.at(0));
        n_machines = std::stoi(elemens.at(1));
        n_operations_in_job = new int[n_jobs];
        releaseTime = new int[n_jobs];
        
        std::getline(infile, line);
        std::getline(infile, line);
        elemens = split(line, ' ');
        n_operations = 0;
        if (n_jobs > 0 && n_machines > 0) {
            for (int job = 0; job < n_jobs; ++job) {
                n_operations_in_job[job] = std::stoi(elemens.at(job));
                n_operations += n_operations_in_job[job];
            }
            
            std::getline(infile, line);
            std::getline(infile, line);
            elemens = split(line, ' ');
            for (int job = 0; job < n_jobs; ++job)
                releaseTime[job] = std::stoi(elemens.at(job));
            
            operationIsFromJob = new int[n_operations];
            sumOfMinPij = 0;
            bestWorkloadFound = INT_MAX;
            std::getline(infile, line);
            
            assignationMinPij = new int[n_operations];
            minWorkload = new int[n_machines];
            assignationBestWorkload = new int[n_operations];
            assignationBestMakespan = new int[n_operations];
            
            bestWorkloads = new int[n_machines];
            jobOperationHasNumber = new int *[n_jobs];
            processingTime = new int *[n_operations];
            jobMachineToMap = new int *[n_jobs];
            mapToJobMachine = new int *[n_jobs * n_machines];
            
            earliest_starting_time = new int[n_operations];/** Length equals to number of operations. **/
            earliest_ending_time = new int[n_operations]; /** Length equals to number of operations. **/
            eet_of_job = new int[n_jobs]; /** Length equals to number of jobs. **/
            sum_shortest_proc_times = new int[n_machines]; /** D^{k}_{Ñ}. Length equals to number of machines.**/
            
            avg_op_per_machine = e_function(n_operations / n_machines); /** Ñ parameter (N tilde).  **/
            min_sum_shortest_proc_times = 0; /** D_{{k_0}, Ñ}. **/
            max_eet_of_jobs = 0;
            sum_M_smallest_est = 0;
            
            int minPij = INT_MAX;
            int minMachine = 0;
            int map = 0;
            
            for (int job = 0; job < n_jobs; ++job) {
                jobMachineToMap[job] = new int[n_machines];
                for (int machine = 0; machine < n_machines; ++machine) {
                    mapToJobMachine[map] = new int[2];
                    mapToJobMachine[map][0] = job;
                    mapToJobMachine[map][1] = machine;
                    jobMachineToMap[job][machine] = map;
                    map++;
                }
            }
            
            int sorted_processing[n_machines][n_operations]; //new int * [n_machines];/** Stores the processing times from min to max for each machine. **/
            int sorted_est[n_operations]; // new int [n_operations];
            
            for (int machine = 0; machine < n_machines; ++machine)
                minWorkload[machine] = 0;
            
            for (int operation = 0; operation < n_operations; ++operation) {
                processingTime[operation] = new int[n_machines];
                std::getline(infile, line);
                elemens = split(line, ' ');
                minPij = INT_MAX;
                minMachine = 0;
                for (int machine = 0; machine < n_machines; ++machine) {
                    processingTime[operation][machine] = std::stoi(elemens.at(machine));
                    sorted_processing[machine][operation] = processingTime[operation][machine];
                    if (processingTime[operation][machine] < minPij) {
                        minPij = processingTime[operation][machine];
                        minMachine = machine;
                    }
                }
                sumOfMinPij += minPij;
                minWorkload[minMachine] += processingTime[operation][minMachine];
                assignationMinPij[operation] = minMachine;
            }
            
            infile.close();
            earliest_starting_time[0] = releaseTime[0];
            int op_allocated = 0;
            int next_op = 0;
            /** Computes the earlist starting time and the earlist ending time for each job. **/
            for (int job = 0; job < n_jobs; ++job) {
                earliest_starting_time[op_allocated] = releaseTime[job];
                earliest_ending_time[op_allocated] = earliest_starting_time[op_allocated] + processingTime[op_allocated][assignationMinPij[op_allocated]];
                sorted_est[op_allocated] = earliest_starting_time[op_allocated];
                for (int operation = 1; operation < n_operations_in_job[job]; ++operation) {
                    next_op = op_allocated + operation;
                    earliest_starting_time[next_op] = earliest_ending_time[next_op - 1];
                    earliest_ending_time[next_op] = earliest_starting_time[next_op] + processingTime[next_op][assignationMinPij[next_op]];
                    sorted_est[next_op] = earliest_starting_time[next_op];
                }
                op_allocated += n_operations_in_job[job];
                eet_of_job[job] = earliest_ending_time[op_allocated - 1];
                
                if (eet_of_job[job] > max_eet_of_jobs)
                    max_eet_of_jobs = eet_of_job[job];
            }
            
            std::sort(sorted_est, sorted_est + n_operations);
            for (int machine = 0; machine < n_machines; ++machine)
                std::sort(sorted_processing[machine], sorted_processing[machine] + n_operations);
            
            sum_M_smallest_est = 0;
            min_sum_shortest_proc_times = INT_MAX;
            for (int machine = 0; machine < n_machines; ++machine) {
                
                sum_M_smallest_est += sorted_est[machine];
                sum_shortest_proc_times[machine] = 0;
                
                for (int n = 0; n < avg_op_per_machine; ++n)
                    sum_shortest_proc_times[machine] += sorted_processing[machine][n];
                
                if (sum_shortest_proc_times[machine] < min_sum_shortest_proc_times)
                    min_sum_shortest_proc_times = sum_shortest_proc_times[machine];
            }
            
            totalVariables = n_operations;
            
            int operationCounter = 0;
            for (int job = 0; job < n_jobs; ++job) {
                jobOperationHasNumber[job] = new int[n_operations_in_job[job]];
                for (int operation = 0; operation < n_operations_in_job[job]; ++operation) {
                    jobOperationHasNumber[job][operation] = operationCounter;
                    operationIsFromJob[operationCounter++] = job;
                }
            }
            
            int temp_f2 = e_function(sumOfMinPij / n_machines);
            int temp_f1 = e_function((sum_M_smallest_est + sumOfMinPij) / n_machines);
            
            bestBound_maxWorkload = temp_f2 >= min_sum_shortest_proc_times ? temp_f2 : min_sum_shortest_proc_times;
            bestBound_makespan = max_eet_of_jobs >= temp_f1 ? max_eet_of_jobs : temp_f1;
            
            goodSolutionWithMaxWorkload(getNumberOfObjectives(), getNumberOfVariables());
            buildSolutionWithGoodMaxWorkload(goodSolutionWithMaxWorkload);
            
            /*if (goodSolutionWithMaxWorkload.getObjective(1) < bestBound_maxWorkload)
             bestBound_maxWorkload = goodSolutionWithMaxWorkload.getObjective(1);
             */
        }
    } else {
        printf("File not found\n");
        exit(1);
    }
}

ProblemType ProblemFJSSP::getType() const {
    return ProblemType::permutation_with_repetition_and_combination;
}
int ProblemFJSSP::getStartingRow() {
    return 0;
}
int ProblemFJSSP::getFinalLevel() {
    return n_operations - 1;
}
int ProblemFJSSP::getTotalElements() {
    return n_jobs;
}
int * ProblemFJSSP::getElemensToRepeat() {
    return n_operations_in_job;
}

/** If position = 0 returns the job, if position = 1 returns the machine. (Decodes the map in job or machine). **/
int ProblemFJSSP::getDecodeMap(int map, int position) {
    return mapToJobMachine[map][position];
}

/** Returns the map corresponding to the configuration of job and machine. (Codes the job and machine in a map). **/
int ProblemFJSSP::getCodeMap(int job, int machine) {
    return jobMachineToMap[job][machine];
}
int ProblemFJSSP::getTimesValueIsRepeated(int value) {
    return n_operations_in_job[value];
}
int ProblemFJSSP::getNumberOfJobs() const {
    return n_jobs;
}
int ProblemFJSSP::getNumberOfOperations() const {
    return n_operations;
}
int ProblemFJSSP::getNumberOfMachines() const {
    return n_machines;
}
int ProblemFJSSP::getSumOfMinPij() const {
    return sumOfMinPij;
}
int ProblemFJSSP::getBestWorkloadFound() const {
    return bestWorkloadFound;
}
int ProblemFJSSP::getBestMakespanFound() const {
    return bestMakespanFound;
}
int ProblemFJSSP::getAssignationMinPij(int n_operation) const {
    return assignationMinPij[n_operation];
}
int ProblemFJSSP::getAssignationBestWorkload(int n_operation) const {
    return assignationBestWorkload[n_operation];
}
int ProblemFJSSP::getAssignationBestMakespan(int n_operation) const {
    return assignationBestMakespan[n_operation];
}

int ProblemFJSSP::getBestWorkload(int n_machine) const {
    return bestWorkloads[n_machine];
}
int ProblemFJSSP::getMinWorkload(int n_machine) const {
    return minWorkload[n_machine];
}
int ProblemFJSSP::getMapOfJobMachine(int map, int machine_or_job) const {
    return mapToJobMachine[map][machine_or_job];
}
int ProblemFJSSP::getJobMachineToMap(int job, int machine) const {
    return jobMachineToMap[job][machine];
}
int ProblemFJSSP::getOperationInJobIsNumber(int job, int operation) const {
    return jobOperationHasNumber[job][operation];
}
int ProblemFJSSP::getOperationIsFromJob(int n_operation) const {
    return operationIsFromJob[n_operation];
}
int ProblemFJSSP::getProccessingTime(int operation, int machine) const {
    return processingTime[operation][machine];
}
int ProblemFJSSP::getNumberOfOperationsInJob(int job) const {
    return n_operations_in_job[job];
}
int ProblemFJSSP::getReleaseTimeOfJob(int job) const {
    return releaseTime[job];
}
int ProblemFJSSP::e_function(double value) const {
    return floor(value) + 1;
}

int ProblemFJSSP::getEarliestStartingTime(int nOperation) const {
    return earliest_starting_time[nOperation];
}
int ProblemFJSSP::getEarliestEndingTime(int nOperation) const {
    return earliest_ending_time[nOperation];
}
int ProblemFJSSP::getEarliestEndingJobTime(int nJob) const {
    return eet_of_job[nJob];
}
int ProblemFJSSP::getSumShortestProcTimeInMachine(int nMachine) const {
    return sum_shortest_proc_times[nMachine];
}
int ProblemFJSSP::getAvgOperationPerMachine() const {
    return avg_op_per_machine;
}
int ProblemFJSSP::getMinSumShortestProcTime() const {
    return min_sum_shortest_proc_times;
}
int ProblemFJSSP::getMaxEarliestEndingTime() const {
    return max_eet_of_jobs;
}
int ProblemFJSSP::getSumOf_M_smallestEST() const {
    return sum_M_smallest_est;
}

void ProblemFJSSP::setEarliestStartingTime(int nOperation, int nValue) {
    earliest_starting_time[nOperation] = nValue;
}
void ProblemFJSSP::setEarliestEndingTime(int nOperation, int nValue) {
    earliest_ending_time[nOperation] = nValue;
}
void ProblemFJSSP::setEarliestEndingJobTime(int nJob, int nValue) {
    eet_of_job[nJob] = nValue;
}
void ProblemFJSSP::setSumShortestProcTimeInMachine(int nMachine, int nValue) {
    sum_shortest_proc_times[nMachine] = nValue;
}
void ProblemFJSSP::setAvgOperationPerMachine(int nValue) {
    avg_op_per_machine = nValue;
}
void ProblemFJSSP::setMinShortestProcTime(int nValue) {
    min_sum_shortest_proc_times = nValue;
}
void ProblemFJSSP::setMaxEarliestEndingTime(int nValue) {
    max_eet_of_jobs = nValue;
}
void ProblemFJSSP::setSumOf_M_smallestEST(int nValue) {
    sum_M_smallest_est = nValue;
}

void ProblemFJSSP::printInstance() {
}

void ProblemFJSSP::printProblemInfo() const{
    char sep = '-';
    printf("Total jobs: %d\n", n_jobs);
    printf("Total machines: %d\n", n_machines);
    printf("Total operations: %d\n", n_operations);
    
    printf("Operations in each job:\n");
    
    int job = 0, machine = 0, operation = 0;
    for (job = 0; job < n_jobs; ++job)
        printf("%2d ", n_operations_in_job[job]);
    printf("\n");
    
    printf("Release time for each job:\n");
    for (job = 0; job < n_jobs; ++job)
        printf("%2d ", releaseTime[job]);
    printf("\n");
    
    printf("Processing times: \n");
    printf("\t\t\t");
    for (machine = 0; machine < n_machines; ++machine)
        printf("M%-3d", machine);
    printf("| EST EET\n");
    for (operation = 0; operation < n_operations; ++operation) {
        
        printf("[J%-2d] %2c %2d:", operationIsFromJob[operation], 'A' + operation, operation);
        for (machine = 0; machine < n_machines; ++machine)
            if (processingTime[operation][machine] == INF_PROC_TIME)
                printf("%4c", sep);
            else
                printf("%4d", processingTime[operation][machine]);
        printf(" | %4d %4d\n", earliest_starting_time[operation], earliest_ending_time[operation]);
    }
    
    printf("Avg operations per machine (Ñ): %4d\n", avg_op_per_machine);
    printf("Sum of the Ñ shortest processing times (D^{k}_{Ñ}):\n");
    for (machine = 0; machine < n_machines; ++machine)
        printf("%4d ", sum_shortest_proc_times[machine]);
    
    printf("\nEarliest ending time of each job (EET_j):\n");
    for (job = 0; job < n_jobs; ++job)
        printf("%4d ", eet_of_job[job]);
    
    int temp_f2 = e_function(sumOfMinPij / n_machines);
    int temp_f1 = e_function((sum_M_smallest_est + sumOfMinPij) / n_machines);
    
    printf("\nSum of the minimun processing times (sum_i sum_j Gamma_{i,j}): %4d\n", sumOfMinPij);
    printf("Minimum D^{k}_{Ñ}: %4d\n", min_sum_shortest_proc_times);
    printf("Maximum EET from jobs: %4d\n", max_eet_of_jobs);
    printf("Sum of the M smallest EST (R_{M}): %4d\n", sum_M_smallest_est);
    printf("E((R_M + Gamma_{i,j}) / M): %4d\n", temp_f1);
    printf("E(Gamma_{i,j} / M): %4d\n", temp_f2);
    printf("f^'_1 (bound): max(%4d, %4d, ) = %4d\n", max_eet_of_jobs, e_function((sum_M_smallest_est + sumOfMinPij) / n_machines), bestBound_makespan);
    printf("f^'_2 (bound): max(%4d, %4d) =  %4d\n", e_function(sumOfMinPij / n_machines), min_sum_shortest_proc_times, bestBound_maxWorkload);
    printf("f^*_3: %4d\n", sumOfMinPij);
}

void ProblemFJSSP::printSolutionInfo(const Solution &solution) const {
    printSchedule(solution);
}

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
    int operationOfJob[n_jobs];
    int startingTime[n_operations];
    int endingTime[n_operations];
    int timeInMachine[n_machines];
    int workload[n_machines];
    
    char gantt[n_machines][MAX_GANTT_LIMIT];
    int time = 0;
    
    for (operation = 0; operation < n_operations; ++operation) {
        startingTime[operation] = 0;
        endingTime[operation] = 0;
        operation_in_machine[operation] = 0;
    }
    
    for (job = 0; job < n_jobs; ++job)
        operationOfJob[job] = 0;
    
    for (machine = 0; machine < n_machines; ++machine) {
        timeInMachine[machine] = 0;
        workload[machine] = 0;
        /**creates an empty gantt**/
        for (time = 0; time < MAX_GANTT_LIMIT; time++) {
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
            if (timeInMachine[machine] >= releaseTime[job]) {
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            } else { /** If the job has to wait for the release time.**/
                startingTime[numberOp] = releaseTime[job];
                timeInMachine[machine] = releaseTime[job] + processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            }
            
        } else {
            if (endingTime[numberOp - 1] > timeInMachine[machine]) { /**The operation is waiting for their dependency operation.**/
                
                startingTime[numberOp] = endingTime[numberOp - 1];
                timeInMachine[machine] = endingTime[numberOp - 1] + processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
                
            } else { /**The operation starts when the machine is avaliable.**/
                
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += processingTime[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
                
            }
        }
        
        for (time = startingTime[numberOp]; time < endingTime[numberOp]; time++)
            gantt[machine][time] = 'A' + numberOp;
        
        operationOfJob[job]++;
        
        if (timeInMachine[machine] > makespan)
            makespan = timeInMachine[machine];
        
        if (workload[machine] > maxWorkload)
            maxWorkload = workload[machine];
    }
    
    printf("\tOp :  M  ti -  tf\n");
    for (operation = 0; operation < n_operations; ++operation)
        printf("%3c %3d: %2d %3d - %3d \n", 'A' + operation, operation, operation_in_machine[operation], startingTime[operation], endingTime[operation]);
    /*
     for (machine = n_machines - 1;  machine >= 0; --machine) {
     printf("M%d  | ", machine);
     for (time = 0; time <= makespan; ++time)
     printf("%4c", gantt[machine][time]);
     printf("| %3d\n", workload[machine]);
     }
     printf("----");
     for (time = 0; time <= makespan; ++time)
     printf("---");
     printf("--\n");
     
     printf("Time:");
     for (time = 0; time <= makespan; ++time)
     printf("%4d", (time));
     printf("\n");
     */
    printf("makespan: %d\nmaxWorkLoad: %d\ntotalWorkload: %d \n", makespan, maxWorkload, totalWorkload);
    
}

void ProblemFJSSP::printSolution(const Solution & solution) const {
    printPartialSolution(solution, n_operations - 1);
}

void ProblemFJSSP::printPartialSolution(const Solution & solution, int level) const {
    
    int indexVar = 0;
    int withVariables = 1;
    
    for (indexVar = 0; indexVar < getNumberOfObjectives(); indexVar++)
        printf("%4.0f ", solution.getObjective(indexVar));
    
    if (withVariables == 1) {
        
        printf(" | ");
        
        for (indexVar = 0; indexVar <= level; indexVar++)
            printf("%4d ", solution.getVariable(indexVar));
        
        for (indexVar = level + 1; indexVar < n_operations; indexVar++)
            printf("  - ");
        
        printf("|");
    }
}

/** This function return the number of variables which are out of range from varible values.**/
int ProblemFJSSP::validateVariablesOf(const Solution& solution) const {
    int number_of_invalid_variables = 0;
    for (int variable = 0; variable < totalVariables; ++variable)
        if (solution.getVariable(variable) < lowerBound[variable] || solution.getVariable(variable) > upperBound[variable])
            number_of_invalid_variables++;
    return number_of_invalid_variables;
}
