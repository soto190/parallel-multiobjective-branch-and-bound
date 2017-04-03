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
Solution::Solution():totalObjectives(0), totalVariables(0) {
    makespan = 0;
	energy = 0;
	machineWithMakespan = 0;
    build_up_to = -1;
    objective = nullptr;
    variable = nullptr;
    execTime = nullptr;
    partialObjective = nullptr;
}

Solution::Solution(int numberOfObjectives, int numberOfVariables):
    totalObjectives(numberOfObjectives),
    totalVariables(numberOfVariables),
    build_up_to(-1),
    makespan(0),
    energy(0),
    machineWithMakespan(0),
    objective(new double[numberOfObjectives]),
    variable(new int[numberOfVariables]),
    execTime(new double[16]),
    partialObjective(new double * [numberOfVariables]){
    
	int var = 0, obj = 0;
	for (obj = 0; obj < numberOfObjectives; ++obj)
		objective[obj] = 0;

	for (var = 0; var < numberOfVariables; ++var) {
		variable[var] = -1;
		partialObjective[var] = new double[numberOfObjectives];
		for (obj = 0; obj < numberOfObjectives; ++obj)
			partialObjective[var][obj] = 0;
	}

	for (var = 0; var < 16; ++var)
		execTime[var] = 0;
}

Solution::Solution(const Solution &solution):
    totalObjectives(solution.getNumberOfObjectives()),
    totalVariables(solution.getNumberOfVariables()),
    build_up_to(solution.getBuildUpTo()),
    makespan(solution.makespan),
    energy(solution.energy),
    machineWithMakespan(solution.machineWithMakespan){
    
	objective = new double[totalObjectives];
	variable = new int[totalVariables];
	partialObjective = new double *[totalVariables];
	execTime = new double[16];

	int var = 0, obj = 0;

	for (obj = 0; obj < totalObjectives; ++obj)
		objective[obj] = solution.getObjective(obj);

	for (var = 0; var < totalVariables; ++var) {
		variable[var] = solution.getVariable(var);
		partialObjective[var] = new double[totalObjectives];
		for (obj = 0; obj < totalObjectives; ++obj)
            partialObjective[var][obj] = solution.getPartialObjective(var, obj);
	}

	for (var = 0; var < 16; ++var)
		execTime[var] = solution.execTime[var];
}

Solution& Solution::operator()(int numberOfObjectives, int numberOfVariables) {
    makespan = 0;
    energy = 0;
    machineWithMakespan = 0;
    build_up_to = -1;
    totalObjectives = numberOfObjectives;
    totalVariables = numberOfVariables;
    
    /** Freeing previously used memory. **/
    if(objective != nullptr){
        delete[] objective;
        delete[] variable;
        delete[] execTime;
        int index = 0;
        for (index = 0; index < totalVariables; ++index)
            delete[] partialObjective[index];
        delete[] partialObjective;
    }
    
    objective = new double[numberOfObjectives];
    variable = new int[numberOfVariables];
    execTime = new double[16];
    partialObjective = new double *[numberOfVariables];
    
    int var = 0, obj = 0;
    for (obj = 0; obj < numberOfObjectives; ++obj)
        objective[obj] = 0;
    
    for (var = 0; var < numberOfVariables; ++var) {
        variable[var] = -1;
        partialObjective[var] = new double[numberOfObjectives];
        for (obj = 0; obj < numberOfObjectives; ++obj)
            partialObjective[var][obj] = 0;
    }
    
    for (var = 0; var < 16; ++var)
        execTime[var] = 0;
    
    return *this;
}

Solution& Solution::operator=(const Solution &solution) {
    
    if (this == &solution) return *this;
    
    build_up_to = solution.getBuildUpTo();
    totalObjectives = solution.getNumberOfObjectives();
    totalVariables = solution.getNumberOfVariables();
    
    /** Freeing previously used memory. **/
    if (objective != nullptr) {
        delete[] objective;
        delete[] variable;
        delete[] execTime;
        int index = 0;
        for (index = 0; index < totalVariables; ++index)
            delete[] partialObjective[index];
        delete[] partialObjective;
    }
    
    objective = new double[totalObjectives];
    variable = new int[totalVariables];
    execTime = new double[16];
    partialObjective = new double *[totalVariables];
    
    int var = 0, obj = 0;
    
    for (obj = 0; obj < totalObjectives; ++obj)
        objective[obj] = solution.getObjective(obj);
    
    for (var = 0; var < totalVariables; ++var) {
        variable[var] = solution.getVariable(var);
        partialObjective[var] = new double[totalObjectives];
        for (obj = 0; obj < totalObjectives; ++obj)
            partialObjective[var][obj] =
            solution.partialObjective[var][obj];
    }
    
    /**Section for the HCSP problem**/
    for (var = 0; var < 16; ++var)
        execTime[var] = solution.execTime[var];
    
    return *this;
}

int Solution::operator==(const Solution &solution){
    int index = 0;
    
    for (index = 0; index < totalObjectives; ++index)
        if (objective[index] < solution.getObjective(index))
            return 0;
    
    return 1;
}

Solution::~Solution() {
	delete[] objective;
	delete[] variable;
	delete[] execTime;
	int index = 0;
	for (index = 0; index < totalVariables; ++index)
		delete[] partialObjective[index];

	delete[] partialObjective;
}

void Solution::setObjective(int index, double value) { objective[index] = value; }
int Solution::setVariable(int index, int value) {
    int last_value = variable[index];
    variable[index] = value;
    return last_value;
}
void Solution::setPartialObjective(int var, int objective, double value) { partialObjective[var][objective] = value; }
double Solution::getObjective(int index) const { return objective[index]; }
int Solution::getVariable(int index) const { return variable[index]; }
int Solution::getNumberOfVariables() const { return totalVariables; }
int Solution::getNumberOfObjectives() const { return totalObjectives; }
int Solution::getPartialObjective(int var, int objective) const{ return partialObjective[var][objective]; }
int Solution::getBuildUpTo() const { return build_up_to; }

DominanceRelation Solution::dominates(const Solution & solution) const {
	int nObj = 0;
	int localSolIsBetterIn = 0;
	int exterSolIsBetterIn = 0;
	int equals = 1;
    double objA = 0;
    double objB = 0;

	/**
	 * For more objectives consider
	 * if (solAIsBetterIn > 0 and solBIsBetterIn > 0) break the FOR because the solutions are non-dominated.
	 **/
	for (nObj = 0; nObj < totalObjectives; ++nObj) {
		objA = getObjective(nObj);
		objB = solution.getObjective(nObj);

		if (objA < objB) {
			localSolIsBetterIn++;
			equals = 0;
		} else if (objB < objA) {
			exterSolIsBetterIn++;
			equals = 0;
		}
	}

	if (equals == 1)
        return DominanceRelation::Equals;
	else if (localSolIsBetterIn > 0 && exterSolIsBetterIn == 0)
        return DominanceRelation::Dominates;
	else if (exterSolIsBetterIn > 0 && localSolIsBetterIn == 0)
        return DominanceRelation::Dominated;
	else
        return DominanceRelation::Nondominated;
}

void Solution::print() const {
	int nObj, nVar;
	for (nObj = 0; nObj < totalObjectives; ++nObj)
		printf("%f ", getObjective(nObj));

	printf("| ");

	for (nVar = 0; nVar < totalVariables; ++nVar)
		printf("%d ", getVariable(nVar));

	printf("|\n");
}

void Solution::printObjectives() const {
	int nObj;
	for (nObj = 0; nObj < totalObjectives; ++nObj)
		printf("%f ", getObjective(nObj));

	printf("\n");
}

void Solution::printVariables() const {
	int nVar;
	for (nVar = 0; nVar < totalVariables; ++nVar)
		printf("%d ", getVariable(nVar));

	printf("\n");
}
