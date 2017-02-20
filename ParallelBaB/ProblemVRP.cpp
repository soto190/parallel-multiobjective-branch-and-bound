//  ProblemVRP.cpp
//  PhDProject
//
//  Created by Carlos Soto on 21/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "ProblemVRP.hpp"

ProblemVRP::~ProblemVRP(){
}


double ProblemVRP::evaluate(Solution & solution){return 0.0;}
double ProblemVRP::evaluatePartial(Solution * solution, int levelEvaluation){return 0.0;}
double ProblemVRP::evaluateLastLevel(Solution * solution){return 0.0;}
double ProblemVRP::removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel){return 0.0;}
double ProblemVRP::removeLastLevelEvaluation(Solution * solution, int newLevel){return 0.0;}

int ProblemVRP::getLowerBound(int indexVar){
    return 0;
}

int ProblemVRP::getUpperBound(int indexVar){
    return 1;
}

int ProblemVRP::getLowerBoundInObj(int nObj){
    return INT_MAX;
}

ProblemType ProblemVRP::getType(){return ProblemType::combination;}
int ProblemVRP::getStartingLevel(){return 0;}
int ProblemVRP::getFinalLevel(){return 0;}

int ProblemVRP::getTotalElements(){
    return 0;
}

int * ProblemVRP::getElemensToRepeat(){
    return nullptr;
}

int ProblemVRP::getMapping(int map, int position){
    return 0;
}

int ProblemVRP::getMappingOf(int value1, int value2){
    return 0;
}

int ProblemVRP::getTimesValueIsRepeated(int value){
    return 0;
}

Solution* ProblemVRP::createSolution(){
    Solution * solution = new Solution(this->getNumberOfObjectives(), this->getNumberOfVariables());
    return solution;
}

void ProblemVRP::createDefaultSolution(Solution *solution){

}

Solution * ProblemVRP::getSolutionWithLowerBoundInObj(int nObj){
    Solution * solution = new Solution(this->getNumberOfObjectives(), this->getNumberOfVariables());
    
    return solution;
}

void ProblemVRP::printInstance(){
    printf("Hello from Problem VRP!\n");
}

void ProblemVRP::printProblemInfo(){
}

void ProblemVRP::printSolutionInfo(Solution *solution){
    printf("TODO: Implement this function.\n");
}

void ProblemVRP::loadInstance(char *path[]){
}

void ProblemVRP::printSolution(Solution * solution){
    printPartialSolution(solution, this->totalVariables);
}

void ProblemVRP::printPartialSolution(Solution * solution, int level){
    
}
