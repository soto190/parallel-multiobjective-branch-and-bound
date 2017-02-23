//
//  ProblemVRP.hpp
//  PhDProject
//
//  Created by Carlos Soto on 21/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#ifndef ProblemVRP_hpp
#define ProblemVRP_hpp

#include <stdio.h>
#include "Problem.hpp"


class ProblemVRP: public Problem {
public:
    
    ProblemVRP(int totalObjectives, int totalVariables):Problem(totalObjectives, totalVariables){}
    ~ProblemVRP();
    
    double evaluate(Solution & solution);
    double evaluatePartial(Solution & solution, int levelEvaluation);
    double evaluateLastLevel(Solution * solution);
    double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel);
    double removeLastLevelEvaluation(Solution * solution, int newLevel);
    
    void createDefaultSolution(Solution * solution);
    Solution * getSolutionWithLowerBoundInObj(int nObj);

    void printSolution(Solution * solution);
    void printPartialSolution(Solution * solution, int level);
    
    Solution* createSolution();
    
    int getLowerBound(int indexVar);
    int getUpperBound(int indexVar);
    int getLowerBoundInObj(int nObj);
    ProblemType getType();
    int getStartingLevel();
    int getFinalLevel();
    
    int * getElemensToRepeat();
    int getTotalElements();
    int getMapping(int map, int position);
    int getMappingOf(int value1, int value2);
    int getTimesValueIsRepeated(int value);
    
    void printInstance();
    void printProblemInfo();
    void printSolutionInfo(Solution * solution);
    void loadInstance(char* path[]);
    
    
};


#endif /* ProblemVRP_hpp */
