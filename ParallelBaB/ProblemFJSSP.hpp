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
#include <fstream>
#include <string>
#include <math.h>
#include <regex>
#include "Problem.hpp"
#include "myutils.hpp"

//#define MAKESPAN  0;
//#define MAXWORKLOAD 1;
//#define TOTALWORKLOAD 2;
//enum FJSSPObj{MAKESPAN = 0, MAXWORKLOAD = 1, TOTALWORKLOAD = 2};

class ProblemFJSSP: public Problem {
public:
    
    ProblemFJSSP(int totalObjectives, int totalVariables):Problem(totalObjectives, totalVariables){
        this->totalJobs = 0;
        this->totalOperations = 0;
        this->totalMachines = 0;
    };
   
    ~ProblemFJSSP();
    
    double evaluate(Solution * solution);
    double evaluatePartial(Solution * solution, int levelEvaluation);
    double evaluateLastLevel(Solution * solution);
    double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel);
    double removeLastLevelEvaluation(Solution * solution, int newLevel);
    
    void createDefaultSolution(Solution * solution);
    Solution * getSolutionWithLowerBoundInObj(int nObj);
    void printSolution(Solution * solution);
    void printPartialSolution(Solution * solution, int level);
    void printSolutionInfo(Solution * solution);
    
    int getLowerBound(int indexVar);
    int getUpperBound(int indexVar);
    ProblemType getType();
    int getStartingLevel();
    int getFinalLevel();
    
    int * getElemensToRepeat();
    int getTotalElements();
    
    
    
    Solution* createSolution();
    Solution* goodSolutionWithMaxWorkload;
    
    
    /**FJSSP functions**/
    
    /** The Soluion's representation is a vector:
     Order
     Of
     Ejecution
     | [ 0] = Job
     | [ 1] = Machine
     | [ 2] = Job
     | [ 3] = Machine
     | [ 4] = Job
     | [ 5] = Machine
     | [ 6] = Job
     | [ 7] = Machine
     | [ 8] = Job
     | [ 9] = Machine
     | [10] = Job
     | [11] = Machine
     | [12] = Job
     | [13] = Machine
     | [14] = Job
     | [15] = Machine
     | [16] = Job
     V [17] = Machine
     **/
    
    
    int totalJobs;
    int totalOperations;
    int totalMachines;
    int sumOfMinPij;
    int bestWorkloadFound;
    int * assignationMinPij;
    int * assignationBestWorkload;
    int * bestWorkloads;
    int * minWorkload;

    
    
    /** jobaHasNoperations saids how many operations have each job.
     if we have 2 jobs with 3 operations and 1 job with 2 operations.
     Job 0 has 3 operations: [0] = 3
     Job 1 has 3 operations: [1] = 3
     Job 2 has 2 operations: [2] = 2
     **/
    int * jobHasNoperations;
    int * releaseTime;
    
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
    int ** operationInJobIsNumber;

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
    int * operationIsFromJob;
    
    int ** processingTime;
    void loadInstance(char* path[]);

    void printInstance();
    void printProblemInfo();
    void printSchedule(Solution * solution);
    
    double evaluatePartialTest3(Solution * solution, int currentLevel);

    int getLowerBoundInObj(int nObj);
    void buildSolutionWithGoodMaxWorkload(Solution * solution);
    void buildSolutionWithGoodMaxWorkloadv2(Solution * solution);

};

#endif /* ProblemFJSSP_hpp */
