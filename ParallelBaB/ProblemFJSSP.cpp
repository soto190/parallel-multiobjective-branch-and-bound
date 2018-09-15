//
//  ProblemFJSSP.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 14/11/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "ProblemFJSSP.hpp"

ProblemFJSSP::ProblemFJSSP() :
Problem(0, 0),
n_jobs(0),
n_operations(0),
n_machines(0),
sum_of_min_Pij(0),
avg_op_per_machine(0),
best_bound_max_workload(0),
best_bound_makespan(0),
sum_M_smallest_est(0),
min_sum_shortest_proc_times(0),
best_workload_found(0),
best_makespan_found(INT_MAX),
max_eet_of_jobs(0),
f_min(nullptr),
f_max(nullptr),
encode_job_machine_to_map(nullptr),
decode_map_to_job_machine(nullptr),
n_operations_in_job(nullptr),
release_time(nullptr),
operation_belongs_to_job(nullptr),
assignation_min_Pij(nullptr),
assignation_best_max_workload(nullptr),
assignation_best_makespan(nullptr),
best_workloads(nullptr),
min_workloads(nullptr),
job_operation_is_number(nullptr),
processing_time(nullptr),
earliest_starting_time(nullptr), /** Length equals to number of operations. **/
earliest_ending_time(nullptr), /** Length equals to number of operations. **/
eet_of_job(nullptr), /** Length equals to number of jobs. **/
sum_shortest_proc_times(nullptr) { /** D^{k}_{Ñ}. Length equals to number of machines.**/

    machines_aviability.reserve(0);
}

ProblemFJSSP::ProblemFJSSP(int number_of_objectives, int number_of_variables) :
Problem(number_of_objectives, number_of_variables),
n_jobs(0),
n_operations(0),
n_machines(0),
sum_of_min_Pij(0),
avg_op_per_machine(0),
best_bound_max_workload(0),
best_bound_makespan(0),
sum_M_smallest_est(0),
min_sum_shortest_proc_times(0),
best_workload_found(0),
best_makespan_found(INT_MAX),
max_eet_of_jobs(0),
f_min(nullptr),
f_max(nullptr),
encode_job_machine_to_map(nullptr),
decode_map_to_job_machine(nullptr),
n_operations_in_job(nullptr),
release_time(nullptr),
operation_belongs_to_job(nullptr),
assignation_min_Pij(nullptr),
assignation_best_max_workload(nullptr),
assignation_best_makespan(nullptr),
best_workloads(nullptr),
min_workloads(nullptr),
job_operation_is_number(nullptr),
processing_time(nullptr),
earliest_starting_time(nullptr), /** Length equals to number of operations. **/
earliest_ending_time(nullptr), /** Length equals to number of operations. **/
eet_of_job(nullptr), /** Length equals to number of jobs. **/
sum_shortest_proc_times(nullptr) { /** D^{k}_{Ñ}. Length equals to number of machines.**/
    machines_aviability.reserve(number_of_variables);
}

ProblemFJSSP::ProblemFJSSP(const ProblemFJSSP& toCopy) :
Problem(toCopy),
n_jobs(toCopy.getNumberOfJobs()),
n_operations(toCopy.getNumberOfOperations()),
n_machines(toCopy.getNumberOfMachines()),
sum_of_min_Pij(toCopy.getSumOfMinPij()),
best_workload_found(toCopy.getBestWorkloadFound()),
best_makespan_found(toCopy.getBestMakespanFound()),
goodSolutionWithMaxWorkload(toCopy.getSolutionWithGoodMaxWorkload()) {

    if (lower_bound != nullptr)
        delete [] lower_bound;

    if (upper_bound != nullptr)
        delete [] upper_bound;

    lower_bound = new int[n_variables];
    upper_bound = new int[n_variables];
    
    avg_op_per_machine = toCopy.getAvgOperationPerMachine();
    min_sum_shortest_proc_times = toCopy.getMinSumShortestProcTime();
    sum_M_smallest_est = toCopy.getSumOf_M_smallestEST();
    
    best_bound_makespan = toCopy.getBestBoundMakespan();
    best_bound_max_workload = toCopy.getBestBoundMaxWorkload();
    
    std::strcpy(name, toCopy.name);

    f_min = new int[n_objectives];
    f_max = new int[n_objectives];
    for (unsigned int obj = 0; obj < n_objectives; ++obj) {
        f_min[obj] = toCopy.getFmin(obj);
        f_max[obj] = toCopy.getFmax(obj);
    }
    
    encode_job_machine_to_map = new int *[n_jobs];
    decode_map_to_job_machine = new int *[n_jobs * n_machines];
    processing_time = new int *[n_operations];
    job_operation_is_number = new int *[n_jobs];
    
    release_time = new int[n_jobs];
    n_operations_in_job = new int[n_jobs];
    operation_belongs_to_job = new int[n_operations];
    assignation_best_max_workload = new int[n_operations];
    assignation_min_Pij = new int[n_operations];
    assignation_best_makespan = new int[n_operations];
    best_workloads = new int[n_machines];
    min_workloads = new int[n_machines];
    
    earliest_starting_time = new int[n_operations];/** Length equals to number of operations. **/
    earliest_ending_time = new int[n_operations]; /** Length equals to number of operations. **/
    eet_of_job = new int[n_jobs]; /** Length equals to number of jobs. **/
    sum_shortest_proc_times = new int[n_machines]; /** D^{k}_{Ñ}. Length equals to number of machines **/
    max_eet_of_jobs = toCopy.getMaxEarliestEndingTime(); /** TODO: Fix this.**/
    
    int job = 0, map = 0, machine = 0, operation = 0, operationCounter = 0;
    
    for (job = 0; job < n_jobs; ++job) {
        n_operations_in_job[job] = toCopy.getNumberOfOperationsInJob(job);
        release_time[job] = toCopy.getReleaseTimeOfJob(job);
        eet_of_job[job] = toCopy.getEarliestEndingJobTime(job);
        
        encode_job_machine_to_map[job] = new int[toCopy.getNumberOfMachines()];
        
        for (machine = 0; machine < n_machines; ++machine) {
            decode_map_to_job_machine[map] = new int[2];
            decode_map_to_job_machine[map][0] = toCopy.getMapOfJobMachine(map, 0);
            decode_map_to_job_machine[map][1] = toCopy.getMapOfJobMachine(map, 1);
            encode_job_machine_to_map[job][machine] = toCopy.getJobMachineToMap(job, machine);
            map++;
        }
    }
    
    for (machine = 0; machine < n_machines; ++machine) {
        min_workloads[machine] = toCopy.getMinWorkload(machine);
        best_workloads[machine] = toCopy.getBestWorkload(machine);
        sum_shortest_proc_times[machine] = toCopy.getSumShortestProcTimeInMachine(machine);
    }
    
    for (operation = 0; operation < n_operations; ++operation) {
        lower_bound[operation] = toCopy.getLowerBound(operation);
        upper_bound[operation] = toCopy.getUpperBound(operation);
        earliest_starting_time[operation] = toCopy.getEarliestStartingTime(operation);
        earliest_ending_time[operation] = toCopy.getEarliestEndingTime(operation);
        
        processing_time[operation] = new int[toCopy.getNumberOfMachines()];
        for (machine = 0; machine < n_machines; ++machine)
            processing_time[operation][machine] = toCopy.getProccessingTime(operation, machine);
        
        assignation_min_Pij[operation] = toCopy.getAssignationMinPij(operation);
        assignation_best_max_workload[operation] = toCopy.getAssignationBestWorkload(operation);
        assignation_best_makespan[operation] = toCopy.getAssignationBestMakespan(operation);
    }
    
    for (job = 0; job < n_jobs; ++job) {
        job_operation_is_number[job] = new int[toCopy.getNumberOfOperationsInJob(job)];
        
        for (operation = 0; operation < n_operations_in_job[job]; ++operation) {
            job_operation_is_number[job][operation] = toCopy.getOperationInJobIsNumber(job, operation);
            operation_belongs_to_job[operationCounter] = toCopy.getOperationIsFromJob(operationCounter);
            operationCounter++;
        }
    }

    machines_aviability.reserve(n_operations);
    for (int operation = 0; operation < toCopy.getNumberOfOperations(); ++operation) {
        unsigned long machines_aviable = toCopy.getNumberOfMachinesAvaibleForOperation(operation);
        machines_aviability.push_back(vector<int>());
        for (unsigned long machine = 0; machine < machines_aviable; ++machine)
            machines_aviability.at(operation).push_back(toCopy.getMachinesAvaibleForOperation(operation, machine));
    }
}

ProblemFJSSP::ProblemFJSSP(const Payload_problem_fjssp& problem) :
Problem(problem.n_objectives, problem.n_operations) {
    n_objectives = problem.n_objectives;
    n_variables = problem.n_operations;
    n_jobs = problem.n_jobs;
    n_machines = problem.n_machines;
    n_operations = problem.n_operations;
    
    sum_of_min_Pij = 0;
    best_workload_found = INT_MAX;
    
    n_operations_in_job = new int[n_jobs];
    release_time = new int[n_jobs];

    f_min = new int[n_objectives];
    f_max = new int[n_objectives];
    
    operation_belongs_to_job = new int[n_operations];
    assignation_min_Pij = new int[n_operations];
    min_workloads = new int[n_machines];
    assignation_best_max_workload = new int[n_operations];
    assignation_best_makespan = new int[n_operations];
    best_workloads = new int[n_machines];
    job_operation_is_number = new int *[n_jobs];
    processing_time = new int *[n_operations];
    encode_job_machine_to_map = new int *[n_jobs];
    decode_map_to_job_machine = new int *[n_jobs * n_machines];
    
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
        release_time[n_job] = problem.release_times[n_job];
    }
    
    /** Creates the mapping. **/
    for (int job = 0; job < n_jobs; ++job) {
        encode_job_machine_to_map[job] = new int[n_machines];
        for (int machine = 0; machine < n_machines; ++machine) {
            decode_map_to_job_machine[map] = new int[2];
            decode_map_to_job_machine[map][0] = job;
            decode_map_to_job_machine[map][1] = machine;
            encode_job_machine_to_map[job][machine] = map;
            map++;
        }
    }
    
    int sorted_processing[n_machines][n_operations]; //new int * [n_machines];/** Stores the processing times from min to max for each machine. **/
    int sorted_est[n_operations];
    for (int machine = 0; machine < n_machines; ++machine)
        min_workloads[machine] = 0;
    
    for (int n_op = 0; n_op < n_operations; ++n_op) {
        processing_time[n_op] = new int[n_machines];
        for (int n_mach = 0; n_mach < n_machines; ++n_machines)
            processing_time[n_op][n_mach] = problem.processing_times[n_op * problem.n_machines + n_mach];
    }
    
    for (int operation = 0; operation < n_operations; ++operation) {
        minPij = INT_MAX;
        minMachine = 0;
        for (int machine = 0; machine < n_machines; ++machine) {
            sorted_processing[machine][operation] = processing_time[operation][machine];
            if (processing_time[operation][machine] < minPij) {
                minPij = processing_time[operation][machine];
                minMachine = machine;
            }
        }
        sum_of_min_Pij += minPij;
        min_workloads[minMachine] += processing_time[operation][minMachine];
        assignation_min_Pij[operation] = minMachine;
    }
    
    earliest_starting_time[0] = release_time[0];
    int op_allocated = 0;
    int next_op = 0;
    /** Computes the earliest starting time and the earliest ending time for each job. **/
    for (int job = 0; job < n_jobs; ++job) {
        earliest_starting_time[op_allocated] = release_time[job];
        earliest_ending_time[op_allocated] = earliest_starting_time[op_allocated] + processing_time[op_allocated][assignation_min_Pij[op_allocated]];
        sorted_est[op_allocated] = earliest_starting_time[op_allocated];
        
        for (int operation = 1; operation < n_operations_in_job[job]; ++operation) {
            next_op = op_allocated + operation;
            earliest_starting_time[next_op] = earliest_ending_time[next_op - 1];
            earliest_ending_time[next_op] = earliest_starting_time[next_op] + processing_time[next_op][assignation_min_Pij[next_op]];
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
        job_operation_is_number[job] = new int[n_operations_in_job[job]];
        for (int operation = 0; operation < n_operations_in_job[job]; ++operation) {
            job_operation_is_number[job][operation] = operationCounter;
            operation_belongs_to_job[operationCounter++] = job;
        }
    }
    
    int temp_f2 = e_function(sum_of_min_Pij / n_machines);
    int temp_f1 = e_function((sum_M_smallest_est + sum_of_min_Pij) / n_machines);
    
    best_bound_max_workload = temp_f2 >= min_sum_shortest_proc_times ? temp_f2 : min_sum_shortest_proc_times;
    best_bound_makespan = max_eet_of_jobs >= temp_f1 ? max_eet_of_jobs : temp_f1;
    
    goodSolutionWithMaxWorkload(getNumberOfObjectives(), getNumberOfVariables());
    buildSolutionWithGoodMaxWorkload(goodSolutionWithMaxWorkload);
}

void ProblemFJSSP::loadInstancePayload(const Payload_problem_fjssp& problem) {
    n_objectives = problem.n_objectives;
    n_variables = problem.n_operations;
    n_jobs = problem.n_jobs;
    n_machines = problem.n_machines;
    n_operations = problem.n_operations;
    
    sum_of_min_Pij = 0;
    best_workload_found = INT_MAX;
    
    upper_bound = new int[n_operations];
    lower_bound = new int[n_operations];

    f_min = new int[n_objectives];
    f_max = new int[n_objectives];

    n_operations_in_job = new int[n_jobs];
    release_time = new int[n_jobs];
    
    operation_belongs_to_job = new int[n_operations];
    assignation_min_Pij = new int[n_operations];
    min_workloads = new int[n_machines];
    assignation_best_max_workload = new int[n_operations];
    assignation_best_makespan = new int[n_operations];
    best_workloads = new int[n_machines];
    job_operation_is_number = new int *[n_jobs];
    processing_time = new int *[n_operations];
    encode_job_machine_to_map = new int *[n_jobs];
    decode_map_to_job_machine = new int *[n_jobs * n_machines];
    
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
        release_time[n_job] = problem.release_times[n_job];
    }
    
    for (int n_op = 0; n_op < n_operations; ++n_op) {
        processing_time[n_op] = new int[n_machines];
        for (int n_mach = 0; n_mach < n_machines; ++n_mach)
            processing_time[n_op][n_mach] = problem.processing_times[n_op * problem.n_machines + n_mach];
        lower_bound[n_op] = 0;
        upper_bound[n_op] = n_jobs * n_machines;
    }
    
    /** Creates the mapping. **/
    for (int job = 0; job < n_jobs; ++job) {
        encode_job_machine_to_map[job] = new int[n_machines];
        for (int machine = 0; machine < n_machines; ++machine) {
            decode_map_to_job_machine[map] = new int[2];
            decode_map_to_job_machine[map][0] = job;
            decode_map_to_job_machine[map][1] = machine;
            encode_job_machine_to_map[job][machine] = map;
            map++;
        }
    }
    
    int sorted_processing[n_machines][n_operations];/** Stores the processing times from min to max for each machine. **/
    int sorted_est[n_operations];
    for (int machine = 0; machine < n_machines; ++machine)
        min_workloads[machine] = 0;
    
    int minPij = INT_MAX;
    int minMachine = 0;
    for (int operation = 0; operation < n_operations; ++operation) {
        minPij = INT_MAX;
        minMachine = 0;
        for (int machine = 0; machine < n_machines; ++machine) {
            sorted_processing[machine][operation] = processing_time[operation][machine];
            if (processing_time[operation][machine] < minPij) {
                minPij = processing_time[operation][machine];
                minMachine = machine;
            }
        }
        sum_of_min_Pij += minPij;
        min_workloads[minMachine] += processing_time[operation][minMachine];
        assignation_min_Pij[operation] = minMachine;
    }
    
    earliest_starting_time[0] = release_time[0];
    int op_allocated = 0;
    int next_op = 0;
    /** Computes the earlier starting time and the earlier ending time for each job. **/
    for (int job = 0; job < n_jobs; ++job) {
        earliest_starting_time[op_allocated] = release_time[job];
        earliest_ending_time[op_allocated] = earliest_starting_time[op_allocated] + processing_time[op_allocated][assignation_min_Pij[op_allocated]];
        sorted_est[op_allocated] = earliest_starting_time[op_allocated];
        for (int operation = 1; operation < n_operations_in_job[job]; ++operation) {
            next_op = op_allocated + operation;
            earliest_starting_time[next_op] = earliest_ending_time[next_op - 1];
            earliest_ending_time[next_op] = earliest_starting_time[next_op] + processing_time[next_op][assignation_min_Pij[next_op]];
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
        job_operation_is_number[job] = new int[n_operations_in_job[job]];
        for (int operation = 0; operation < n_operations_in_job[job]; ++operation) {
            job_operation_is_number[job][operation] = operationCounter;
            operation_belongs_to_job[operationCounter++] = job;
        }
    }
    
    int temp_f2 = e_function(sum_of_min_Pij / n_machines);
    int temp_f1 = e_function((sum_M_smallest_est + sum_of_min_Pij) / n_machines);
    
    best_bound_max_workload = temp_f2 >= min_sum_shortest_proc_times ? temp_f2 : min_sum_shortest_proc_times;
    best_bound_makespan = max_eet_of_jobs >= temp_f1 ? max_eet_of_jobs : temp_f1;
    
    goodSolutionWithMaxWorkload(getNumberOfObjectives(), getNumberOfVariables());
    buildSolutionWithGoodMaxWorkload(goodSolutionWithMaxWorkload);
}

ProblemFJSSP& ProblemFJSSP::operator=(const ProblemFJSSP &toCopy) {
    
    if (this == &toCopy)
        return *this;
    
    n_jobs = toCopy.getNumberOfJobs();
    n_operations = toCopy.getNumberOfOperations();
    n_machines = toCopy.getNumberOfMachines();
    n_variables = toCopy.getNumberOfVariables();
    n_objectives = toCopy.getNumberOfObjectives();
    n_constraints = toCopy.getNumberOfConstraints();
    
    sum_M_smallest_est = toCopy.getSumOf_M_smallestEST();
    avg_op_per_machine = toCopy.getAvgOperationPerMachine();
    min_sum_shortest_proc_times = toCopy.getMinSumShortestProcTime();
    
    best_bound_makespan = toCopy.getBestBoundMakespan();
    best_bound_max_workload = toCopy.getBestBoundMaxWorkload();
    
    if (processing_time != nullptr) {
        int job = 0, operation = 0;
        for (job = 0; job < n_jobs; ++job) {
            delete[] encode_job_machine_to_map[job];
            delete[] job_operation_is_number[job];
        }
        
        for (job = 0; job < n_jobs * n_machines; ++job)
            delete[] decode_map_to_job_machine[job];
        
        for (operation = 0; operation < n_operations; ++operation)
            delete[] processing_time[operation];
        
        delete[] encode_job_machine_to_map;
        delete[] decode_map_to_job_machine;
        delete[] processing_time;
        delete[] job_operation_is_number;
        delete[] n_operations_in_job;
        delete[] release_time;
        delete[] operation_belongs_to_job;
        delete[] assignation_min_Pij;
        delete[] min_workloads;
        delete[] assignation_best_max_workload;
        delete[] assignation_best_makespan;
        delete[] best_workloads;
        
        delete[] upper_bound;
        delete[] lower_bound;

        delete[] f_min;
        delete[] f_max;
        
        delete[] earliest_starting_time;
        delete[] earliest_ending_time;
        delete[] eet_of_job;
        delete[] sum_shortest_proc_times;
    }
    
    lower_bound = new int[n_variables];
    upper_bound = new int[n_variables];

    f_min = new int[n_objectives];
    f_max = new int[n_objectives];

    for (unsigned int obj = 0; obj < n_objectives; ++obj) {
        f_min[obj] = toCopy.getFmin(obj);
        f_max[obj] = toCopy.getFmax(obj);
    }
    
    std::strcpy(name, toCopy.name);
    
    goodSolutionWithMaxWorkload = toCopy.getSolutionWithGoodMaxWorkload();
    
    sum_of_min_Pij = toCopy.getSumOfMinPij();
    best_workload_found = toCopy.getBestWorkloadFound();
    best_makespan_found = toCopy.getBestMakespanFound();
    
    encode_job_machine_to_map = new int *[n_jobs];
    decode_map_to_job_machine = new int *[n_jobs * n_machines];
    
    n_operations_in_job = new int[n_jobs];
    release_time = new int[n_jobs];
    operation_belongs_to_job = new int[n_operations];
    assignation_min_Pij = new int[n_operations];
    assignation_best_max_workload = new int[n_operations];
    assignation_best_makespan = new int[n_operations];
    best_workloads = new int[n_machines];
    min_workloads = new int[n_machines];
    
    earliest_starting_time = new int[n_operations];/** Length equals to number of operations. **/
    earliest_ending_time = new int[n_operations]; /** Length equals to number of operations. **/
    eet_of_job = new int[n_jobs]; /** Length equals to number of jobs. **/
    sum_shortest_proc_times = new int[n_machines]; /** D^{k}_{Ñ}. Length equals to number of machines **/
    
    int job = 0, map = 0, machine = 0, operation = 0, operationCounter = 0;
    
    for (job = 0; job < n_jobs; ++job) {
        n_operations_in_job[job] = toCopy.getNumberOfOperationsInJob(job);
        release_time[job] = toCopy.getReleaseTimeOfJob(job);
        eet_of_job[job] = toCopy.getEarliestEndingJobTime(job);
        
        encode_job_machine_to_map[job] = new int[n_machines];
        
        for (machine = 0; machine < n_machines; ++machine) {
            decode_map_to_job_machine[map] = new int[2];
            decode_map_to_job_machine[map][0] = toCopy.getMapOfJobMachine(map, 0);
            decode_map_to_job_machine[map][1] = toCopy.getMapOfJobMachine(map, 1);
            encode_job_machine_to_map[job][machine] = toCopy.getJobMachineToMap(job, machine);
            map++;
        }
    }
    
    for (machine = 0; machine < n_machines; ++machine) {
        min_workloads[machine] = toCopy.getMinWorkload(machine);
        best_workloads[machine] = toCopy.getBestWorkload(machine);
        sum_shortest_proc_times[machine] = toCopy.getSumShortestProcTimeInMachine(machine);
    }
    
    job_operation_is_number = new int *[n_jobs];
    processing_time = new int *[n_operations];
    
    for (operation = 0; operation < n_operations; ++operation) {
        lower_bound[operation] = toCopy.getLowerBound(operation);
        upper_bound[operation] = toCopy.getUpperBound(operation);
        
        earliest_starting_time[operation] = toCopy.getEarliestStartingTime(operation);
        earliest_ending_time[operation] = toCopy.getEarliestEndingTime(operation);
        
        processing_time[operation] = new int[n_machines];
        for (machine = 0; machine < n_machines; ++machine)
            processing_time[operation][machine] = toCopy.getProccessingTime(operation, machine);
        
        assignation_min_Pij[operation] = toCopy.getAssignationMinPij(operation);
        assignation_best_max_workload[operation] = toCopy.getAssignationBestWorkload(operation);
        assignation_best_makespan[operation] = toCopy.getAssignationBestMakespan(operation);
    }
    
    for (job = 0; job < n_jobs; ++job) {
        job_operation_is_number[job] = new int[n_operations_in_job[job]];
        for (operation = 0; operation < n_operations_in_job[job]; ++operation) {
            job_operation_is_number[job][operation] = toCopy.getOperationInJobIsNumber(job, operation);
            operation_belongs_to_job[operationCounter] = toCopy.getOperationIsFromJob(operationCounter);
            operationCounter++;
        }
    }

    machines_aviability.reserve(n_operations);
    for (int operation = 0; operation < toCopy.getNumberOfOperations(); ++operation) {
        unsigned long machines_aviable = toCopy.getNumberOfMachinesAvaibleForOperation(operation);
        machines_aviability.push_back(vector<int>());
        for (unsigned long machine = 0; machine < machines_aviable; ++machine)
            machines_aviability[operation].push_back(toCopy.getMachinesAvaibleForOperation(operation, machine));
    }

    return *this;
}

ProblemFJSSP::~ProblemFJSSP() {
    
    for (int job = 0; job < n_jobs; ++job) {
        delete[] job_operation_is_number[job];
        delete[] encode_job_machine_to_map[job];
    }
    
    for (int job = 0; job < n_jobs * n_machines; ++job)
        delete[] decode_map_to_job_machine[job];
    
    for (int operation = 0; operation < n_operations; ++operation)
        delete[] processing_time[operation];
    
    delete[] encode_job_machine_to_map;
    delete[] decode_map_to_job_machine;
    delete[] processing_time;
    delete[] n_operations_in_job;
    delete[] release_time;
    delete[] job_operation_is_number;
    delete[] operation_belongs_to_job;
    delete[] assignation_min_Pij;
    delete[] min_workloads;
    delete[] assignation_best_max_workload;
    delete[] assignation_best_makespan;
    delete[] best_workloads;
    
    delete[] upper_bound;
    delete[] lower_bound;

    delete[] f_min;
    delete[] f_max;
    delete[] earliest_starting_time;
    delete[] earliest_ending_time;
    delete[] eet_of_job;
    delete[] sum_shortest_proc_times;

    machines_aviability.clear();
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
    
    int minPij = sum_of_min_Pij;
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
        bestWL[machine] = best_workloads[machine];
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
    
    if (solution.getNumberOfObjectives() == 3)
        solution.setObjective(2, totalWorkload + minPij);

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
    //data.setTotalWorkload is computed internally when the operation is allocated.
    solution.setObjective(0, makespan);
    solution.setObjective(1, max_workload);
    if (solution.getNumberOfObjectives() == 3)
        solution.setObjective(2, data.getTotalWorkload());
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
    if (solution.getNumberOfObjectives() == 3)
        solution.setObjective(2, data.getTotalWorkload());
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
            
            while (processing_time[countOperations][machine] == INF_PROC_TIME)
                if (machine++ == n_machines - 1)
                    machine = 0;
            
            map = encode_job_machine_to_map[job][machine];
            solution.setVariable(countOperations, map);
            
            if (machine++ == n_machines - 1)
                machine = 0;
            
            countOperations++;
        }
    
    evaluate(solution);
}

void ProblemFJSSP::updateBestMaxWorkloadSolution(FJSSPdata& data) {
    if (data.getObjective(MAX_WORKLOAD) < best_workload_found) {
        
        best_workload_found = data.getMaxWorkload();
        if (best_workload_found < best_bound_max_workload) {
            best_bound_max_workload = best_workload_found;
            f_min[1] = best_bound_max_workload;
        }

        for (int n_op = 0; n_op < n_operations; ++n_op) {
            assignation_best_max_workload[n_op] = data.getOperationAllocation(n_op);
            goodSolutionWithMaxWorkload.setVariable(n_op, assignation_best_max_workload[n_op]);
        }
        
        for (int m_mach = 0; m_mach < n_machines; ++m_mach)
            data.setBestWorkloadInMachine(m_mach, data.getWorkloadOnMachine(m_mach));
    }
}

void ProblemFJSSP::updateBestMakespanSolution(FJSSPdata& data) {
    if (data.getObjective(MAKESPAN) < best_makespan_found) {
        
        best_makespan_found = data.getMakespan();
        if (best_makespan_found < best_bound_makespan) {
            best_bound_makespan = best_makespan_found;
            f_min[0] = best_bound_makespan;
        }

        for (int n_op = 0; n_op < n_operations; ++n_op)
            assignation_best_makespan[n_op] = data.getOperationAllocation(n_op);
    }
}

void ProblemFJSSP::updateBestBoundsWith(const Solution& solution) {
    updateBestMakespanSolutionWith(solution);
    updateBestMaxWorkloadSolutionWith(solution);
}

void ProblemFJSSP::updateBestMakespanSolutionWith(const Solution& solution) {
    if (solution.getObjective(MAKESPAN) < best_makespan_found) {
        
        best_makespan_found = solution.getObjective(MAKESPAN);
        if (best_makespan_found < best_bound_makespan) {
            best_bound_makespan = best_makespan_found;
            f_min[0] = best_bound_makespan;
        }

        for (int n_op = 0; n_op < n_operations; ++n_op)
            assignation_best_makespan[n_op] = getDecodeMap(solution.getVariable(n_op), 1);
    }
}

void ProblemFJSSP::updateBestMaxWorkloadSolutionWith(const Solution& solution) {
    if (solution.getObjective(MAX_WORKLOAD) < best_workload_found) {
        
        best_workload_found = solution.getObjective(MAX_WORKLOAD);
        if (best_workload_found < best_bound_max_workload) {
            best_bound_max_workload = best_workload_found;
            f_min[1] = best_bound_max_workload;
        }

        for (int n_op = 0; n_op < n_operations; ++n_op) {
            assignation_best_max_workload[n_op] = getDecodeMap(solution.getVariable(n_op), 1);
            goodSolutionWithMaxWorkload.setVariable(n_op, assignation_best_max_workload[n_op]);
        }
    }
}

void ProblemFJSSP::getSolutionWithLowerBoundInObj(int nObj, Solution& solution) {
    
    if (nObj == 0)
        createDefaultSolution(solution);
    
    else if (nObj == 1)
        solution = goodSolutionWithMaxWorkload;
    
    else if (nObj == 2)
        for (int operation = 0; operation < n_operations; ++operation)
            solution.setVariable(operation, encode_job_machine_to_map[operation_belongs_to_job[operation]][assignation_min_Pij[operation]]);
    
    evaluate(solution);
}

/**
 *
 * Creates a good solution with max workload based on the best solution for total workload.
 *
 **/
void ProblemFJSSP::buildSolutionWithGoodMaxWorkload(Solution & solution) {
    buildSolutionWithGoodMaxWorkloadv2(solution);
    evaluate(solution);
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
            
            machine = assignation_min_Pij[counterOperations];
            procTiOp = processing_time[counterOperations][machine];
            assignation_best_max_workload[counterOperations] = machine;
            
            workload[machine] += procTiOp;
            totalWorkload += procTiOp;
            
            solution.setVariable(counterOperations, encode_job_machine_to_map[nJob][machine]);
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
            if (decode_map_to_job_machine[solution.getVariable(nOperation)][1] == maxWorkloadedMachine)
                for (int nMachine = 0; nMachine < getNumberOfMachines(); ++nMachine)
                    if (nMachine != maxWorkloadedMachine
                        && (workload[nMachine] + processing_time[nOperation][nMachine]) < minWorkload) {
                        bestOperation = nOperation;
                        bestMachine = nMachine;
                        minWorkload = workload[nMachine] + processing_time[nOperation][nMachine];
                    }
        
        /** Applies the change. **/
        totalWorkload -= processing_time[bestOperation][maxWorkloadedMachine];
        workload[maxWorkloadedMachine] -= processing_time[bestOperation][maxWorkloadedMachine];
        
        totalWorkload += processing_time[bestOperation][bestMachine];
        workload[bestMachine] += processing_time[bestOperation][bestMachine];
        
        solution.setVariable(bestOperation, encode_job_machine_to_map[operation_belongs_to_job[bestOperation]][bestMachine]);
        assignation_best_max_workload[bestOperation] = bestMachine;
        
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
            workload[lastMaxWorkloadedMachine] += processing_time[bestOperation][lastMaxWorkloadedMachine];
            workload[lastBestWorkloadMachine] -= processing_time[bestOperation][lastBestWorkloadMachine];
            
            totalWorkload += processing_time[bestOperation][lastMaxWorkloadedMachine];
            totalWorkload -= processing_time[bestOperation][lastBestWorkloadMachine];
            
            solution.setVariable(bestOperation, encode_job_machine_to_map[operation_belongs_to_job[bestOperation]][lastMaxWorkloadedMachine]);
            solution.setObjective(1, workload[lastMaxWorkloadedMachine]);
            
            assignation_best_max_workload[bestOperation] = lastMaxWorkloadedMachine;
            
            maxWorkloadIsReduced = 0;
        }
    }
    best_workload_found = 0;
    for (int nMachine = 0; nMachine < getNumberOfMachines(); ++nMachine) {
        best_workloads[nMachine] = workload[nMachine];
        if (best_workloads[nMachine] > best_workload_found)
            best_workload_found = best_workloads[nMachine];
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
    return best_bound_max_workload;
}

void ProblemFJSSP::computeNadirPoints() {
    Solution s_nadir(n_objectives, n_variables);

    for (unsigned int var = 0; var < n_variables; ++var) {
        int max_mach = 0;
        int max_proc = 0;
        unsigned long n_machines_for_op = getNumberOfMachinesAvaibleForOperation(var);
        for (unsigned long n_mach = 0; n_mach < n_machines_for_op; ++n_mach) {
            int machine = getMachinesAvaibleForOperation(var, n_mach);
            if(getProccessingTime(var, machine) > max_proc) {
                max_proc = getProccessingTime(var, machine);
                max_mach = machine;
            }
            int encode = getEncodeMap(getOperationIsFromJob(var), max_mach);
            s_nadir.setVariable(var, encode);
        }
    }
    evaluate(s_nadir);

    for (unsigned int n_obj = 0; n_obj < n_objectives; ++n_obj)
        f_max[n_obj] = s_nadir.getObjective(n_obj);
}

int ProblemFJSSP::getFmin(unsigned int n_obj) const {
    return f_min[n_obj];
}

int ProblemFJSSP::getFmax(unsigned int n_obj) const {
    return f_max[n_obj];
}

int ProblemFJSSP::getBestBoundMakespan() const {
    return best_bound_makespan;
}

int ProblemFJSSP::getLowerBoundInObj(int nObj) const {
    
    switch (nObj) {
        case 0:
            return best_bound_makespan;
            break;
        case 1:
            return best_bound_max_workload;
            break;
        case 2:
            return sum_of_min_Pij;
            break;
    }
    return -1;
}

const Solution& ProblemFJSSP::getSolutionWithGoodMaxWorkload() const {
    return goodSolutionWithMaxWorkload;
}

const ParetoFront ProblemFJSSP::getOptimalParetoFront(unsigned int instance) {
    ParetoFront optimalParetoFront;
    switch (instance) {
        case 1: /**Kacem1**/
            return getOptimalParetoFrontForKacem1();
            break;

        case 2: /**Kacem2**/
            break;

        case 3: /**Fattahi9**/
            return getOptimalParetoFrontForFattahi9();
            break;

        default:
            break;
    }
    return optimalParetoFront;
}

const ParetoFront ProblemFJSSP::getOptimalParetoFrontForKacem1() {
    ParetoFront optimalParetoFront;
    Solution sol(getNumberOfObjectives(), getNumberOfVariables());
    int opt_objectives[2] = {16, 8};
    int opt_variables[12] = {0, 1, 3, 5, 9, 5, 12, 11, 13, 13, 17, 16};

    for (unsigned int obj = 0; obj < getNumberOfObjectives(); ++obj)
        sol.setObjective(obj, opt_objectives[obj]);
    for (unsigned int var = 0; var < getNumberOfVariables(); ++var)
        sol.setVariable(var, opt_variables[var]);

    optimalParetoFront.push_back(sol);
    return optimalParetoFront;
}

const ParetoFront ProblemFJSSP::getOptimalParetoFrontForFattahi9() {
    ParetoFront optimalParetoFront;
    Solution sol_1(getNumberOfObjectives(), getNumberOfVariables());
    Solution sol_2(getNumberOfObjectives(), getNumberOfVariables());
    Solution sol_3(getNumberOfObjectives(), getNumberOfVariables());

    int opt_objectives_1[2] = {210,  185};
    int opt_variables_1[9] = {1, 6, 7, 7, 5, 0, 2, 3, 5};

    int opt_objectives_2[2] = {220,  170};
    int opt_variables_2[9] = {5, 6, 7, 7, 3, 0, 0, 5, 2};

    int opt_objectives_3[2] = {280,  160};
    int opt_variables_3[9] = {6, 7, 8, 3, 3, 5, 0, 1, 1};


    for (unsigned int obj = 0; obj < getNumberOfObjectives(); ++obj){
        sol_1.setObjective(obj, opt_objectives_1[obj]);
        sol_2.setObjective(obj, opt_objectives_2[obj]);
        sol_3.setObjective(obj, opt_objectives_3[obj]);
    }

    for (unsigned int var = 0; var < getNumberOfVariables(); ++var){
        sol_1.setVariable(var, opt_variables_1[var]);
        sol_2.setVariable(var, opt_variables_2[var]);
        sol_3.setVariable(var, opt_variables_3[var]);
    }

    optimalParetoFront.push_back(sol_1);
    optimalParetoFront.push_back(sol_2);
    optimalParetoFront.push_back(sol_3);

    return optimalParetoFront;
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
        
        if (processing_time != nullptr) {
            for (operation = 0; operation < n_operations; ++operation)
                delete[] processing_time[operation];
            delete[] processing_time;
        }
        if (job_operation_is_number != nullptr) {
            for (job = 0; job < n_jobs; ++job)
                delete[] job_operation_is_number[job];
            delete[] job_operation_is_number;
        }
        if (encode_job_machine_to_map != nullptr) {
            for (job = 0; job < n_jobs; ++job)
                delete[] encode_job_machine_to_map[job];
            delete[] encode_job_machine_to_map;
        }
        if (decode_map_to_job_machine != nullptr) {
            for (job = 0; job < n_jobs * n_machines; ++job)
                delete[] decode_map_to_job_machine[job];
            delete[] decode_map_to_job_machine;
        }
        if (n_operations_in_job != nullptr)
            delete[] n_operations_in_job;
        if (release_time != nullptr)
            delete[] release_time;
        if (operation_belongs_to_job != nullptr)
            delete[] operation_belongs_to_job;
        if (assignation_min_Pij != nullptr)
            delete[] assignation_min_Pij;
        if (min_workloads != nullptr)
            delete[] min_workloads;
        if (assignation_best_max_workload != nullptr)
            delete[] assignation_best_max_workload;
        if (assignation_best_makespan != nullptr)
            delete[] assignation_best_makespan;
        if (best_workloads != nullptr)
            delete[] best_workloads;
        if (earliest_starting_time != nullptr)
            delete[] earliest_starting_time;
        if (earliest_ending_time != nullptr)
            delete[] earliest_ending_time;
        if (eet_of_job != nullptr)
            delete[] eet_of_job;
        if (sum_shortest_proc_times != nullptr)
            delete[] sum_shortest_proc_times;
        if (f_min != nullptr)
            delete [] f_min;
        if (f_max != nullptr)
            delete [] f_max;

        f_min = new int[n_objectives];
        f_max = new int[n_objectives];

        sum_of_min_Pij = 0;
        best_workload_found = INT_MAX;
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
        
        /** If the instance is Kacem then it has the release time in the first value of each job. **/
        const int kacem_legnth = 5;
        char kacem[kacem_legnth] {'K', 'a', 'c', 'e', 'm'};
        for (int character = 0; character < kacem_legnth && instance_with_release_time == 1; ++character)
            instance_with_release_time = (kacem[character] == name_file_ext[0][character]) ? 1 : 0;
        
        if (n_jobs > 0 && n_machines > 0) {
            
            n_operations_in_job = new int[n_jobs];
            release_time = new int[n_jobs];
            
            std::string * job_line = new std::string[n_jobs];
            for (int n_job = 0; n_job < n_jobs; ++n_job) {
                std::getline(infile, job_line[n_job]);
                split(job_line[n_job], ' ', elemens); /** Stores the text corresponding to each job processing times. **/
                n_operations_in_job[n_job] = (instance_with_release_time == 1) ? std::stoi(elemens.at(1)): std::stoi(elemens.at(0));
                n_operations += n_operations_in_job[n_job];
                release_time[n_job] = ((instance_with_release_time == 1) ? std::stoi(elemens.at(0)) : 0);
            }
            
            operation_belongs_to_job = new int[n_operations];
            assignation_min_Pij = new int[n_operations];
            min_workloads = new int[n_machines];
            assignation_best_max_workload = new int[n_operations];
            assignation_best_makespan = new int[n_operations];
            best_workloads = new int[n_machines];
            job_operation_is_number = new int *[n_jobs];
            processing_time = new int *[n_operations];
            encode_job_machine_to_map = new int *[n_jobs];
            decode_map_to_job_machine = new int *[n_jobs * n_machines];
            
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
                encode_job_machine_to_map[job] = new int[n_machines];
                for (int machine = 0; machine < n_machines; ++machine) {
                    decode_map_to_job_machine[map] = new int[2];
                    decode_map_to_job_machine[map][0] = job;
                    decode_map_to_job_machine[map][1] = machine;
                    encode_job_machine_to_map[job][machine] = map;
                    map++;
                }
            }
            int sorted_processing[n_machines][n_operations]; /** Stores the processing times from min to max for each machine. **/
            int sorted_est[n_operations];
            
            for (int operation = 0; operation < n_operations; ++operation)
                machines_aviability.push_back(vector<int>());
            
            for (int machine = 0; machine < n_machines; ++machine)
                min_workloads[machine] = 0;
            
            int op_counter = 0;
            for (int n_job = 0; n_job < n_jobs; ++n_job) {
                int token = (instance_with_release_time == 1) ? 2 : 1;
                split(job_line[n_job], ' ', elemens);
                for (int n_op_in_job = 0; n_op_in_job < n_operations_in_job[n_job]; ++n_op_in_job) {
                    
                    int op_can_be_proc_in_n_mach = std::stoi(elemens.at(token++));
                    processing_time[op_counter] = new int[n_machines];
                    
                    for (int n_machine = 0; n_machine < n_machines; ++n_machine)
                        processing_time[op_counter][n_machine] = INF_PROC_TIME;
                    
                    for (int n_mach = 0; n_mach < op_can_be_proc_in_n_mach; ++n_mach) {
                        int machine = std::stoi(elemens.at(token++));
                        int proc_ti = std::stoi(elemens.at(token++));
                        processing_time[op_counter][machine - 1] = proc_ti;
                        
                        if (processing_time[op_counter][machine - 1] != INF_PROC_TIME)
                            machines_aviability.at(op_counter).push_back(machine - 1);
                    }
                    
                    op_counter++;
                }
            }
            
            for (int operation = 0; operation < n_operations; ++operation) {
                minPij = INT_MAX;
                minMachine = 0;
                for (int machine = 0; machine < n_machines; ++machine) {
                    sorted_processing[machine][operation] = processing_time[operation][machine];
                    if (processing_time[operation][machine] < minPij) {
                        minPij = processing_time[operation][machine];
                        minMachine = machine;
                    }
                }
                sum_of_min_Pij += minPij;
                min_workloads[minMachine] += processing_time[operation][minMachine];
                assignation_min_Pij[operation] = minMachine;
            }
            
            infile.close();
            earliest_starting_time[0] = release_time[0];
            int op_allocated = 0;
            int next_op = 0;
            for (int job = 0; job < n_jobs; ++job) {
                earliest_starting_time[op_allocated] = release_time[job];
                earliest_ending_time[op_allocated] = earliest_starting_time[op_allocated] + processing_time[op_allocated][assignation_min_Pij[op_allocated]];
                sorted_est[op_allocated] = earliest_starting_time[op_allocated];
                for (int operation = 1; operation < n_operations_in_job[job]; ++operation) {
                    next_op = op_allocated + operation;
                    earliest_starting_time[next_op] = earliest_ending_time[next_op - 1];
                    earliest_ending_time[next_op] = earliest_starting_time[next_op] + processing_time[next_op][assignation_min_Pij[next_op]];
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
            
            n_variables = n_operations;
            
            int operationCounter = 0;
            for (int job = 0; job < n_jobs; ++job) {
                job_operation_is_number[job] = new int[n_operations_in_job[job]];
                for (int operation = 0; operation < n_operations_in_job[job]; ++operation) {
                    job_operation_is_number[job][operation] = operationCounter;
                    operation_belongs_to_job[operationCounter++] = job;
                }
            }
            
            int temp_f2 = e_function(sum_of_min_Pij / n_machines);
            int temp_f1 = e_function((sum_M_smallest_est + sum_of_min_Pij) / n_machines);
            
            best_bound_max_workload = temp_f2 >= min_sum_shortest_proc_times ? temp_f2 : min_sum_shortest_proc_times;
            best_bound_makespan = max_eet_of_jobs >= temp_f1 ? max_eet_of_jobs : temp_f1;
            
            goodSolutionWithMaxWorkload(getNumberOfObjectives(), getNumberOfVariables());
            buildSolutionWithGoodMaxWorkload(goodSolutionWithMaxWorkload);

            computeNadirPoints();
            for (unsigned int n_obj = 0; n_obj < n_objectives; ++n_obj)
                f_min[n_obj] = getLowerBoundInObj(n_obj);
            
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
        
        if (processing_time != nullptr) {
            int job = 0, operation = 0;
            for (job = 0; job < n_jobs; ++job) {
                delete[] job_operation_is_number[job];
                delete[] encode_job_machine_to_map[job];
            }
            
            for (job = 0; job < n_jobs * n_machines; ++job)
                delete[] decode_map_to_job_machine[job];
            
            for (operation = 0; operation < n_operations; ++operation)
                delete[] processing_time[operation];
            
            delete[] encode_job_machine_to_map;
            delete[] decode_map_to_job_machine;
            delete[] processing_time;
            delete[] n_operations_in_job;
            delete[] release_time;
            delete[] job_operation_is_number;
            delete[] operation_belongs_to_job;
            delete[] assignation_min_Pij;
            delete[] min_workloads;
            delete[] assignation_best_max_workload;
            delete[] assignation_best_makespan;
            delete[] best_workloads;
            
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
        release_time = new int[n_jobs];
        
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
                release_time[job] = std::stoi(elemens.at(job));
            
            operation_belongs_to_job = new int[n_operations];
            sum_of_min_Pij = 0;
            best_workload_found = INT_MAX;
            std::getline(infile, line);
            
            assignation_min_Pij = new int[n_operations];
            min_workloads = new int[n_machines];
            assignation_best_max_workload = new int[n_operations];
            assignation_best_makespan = new int[n_operations];
            
            best_workloads = new int[n_machines];
            job_operation_is_number = new int *[n_jobs];
            processing_time = new int *[n_operations];
            encode_job_machine_to_map = new int *[n_jobs];
            decode_map_to_job_machine = new int *[n_jobs * n_machines];
            
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
                encode_job_machine_to_map[job] = new int[n_machines];
                for (int machine = 0; machine < n_machines; ++machine) {
                    decode_map_to_job_machine[map] = new int[2];
                    decode_map_to_job_machine[map][0] = job;
                    decode_map_to_job_machine[map][1] = machine;
                    encode_job_machine_to_map[job][machine] = map;
                    map++;
                }
            }
            
            int sorted_processing[n_machines][n_operations]; //new int * [n_machines];/** Stores the processing times from min to max for each machine. **/
            int sorted_est[n_operations]; // new int [n_operations];
            
            for (int machine = 0; machine < n_machines; ++machine)
                min_workloads[machine] = 0;
            
            for (int operation = 0; operation < n_operations; ++operation) {
                processing_time[operation] = new int[n_machines];
                std::getline(infile, line);
                elemens = split(line, ' ');
                minPij = INT_MAX;
                minMachine = 0;
                for (int machine = 0; machine < n_machines; ++machine) {
                    processing_time[operation][machine] = std::stoi(elemens.at(machine));
                    sorted_processing[machine][operation] = processing_time[operation][machine];
                    if (processing_time[operation][machine] < minPij) {
                        minPij = processing_time[operation][machine];
                        minMachine = machine;
                    }
                }
                sum_of_min_Pij += minPij;
                min_workloads[minMachine] += processing_time[operation][minMachine];
                assignation_min_Pij[operation] = minMachine;
            }
            
            infile.close();
            earliest_starting_time[0] = release_time[0];
            int op_allocated = 0;
            int next_op = 0;
            /** Computes the earlist starting time and the earlist ending time for each job. **/
            for (int job = 0; job < n_jobs; ++job) {
                earliest_starting_time[op_allocated] = release_time[job];
                earliest_ending_time[op_allocated] = earliest_starting_time[op_allocated] + processing_time[op_allocated][assignation_min_Pij[op_allocated]];
                sorted_est[op_allocated] = earliest_starting_time[op_allocated];
                for (int operation = 1; operation < n_operations_in_job[job]; ++operation) {
                    next_op = op_allocated + operation;
                    earliest_starting_time[next_op] = earliest_ending_time[next_op - 1];
                    earliest_ending_time[next_op] = earliest_starting_time[next_op] + processing_time[next_op][assignation_min_Pij[next_op]];
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
            
            n_variables = n_operations;
            
            int operationCounter = 0;
            for (int job = 0; job < n_jobs; ++job) {
                job_operation_is_number[job] = new int[n_operations_in_job[job]];
                for (int operation = 0; operation < n_operations_in_job[job]; ++operation) {
                    job_operation_is_number[job][operation] = operationCounter;
                    operation_belongs_to_job[operationCounter++] = job;
                }
            }
            
            int temp_f2 = e_function(sum_of_min_Pij / n_machines);
            int temp_f1 = e_function((sum_M_smallest_est + sum_of_min_Pij) / n_machines);
            
            best_bound_max_workload = temp_f2 >= min_sum_shortest_proc_times ? temp_f2 : min_sum_shortest_proc_times;
            best_bound_makespan = max_eet_of_jobs >= temp_f1 ? max_eet_of_jobs : temp_f1;
            
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
int ProblemFJSSP::getDecodeMap(int code, int parameter) const {
    return decode_map_to_job_machine[code][parameter];
}

/** Returns the map corresponding to the configuration of job and machine. (Codes the job and machine in a map). **/
int ProblemFJSSP::getEncodeMap(int job, int machine) const {
    return encode_job_machine_to_map[job][machine];
}

int ProblemFJSSP::getTimesThatValueCanBeRepeated(int value) {
    return n_operations_in_job[value];
}

unsigned int ProblemFJSSP::getNumberOfJobs() const {
    return n_jobs;
}

unsigned int ProblemFJSSP::getNumberOfOperations() const {
    return n_operations;
}

unsigned int ProblemFJSSP::getNumberOfMachines() const {
    return n_machines;
}

int ProblemFJSSP::getSumOfMinPij() const {
    return sum_of_min_Pij;
}

int ProblemFJSSP::getBestObjectiveFound(unsigned int objective) const {
    switch (objective) {
        case 0:
            return getBestMakespanFound();
            break;

        case 1:
            return getBestWorkloadFound();
            break;

        case 2:
            return getSumOfMinPij();
            break;

        default:
            return 0;
            break;
    }
}

int ProblemFJSSP::getBestWorkloadFound() const {
    return best_workload_found;
}

int ProblemFJSSP::getBestMakespanFound() const {
    return best_makespan_found;
}

int ProblemFJSSP::getAssignationMinPij(int n_operation) const {
    return assignation_min_Pij[n_operation];
}

int ProblemFJSSP::getAssignationBestWorkload(int n_operation) const {
    return assignation_best_max_workload[n_operation];
}

int ProblemFJSSP::getAssignationBestMakespan(int n_operation) const {
    return assignation_best_makespan[n_operation];
}

int ProblemFJSSP::getBestWorkload(int n_machine) const {
    return best_workloads[n_machine];
}

int ProblemFJSSP::getMinWorkload(int n_machine) const {
    return min_workloads[n_machine];
}

int ProblemFJSSP::getMapOfJobMachine(int map, int machine_or_job) const {
    return decode_map_to_job_machine[map][machine_or_job];
}

int ProblemFJSSP::getJobMachineToMap(int job, int machine) const {
    return encode_job_machine_to_map[job][machine];
}

int ProblemFJSSP::getOperationInJobIsNumber(int job, int operation) const {
    return job_operation_is_number[job][operation];
}

int ProblemFJSSP::getOperationIsFromJob(int n_operation) const {
    return operation_belongs_to_job[n_operation];
}

int ProblemFJSSP::getProccessingTime(int operation, int machine) const {
    return processing_time[operation][machine];
}

int ProblemFJSSP::getNumberOfOperationsInJob(int job) const {
    return n_operations_in_job[job];
}

int ProblemFJSSP::getReleaseTimeOfJob(int job) const {
    return release_time[job];
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

unsigned long ProblemFJSSP::getNumberOfMachinesAvaibleForOperation(unsigned long operation) const {
    return machines_aviability.at(operation).size();
}

int ProblemFJSSP::getMachinesAvaibleForOperation(unsigned long operation, unsigned long machine) const {
    return machines_aviability.at(operation).at(machine);
}

bool ProblemFJSSP::operationCanBeAllocatedInMachine(int operation, int machine ) const {
    if (getProccessingTime(operation, machine) != INF_PROC_TIME)
        return true;
    return false;
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

void ProblemFJSSP::printProblemInfo() const {
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
        printf("%2d ", release_time[job]);
    printf("\n");
    
    printf("Processing times: \n");
    printf("\t\t");
    for (machine = 0; machine < n_machines; ++machine)
        printf("M%-3d", machine);
    printf("| EST EET\n");
    for (operation = 0; operation < n_operations; ++operation) {
        
        printf("[J%-2d] %2d:", operation_belongs_to_job[operation], operation);
        for (machine = 0; machine < n_machines; ++machine)
            if (processing_time[operation][machine] == INF_PROC_TIME)
                printf("%4c", sep);
            else
                printf("%4d", processing_time[operation][machine]);
        printf(" | %4d %4d\n", earliest_starting_time[operation], earliest_ending_time[operation]);
    }
    
    printf("Avg operations per machine (Ñ): %4d\n", avg_op_per_machine);
    printf("Sum of the Ñ shortest processing times (D^{k}_{Ñ}):\n");
    for (machine = 0; machine < n_machines; ++machine)
        printf("%4d ", sum_shortest_proc_times[machine]);
    
    printf("\nEarliest ending time of each job (EET_j):\n");
    for (job = 0; job < n_jobs; ++job)
        printf("%4d ", eet_of_job[job]);
    
    int temp_f2 = e_function(sum_of_min_Pij / n_machines);
    int temp_f1 = e_function((sum_M_smallest_est + sum_of_min_Pij) / n_machines);
    
    printf("\nSum of the minimun processing times (sum_i sum_j Gamma_{i,j}): %4d\n", sum_of_min_Pij);
    printf("Minimum D^{k}_{Ñ}: %4d\n", min_sum_shortest_proc_times);
    printf("Maximum EET from jobs: %4d\n", max_eet_of_jobs);
    printf("Sum of the M smallest EST (R_{M}): %4d\n", sum_M_smallest_est);
    printf("E((R_M + Gamma_{i,j}) / M): %4d\n", temp_f1);
    printf("E(Gamma_{i,j} / M): %4d\n", temp_f2);
    printf("f^'_1 (bound): max(%4d, %4d, ) = %4d\n", max_eet_of_jobs, e_function((sum_M_smallest_est + sum_of_min_Pij) / n_machines), best_bound_makespan);
    printf("f^'_2 (bound): max(%4d, %4d) =  %4d\n", e_function(sum_of_min_Pij / n_machines), min_sum_shortest_proc_times, best_bound_max_workload);
    printf("f^*_3: %4d\n", sum_of_min_Pij);

    for (unsigned int n_obj = 0; n_obj < n_objectives; ++n_obj)
        printf("f_min_%d: %d\n", n_obj + 1, f_min[n_obj]);
    for (unsigned int n_obj = 0; n_obj < n_objectives; ++n_obj)
        printf("f_max_%d: %d\n", n_obj + 1, f_max[n_obj]);
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
        job = decode_map_to_job_machine[map][0];
        machine = decode_map_to_job_machine[map][1];
        
        numberOp = job_operation_is_number[job][operationOfJob[job]];
        operation_in_machine[numberOp] = machine;
        
        /** With the number of operation and the machine we can continue. **/
        workload[machine] += processing_time[numberOp][machine];
        totalWorkload += processing_time[numberOp][machine];
        
        if (operationOfJob[job] == 0) { /** If it is the first operation of the job.**/
            if (timeInMachine[machine] >= release_time[job]) {
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += processing_time[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            } else { /** If the job has to wait for the release time.**/
                startingTime[numberOp] = release_time[job];
                timeInMachine[machine] = release_time[job] + processing_time[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
            }
            
        } else {
            if (endingTime[numberOp - 1] > timeInMachine[machine]) { /**The operation is waiting for their dependency operation.**/
                
                startingTime[numberOp] = endingTime[numberOp - 1];
                timeInMachine[machine] = endingTime[numberOp - 1] + processing_time[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
                
            } else { /**The operation starts when the machine is avaliable.**/
                
                startingTime[numberOp] = timeInMachine[machine];
                timeInMachine[machine] += processing_time[numberOp][machine];
                endingTime[numberOp] = timeInMachine[machine];
                
            }
        }

        operationOfJob[job]++;
        
        if (timeInMachine[machine] > makespan)
            makespan = timeInMachine[machine];
        
        if (workload[machine] > maxWorkload)
            maxWorkload = workload[machine];
    }
    
    printf("\tOp :  M  ti -  tf\n");
    for (operation = 0; operation < n_operations; ++operation)
        printf("%3d: %2d %3d - %3d \n", operation, operation_in_machine[operation], startingTime[operation], endingTime[operation]);
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

/** This function returns true if the solution is valid. **/
bool ProblemFJSSP::validateVariablesOf(const Solution& solution) const {
    bool is_valid = true;
    int number_of_operations_allocated_from_job[getNumberOfOperations()];

    for (int job = 0; job < getNumberOfJobs(); job++)
        number_of_operations_allocated_from_job[job] = 0;

    for (int variable = 0; variable < n_variables && is_valid; ++variable) {
        int code = solution.getVariable(variable);
        int job = getDecodeMap(code, 0);
        int machine = getDecodeMap(code, 1);
        int operation = getOperationInJobIsNumber(job, number_of_operations_allocated_from_job[job]);

        if (!(operationCanBeAllocatedInMachine(operation, machine) && number_of_operations_allocated_from_job[job] < getNumberOfOperationsInJob(job)))
            is_valid = false;

        number_of_operations_allocated_from_job[job]++;
    }

    return is_valid;
}
