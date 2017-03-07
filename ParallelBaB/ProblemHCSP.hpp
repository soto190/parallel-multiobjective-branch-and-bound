//
//  ProblemHCSP.hpp
//  PhDProject
//
//  Created by Carlos Soto on 21/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#ifndef ProblemHCSP_hpp
#define ProblemHCSP_hpp

#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <math.h>
#include <regex>
#include "Problem.hpp"
#include "myutils.hpp"

//enum HCSPObj{MAKESPAN = 0, ENERGY = 0};

class ProblemHCSP: public Problem {
public:
    
    ProblemHCSP(int totalObjectives, int totalVariables):Problem(totalObjectives, totalVariables){
        this->totalTasks = 0;
        this->totalMachines = 0;
        this->totalConfig = 0;
        this->totalMappings = 0;

        this->mappingConfig = new int * [1];
        this->maxConfigIn = new int[1];
        this->processingTime = new double * [1];
        this->voltage = new double * [1];
        this->speed = new double * [1];

        this->totalTasks = 0;
        this->totalMachines = 0;
        this->totalConfig = 0;
        this->totalMappings = 0;
    };
    ~ProblemHCSP();
    
    double evaluate(Solution & solution);
    double evaluatePartial(Solution & solution, int levelEvaluation);
    double evaluateLastLevel(Solution * solution);
    double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel);
    double removeLastLevelEvaluation(Solution * solution, int newLevel);
    
    void createDefaultSolution(Solution * solution);
    Solution * getSolutionWithLowerBoundInObj(int nObj);

    void printSolution(Solution * solution);
    void printPartialSolution(Solution * solution, int level);

    int getLowerBound(int indexVar);
    int getUpperBound(int indexVar);
    int getLowerBoundInObj(int nObj);

    ProblemType getType();
    int getStartingLevel();
    int getFinalLevel();
    
    int getMapping(int map, int position);
    int getMappingOf(int value1, int value2);
    int getTimesValueIsRepeated(int value);
    
    int * getElemensToRepeat();
    int getTotalElements();
    
    Solution* createSolution();
    
    /**HCSP functions**/
    
    int ** mappingConfig;
    int * maxConfigIn;
    double ** processingTime;
    double ** voltage;
    double ** speed;
    
    int totalTasks;
    int totalMachines;
    int totalConfig;
    int totalMappings;
    
    void loadInstance(char* path[]);
    void readMachinesConfigurations(char* path);
    
    double computeProcessingTime(int task, int machine, int config);
    double computeEnergy(int task, int machine, int config, double proc_time);
    
    void printInstance();
    void printProblemInfo();
    void printSolutionInfo(Solution * solution);

    
};
#endif /* ProblemHCSP_hpp */
