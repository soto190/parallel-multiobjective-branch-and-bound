//
//  Problem.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "Problem.hpp"

Problem::Problem(int totalObjectives, int totalVariables){
    this->totalObjectives = totalObjectives;
    this->totalVariables = totalVariables;
    this->lowerBound = new int[totalVariables];
    this->upperBound = new int[totalVariables];
}

Problem::~Problem(){
    delete [] this->lowerBound;
    delete [] this->upperBound;
    delete [] this->name;
}

void Problem::setName(const char* name){
    this->name = new char[255];
    std::strcpy(this->name, name);
}

char * Problem::getName(){
    return this->name;
}

void Problem::setNumberOfVariables(int numberOfVariables){
    this->totalVariables = numberOfVariables;
}

int Problem::getNumberOfObjectives(){
    return this->totalObjectives;
}

int Problem::getNumberOfVariables(){
    return this->totalVariables;
}

int Problem::getNumberOfConstraints(){
    return this->totalConstraints;
}

int Problem::getLowerBound(int index){
    return this->lowerBound[index];
}

int Problem::getUpperBound(int index){
    return this->upperBound[index];
}

int Problem::getLowerBoundInObj(int nObj){
    return INT_MAX;
}

int Problem::getTotalElements(){
    return 0;
}

int * Problem::getElemensToRepeat(){
    return nullptr;
}

int Problem::getMapping(int map, int position){
    return 0;
}

int Problem::getMappingOf(int value1, int value2){
    return 0;
}

Solution* Problem::createSolution(){
    Solution* solution = new Solution(this->totalObjectives, this->totalVariables);
    return solution;
}

double Problem::evaluate(Solution * solution){
    return 0;
}

double Problem::evaluatePartial(Solution * solution, int levelEvaluation){
    return 0;
}

void Problem::createDefaultSolution(Solution * solution){
}

Solution * Problem::getSolutionWithLowerBoundInObj(int nObj){
    Solution* solution = new Solution(this->totalObjectives, this->totalVariables);
    return solution;
}

void Problem::printSolution(Solution * solution){}

void Problem::printPartialSolution(Solution * solution, int level){}

void Problem::printInstance(){}

void Problem::printProblemInfo(){}

void Problem::printSolutionInfo(Solution *solution){}

void Problem::loadInstance(char * filePath[]){
}
