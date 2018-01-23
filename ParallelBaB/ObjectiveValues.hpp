//
//  ObjectiveValues.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef ObjectiveValues_hpp
#define ObjectiveValues_hpp

#include <stdio.h>
#include "myutils.hpp"

enum Dom {Domtes = 1, Domted = -1, Nondom = 0, Eq = 11};
enum SORTING_TYPES{DIST_1 = 0, DIST_2 = 1, DIST_COMB = 3, DOMINANCE = 4};

class ObjectiveValues{
    int n_objectives;
    int variable_value;
    int objective[3];
    float distance[3];
    float mixed;
    
public:
    ObjectiveValues();
    ObjectiveValues(int numbr_of_objectives);
    ObjectiveValues(int var_value, int obj1, int obj2);
    ObjectiveValues(int var_value, int obj1, int obj2, int obj3);

    ObjectiveValues(const ObjectiveValues & toCopy);
    ~ObjectiveValues();
    
    void setValue(int n_value);
    void setObjective(int n_objective, int value);
    void setDistance(int n_obj, float n_dist);
    int getValue() const;
    int getObjective(int n_obj) const;
    int getNumberOfObjectives() const;
    float getDistance(int n_obj) const;
    float getCombination() const;
    float getSomethingToSort(const SORTING_TYPES sort) const;
    bool operator==(const ObjectiveValues& right_hand_solution) const;
    bool operator<(const ObjectiveValues& right_hand_solution) const;
    bool operator<=(const ObjectiveValues& right_hand_solution) const;
    bool operator>(const ObjectiveValues& right_hand_solution) const;
    bool operator>=(const ObjectiveValues& right_hand_solution) const;
    Dom dominance(const ObjectiveValues& right_hand_solution) const;
    void print();
};
#endif /* ObjectiveValues_hpp */
