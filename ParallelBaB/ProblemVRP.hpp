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
    
    double evaluate(Solution * solution);
    double evaluatePartial(Solution * solution, int levelEvaluation);
    double evaluateLastLevel(Solution * solution);
    double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel);
    double removeLastLevelEvaluation(Solution * solution, int newLevel);
    
    void createDefaultSolution(Solution * solution);

    Solution* createSolution();
    
    int getLowerBound(int indexVar);
    int getUpperBound(int indexVar);
    ProblemType getType();
    int getStartingLevel();
    int getFinalLevel();
    
    
    void printInstance();
    void printProblemInfo();
    void loadInstance(char* path[]);
    
};


#endif /* ProblemVRP_hpp */
