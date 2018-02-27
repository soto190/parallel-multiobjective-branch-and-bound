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
n_variables(0),
rank(0),
distance(0),
sort_by(0),
index(0),
dominated_by(0) {
    build_up_to = -1;
    objective = nullptr;
    variable = nullptr;
    execTime = nullptr;
}

Solution::Solution(int numberOfObjectives, int numberOfVariables):
n_objectives(numberOfObjectives),
n_variables(numberOfVariables),
rank(0),
distance(0),
sort_by(0),
dominated_by(0),
build_up_to(-1),
objective(new double[numberOfObjectives]),
variable(new int[numberOfVariables]),
index(0),
execTime(new double[16]) {
    
    int var = 0, obj = 0;
    for (obj = 0; obj < numberOfObjectives; ++obj)
        objective[obj] = 0;
    
    for (var = 0; var < numberOfVariables; ++var)
        variable[var] = -1;
    
    for (var = 0; var < 16; ++var)
        execTime[var] = 0;
}

Solution::Solution(const Solution &solution):
n_objectives(solution.getNumberOfObjectives()),
n_variables(solution.getNumberOfVariables()),
rank(solution.getRank()),
distance(solution.getDistance()),
sort_by(solution.getSortByObjective()),
dominated_by(solution.getDominatedBy()),
build_up_to(solution.getBuildUpTo()),
index(solution.getIndex()) {
    
    objective = new double[n_objectives];
    variable = new int[n_variables];
    execTime = new double[16];
    
    int var = 0, obj = 0;
    
    for (obj = 0; obj < n_objectives; ++obj)
        objective[obj] = solution.getObjective(obj);
    
    for (var = 0; var < n_variables; ++var)
        variable[var] = solution.getVariable(var);
    
    //for (var = 0; var < 16; ++var)
      //  execTime[var] = solution.execTime[var];
}

Solution::~Solution() {
    delete[] objective;
    delete[] variable;
    delete[] execTime;
}

Solution& Solution::operator()(int numberOfObjectives, int numberOfVariables) {
    
    index = 0;
    rank = 0;
    dominated_by = 0;
    distance = 0;
    sort_by = 0;
    build_up_to = -1;
    n_objectives = numberOfObjectives;
    n_variables = numberOfVariables;
    
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
    
    return *this;
}

Solution& Solution::operator=(const Solution &solution) {
    
    if (this == &solution)
        return *this;
    
    build_up_to = solution.getBuildUpTo();
    n_objectives = solution.getNumberOfObjectives();
    n_variables = solution.getNumberOfVariables();
    rank = solution.getRank();
    distance = solution.getDistance();
    index = solution.getIndex();
    sort_by = solution.getSortByObjective();
    dominated_by = solution.getDominatedBy();
    /** Freeing previously used memory. **/
    if (objective != nullptr) {
        delete[] objective;
        delete[] variable;
        delete[] execTime;
    }
    
    objective = new double[n_objectives];
    variable = new int[n_variables];
    execTime = new double[16];
    
    int var = 0, obj = 0;
    
    for (obj = 0; obj < n_objectives; ++obj)
        objective[obj] = solution.getObjective(obj);
    
    for (var = 0; var < n_variables; ++var)
        variable[var] = solution.getVariable(var);
    
    /**Section for the HCSP problem**/
    //for (var = 0; var < 16; ++var)
      //  execTime[var] = solution.execTime[var];
    
    return *this;
}

bool Solution::operator==(const Solution &solution) const {
    for (int index = 0; index < n_objectives; ++index)
        if (objective[index] < solution.getObjective(index) || objective[index] > solution.getObjective(index))
            return false;
    return true;
}

bool Solution::operator>(const Solution &solution) const {
    bool result = true;
    switch (sort_by) {
        case 0:
            result = objective[sort_by] > solution.getObjective(sort_by);
            break;

        case 1:
            result = objective[sort_by] > solution.getObjective(sort_by);
            break;

        case 2:
            result = distance > solution.getDistance();
            break;

        default:
            result = objective[0] > solution.getObjective(0);
            break;
    }
    return result;
}

bool Solution::operator<(const Solution &solution) const {
    bool result = true;
    switch (sort_by) {
        case 0:
            result = objective[sort_by] < solution.getObjective(sort_by);
            break;

        case 1:
            result = objective[sort_by] < solution.getObjective(sort_by);
            break;

        case 2:
            result = distance < solution.getDistance();
            break;

        default:
            result = objective[0] < solution.getObjective(0);
            break;
    }
    return result;
}

void Solution::setBuildUpTo(int index) {
    build_up_to = index;
}

void Solution::setRank(int n_rank) {
    rank = n_rank;
}

void Solution::setDistance(double n_distance) {
    distance = n_distance;
}

void Solution::setIndex(int new_index) {
    index = new_index;
}

void Solution::setDominatedBy(int n_value) {
    dominated_by = n_value;
}

void Solution::setObjective(int index_obj, double value) throw(SolutionException) {
    try{
        if (index_obj >= n_objectives)
            throw SolutionException(SolutionErrorCode::OBJECTIVES_OUT_OF_RANGE, "when calling setObjective(obj:" + std::to_string(static_cast<long long>(index_obj)) + ", value:" + std::to_string(static_cast<long long>(value)) + ")");
        
        objective[index_obj] = value;

    } catch (SolutionException& SolutionEx) {
        printf("%s\n",  SolutionEx.what());
    }
}

int Solution::setVariable(int index_variable, int value) throw(SolutionException) {
    try {
        if (index_variable >= n_variables)
            throw SolutionException(SolutionErrorCode::VARIABLES_OUT_OF_RANGE, "when calling setVariable(var:" + std::to_string(static_cast<long long>(index_variable)) + ", value:" + std::to_string(static_cast<long long>(value)) + ")");

        int last_value = variable[index_variable];
        variable[index_variable] = value;
        build_up_to = index_variable;
        return last_value;
        
    } catch (SolutionException& SolutionEx) {
        printf("%s\n",  SolutionEx.what());
    }
    
    return -1;
}

void Solution::setSortByObjective(unsigned int objective) {
    sort_by = objective;
}

double Solution::getObjective(int index_obj) const throw(SolutionException) {
    try{
        if (index_obj >= n_objectives)
            throw SolutionException(SolutionErrorCode::OBJECTIVES_OUT_OF_RANGE, "when calling getObjective(obj:" + std::to_string(static_cast<long long>(index_obj)) + ")");
        
        return objective[index_obj];
        
    } catch (SolutionException& solutionEx) {
        printf("%s\n",  solutionEx.what());
    }
    
    return -1;
}

int Solution::getVariable(int index_variable) const throw(SolutionException) {
    try {
        if (index_variable >= n_variables)
            throw SolutionException(SolutionErrorCode::VARIABLES_OUT_OF_RANGE, "when calling getVariable(var:" + std::to_string(static_cast<long long>(index_variable)) + ")");
        
        return variable[index_variable];

    } catch (SolutionException& SolutionEx) {
        printf("%s\n",  SolutionEx.what());
    }
    return -1;
}

int Solution::getNumberOfVariables() const {
    return n_variables;
}

int Solution::getNumberOfObjectives() const {
    return n_objectives;
}

int Solution::getBuildUpTo() const {
    return build_up_to;
}

int Solution::getRank() const {
    return rank;
}

double Solution::getDistance() const {
    return distance;
}

int Solution::getIndex() const {
    return index;
}

unsigned int Solution::getSortByObjective() const {
    return sort_by;
}

int Solution::getDominatedBy() const {
    return dominated_by;
}

void Solution::incrementDominatedBy() {
    dominated_by++;
}

void Solution::decrementDominatedBy() {
    dominated_by--;
}

DominanceRelation Solution::dominanceTest(const Solution & solution) const {
    int n_obj = 0;
    int localSolIsBetterIn = 0;
    int exterSolIsBetterIn = 0;
    int equals = 1;
    double obj_A = 0;
    double obj_B = 0;
    
    /**
     * For more objectives consider
     * if (solAIsBetterIn > 0 and solBIsBetterIn > 0) break the FOR because the solutions are non-dominated.
     **/
    for (n_obj = 0; n_obj < n_objectives; ++n_obj) {
        obj_A = getObjective(n_obj);
        obj_B = solution.getObjective(n_obj);
        
        if (obj_A < obj_B) {
            localSolIsBetterIn++;
            equals = 0;
        } else if (obj_B < obj_A) {
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
    char sep = '-';
    for (int n_obj = 0; n_obj < n_objectives; ++n_obj)
        printf("%4.0f ", getObjective(n_obj));
    
    printf("| ");
    
    for (int n_var = 0; n_var < n_variables; ++n_var)
        if (variable[n_var] == -1)
            printf("%6c", sep);
        else
            printf("%6d", variable[n_var]);
    printf(" | ");
    printf("%6.3f", (distance == INFINITY ? 999999.999 : distance));
    printf(" |\n");
}

void Solution::printObjectives() const {
    for (int n_obj = 0; n_obj < n_objectives; ++n_obj)
        printf("%f ", getObjective(n_obj));
    printf("\n");
}

void Solution::printVariables() const {
    for (int n_var = 0; n_var < n_variables; ++n_var)
        printf("%d ", getVariable(n_var));
    printf("\n");
}
