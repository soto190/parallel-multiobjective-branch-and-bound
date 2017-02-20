//
//  ProblemTSP.hpp
//  PhDProject
//
//  Created by Carlos Soto on 21/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#ifndef ProblemTSP_hpp
#define ProblemTSP_hpp

#include <stdio.h>
#include <fstream>
#include <string>
#include <math.h>
#include <regex>
#include "Problem.hpp"
#include "myutils.hpp"



/**
 * In TSP the solution [0 1 2 3 4 5 6] has the same objective values as [0 6 5 4 3 2 1].
 * This can be avoided by deleting from the stack the level 1 and the value 6 [{1, 6} {2, 5} {3, 4} { 4, 3} {5, 2} {6, 1}]. But only this permutation has the same value the permutations [0 6 4 5 3 2 1] has a different objective value.
 */

enum TSPObj {DISTANCE = 0, COST = 1};
class ProblemTSP: public Problem {
public:
    
    ProblemTSP(int totalObjectives, int totalVariables):Problem(totalObjectives, totalVariables){};
    ~ProblemTSP();
    
    double evaluate(Solution & solution);
    
    /**
     *
     */
    double evaluatePartial(Solution * solution, int levelEvaluation);
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
    
    /**TSP functions**/
    int** cityCoord;
    double** euclideanDistance;
    int** costCoord;
    double** costs;
    
    double computeEuclideanDistance(int x1, int y1, int x2, int y2);
    
    void loadInstance(char* pathsFile[]);
    
    /**Print information**/
    void printInstance();
    void printProblemInfo();
    void printSolutionInfo(Solution * solution);

    
private:
    void readCost(char * pathFile);
};

#endif /* ProblemTSP_hpp */
