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
    
    ProblemVRP(const ProblemVRP& toCopy);
    ProblemVRP(int totalObjectives, int totalVariables);
    ~ProblemVRP();
    
    ProblemVRP& operator=(const ProblemVRP& toCopy);
    
    double evaluate(Solution & solution);
    double evaluatePartial(Solution & solution, int levelEvaluation);
    double evaluateLastLevel(Solution * solution);
    double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel);
    double removeLastLevelEvaluation(Solution * solution, int newLevel);
    
    void createDefaultSolution(Solution & solution);
    void getSolutionWithLowerBoundInObj(int nObj, Solution& solution);
    
    int getLowerBound(int indexVar) const;
    int getUpperBound(int indexVar) const;
    double getLowerBoundInObj(int nObj) const;
    double getUpperBoundInObj(int nObj) const;

    ProblemType getType() const;
    int getStartingRow();
    int getFinalLevel();
    
    int * getElemensToRepeat();
    int getTotalElements();
    int getDecodeMap(int map, int position) const;
    int getEncodeMap(int value1, int value2) const;
    int getTimesThatValueCanBeRepeated(int value);
    
    void printSolution(const Solution & solution) const;
    void printPartialSolution(const Solution & solution, int level) const;
    void printSolutionInfo(const Solution & solution) const;
    
    void printInstance();
    void printProblemInfo() const;
    void loadInstance(char filePath[2][255], char file_extension[4]);
};
#endif /* ProblemVRP_hpp */

