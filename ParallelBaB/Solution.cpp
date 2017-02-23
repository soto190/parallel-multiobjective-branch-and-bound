//
//  Solution.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "Solution.hpp"


/**
 * The class Problem have the information about the number of objectives and variables.
 *
 *
 */
Solution::Solution(){
}

Solution::Solution(int numberOfObjectives, int numberOfVariables){
    this->makespan = 0;
    this->energy = 0;
    this->machineWithMakespan = 0;
    this->totalObjectives = numberOfObjectives;
    this->totalVariables = numberOfVariables;
    
    this->objective = new double[numberOfObjectives];
    this->variable = new int[numberOfVariables];
    this->execTime = new double[16];
    this->partialObjective = new double * [numberOfVariables];
    
    int var = 0, obj = 0;
    for (obj = 0; obj < numberOfObjectives; obj++)
        this->objective[obj] = 0;
    
    for (var = 0; var < numberOfVariables; var++){
        this->variable[var] = 0;
        this->partialObjective[var] = new double[numberOfObjectives];
        for (obj = 0; obj < numberOfObjectives; obj++)
            this->partialObjective[var][obj] = 0;
    }
    
    for (var = 0; var < 16; var++)
        this->execTime[var] = 0;
}

Solution::Solution(const Solution &solution){
    
    this->build_up_to = solution.getBuildUpTo();
    this->totalObjectives = solution.getNumberOfObjectives();
    this->totalVariables = solution.getNumberOfVariables();
    
    this->objective = new double[this->totalObjectives];
    this->variable = new int[this->totalVariables];
    this->partialObjective = new double * [this->totalVariables];
    this->execTime = new double[16];
    
    int var = 0, obj = 0;
    
    for (obj = 0; obj < this->totalObjectives; obj++)
        this->objective[obj] = solution.getObjective(obj);
    
    for (var = 0; var < this->totalVariables; var++){
        this->variable[var] = solution.getVariable(var);
        this->partialObjective[var] = new double[this->totalObjectives];
        for (obj = 0; obj < this->totalObjectives; obj++)
            this->partialObjective[var][obj] = solution.partialObjective[var][obj];
    }
    
    for (var = 0; var < 16; var++)
        this->execTime[var] = solution.execTime[var];
    
}

Solution::~Solution(){
    delete [] objective;
    delete [] variable;
    delete [] execTime;
    int index = 0;
    for (index = 0; index < this->totalVariables; index++)
        delete [] this->partialObjective[index];
    
    delete [] this->partialObjective;
}

void Solution::setObjective(int index, double value){
    this->objective[index] = value;
}

void Solution::setVariable(int index, int value){
    this->variable[index] = value;
}

void Solution::setPartialObjective(int var, int objective, double value){
    this->partialObjective[var][objective] = value;
}

int Solution::getVariable(int index) const{
    return this->variable[index];
}

double Solution::getObjective(int index) const{
    return this->objective[index];
}

int Solution::getNumberOfVariables() const{
    return this->totalVariables;
}

int Solution::getNumberOfObjectives() const{
    return this->totalObjectives;
}

int Solution::getBuildUpTo() const{
    return this->build_up_to;
}

int Solution::dominates(const Solution & solution) const{
    int nObj = 0;
    int localSolIsBetterIn = 0;
    int exterSolIsBetterIn = 0;
    int equals = 1;
    
    /**
     * For more objectives consider
     * if (solAIsBetterIn > 0 and solBIsBetterIn > 0) break the FOR because the solutions are non-dominated.
     **/
    for (nObj = 0; nObj < this->totalObjectives; nObj++) {
        double objA = this->getObjective(nObj);
        double objB = solution.getObjective(nObj);
        
        if(objA < objB){
            localSolIsBetterIn++;
            equals = 0;
        }
        else if(objB < objA){
            exterSolIsBetterIn++;
            equals = 0;
        }
    }
    
    if(equals == 1)
        return 11;
    else if (localSolIsBetterIn > 0 && exterSolIsBetterIn == 0)
        return 1;
    else if (exterSolIsBetterIn > 0 && localSolIsBetterIn == 0)
        return -1;
    else
        return 0;
}

Solution& Solution::operator()(int numberOfObjectives, int numberOfVariables){
    this->makespan = 0;
    this->energy = 0;
    this->machineWithMakespan = 0;
    this->totalObjectives = numberOfObjectives;
    this->totalVariables = numberOfVariables;
    
    this->objective = new double[numberOfObjectives];
    this->variable = new int[numberOfVariables];
    this->execTime = new double[16];
    this->partialObjective = new double * [numberOfVariables];
    
    int var = 0, obj = 0;
    for (obj = 0; obj < numberOfObjectives; obj++)
        this->objective[obj] = 0;
    
    for (var = 0; var < numberOfVariables; var++){
        this->variable[var] = 0;
        this->partialObjective[var] = new double[numberOfObjectives];
        for (obj = 0; obj < numberOfObjectives; obj++)
            this->partialObjective[var][obj] = 0;
    }
    
    for (var = 0; var < 16; var++)
        this->execTime[var] = 0;
    
    return *this;
}

Solution& Solution::operator=(const Solution &solution){
    
    this->build_up_to = solution.getBuildUpTo();
    this->totalObjectives = solution.getNumberOfObjectives();
    this->totalVariables = solution.getNumberOfVariables();
    
    this->objective = new double[this->totalObjectives];
    this->variable = new int[this->totalVariables];
    this->execTime = new double[16];
    this->partialObjective = new double * [this->totalVariables];
    
    int var = 0, obj = 0;
    
    for (obj = 0; obj < this->totalObjectives; obj++)
        this->objective[obj] = solution.getObjective(obj);
    
    for (var = 0; var < this->totalVariables; var++){
        this->variable[var] = solution.getVariable(var);
        this->partialObjective[var] = new double[this->totalObjectives];
        for (obj = 0; obj < this->totalObjectives; obj++)
            this->partialObjective[var][obj] = solution.partialObjective[var][obj];
    }
    
    /**Section for the HCSP problem**/
    for (var = 0; var < 16; var++)
        this->execTime[var] = solution.execTime[var];
    
    return *this;
}

void Solution::print() const{
    int nObj, nVar;
    for (nObj = 0; nObj < this->totalObjectives; nObj++)
        printf("%f ", this->getObjective(nObj));
    
    printf("| ");
        
    for (nVar = 0; nVar < this->totalVariables; nVar++)
            printf("%d ", this->getVariable(nVar));
    
    printf("|\n");
}

void Solution::printObjectives() const{
    int nObj;
    for (nObj = 0; nObj < this->totalObjectives; nObj++)
        printf("%f ", this->getObjective(nObj));
    
    printf("\n");
}

void Solution::printVariables() const{
    int nVar;
    for (nVar = 0; nVar < this->totalVariables; nVar++)
        printf("%d ", this->getVariable(nVar));
    
    printf("\n");
}
