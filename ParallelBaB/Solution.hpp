//
//  Solution.hpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#ifndef Solution_hpp
#define Solution_hpp

#include <stdio.h>

//template <class TypeVariable>
class Solution{
    
public:
    Solution();
    Solution(int totalObjectives, int totalVariables);
    Solution(const Solution &solution);
    
    ~Solution();
    
    double * objective;
    double ** partialObjective;
    int * variable;
    
    int build_up_to = -1;
    
    int totalVariables;
    int totalObjectives;
    
    void setVariable(int index, int value);
    void setObjective(int index, double value);
    void setPartialObjective(int var, int objective, double value);
    
    int getVariable(int index) const;
    double getObjective(int nObjective) const;
    
    int getPartialObjective(int var, int objective) const;
    int getNumberOfVariables() const;
    int getNumberOfObjectives() const;
    
    int getBuildUpTo() const;
    
    double makespan;
    double energy;
    double * execTime;
    int machineWithMakespan;
    
    int dominates(const Solution &solution) const;
    
    /*overloading operators*/
    Solution& operator=(const Solution &solution);
    Solution& operator()(int numberOfObjectives, int numberOfVariables);
    int operator==(const Solution & solution);
    void print() const;
    void printObjectives() const;
    void printVariables() const;
    
};

#endif /* Solution_hpp */
