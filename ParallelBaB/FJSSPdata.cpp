//
//  FJSSPdata.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/20/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "FJSSPdata.hpp"
FJSSPdata::FJSSPdata(int numberOfJobs, int numberOfOperations, int numberOfMachines):
n_jobs(numberOfJobs),
n_operations(numberOfOperations),
n_machines(numberOfMachines),
machine_makespan(0),
total_workload(0),
max_workload(0),
makespan(0),
min_total_workload(0){
    n_operations_allocated = new int[n_jobs];
    operation_allocated_in = new int [n_operations];
    starting_time = new int[n_operations];
    ending_time = new int[n_operations];
    time_on_machine = new int[n_machines];
    workload_in_machine = new int[n_machines];
    best_workloads_in_machine = new int[n_machines];
    temp_best_wl_m = new int[n_machines];
    
    machine_allocations.resize(n_machines);
    
    for (int job = 0; job < n_jobs; ++job)
        n_operations_allocated[job] = 0;
    
    for (int machine = 0; machine < n_machines; ++machine){
        time_on_machine[machine] = 0;
        workload_in_machine[machine] = 0;
        best_workloads_in_machine[machine] = 0;
        temp_best_wl_m[machine] = 0;
    }
    
    for (int operation = 0; operation < n_operations; ++operation){
        starting_time[operation] = 0;
        ending_time[operation] = 0;
        operation_allocated_in[operation] = -1;
    }
}

FJSSPdata::FJSSPdata(const FJSSPdata& toCopy):
n_jobs(toCopy.getNumberOfJobs()),
n_operations(toCopy.getNumberOfOperations()),
n_machines(toCopy.getNumberOfMachines()),
machine_makespan(toCopy.getMakespanMachine()),
total_workload(toCopy.getTotalWorkload()),
max_workload(toCopy.getMaxWorkload()),
makespan(toCopy.getMakespan()),
min_total_workload(toCopy.getMinTotalWorkload()),
machine_allocations(toCopy.getMachineAllocations()){
    
    n_operations_allocated = new int[n_jobs];
    operation_allocated_in = new int[n_operations];
    starting_time = new int[n_operations];
    ending_time = new int[n_operations];
    time_on_machine = new int[n_machines];
    workload_in_machine = new int[n_machines];
    best_workloads_in_machine = new int[n_machines];
    temp_best_wl_m = new int[n_machines];
    
    for (int job = 0; job < n_jobs; ++job)
        n_operations_allocated[job] = toCopy.getNumberOfOperationsAllocatedFromJob(job);
    
    for (int machine = 0; machine < n_machines; ++machine){
        time_on_machine[machine] = toCopy.getTimeOnMachine(machine);
        workload_in_machine[machine] = toCopy.getWorkloadOnMachine(machine);
        best_workloads_in_machine[machine] = toCopy.getBestWorkloadInMachine(machine);
        temp_best_wl_m[machine] = toCopy.getTempBestWorkloadInMachine(machine);
    }
    
    for (int operation = 0; operation < n_operations; ++operation){
        starting_time[operation] = toCopy.getStartingTime(operation);
        ending_time[operation] = toCopy.getEndingTime(operation);
        operation_allocated_in[operation] = toCopy.getOperationAllocation(operation);
    }
    
}

FJSSPdata::~FJSSPdata(){
    delete [] n_operations_allocated;
    delete [] operation_allocated_in;
    delete [] starting_time;
    delete [] ending_time;
    delete [] time_on_machine;
    delete [] workload_in_machine;
    delete [] best_workloads_in_machine;
    delete [] temp_best_wl_m;
    
    for(int m = 0; m < n_machines; ++m){
        machine_allocations[m].clear();
        machine_allocations[m].resize(0);
    }
}

FJSSPdata& FJSSPdata::operator()(int numberOfJobs, int numberOfOperations, int numberOfMachines){
    if (starting_time != nullptr) {
        delete [] n_operations_allocated;
        delete [] operation_allocated_in;
        delete [] starting_time;
        delete [] ending_time;
        delete [] time_on_machine;
        delete [] workload_in_machine;
        delete [] best_workloads_in_machine;
    }
    
    n_jobs = numberOfJobs;
    n_operations = numberOfOperations;
    n_machines = numberOfMachines;
    makespan = 0;
    total_workload = 0;
    max_workload = 0;
    
    n_operations_allocated = new int[n_jobs];
    operation_allocated_in = new int[n_operations];
    starting_time = new int[n_operations];
    ending_time = new int[n_operations];
    time_on_machine = new int[n_machines];
    workload_in_machine = new int[n_machines];
    machine_allocations.resize(n_machines);
    
    for (int job = 0; job < n_jobs; ++job)
        n_operations_allocated[job] = 0;
    
    for (int machine = 0; machine < n_machines; ++machine){
        time_on_machine[machine] = 0;
        workload_in_machine[machine] = 0;
    }
    
    for (int operation = 0; operation < n_operations; ++operation){
        operation_allocated_in[operation] = -1;
        starting_time[operation] = 0;
        ending_time[operation] = 0;
    }
    
    return *this;
}

int FJSSPdata::getNumberOfJobs() const {
    return n_jobs;
}


int FJSSPdata::getNumberOfMachines() const {
    return n_machines;
}

int FJSSPdata::getNumberOfOperations() const {
    return n_operations;
}

int FJSSPdata::getMakespanMachine() const {
    return machine_makespan;
}

int FJSSPdata::getNumberOfOperationsAllocatedFromJob(int job) const throw(FJSSPdataException){
    try {
        if (job >= n_jobs)
            throw FJSSPdataException(JOB_OUT_OF_RANGE, "when calling getNumberOfOperationsAllocatedFromJob(job:" + std::to_string(static_cast<long long>(job)) + ")");
        
        return n_operations_allocated[job];

    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
    return -1;
}

int FJSSPdata::getLastOperationAllocatedInJob(int job) const throw(FJSSPdataException){
    try {
        if (job >= n_jobs)
            throw FJSSPdataException(JOB_OUT_OF_RANGE, "when calling getLastOperationAllocatedInJob(job:" + std::to_string(static_cast<long long>(job)) + ")");
        
        return n_operations_allocated[job] - 1;
        
    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
    return - 1;
}

int FJSSPdata::getOperationAllocation(int operation) const throw(FJSSPdataException){
    try {
        if (operation >= n_operations)
            throw FJSSPdataException(OPERATION_OUT_OF_RANGE, "when calling getOperationAllocation(op:" + std::to_string(static_cast<long long>(operation)) + ")");
        
        return operation_allocated_in[operation];
        
    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
    return -1;
}

int FJSSPdata::getStartingTime(int operation) const throw(FJSSPdataException){
    try {
        if (operation >= n_operations)
            throw FJSSPdataException(OPERATION_OUT_OF_RANGE, "when calling getStartingTime(op:" + std::to_string(static_cast<long long>(operation)) + ")");
        
        return starting_time[operation];
        
    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
    return -1;
}

int FJSSPdata::getEndingTime(int operation) const throw(FJSSPdataException){
    try {
        if (operation >= n_operations)
            throw FJSSPdataException(OPERATION_OUT_OF_RANGE, "when calling getEndingTime(op:" + std::to_string(static_cast<long long>(operation)) + ")");
        
        return ending_time[operation];
        
    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
    return -1;
}

int FJSSPdata::getTimeOnMachine(int machine) const throw(FJSSPdataException){
    try {
        if (machine >= n_machines)
            throw FJSSPdataException(MACHINE_OUT_OF_RANGE, "when calling getTimeOnMachine(machine:" + std::to_string(static_cast<long long>(machine)) + ")");
        
        return time_on_machine[machine];
        
    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
    return -1;
}

int FJSSPdata::getWorkloadOnMachine(int machine) const throw(FJSSPdataException){
    try {
        if (machine >= n_machines)
            throw FJSSPdataException(MACHINE_OUT_OF_RANGE, "when calling getWorkloadOnMachine(machine:" + std::to_string(static_cast<long long>(machine)) + ")");
        
        return workload_in_machine[machine];
        
    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
    return -1;
}

int FJSSPdata::getMakespan() const{
    return makespan;
}

int FJSSPdata::getTotalWorkload() const {
    return total_workload;
}

int FJSSPdata::getMaxWorkload() const {
    return max_workload;
}

int FJSSPdata::getMinTotalWorkload() const{
    return min_total_workload;
}

int FJSSPdata::getObjective(int n_obj) const{
    int value = 0;
    switch (n_obj) {
        case 0:
            value = getMakespan();
            break;
        case 1:
            value = getMaxWorkload();
            break;
        case 2:
            value = getMinTotalWorkload();
            break;
        default:
            printf("Invalid objective in fjssp data.\n");
            break;
    }
    return value;
}

int FJSSPdata::getBestWorkloadInMachine(int machine) const throw(FJSSPdataException){
    try {
        if (machine >= n_machines)
            throw FJSSPdataException(MACHINE_OUT_OF_RANGE, "when calling getBestWorkloadInMachine(machine:" + std::to_string(static_cast<long long>(machine)) + ")");
        
        return best_workloads_in_machine[machine];
        
    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
    return -1;
}

int FJSSPdata::getTempBestWorkloadInMachine(int machine) const throw(FJSSPdataException){
    try {
        if (machine >= n_machines)
            throw FJSSPdataException(MACHINE_OUT_OF_RANGE, "when calling getTempBestWorkloadInMachine(machine:" + std::to_string(static_cast<long long>(machine)) + ")");
        
        return temp_best_wl_m[machine];
        
    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
    return -1;
}

size_t FJSSPdata::getNumberOfOperationsAllocatedIn(int machine) const throw(FJSSPdataException) {
    try {
        if (machine >= n_machines)
            throw FJSSPdataException(MACHINE_OUT_OF_RANGE, "when calling getNumberOfOperationsAllocatedIn(machine:" + std::to_string(static_cast<long long>(machine)) + ")");
        
        return machine_allocations[machine].size();
        
    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
    return -1;
}

const std::vector<std::deque<int> >& FJSSPdata::getMachineAllocations() const {
    return machine_allocations;
}

void FJSSPdata::setMakespanMachine(int machine) {
    machine_makespan = machine;
}

void FJSSPdata::setNumberOfOperationsAllocatedInJob(int job, int n_allocated) throw(FJSSPdataException){
    try {
        if (job >= n_jobs)
            throw FJSSPdataException(JOB_OUT_OF_RANGE, "when calling setNumberOfOperationsAllocatedInJob(job:" + std::to_string(static_cast<long long>(job)) + ")");
        
        n_operations_allocated[job] = n_allocated;
        
    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
}

void FJSSPdata::setOperationAllocation(int job, int operation, int machine, int processing_time) throw(FJSSPdataException){
    try {
        if (job >= n_jobs)
            throw FJSSPdataException(JOB_OUT_OF_RANGE, "when calling setOperationAllocation(job:" + std::to_string(static_cast<long long>(job)) + ")");
        
        if (operation >= n_operations)
            throw FJSSPdataException(OPERATION_OUT_OF_RANGE, "when calling setOperationAllocation(operation:" + std::to_string(static_cast<long long>(operation)) + ")");
        
        if (machine >= n_machines)
            throw FJSSPdataException(MACHINE_OUT_OF_RANGE, "when calling setOperationAllocation(machine:" + std::to_string(static_cast<long long>(machine)) + ")");
        
        total_workload += processing_time;
        workload_in_machine[machine] += processing_time;
        n_operations_allocated[job]++;
        operation_allocated_in[operation] = machine;
        machine_allocations[machine].push_back(operation);
        
    } catch (FJSSPdataException& fjssp_data_ex) {
        printf("%s\n",  fjssp_data_ex.what());
    }
}

void FJSSPdata::setStartingTime(int operation, int start_time) {
    starting_time[operation] = start_time;
}

void FJSSPdata::setEndingTime(int operation, int end_time) {
    ending_time[operation] = end_time;
}

void FJSSPdata::setTimeOnMachine(int machine, int time) {
    time_on_machine[machine] = time;}

void FJSSPdata::setWorkloadOnMachine(int machine, int workload) {
    workload_in_machine[machine] = workload;
}

void FJSSPdata::setMakespan(int value){
    makespan = value;
}

void FJSSPdata::setTotalWorkload(int workload) {
    total_workload = workload;
}

void FJSSPdata::setMaxWorkload(int workload) {
    max_workload = workload;
}

void FJSSPdata::setTempBestWorkloadInMachine(int machine, int value){
    temp_best_wl_m[machine] = value;
}

void FJSSPdata::setBestWorkloadInMachine(int machine, int value){
    best_workloads_in_machine[machine] = value;
}

void FJSSPdata::setMinTotalWorkload(int value){
    min_total_workload = value;
}

void FJSSPdata::increaseTempWorkloadIn(int machine, int time){
    temp_best_wl_m[machine] += time;
}

void FJSSPdata::decreaseTempWorkloadIn(int machine, int time){
    temp_best_wl_m[machine] -= time;
}

void FJSSPdata::increaseOperationsAllocatedIn(int job){
    n_operations_allocated[job]++;
}

void FJSSPdata::decreaseOperationsAllocatedIn(int job){
    n_operations_allocated[job]--;
}

void FJSSPdata::increaseWorkloadIn(int machine, int time){
    workload_in_machine[machine] += time;
}

void FJSSPdata::decreaseWorkloadIn(int machine, int time){
    workload_in_machine[machine] -= time;
}

void FJSSPdata::increaseTimeOnMachine(int machine, int time){
    time_on_machine[machine] += time;
}

void FJSSPdata::decreaseTimeOnMachine(int machine, int time){
    time_on_machine[machine] -= time;
}

void FJSSPdata::increaseToltalWorkload(int time){
    total_workload += time;
}

void FJSSPdata::decreaseToltalWorkload(int time){
    total_workload -= time;
}

void FJSSPdata::deallocateOperation(int job, int operation){
    int procceessiinngg = ending_time[operation] - starting_time[operation];
    int machine = operation_allocated_in[operation]; /** Get the machine in which the operation was allocated. **/
    machine_allocations[machine].pop_back(); /** Remove the operation from machine. **/
    
    if(machine_allocations[machine].empty())
        time_on_machine[machine] = 0;
    else
        time_on_machine[machine] = ending_time[machine_allocations[machine].back()]; /** Reduce time_on_machine. **/
    
    operation_allocated_in[operation] = -1;
    starting_time[operation] = 0;
    ending_time[operation] = 0;
    
    n_operations_allocated[job]--;
    total_workload -= procceessiinngg;
    workload_in_machine[machine] -= procceessiinngg;
}

void FJSSPdata::reset(){
    total_workload = min_total_workload;
    max_workload = 0;
    
    for (int job = 0; job < n_jobs; ++job)
        n_operations_allocated[job] = 0;
    
    for (int machine = 0; machine < n_machines; ++machine){
        time_on_machine[machine] = 0;
        workload_in_machine[machine] = 0;
        temp_best_wl_m[machine] = best_workloads_in_machine[machine];
        machine_allocations[machine].clear();
    }
    
    for (int operation = 0; operation < n_operations; ++operation){
        operation_allocated_in[operation] = -1;
        starting_time[operation] = 0;
        ending_time[operation] = 0;
    }
}

void FJSSPdata::print() const{
    char gantt[n_machines][255];
    int time = 0;
    int machine = 0;
    int operation = 0;
    
    printf("\tOp :  M  ti -  tf\n");
    for (operation = 0; operation < n_operations; ++operation)
        if(operation_allocated_in[operation] > -1)
            printf("%3c %3d: %2d %3d - %3d \n", 'a' + operation, operation, operation_allocated_in[operation], starting_time[operation], ending_time[operation]);
        else
            printf("%3c %3d:  - %3d - %3d \n", 'a' + operation, operation, starting_time[operation], ending_time[operation]);
    
    /**creates an empty gantt**/
    for (machine = 0; machine < n_machines; ++machine)
        for (time = 0; time < 255; ++time)
            gantt[machine][time] = ' ';
    
    for (operation = 0; operation < n_operations; ++operation)
        for (time = starting_time[operation]; time < ending_time[operation]; ++time)
            gantt[operation_allocated_in[operation]][time] = 'a' + operation;
    
    for (machine = 0; machine < n_machines; ++machine) {
        printf("M%d  |", machine);
        for (time = 0; time <= makespan; ++time)
            printf("%3c", gantt[machine][time]);
        printf("| %3d %3d\n", workload_in_machine[machine], time_on_machine[machine]);
    }
    
    printf("----");
    for (time = 0; time <= makespan; ++time)
        printf("---");
    printf("--\n");
    
    printf("Time:");
    for (time = 0; time <= makespan; ++time)
        printf("%3d", (time));
    printf("\n");
    
    for (machine = 0; machine < n_machines; ++machine)
        printf("BestWL: %6d Temp: %6d\n", best_workloads_in_machine[machine], temp_best_wl_m[machine]);
    
    printf("makespan: %d\nmaxWorkLoad: %d\ntotalWorkload: %d \n", makespan, max_workload, total_workload);
}
