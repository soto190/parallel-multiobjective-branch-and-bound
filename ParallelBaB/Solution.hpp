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

typedef struct {
    int n_objectives;
    int n_variables;
    int build_up_to;
    double * objective;
    int * variable;
} Payload_solution;

//template <class TypeVariable>
class Solution{
    
private:
    int n_objectives;
    int n_variables;
    int build_up_to = -1;
    double * objective;
    int * variable;
    
public:
    
    int machineWithMakespan; /** TODO: this can be removed and all the related functions. **/
    double * execTime; /** TODO: this can be removed and all the related functions. **/
    
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
