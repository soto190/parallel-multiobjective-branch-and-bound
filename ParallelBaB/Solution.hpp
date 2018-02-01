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
#include <string>
#include "SolutionException.hpp"

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
    int rank;
    int dominated_by;
    double * objective;
    int * variable;
    
public:
    
    int index; /** TODO: this can be removed and all the related functions. **/
    double * execTime; /** TODO: this can be removed and all the related functions. **/
    
    Solution();
    Solution(int number_of_objectives, int number_of_variables);
    Solution(const Solution &solution);
    ~Solution();
    
    int setVariable(int index, int value) throw(SolutionException);
    void setObjective(int index, double value) throw(SolutionException);
    void setBuildUpTo(int index);
    void setRank(int n_rank);
    void setDominatedBy(int n_value);
    void incrementDominatedBy();
    void decrementDominatedBy();
    
    double getObjective(int n_objective) const throw(SolutionException);
    int getVariable(int index) const throw(SolutionException);
    int getNumberOfVariables() const;
    int getNumberOfObjectives() const;
    int getBuildUpTo() const;
    int getRank() const;
    int getDominatedBy() const;
    
    DominanceRelation dominates(const Solution &solution) const;
    
    /** Overloading operators. **/
    Solution& operator=(const Solution &solution);
    Solution& operator()(int number_of_objectives, int number_of_variables);
    int operator==(const Solution & solution);
    
    /** print functions. **/
    void print() const;
    void printObjectives() const;
    void printVariables() const;
};
#endif /* Solution_hpp */
