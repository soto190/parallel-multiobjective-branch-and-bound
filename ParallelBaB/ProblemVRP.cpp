//  ProblemVRP.cpp
//  PhDProject
//
//  Created by Carlos Soto on 21/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "ProblemVRP.hpp"

ProblemVRP::ProblemVRP(const ProblemVRP& toCopy):Problem(toCopy){}
ProblemVRP::ProblemVRP(int totalObjectives, int totalVariables ):Problem(totalObjectives, totalVariables){}
ProblemVRP::~ProblemVRP(){}

ProblemVRP& ProblemVRP::operator=(const ProblemVRP &toCopy){return *this;}
double ProblemVRP::evaluate(Solution & solution){return 0.0;}
double ProblemVRP::evaluatePartial(Solution & solution, int levelEvaluation){return 0.0;}
double ProblemVRP::evaluateLastLevel(Solution * solution){return 0.0;}
double ProblemVRP::removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel){return 0.0;}
double ProblemVRP::removeLastLevelEvaluation(Solution * solution, int newLevel){return 0.0;}

int ProblemVRP::getLowerBound(int indexVar) const{ return 0; }
int ProblemVRP::getUpperBound(int indexVar) const{ return 1; }
int ProblemVRP::getLowerBoundInObj(int nObj) const{ return INT_MAX;}

ProblemType ProblemVRP::getType() const {return ProblemType::combination;}
int ProblemVRP::getStartingRow(){return 0;}
int ProblemVRP::getFinalLevel(){return 0;}
int ProblemVRP::getTotalElements(){return 0;}
int * ProblemVRP::getElemensToRepeat(){return nullptr;}
int ProblemVRP::getDecodeMap(int map, int position){ return 0;}
int ProblemVRP::getCodeMap(int value1, int value2){return 0;}
int ProblemVRP::getTimesValueIsRepeated(int value){return 0;}
void ProblemVRP::createDefaultSolution(Solution& solution){}
void ProblemVRP::getSolutionWithLowerBoundInObj(int nObj, Solution& solution){}
void ProblemVRP::printInstance(){ printf("Hello from Problem VRP!\n");}
void ProblemVRP::printProblemInfo(){}
void ProblemVRP::loadInstance(char *path[]){}
void ProblemVRP::printSolution(const Solution & solution) const{ printPartialSolution(solution, this->totalVariables);}
void ProblemVRP::printPartialSolution(const Solution & solution, int level) const{}
void ProblemVRP::printSolutionInfo(const Solution & solution) const{ printf("TODO: Implement this function.\n"); }
