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
Solution::Solution():
n_objectives(0),
n_variables(0) {
	machineWithMakespan = 0;
    build_up_to = -1;
    objective = nullptr;
    variable = nullptr;
    execTime = nullptr;
}

<<<<<<< HEAD
Solution::Solution(int numberOfObjectives, int numberOfVariables):
n_objectives(numberOfObjectives),
n_variables(numberOfVariables),
build_up_to(-1),
machineWithMakespan(0),
objective(new double[numberOfObjectives]),
variable(new int[numberOfVariables]),
execTime(new double[16]){
    
	int var = 0, obj = 0;
	for (obj = 0; obj < numberOfObjectives; ++obj)
		objective[obj] = 0;

	for (var = 0; var < numberOfVariables; ++var)
		variable[var] = -1;
=======
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
    
    int obj = 0, var = 0;
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
    
    this->build_up_to = solution.build_up_to;
    this->totalObjectives = solution.totalObjectives;
    this->totalVariables = solution.totalVariables;
    
    this->objective = new double[this->totalObjectives];
    this->variable = new int[this->totalVariables];
    this->partialObjective = new double * [solution.totalVariables];
    this->execTime = new double[16];
    
    int obj = 0, var = 0;
    
    for (obj = 0; obj < this->totalObjectives; obj++)
        this->objective[obj] = solution.objective[obj];
    
    for (var = 0; var < this->totalVariables; var++){
        this->variable[var] = solution.variable[obj];
        this->partialObjective[var] = new double[solution.totalObjectives];
        for (obj = 0; obj < solution.totalObjectives; obj++)
            this->partialObjective[var][obj] = solution.partialObjective[var][obj];
    }
    
    for (var = 0; var < 16; var++)
        this->execTime[var] = solution.execTime[var];
>>>>>>> fe0afa36c3446ca25368797e2d3be5e42c96f31f
    
	for (var = 0; var < 16; ++var)
		execTime[var] = 0;
}

<<<<<<< HEAD
Solution::Solution(const Solution &solution):
n_objectives(solution.getNumberOfObjectives()),
n_variables(solution.getNumberOfVariables()),
build_up_to(solution.getBuildUpTo()),
machineWithMakespan(solution.machineWithMakespan){
    
        objective = new double[n_objectives];
        variable = new int[n_variables];
        execTime = new double[16];
        
        int var = 0, obj = 0;
        
        for (obj = 0; obj < n_objectives; ++obj)
            objective[obj] = solution.getObjective(obj);
      
        for (var = 0; var < n_variables; ++var)
            variable[var] = solution.getVariable(var);
     
        for (var = 0; var < 16; ++var)
            execTime[var] = solution.execTime[var];
}

Solution& Solution::operator()(int numberOfObjectives, int numberOfVariables) {
=======
Solution::~Solution(){
    delete [] objective;
    delete [] variable;
    delete [] execTime;
    int obj = 0;
    for (obj = 0; obj < this->totalVariables; obj++)
        delete [] this->partialObjective[obj];
    
    delete [] this->partialObjective;
}

void Solution::setObjective(int obj, double value){
    this->objective[obj] = value;
}

void Solution::setVariable(int obj, int value){
    this->variable[obj] = value;
}

int Solution::getVariable(int obj){
    return this->variable[obj];
}

double Solution::getObjective(int obj){
    return this->objective[obj];
}
>>>>>>> fe0afa36c3446ca25368797e2d3be5e42c96f31f

    machineWithMakespan = 0;
    build_up_to = -1;
    n_objectives = numberOfObjectives;
    n_variables = numberOfVariables;
    
<<<<<<< HEAD
    /** Freeing previously used memory. **/
    if(objective != nullptr)
        delete[] objective;
    if (variable != nullptr)
        delete [] variable;
    if (execTime != nullptr)
        delete [] execTime;
    
    objective = new double[numberOfObjectives];
    variable = new int[numberOfVariables];
    execTime = new double[16];
    
    int var = 0, obj = 0;
    for (obj = 0; obj < numberOfObjectives; ++obj)
        objective[obj] = 0;
    
    for (var = 0; var < numberOfVariables; ++var)
        variable[var] = -1;
    
    for (var = 0; var < 16; ++var)
        execTime[var] = 0;
=======
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
    
    int obj = 0, var = 0;
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
>>>>>>> fe0afa36c3446ca25368797e2d3be5e42c96f31f
    
    return *this;
}

Solution& Solution::operator=(const Solution &solution) {
    
    if (this == &solution) return *this;
    
    build_up_to = solution.getBuildUpTo();
    n_objectives = solution.getNumberOfObjectives();
    n_variables = solution.getNumberOfVariables();
    
<<<<<<< HEAD
    /** Freeing previously used memory. **/
    if (objective != nullptr) {
        delete[] objective;
        delete[] variable;
        delete[] execTime;
    }
    
    objective = new double[n_objectives];
    variable = new int[n_variables];
    execTime = new double[16];
=======
    int obj = 0, var = 0;
    
    for (obj = 0; obj < this->totalObjectives; obj++)
        this->objective[obj] = solution.objective[obj];
>>>>>>> fe0afa36c3446ca25368797e2d3be5e42c96f31f
    
    int var = 0, obj = 0;
    
<<<<<<< HEAD
    for (obj = 0; obj < n_objectives; ++obj)
        objective[obj] = solution.getObjective(obj);
    
    for (var = 0; var < n_variables; ++var)
        variable[var] = solution.getVariable(var);
    
    /**Section for the HCSP problem**/
    for (var = 0; var < 16; ++var)
        execTime[var] = solution.execTime[var];
=======
    for (var = 0; var < this->totalVariables; var++){
        this->variable[var] = solution.variable[obj];
        this->partialObjective[var] = new double[solution.totalObjectives];
        for (obj = 0; obj < solution.totalObjectives; obj++)
            this->partialObjective[var][obj] = solution.partialObjective[var][obj];
    }
    
    /**Section for the HCSP problem**/
    this->execTime = new double[16];
    for (var = 0; var < 16; var++)
        this->execTime[var] = solution.execTime[var];
>>>>>>> fe0afa36c3446ca25368797e2d3be5e42c96f31f
    
    return *this;
}

int Solution::operator==(const Solution &solution){
    int index = 1;
    
    for (index = 0; index < n_objectives; ++index)
        if (objective[index] < solution.getObjective(index) || objective[index] > solution.getObjective(index))
            return 0;
    return index;
}

Solution::~Solution() {
	delete[] objective;
	delete[] variable;
	delete[] execTime;
}


void Solution::setBuildUpTo(int index){build_up_to = index;}
void Solution::setObjective(int index, double value) { objective[index] = value; }
int Solution::setVariable(int index, int value) {
    int last_value = variable[index];
    variable[index] = value;
    build_up_to = index;
    return last_value;
}

double Solution::getObjective(int index) const { return objective[index]; }
int Solution::getVariable(int index) const { return variable[index]; }
int Solution::getNumberOfVariables() const { return n_variables; }
int Solution::getNumberOfObjectives() const { return n_objectives; }
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
	for (nObj = 0; nObj < n_objectives; ++nObj) {
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
    char sep = '-';
	for (nObj = 0; nObj < n_objectives; ++nObj)
		printf("%4.0f", getObjective(nObj));

	printf(" | ");

	for (nVar = 0; nVar < n_variables; ++nVar)
        if (variable[nVar] == -1)
            printf("%4c", sep);
        else
            printf("%4d", variable[nVar]);

	printf(" |\n");
}

void Solution::printObjectives() const {
	int nObj;
	for (nObj = 0; nObj < n_objectives; ++nObj)
		printf("%f ", getObjective(nObj));

	printf("\n");
}

void Solution::printVariables() const {
	int nVar;
	for (nVar = 0; nVar < n_variables; ++nVar)
		printf("%d ", getVariable(nVar));

	printf("\n");
}
