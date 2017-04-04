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
    
    
    ProblemHCSP(const ProblemHCSP& toCopy);
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
    
    ProblemHCSP& operator=(const ProblemHCSP& toCopy);
    
    double evaluate(Solution & solution);
    double evaluatePartial(Solution & solution, int levelEvaluation);
    double evaluateLastLevel(Solution * solution);
    double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel);
    double removeLastLevelEvaluation(Solution * solution, int newLevel);
    
    void createDefaultSolution(Solution & solution);
    void getSolutionWithLowerBoundInObj(int nObj, Solution& solution);

    int getLowerBound(int indexVar) const;
    int getUpperBound(int indexVar) const;
    int getLowerBoundInObj(int nObj) const;

    ProblemType getType() const;
    int getStartingLevel();
    int getFinalLevel();
    
    int getDecodeMap(int map, int position);
    int getCodeMap(int value1, int value2);
    int getTimesValueIsRepeated(int value);
    
    int * getElemensToRepeat();
    int getTotalElements();
        
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
    
    void printSolution(const Solution & solution) const;
    void printPartialSolution(const Solution & solution, int level) const;
    void printSolutionInfo(const Solution & solution) const;
    void printInstance();
    void printProblemInfo() const;
};
#endif /* ProblemHCSP_hpp */
