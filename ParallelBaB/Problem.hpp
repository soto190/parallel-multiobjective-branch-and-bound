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
#include "Solution.hpp"

//template <class TypeSolution>

class Problem{
public:
    
    Problem (int totalObjectives, int totalVariables);
    virtual ~Problem();
    
    char* name;
    int type;
    int startingLevel;
    int totalObjectives;
    int totalVariables;
    int totalConstraints;
    int* lowerBound;
    int* upperBound;
    
    virtual double evaluate(Solution * solution) = 0;
    virtual double evaluatePartial(Solution * solution, int levelEvaluation) = 0;
    virtual double evaluateLastLevel(Solution * solution) = 0;
    virtual double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel) = 0;
    virtual double removeLastLevelEvaluation(Solution * solution, int newLevel) = 0;
    
    virtual int getType() = 0;
    virtual int getStartingLevel() = 0;
    virtual int getFinalLevel() = 0;
    
    virtual Solution* createSolution() = 0;
    
    void setName(char* name);
    
    int getNumberOfVariables();
    int getNumberOfObjectives();
    int getNumberOfConstraints();
    
    virtual int getLowerBound(int index) = 0;
    virtual int getUpperBound(int index) = 0;
    
    virtual void loadInstance(char* path[]);
    virtual void printInstance();
    virtual void printProblemInfo();
};

#endif /* Problem_hpp */
