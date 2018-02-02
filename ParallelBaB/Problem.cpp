//
//  Problem.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "Problem.hpp"

Problem::Problem() {
    n_objectives = 0;
    n_variables = 0;
    n_constraints = 0;
    starting_level = 0;
    lower_bound = nullptr;
    upper_bound = nullptr;
    type = ProblemType::combination;
}

Problem::Problem(const Problem& toCopy) :
type(toCopy.getType()),
starting_level(toCopy.getStartingRow()),
n_objectives(toCopy.getNumberOfObjectives()),
n_variables(toCopy.getNumberOfVariables()),
n_constraints(toCopy.getNumberOfConstraints()) {
    
    std::strcpy(name, toCopy.name);
    lower_bound = new int[toCopy.getNumberOfVariables()];
    upper_bound = new int[toCopy.getNumberOfVariables()];
    
    for (int index = 0; index < n_variables; index++) {
        lower_bound[index] = toCopy.getLowerBound(index);
        upper_bound[index] = toCopy.getUpperBound(index);
    }
}

Problem::Problem(int numberOfObjectives, int numberOfVariables) :
type(ProblemType::XD),
starting_level(0),
n_objectives(numberOfObjectives),
n_variables(numberOfVariables),
n_constraints(0) {
    
    lower_bound = new int[numberOfVariables];
    upper_bound = new int[numberOfVariables];
}

Problem::~Problem() {
    /*
     delete [] lowerBound;
     delete [] upperBound;
     */
    
}

Problem& Problem::operator=(const Problem &toCopy) {
    
    if (this == &toCopy)
        return *this;
    
    n_objectives = toCopy.getNumberOfObjectives();
    n_variables = toCopy.getNumberOfVariables();
    type = toCopy.getType();
    starting_level = toCopy.getStartingRow();
    n_constraints = toCopy.getNumberOfConstraints();
    
    if (lower_bound != nullptr) {
        delete[] lower_bound;
        delete[] upper_bound;
    }
    
    std::strcpy(name, toCopy.name);
    /*
     lowerBound = new int[toCopy.getNumberOfVariables()];
     upperBound = new int[toCopy.getNumberOfVariables()];
     
     int index = 0;
     for (index = 0; index < totalVariables; index++) {
     lowerBound[index] = toCopy.getLowerBound(index);
     upperBound[index] = toCopy.getUpperBound(index);
     }
     */
    return *this;
}

void Problem::setName(const char* new_name) {
    std::strcpy(name, new_name);
}

void Problem::setNumberOfVariables(int numberOfVariables) {
    n_variables = numberOfVariables;
}

char * Problem::getName() {
    return name;
}

int * Problem::getElemensToRepeat() {
    return nullptr;
}

int Problem::getNumberOfObjectives() const {
    return n_objectives;
}

int Problem::getNumberOfVariables() const {
    return n_variables;
}

int Problem::getNumberOfConstraints() const {
    return n_constraints;
}

int Problem::getStartingRow() const {
    return starting_level;
}

int Problem::getLowerBound(int index) const {
    return 0;
}

int Problem::getUpperBound(int index) const {
    return 0;
}

int Problem::getLowerBoundInObj(int nObj) const {
    return INT_MAX;
}

int Problem::getTotalElements() {
    return 0;
}

int Problem::getDecodeMap(int map, int position) {
    return 0;
}

int Problem::getCodeMap(int value1, int value2) {
    return 0;
}

int Problem::getTimesThatValueCanBeRepeated(int value) {
    return 0;
}

double Problem::evaluate(Solution & solution) {
    return 0;
}

double Problem::evaluatePartial(Solution & solution, int levelEvaluation) {
    return 0;
}

void Problem::createDefaultSolution(Solution & solution) {
}

void Problem::getSolutionWithLowerBoundInObj(int nObj, Solution& sol) {
}

void Problem::printSolution(const Solution & solution) const {
}

void Problem::printPartialSolution(const Solution & solution, int level) const {
}

void Problem::printInstance() {
}

void Problem::printProblemInfo() const {
}

void Problem::printSolutionInfo(const Solution & solution) const {
}

void Problem::loadInstance(char filePath[2][255], char file_extension[4]) {
}
