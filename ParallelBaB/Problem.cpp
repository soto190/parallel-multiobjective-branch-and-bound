//
//  Problem.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "Problem.hpp"

Problem::Problem(const Problem& toCopy):
totalObjectives(toCopy.getNumberOfObjectives()),
totalVariables(toCopy.getNumberOfVariables()),
type(toCopy.getType()),
startingLevel(toCopy.getStartingRow()),
totalConstraints(toCopy.getNumberOfConstraints()){
    
        name = new char[255];
        std::strcpy(name, toCopy.name);
    /*    lowerBound = new int[toCopy.getNumberOfVariables()];
        upperBound = new int[toCopy.getNumberOfVariables()];
    
        int index = 0;
        for (index = 0; index < totalVariables; index++) {
            lowerBound[index] = toCopy.getLowerBound(index);
            upperBound[index] = toCopy.getUpperBound(index);
        }
     */
}

Problem::Problem(int numberOfObjectives, int numberOfVariables):
totalObjectives(numberOfObjectives),
totalVariables(numberOfVariables),
type(ProblemType::XD),
startingLevel(0),
totalConstraints(0){
    
    name = new char[255];
    /*
     lowerBound = new int[numberOfVariables];
     upperBound = new int[numberOfVariables];
     */
}

Problem::~Problem(){
    /*
    delete [] lowerBound;
    delete [] upperBound;
    */delete [] name;
     
}

Problem& Problem::operator=(const Problem &toCopy){
    
    if (this == &toCopy) return *this;
    
    totalObjectives = toCopy.getNumberOfObjectives();
    totalVariables = toCopy.getNumberOfVariables();
    type = toCopy.getType();
    startingLevel = toCopy.getStartingRow();
    totalConstraints = toCopy.getNumberOfConstraints();
    
    if (lowerBound != nullptr) {
        delete[] lowerBound;
        delete[] upperBound;
        delete[] name;
    }
    
    name = new char[255];
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

void Problem::setName(const char* new_name){ std::strcpy(name, new_name);}
void Problem::setNumberOfVariables(int numberOfVariables){ totalVariables = numberOfVariables; }

char * Problem::getName(){ return name; }
int * Problem::getElemensToRepeat(){ return nullptr; }

int Problem::getNumberOfObjectives() const{ return totalObjectives; }
int Problem::getNumberOfVariables() const{ return totalVariables; }
int Problem::getNumberOfConstraints() const{ return totalConstraints; }
int Problem::getStartingRow() const{ return startingLevel; }
int Problem::getLowerBound(int index) const { return 0; }
int Problem::getUpperBound(int index) const { return 0; }
int Problem::getLowerBoundInObj(int nObj)const{ return INT_MAX; }
int Problem::getTotalElements(){ return 0; }
int Problem::getDecodeMap(int map, int position){ return 0; }
int Problem::getCodeMap(int value1, int value2){ return 0; }
int Problem::getTimesValueIsRepeated(int value){ return 0; }

double Problem::evaluate(Solution & solution){ return 0; }
double Problem::evaluatePartial(Solution & solution, int levelEvaluation){ return 0;}
void Problem::createDefaultSolution(Solution & solution){}
void Problem::getSolutionWithLowerBoundInObj(int nObj, Solution& sol){}
void Problem::printSolution(const Solution & solution) const{}
void Problem::printPartialSolution(const Solution & solution, int level) const{}
void Problem::printInstance(){}
void Problem::printProblemInfo()const{}
void Problem::printSolutionInfo(Solution & solution)const{}
void Problem::loadInstance(char * filePath[]){}
