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

enum DominanceRelation {
    Dominates = 1, Dominated = -1, Nondominated = 0, Equals = 11
};

//template <class TypeVariable>
class Solution{
    
private:
    double * objective;
    int * variable;
    
    int build_up_to = -1;
    
    int n_variables;
    int n_objectives;
    
public:
    
    int machineWithMakespan;
    double * execTime;
    
    Solution();
    Solution(int totalObjectives, int totalVariables);
    Solution(const Solution &solution);
    ~Solution();
    
    int setVariable(int index, int value);
    void setObjective(int index, double value);
    void setBuildUpTo(int index);
    
    double getObjective(int nObjective) const;
    int getVariable(int index) const;
    int getNumberOfVariables() const;
    int getNumberOfObjectives() const;
    int getBuildUpTo() const;
    
    DominanceRelation dominates(const Solution &solution) const;
    
    /** Overloading operators. **/
    Solution& operator=(const Solution &solution);
    Solution& operator()(int numberOfObjectives, int numberOfVariables);
    int operator==(const Solution & solution);
    
    /** print functions. **/
    void print() const;
    void printObjectives() const;
    void printVariables() const;
    
};

#endif /* Solution_hpp */
