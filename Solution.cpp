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
Solution::Solution(int totalObjectives, int totalVariables){
    this->makespan = 0;
    this->energy = 0;
    this->machineWithMakespan = 0;
    this->totalObjectives = totalObjectives;
    this->totalVariables = totalVariables;
    
    this->objective = new double[totalObjectives];
    this->variable = new int[totalVariables];
    this->execTime = new double[16];
    int index;
    
    for (index = 0; index < totalObjectives; index++)
        Solution::objective[index] = 0;
    
    for (index = 0; index < totalVariables; index++)
        Solution::variable[index] = 0;
    
    for (index = 0; index < 16; index++)
        this->execTime[index] = 0;
}

Solution::Solution(const Solution &solution){
    
    this->totalObjectives = solution.totalObjectives;
    this->totalVariables = solution.totalVariables;
    
    this->objective = new double[solution.totalObjectives];
    this->variable = new int[solution.totalVariables];
    this->execTime = new double[16];
    
    int index = 0;
    
    for (index = 0; index < this->totalObjectives; index++)
        this->objective[index] = solution.objective[index];
    
    for (index = 0; index < this->totalVariables; index++)
        this->variable[index] = solution.variable[index];
    
    for (index = 0; index < 16; index++)
        this->execTime[index] = solution.execTime[index];
    
}

Solution::~Solution(){
    delete [] objective;
    delete [] variable;
    delete [] execTime;
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
