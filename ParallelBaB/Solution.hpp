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
    int * variable;
    
    int totalVariables;
    int totalObjectives;
    
    void setVariable(int index, int value);
    void setObjective(int index, double value);
    
    int getVariable(int index);
    double getObjective(int nObjective);
    
    double makespan;
    double energy;
    double * execTime;
    int machineWithMakespan;
    
    /*overloading operators*/
    void operator=(const Solution &solution);
    void print();
    void printObjectives();
    void printVariables();
    
};

#endif /* Solution_hpp */
