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
    
    ProblemTSP(const ProblemTSP& toCopy);
    ProblemTSP(int totalObjectives, int totalVariables);
    ~ProblemTSP();

    ProblemTSP& operator=(const ProblemTSP& toCopy);
    double evaluate(Solution & solution);
    
    /**
     *
     */
    double evaluatePartial(Solution & solution, int levelEvaluation);
    double evaluateLastLevel(Solution * solution);
    double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel);
    double removeLastLevelEvaluation(Solution * solution, int newLevel);
    
    void createDefaultSolution(Solution & solution);
    void getSolutionWithLowerBoundInObj(int nObj, Solution& solution);

    int getLowerBound(int indexVar) const;
    int getUpperBound(int indexVar) const;
    int getLowerBoundInObj(int nObj) const;

    ProblemType getType() const;
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
    void printSolution(const Solution & solution) const;
    void printPartialSolution(const Solution & solution, int level) const;
    void printSolutionInfo(const Solution & solution) const;

    void printInstance();
    void printProblemInfo() const;

private:
    void readCost(char * pathFile);
};

#endif /* ProblemTSP_hpp */
