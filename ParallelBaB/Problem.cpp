//
//  Problem.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "Problem.hpp"

Problem::Problem(){
    
    this->totalObjectives = 0;
    this->totalVariables = 0;
    this->type = ProblemType::XD;
    this->startingLevel = 0;
    this->totalConstraints = 0;
    
    this->lowerBound = nullptr;
    this->upperBound = nullptr;
    this->name = nullptr;
}

Problem::Problem(const Problem& toCopy){
    this->totalObjectives = toCopy.getNumberOfObjectives();
    this->totalVariables = toCopy.getNumberOfVariables();
    this->type = toCopy.getType();
    this->startingLevel = toCopy.startingLevel;
    this->totalConstraints = toCopy.getNumberOfConstraints();
    this->name = new char[255];
    std::strcpy(this->name, toCopy.name);
    this->lowerBound = new int[toCopy.getNumberOfVariables()];
    this->upperBound = new int[toCopy.getNumberOfVariables()];
    
    int index = 0;
    for (index = 0; index < totalVariables; index++) {
        
        this->lowerBound[index] = toCopy.lowerBound[index];
        this->upperBound[index] = toCopy.upperBound[index];
    }
    
}

Problem::Problem(int totalObjectives, int totalVariables){
    this->totalObjectives = totalObjectives;
    this->totalVariables = totalVariables;
    this->type = ProblemType::XD;
    this->startingLevel = 0;
    this->totalConstraints = 0;
   
    this->name = new char[255];
    this->lowerBound = new int[totalVariables];
    this->upperBound = new int[totalVariables];
    
}

Problem::~Problem(){
    delete [] this->lowerBound;
    delete [] this->upperBound;
    delete [] this->name;
}

Problem& Problem::operator=(const Problem &toCopy){
    
    if (this == &toCopy) return *this;
    
    this->totalObjectives = toCopy.getNumberOfObjectives();
    this->totalVariables = toCopy.getNumberOfVariables();
    this->type = toCopy.getType();
    this->startingLevel = toCopy.startingLevel;
    this->totalConstraints = toCopy.getNumberOfConstraints();
    
    if (name != nullptr) {
        delete[] lowerBound;
        delete[] upperBound;
        delete[] name;
    }
    
    this->name = new char[255];
    std::strcpy(this->name, toCopy.name);
    
    this->lowerBound = new int[toCopy.getNumberOfVariables()];
    this->upperBound = new int[toCopy.getNumberOfVariables()];
    
    int index = 0;
    for (index = 0; index < totalVariables; index++) {
        this->lowerBound[index] = toCopy.lowerBound[index];
        this->upperBound[index] = toCopy.upperBound[index];
    }
    
    return *this;
}

void Problem::setName(const char* name){
    
    std::strcpy(this->name, name);
}

char * Problem::getName(){
    return this->name;
}

void Problem::setNumberOfVariables(int numberOfVariables){
    this->totalVariables = numberOfVariables;
}

int Problem::getNumberOfObjectives() const{
    return this->totalObjectives;
}

int Problem::getNumberOfVariables() const{
    return this->totalVariables;
}

int Problem::getNumberOfConstraints() const{
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

int Problem::getTimesValueIsRepeated(int value){
    return 0;
}

double Problem::evaluate(Solution & solution){
    return 0;
}

double Problem::evaluatePartial(Solution & solution, int levelEvaluation){
    return 0;
}

void Problem::createDefaultSolution(Solution & solution){
}

void Problem::getSolutionWithLowerBoundInObj(int nObj, Solution& sol){
}

void Problem::printSolution(Solution & solution){}

void Problem::printPartialSolution(Solution & solution, int level){}

void Problem::printInstance(){}

void Problem::printProblemInfo(){}

void Problem::printSolutionInfo(Solution & solution){}

void Problem::loadInstance(char * filePath[]){
}
