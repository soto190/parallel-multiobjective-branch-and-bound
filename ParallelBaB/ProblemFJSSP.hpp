//
//  ProblemFJSSP.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 14/11/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#ifndef ProblemFJSSP_hpp
#define ProblemFJSSP_hpp

#include <stdio.h>
#include <stdio.h>
#include <vector>
#include <deque>
#include <fstream>
#include <string>
#include <math.h>
#include <regex>
#include "Problem.hpp"
#include "myutils.hpp"

#define INF_PROC_TIME 9999999
#define MAX_GANTT_LIMIT 262144

//#define MAKESPAN  0;
//#define MAXWORKLOAD 1;
//#define TOTALWORKLOAD 2;
//enum FJSSPObj{MAKESPAN = 0, MAXWORKLOAD = 1, TOTALWORKLOAD = 2};

/** jobaHasNoperations saids how many operations have each job.
 if we have 2 jobs with 3 operations and 1 job with 2 operations.
 Job 0 has 3 operations: [0] = 3
 Job 1 has 3 operations: [1] = 3
 Job 2 has 2 operations: [2] = 2
 **/
/** operationInJobIsNumber saids the number of an operation with respect witha all the operations from all the jobs.
 if we have 2 jobs with 3 operations.
 O_{operationInJob, job}
 O_{0,0} is the operation 0
 O_{1,0} is the operation 1
 O_{2,0} is the operation 2
 O_{0,1} is the operation 3
 O_{1,1} is the operation 4
 O_{2,1} is the operation 5
 O_{0,2} is the operation 6
 O_{1,2} is the operation 7
 
 Job\Operation [0] [1] [2]
 [0] 0   1   2
 [1] 3   4   5
 [2] 6   7   -
 **/
/** operationIsFromJob saids to which job corresponds the current operation.
 Operation 0 is from job 0: [0] = 0
 Operation 1 is from job 0: [1] = 0
 Operation 2 is from job 0: [2] = 0
 Operation 3 is from job 1: [3] = 1
 Operation 4 is from job 1: [4] = 1
 Operation 5 is from job 1: [5] = 1
 Operation 6 is from job 2: [6] = 2
 Operation 7 is from job 2: [7] = 2
 **/

typedef struct {
    int n_objectives;
    int n_jobs;
    int n_operations;
    int n_machines;
    int * release_times;
    int * n_operations_in_job;
    int * processing_times; /** length is n_operations x n_machines **/
} Payload_problem_fjssp;

class FJSSPdata{
    
private:
    int n_jobs;
    int n_operations;
    int n_machines;
    int machine_makespan;
    int total_workload;
    int max_workload; /** Critical workload. **/
    int makespan;
    
    int min_total_workload;
    
    int* operation_allocated_in; /** Size = n_operations. Indicates the machine where the operation is allocated. **/
    int* n_operations_allocated; /** Size = n_jobs. Counts the number of the allocated operations from job n. **/
    int* starting_time; /** Size = n_operations. Contains the starting time of the n_operation. **/
    int* ending_time; /** Size = n_operations. Contains the ending time of the n_operation. **/
    int* time_on_machine; /** Size = n_machines. Contains the ending time of the last computed job. **/
    int* workload_in_machine; /** Size = n_machines. Contains the acumulated processing time. **/
    int* best_workloads_in_machine; /** Size = n_machines.**/
    int* temp_best_wl_m; /** Size = n_machines.**/
    std::vector<std::deque<int> > machine_allocations;
    
public:
    FJSSPdata(int numberOfJobs, int numberOfOperations, int numberOfMachines):
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
        
        for (int j = 0; j < n_jobs; ++j)
            n_operations_allocated[j] = 0;
        
        for (int m = 0; m < n_machines; ++m){
            time_on_machine[m] = 0;
            workload_in_machine[m] = 0;
            best_workloads_in_machine[m] = 0;
            temp_best_wl_m[m] = 0;
        }
        
        for (int o = 0; o < n_operations; ++o){
            starting_time[o] = 0;
            ending_time[o] = 0;
            operation_allocated_in[o] = -1;
        }
    }
    
    FJSSPdata(const FJSSPdata& toCopy):
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
        
        for (int j = 0; j < n_jobs; ++j)
            n_operations_allocated[j] = toCopy.getNumberOfOperationsAllocatedInJob(j);
        
        for (int m = 0; m < n_machines; ++m){
            time_on_machine[m] = toCopy.getTimeOnMachine(m);
            workload_in_machine[m] = toCopy.getWorkloadOnMachine(m);
            best_workloads_in_machine[m] = toCopy.getBestWorkloadInMachine(m);
            temp_best_wl_m[m] = toCopy.getTempBestWorkloadInMachine(m);
        }
        
        for (int o = 0; o < n_operations; ++o){
            starting_time[o] = toCopy.getStartingTime(o);
            ending_time[o] = toCopy.getEndingTime(o);
            operation_allocated_in[o] = toCopy.getOperationAllocation(o);
        }
        
    }
    
    ~FJSSPdata(){
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
    
    FJSSPdata& operator()(int numberOfJobs, int numberOfOperations, int numberOfMachines){
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
        
        for (int j = 0; j < n_jobs; ++j)
            n_operations_allocated[j] = 0;
        
        for (int m = 0; m < n_machines; ++m){
            time_on_machine[m] = 0;
            workload_in_machine[m] = 0;
        }
        
        for (int o = 0; o < n_operations; ++o){
            operation_allocated_in[o] = -1;
            starting_time[o] = 0;
            ending_time[o] = 0;
        }
        
        return *this;
    }
    
    int getNumberOfJobs() const { return n_jobs;}
    int getNumberOfMachines() const { return n_machines;}
    int getNumberOfOperations() const { return n_operations;}
    int getMakespanMachine() const { return machine_makespan;}
    int getNumberOfOperationsAllocatedInJob(int job) const { return n_operations_allocated[job];}
    int getLastOperationAllocatedInJob(int job) const {return n_operations_allocated[job] - 1;}
    int getOperationAllocation(int operation) const {return operation_allocated_in[operation];}
    int getStartingTime(int operation) const {return starting_time[operation];}
    int getEndingTime(int operation) const {return ending_time[operation];}
    int getTimeOnMachine(int machine) const {return time_on_machine[machine];}
    int getWorkloadOnMachine(int machine) const { return workload_in_machine[machine];}
    int getMakespan() const{return makespan;}
    int getTotalWorkload() const {return total_workload;}
    int getMaxWorkload() const {return max_workload;}
    int getMinTotalWorkload() const{return min_total_workload;}
    
    int getBestWorkloadInMachine(int machine) const{return best_workloads_in_machine[machine];}
    int getTempBestWorkloadInMachine(int machine) const{return temp_best_wl_m[machine];}
    
    size_t getNumberOfOperationsAllocatedIn(int machine) const { return machine_allocations[machine].size();}
    const std::vector<std::deque<int> >& getMachineAllocations() const {return machine_allocations;}
    
    void setMakespanMachine(int machine) { machine_makespan = machine;}
    void setNumberOfOperationsAllocatedInJob(int job, int n_allocated) { n_operations_allocated[job] = n_allocated;}
    void setOperationAllocation(int job, int operation, int machine, int processing_time){
        
        total_workload += processing_time;
        workload_in_machine[machine] += processing_time;
        n_operations_allocated[job]++;
        operation_allocated_in[operation] = machine;
        machine_allocations[machine].push_back(operation);
        
    }
    
    void setStartingTime(int operation, int start_time) { starting_time[operation] = start_time;}
    void setEndingTime(int operation, int end_time) { ending_time[operation] = end_time;}
    void setTimeOnMachine(int machine, int time) {time_on_machine[machine] = time;}
    void setWorkloadOnMachine(int machine, int workload) { workload_in_machine[machine] = workload;}
    void setMakespan(int value){makespan = value;}
    void setTotalWorkload(int workload) { total_workload = workload;}
    void setMaxWorkload(int workload) { max_workload = workload;}
    
    void setTempBestWorkloadInMachine(int machine, int value){temp_best_wl_m[machine] = value;}
    void setBestWorkloadInMachine(int machine, int value){best_workloads_in_machine[machine] = value;}
    void setMinTotalWorkload(int value){min_total_workload = value;}
    
    void increaseTempWorkloadIn(int machine, int time){temp_best_wl_m[machine] += time;}
    void decreaseTempWorkloadIn(int machine, int time){temp_best_wl_m[machine] -= time;}
    
    void increaseOperationsAllocatedIn(int job){n_operations_allocated[job]++;}
    void decreaseOperationsAllocatedIn(int job){n_operations_allocated[job]--;}
    void increaseWorkloadIn(int machine, int time){workload_in_machine[machine] += time;}
    void decreaseWorkloadIn(int machine, int time){workload_in_machine[machine] -= time;}
    void increaseTimeOnMachine(int machine, int time){time_on_machine[machine] += time;}
    void decreaseTimeOnMachine(int machine, int time){time_on_machine[machine] -= time;}
    void increaseToltalWorkload(int time){ total_workload += time;}
    void decreaseToltalWorkload(int time){ total_workload -= time;}
    
    void deallocateOperation(int job, int operation){
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
    
    void reset(){
        total_workload = min_total_workload;
        max_workload = 0;
        
        for (int j = 0; j < n_jobs; ++j)
            n_operations_allocated[j] = 0;
        
        for (int m = 0; m < n_machines; ++m){
            time_on_machine[m] = 0;
            workload_in_machine[m] = 0;//best_workloads_machines[n_machines];
            temp_best_wl_m[m] = best_workloads_in_machine[m];
            machine_allocations[m].clear();
        }
        
        for (int o = 0; o < n_operations; ++o){
            operation_allocated_in[o] = -1;
            starting_time[o] = 0;
            ending_time[o] = 0;
        }
    }
    
    void print() const{
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
};

class ProblemFJSSP: public Problem {
    
private:
    //    const unsigned int MAX_GANTT_LIMIT = 4096;
    int n_jobs;
    int n_operations;
    int n_machines;
    int sumOfMinPij;
    int bestWorkloadFound;
    int bestMakespanFound;
    int * assignationMinPij;       /** Length equals to numberOfOperations. **/
    int * assignationBestWorkload; /** Length equals to numberOfOperations. **/
    int * assignationBestMakespan; /** Lenght equals to numberOfOperations. **/
    int * bestWorkloads;           /** Length equals to number of machines. **/
    int * minWorkload;             /** Length equals to number of machines. **/
    int * n_operations_in_job;     /** Length equals to number of Jobs. **/
    int * releaseTime;             /** Length equals to number of Jobs. **/
    int * operationIsFromJob;      /** Length equals to numberOfOperations. **/
    int ** processingTime;         /** Length equals to numberOfOperations x numberOfMachines. **/
    int ** mapToJobMachine;        /** Length equals to numberOfMaps x 2. Positio 0 is the job, position 1 is the machine.**/
    int ** jobMachineToMap;        /** Length equals to numberOfJobs x numberOfMachines. **/
    int ** jobOperationHasNumber;  /** Length equals to job x numberOfOperationsInJob. **/
    
    int * earliest_starting_time;  /** Length equals to number of operations. **/
    int * earliest_ending_time;    /** Length equals to number of operations. **/
    int * eet_of_job;              /** Length equals to number of jobs. **/
    int * sum_shortest_proc_times; /** D^{k}_{Ñ}. Length equals to number of machines.**/
    int avg_op_per_machine;        /** Ñ parameter (N tilde).  **/
    int min_sum_shortest_proc_times; /** D_{{k_0}, Ñ}. **/
    int max_eet_of_jobs;            /** Maximum earliest ending time of jobs. **/
    int sum_M_smallest_est;         /** R_MSum of the M smallest starting times (est). **/
    int bestBound_maxWorkload;
    int bestBound_makespan;
public:
    ProblemFJSSP();
    ProblemFJSSP(const ProblemFJSSP& toCopy);
    ProblemFJSSP(int totalObjectives, int totalVariables);
    ProblemFJSSP(const Payload_problem_fjssp& payload_problem);
    
    ~ProblemFJSSP();
    
    ProblemFJSSP& operator=(const ProblemFJSSP& toCopy);
    
    double evaluate(Solution & solution);
    double evaluatePartial(Solution & solution, int levelEvaluation);
    double evaluateLastLevel(Solution * solution);
    double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel);
    double removeLastLevelEvaluation(Solution * solution, int newLevel);
    void evaluateDynamic(Solution & solution, FJSSPdata& data, int level);
    void evaluateRemoveDynamic(Solution & solution, FJSSPdata& data, int level);
    void createDefaultSolution(Solution & solution);
    void getSolutionWithLowerBoundInObj(int nObj, Solution& solution);
    int getLowerBound(int indexVar) const;
    int getUpperBound(int indexVar) const;
    ProblemType getType() const;
    int getStartingRow();
    int getFinalLevel();
    int * getElemensToRepeat();
    int getTotalElements();
    int getDecodeMap(int map, int position);
    int getCodeMap(int value1, int value2);
    int getTimesValueIsRepeated(int value);
    
    Solution goodSolutionWithMaxWorkload;
    void updateBestMaxWorkloadSolution(FJSSPdata& data);
    void updateBestMakespanSolution(FJSSPdata& data);
    void updateBestMakespanSolutionWith(const Solution& solution);
    void updateBestMaxWorkloadSolutionWith(const Solution& solution);
    
    int getNumberOfJobs() const;
    int getNumberOfOperations() const;
    int getNumberOfMachines() const;
    int getSumOfMinPij() const;
    int getBestWorkloadFound() const;
    int getBestMakespanFound() const;
    int getAssignationMinPij(int n_operation) const;
    int getAssignationBestWorkload(int n_operation) const;
    int getAssignationBestMakespan(int n_opeartion) const;
    int getBestWorkload(int n_machine) const;
    int getMinWorkload(int n_machinen) const;
    int getMapOfJobMachine(int job, int machine) const;
    int getJobMachineToMap(int job, int machine) const;
    int getOperationInJobIsNumber(int job, int operation) const;
    int getOperationIsFromJob(int n_operation) const;
    int getProccessingTime(int operation, int machine) const;
    int getNumberOfOperationsInJob(int job) const;
    int getReleaseTimeOfJob(int job) const;
    int getLowerBoundInObj(int nObj) const;
    int getBestBoundMaxWorkload() const;
    int getBestBoundMakespan() const;
    int getEarliestStartingTime(int nOperation) const;
    int getEarliestEndingTime(int nOperation) const;
    int getEarliestEndingJobTime(int nObj) const;
    int getSumShortestProcTimeInMachine(int nMachine) const;
    int getAvgOperationPerMachine() const;
    int getMinSumShortestProcTime() const;
    int getMaxEarliestEndingTime() const;
    int getSumOf_M_smallestEST() const;
    
    void setEarliestStartingTime(int nOperation, int nValue);
    void setEarliestEndingTime(int nOperation, int nValue);
    void setEarliestEndingJobTime(int nObj, int nValue);
    void setSumShortestProcTimeInMachine(int nMachine, int nValue);
    void setAvgOperationPerMachine(int nValue);
    void setMinShortestProcTime(int nValue);
    void setMaxEarliestEndingTime(int nValue);
    void setSumOf_M_smallestEST(int nValue);
    
    double evaluatePartialTest4(Solution & solution, int currentLevel);
    
    void printSolution(const Solution & solution) const;
    void printPartialSolution(const Solution & solution, int level) const;
    void printSolutionInfo(const Solution & solution) const;
    
    void loadInstance(char path[2][255], char file_extension[10]);
    void loadInstanceFJS(char path[2][255], char file_extension[10]);
    void loadInstanceTXT(char path[2][255], char file_extension[10]);
    void loadInstancePayload(const Payload_problem_fjssp& payload);
    void printInstance();
    void printProblemInfo() const;
    void printSchedule(const Solution & solution) const;
    void buildSolutionWithGoodMaxWorkload(Solution & solution);
    void buildSolutionWithGoodMaxWorkloadv2(Solution & solution);
    int e_function(double value) const;
    
};

#endif /* ProblemFJSSP_hpp */
