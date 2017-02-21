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
    
    int index = 0, index_in = 0;
    for (index = 0; index < numberOfObjectives; index++)
        this->objective[index] = 0;
    
    for (index = 0; index < numberOfVariables; index++){
        this->variable[index] = 0;
        this->partialObjective[index] = new double[numberOfObjectives];
        for (index_in = 0; index_in < numberOfObjectives; index_in++)
            this->partialObjective[index][index_in] = 0;
    }
    
    for (index = 0; index < 16; index++)
        this->execTime[index] = 0;
}

Solution::Solution(const Solution &solution){
    
    this->build_up_to = solution.build_up_to;
    this->totalObjectives = solution.totalObjectives;
    this->totalVariables = solution.totalVariables;
    
    this->objective = new double[this->totalObjectives];
    this->variable = new int[this->totalVariables];
    this->partialObjective = new double * [solution.totalVariables];
    this->execTime = new double[16];
    
    int index = 0, index_in = 0;
    
    for (index = 0; index < this->totalObjectives; index++)
        this->objective[index] = solution.objective[index];
    
    for (index = 0; index < this->totalVariables; index++){
        this->variable[index] = solution.variable[index];
        this->partialObjective[index] = new double[solution.totalObjectives];
        for (index_in = 0; index_in < solution.totalObjectives; index_in++)
            this->partialObjective[index][index_in] = solution.partialObjective[index][index_in];
    }
    
    for (index = 0; index < 16; index++)
        this->execTime[index] = solution.execTime[index];
    
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

int Solution::getVariable(int index){
    return this->variable[index];
}

double Solution::getObjective(int index){
    return this->objective[index];
}

int Solution::dominates(const Solution & solution){
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
        double objB = solution.objective[nObj];
        
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
    
    int index = 0, index_in = 0;
    for (index = 0; index < numberOfObjectives; index++)
        this->objective[index] = 0;
    
    for (index = 0; index < numberOfVariables; index++){
        this->variable[index] = 0;
        this->partialObjective[index] = new double[numberOfObjectives];
        for (index_in = 0; index_in < numberOfObjectives; index_in++)
            this->partialObjective[index][index_in] = 0;
    }
    
    for (index = 0; index < 16; index++)
        this->execTime[index] = 0;
    
    return *this;
}

Solution& Solution::operator=(const Solution &solution){
    
    this->build_up_to = solution.build_up_to;

    this->totalObjectives = solution.totalObjectives;
    this->totalVariables = solution.totalVariables;
    
    this->objective = new double[solution.totalObjectives];
    this->variable = new int[solution.totalVariables];
    
    int index = 0, index_in = 0;
    
    for (index = 0; index < this->totalObjectives; index++)
        this->objective[index] = solution.objective[index];
    
    
    for (index = 0; index < this->totalVariables; index++){
        this->variable[index] = solution.variable[index];
        this->partialObjective[index] = new double[solution.totalObjectives];
        for (index_in = 0; index_in < solution.totalObjectives; index_in++)
            this->partialObjective[index][index_in] = solution.partialObjective[index][index_in];
    }
    
    /**Section for the HCSP problem**/
    this->execTime = new double[16];
    for (index = 0; index < 16; index++)
        this->execTime[index] = solution.execTime[index];
    
    return *this;
}

void Solution::print(){
    int nObj, nVar;
    for (nObj = 0; nObj < this->totalObjectives; nObj++)
        printf("%f ", this->getObjective(nObj));
    
    printf("| ");
        
    for (nVar = 0; nVar < this->totalVariables; nVar++)
            printf("%d ", this->getVariable(nVar));
    
    printf("|\n");
}

void Solution::printObjectives(){
    int nObj;
    for (nObj = 0; nObj < this->totalObjectives; nObj++)
        printf("%f ", this->getObjective(nObj));
    
    printf("\n");
}

void Solution::printVariables(){
    int nVar;
    for (nVar = 0; nVar < this->totalVariables; nVar++)
        printf("%d ", this->getVariable(nVar));
    
    printf("\n");
}
