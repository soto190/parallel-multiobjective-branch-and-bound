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
    
    int getLowerBound(int indexVar);
    int getUpperBound(int indexVar);
    int getType();
    int getStartingLevel();
    int getFinalLevel();
    int * getElemensToRepeat();
    
    Solution* createSolution();
    
    /**FJSSP functions**/
    
    
    int totalJobs;
    int totalOperations;
    int totalMachines;

    int * operationsInJob;
    int ** jobOperations;
    int ** processingTime;

    void loadInstance(char* path[]);

    void printInstance();
    void printProblemInfo();
    double printSchedule(Solution * solution);


};

#endif /* ProblemFJSSP_hpp */
