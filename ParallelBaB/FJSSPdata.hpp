//
//  FJSSPdata.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/20/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef FJSSPdata_hpp
#define FJSSPdata_hpp

#include <stdio.h>
#include <vector>
#include <deque>
#include <fstream>
#include <string>
#include <math.h>

class FJSSPdata{
    
private:
    int n_jobs;
    int n_operations;
    int n_machines;
    int machine_makespan;
    int total_workload;
    int max_workload; /** Also known as critical workload. **/
    int makespan;
    int min_total_workload;
    
    int* operation_allocated_in; /** Size = n_operations. Indicates the machine where the operation is allocated. **/
    int* n_operations_allocated; /** Size = n_jobs. Counts the number of the allocated operations from job n. **/
    int* starting_time;          /** Size = n_operations. Contains the starting time of the n_operation. **/
    int* ending_time;            /** Size = n_operations. Contains the ending time of the n_operation. **/
    int* time_on_machine;        /** Size = n_machines. Contains the ending time of the last computed job. **/
    int* workload_in_machine;    /** Size = n_machines. Contains the acumulated processing time. **/
    int* best_workloads_in_machine; /** Size = n_machines.**/
    int* temp_best_wl_m;         /** Size = n_machines.**/
    std::vector<std::deque<int> > machine_allocations;
    
public:
    FJSSPdata(int numberOfJobs, int numberOfOperations, int numberOfMachines);
    FJSSPdata(const FJSSPdata& toCopy);
    ~FJSSPdata();
    
    FJSSPdata& operator()(int numberOfJobs, int numberOfOperations, int numberOfMachines);
    int getNumberOfJobs() const;
    int getNumberOfMachines() const;
    int getNumberOfOperations() const;
    int getMakespanMachine() const;
    int getNumberOfOperationsAllocatedFromJob(int job) const;
    int getLastOperationAllocatedInJob(int job) const;
    int getOperationAllocation(int operation) const;
    int getStartingTime(int operation) const;
    int getEndingTime(int operation) const;
    int getTimeOnMachine(int machine) const;
    int getWorkloadOnMachine(int machine) const;
    int getMakespan() const;
    int getTotalWorkload() const;
    int getMaxWorkload() const;
    int getMinTotalWorkload() const;
    int getObjective(int n_obj) const;
    int getBestWorkloadInMachine(int machine) const;
    int getTempBestWorkloadInMachine(int machine) const;
    size_t getNumberOfOperationsAllocatedIn(int machine) const;
    const std::vector<std::deque<int> >& getMachineAllocations() const;

    void setMakespanMachine(int machine);
    void setNumberOfOperationsAllocatedInJob(int job, int n_allocated);
    void setOperationAllocation(int job, int operation, int machine, int processing_time);
    void setStartingTime(int operation, int start_time);
    void setEndingTime(int operation, int end_time);
    void setTimeOnMachine(int machine, int time);
    void setWorkloadOnMachine(int machine, int workload);
    void setMakespan(int value);
    void setTotalWorkload(int workload);
    void setMaxWorkload(int workload);
    void setTempBestWorkloadInMachine(int machine, int value);
    void setBestWorkloadInMachine(int machine, int value);
    void setMinTotalWorkload(int value);
    void increaseTempWorkloadIn(int machine, int time);
    void decreaseTempWorkloadIn(int machine, int time);
    void increaseOperationsAllocatedIn(int job);
    void decreaseOperationsAllocatedIn(int job);
    void increaseWorkloadIn(int machine, int time);
    void decreaseWorkloadIn(int machine, int time);
    void increaseTimeOnMachine(int machine, int time);
    void decreaseTimeOnMachine(int machine, int time);
    void increaseToltalWorkload(int time);
    void decreaseToltalWorkload(int time);
    void deallocateOperation(int job, int operation);
    void reset();
    void print() const;
};
#endif /* FJSSPdata_hpp */
