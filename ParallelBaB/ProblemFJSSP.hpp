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
#include <vector>
#include <deque>
#include <fstream>
#include <string>
#include <math.h>
#include <regex>
#include "FJSSPdata.hpp"
#include "Problem.hpp"
#include "myutils.hpp"

#define INF_PROC_TIME 9999999
#define MAX_GANTT_LIMIT 262144

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

enum FJSSPobjectives {MAKESPAN = 0, MAX_WORKLOAD = 1, TOTAL_WORKLOAD =2};

class ProblemFJSSP: public Problem {
private:
    //    const unsigned int MAX_GANTT_LIMIT = 4096;
    int n_jobs;
    int n_operations;
    int n_machines;
    int sum_of_min_Pij;
    int best_workload_found;
    int best_makespan_found;
    int * assignation_min_Pij;       /** Length equals to numberOfOperations. **/
    int * assignation_best_max_workload; /** Length equals to numberOfOperations. **/
    int * assignation_best_makespan; /** Lenght equals to numberOfOperations. **/
    int * best_workloads;           /** Length equals to number of machines. **/
    int * min_workloads;             /** Length equals to number of machines. **/
    int * n_operations_in_job;     /** Length equals to number of Jobs. **/
    int * release_time;             /** Length equals to number of Jobs. **/
    int * operation_belongs_to_job;      /** Length equals to numberOfOperations. **/
    int ** processing_time;         /** Length equals to numberOfOperations x numberOfMachines. **/
    int ** decode_map_to_job_machine;        /** Length equals to numberOfMaps x 2. Positio 0 is the job, position 1 is the machine.**/
    int ** encode_job_machine_to_map;        /** Length equals to numberOfJobs x numberOfMachines. **/
    int ** job_operation_is_number;  /** Length equals to job x numberOfOperationsInJob. **/
    int * earliest_starting_time;  /** Length equals to number of operations. **/
    int * earliest_ending_time;    /** Length equals to number of operations. **/
    int * eet_of_job;              /** Length equals to number of jobs. **/
    int * sum_shortest_proc_times; /** D^{k}_{Ñ}. Length equals to number of machines.**/
    int avg_op_per_machine;        /** Ñ parameter (N tilde).  **/
    int min_sum_shortest_proc_times; /** D_{{k_0}, Ñ}. **/
    int max_eet_of_jobs;            /** Maximum earliest ending time of jobs. **/
    int sum_M_smallest_est;         /** R_MSum of the M smallest starting times (est). **/
    int best_bound_max_workload;
    int best_bound_makespan;
    
    vector<vector<int>> machines_aviability;
    
public:
    Solution goodSolutionWithMaxWorkload;

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
    int getDecodeMap(int code, int parameter) const;
    int getEncodeMap(int parameter1, int parameter2) const;
    int getTimesThatValueCanBeRepeated(int value);
    
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
    unsigned long getNumberOfMachinesAvaibleForOperation(unsigned long nOperation) const;
    int getMachinesAvaibleForOperation(unsigned long nOperation, unsigned long machine) const;

    bool operationCanBeAllocatedInMachine(int operation, int machine) const;
    
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
    bool validateVariablesOf(const Solution& solution) const;
};
#endif /* ProblemFJSSP_hpp */
