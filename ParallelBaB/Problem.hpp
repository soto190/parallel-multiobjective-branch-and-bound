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
    
    Problem();
    Problem(const Problem& toCopy);
    Problem (int n_objectives, int n_variables);
    virtual ~Problem();
    virtual Problem& operator=(const Problem& toCopy);
    
    char name[255];
    ProblemType type;
    int starting_level;
    int n_objectives;
    int n_variables;
    int n_constraints;
    int * lower_bound;
    int * upper_bound;
    
    virtual double evaluate(Solution & solution) = 0;
    virtual double evaluatePartial(Solution & solution, int levelEvaluation) = 0;
    virtual double evaluateLastLevel(Solution * solution) = 0;
    virtual double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel) = 0;
    virtual double removeLastLevelEvaluation(Solution * solution, int newLevel) = 0;
    virtual void createDefaultSolution(Solution & solution);
    
    virtual ProblemType getType() const = 0;
    virtual int getStartingRow() = 0;
    virtual int getFinalLevel() = 0;
    
    virtual void getSolutionWithLowerBoundInObj(int nObj, Solution & sol) = 0;
    
    void setName(const char name[255]);
    char * getName();
    
    void setNumberOfVariables(int numberOfVariables);
    
    int getNumberOfVariables() const;
    int getNumberOfObjectives() const;
    int getNumberOfConstraints() const;
    int getStartingRow() const;
    
    virtual int getTotalElements() = 0;
    virtual int * getElemensToRepeat() = 0;
    virtual int getDecodeMap(int map, int position) = 0;
    virtual int getCodeMap(int value1, int value2);
    virtual int getTimesThatValueCanBeRepeated(int value);
    
    virtual int getLowerBound(int index) const = 0;
    virtual int getUpperBound(int index) const = 0;
    
    virtual int getLowerBoundInObj(int nObj) const = 0;
    
    virtual void printSolution(const Solution & solution) const;
    virtual void printPartialSolution(const Solution & solution, int level) const;
    virtual void printSolutionInfo(const Solution & solution) const;
    virtual void loadInstance(char path[2][255], char file_extension[4]);
    virtual void printInstance();
    virtual void printProblemInfo() const;
};
#endif /* Problem_hpp */

