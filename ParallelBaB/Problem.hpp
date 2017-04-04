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
#include <climits> /*For the Ehecatl wich uses GCC 4.4.7 and Xeon Phi, this activates INT_MAX.*/

enum ProblemType{combination = 0, permutation = 1, permutation_with_repetition_and_combination = 2, XD=190};

class Problem{

public:
    
    Problem(const Problem& toCopy);
    Problem (int totalObjectives, int totalVariables);
    virtual ~Problem();
    virtual Problem& operator=(const Problem& toCopy);
    
    char * name;
    ProblemType type;
    int startingLevel;
    int totalObjectives;
    int totalVariables;
    int totalConstraints;
    int * lowerBound;
    int * upperBound;
    
    virtual double evaluate(Solution & solution) = 0;
    virtual double evaluatePartial(Solution & solution, int levelEvaluation) = 0;
    virtual double evaluateLastLevel(Solution * solution) = 0;
    virtual double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel) = 0;
    virtual double removeLastLevelEvaluation(Solution * solution, int newLevel) = 0;
    virtual void createDefaultSolution(Solution & solution);
    
    virtual ProblemType getType() const = 0;
    virtual int getStartingLevel() = 0;
    virtual int getFinalLevel() = 0;
    
    virtual void getSolutionWithLowerBoundInObj(int nObj, Solution & sol) = 0;
    
    void setName(const char* name);
    char * getName();
    
    void setNumberOfVariables(int numberOfVariables);
    
    int getNumberOfVariables() const;
    int getNumberOfObjectives() const;
    int getNumberOfConstraints() const;
    int getStartingLevel() const;
    
    virtual int getTotalElements() = 0;
    virtual int * getElemensToRepeat() = 0;
    virtual int getDecodeMap(int map, int position) = 0;
    virtual int getCodeMap(int value1, int value2);
    virtual int getTimesValueIsRepeated(int value);
    
    virtual int getLowerBound(int index) const = 0;
    virtual int getUpperBound(int index) const = 0;
    
    virtual int getLowerBoundInObj(int nObj) const = 0;
    
    virtual void printSolution(const Solution & solution) const;
    virtual void printPartialSolution(const Solution & solution, int level) const;
    virtual void printSolutionInfo(Solution & solution) const;
    virtual void loadInstance(char* path[]);
    virtual void printInstance();
    virtual void printProblemInfo() const;
};

#endif /* Problem_hpp */
