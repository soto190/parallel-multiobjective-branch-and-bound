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

Solution* Problem::createSolution(){
    Solution* solution = new Solution(this->totalObjectives, this->totalVariables);
    return solution;
}

double Problem::evaluate(Solution* solution){
    return 0;
}

double Problem::evaluatePartial(Solution* solution, int levelEvaluation){
    return 0;
}

void Problem::printInstance(){
}

void Problem::printProblemInfo(){
}

void Problem::loadInstance(char *filePath[]){
}
