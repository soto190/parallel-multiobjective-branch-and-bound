//
//  Problem.hpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#ifndef Problem_hpp
#define Problem_hpp

#include <stdio.h>
#include <iostream>
#include <cstring>
#include "Solution.hpp"

//template <class TypeSolution>

enum ProblemType{combination = 0, permutation = 1, permutation_with_repetition_and_combination = 2, XD=190};

class Problem{
public:
    
    Problem (int totalObjectives, int totalVariables);
    virtual ~Problem();
    
    char * name;
    int type;
    int startingLevel;
    int totalObjectives;
    int totalVariables;
    int totalConstraints;
    int * lowerBound;
    int * upperBound;
    
    virtual double evaluate(Solution * solution) = 0;
    virtual double evaluatePartial(Solution * solution, int levelEvaluation) = 0;
    virtual double evaluateLastLevel(Solution * solution) = 0;
    virtual double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel) = 0;
    virtual double removeLastLevelEvaluation(Solution * solution, int newLevel) = 0;
    virtual void createDefaultSolution(Solution * solution);
    virtual void printSolution(Solution * solution);
    virtual void printPartialSolution(Solution * solution, int level);
    
    virtual ProblemType getType() = 0;
    virtual int getStartingLevel() = 0;
    virtual int getFinalLevel() = 0;
    
    virtual Solution * createSolution() = 0;
    virtual Solution * getSolutionWithLowerBoundInObj(int nObj) = 0;
    
    void setName(const char* name);
    char * getName();
    
    void setNumberOfVariables(int numberOfVariables);
    
    int getNumberOfVariables();
    int getNumberOfObjectives();
    int getNumberOfConstraints();
    
    virtual int getTotalElements() = 0;
    virtual int * getElemensToRepeat() = 0;
    virtual int getMapping(int map, int position) = 0;
    virtual int getMappingOf(int value1, int value2);
    
    virtual int getLowerBound(int index) = 0;
    virtual int getUpperBound(int index) = 0;
    
    virtual int getLowerBoundInObj(int nObj) = 0;
    
    virtual void loadInstance(char* path[]);
    virtual void printInstance();
    virtual void printProblemInfo();
    virtual void printSolutionInfo(Solution * solution);
};

#endif /* Problem_hpp */
