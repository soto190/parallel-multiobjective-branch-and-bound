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
    int value;
    int obj[2];
    float distance[2];
    float combination;
    
public:
    ObjectiveValues();
    ObjectiveValues(int var_value, int obj1, int obj2);
    ObjectiveValues(const ObjectiveValues & toCopy);
    
    void setValue(int n_value);
    void setObjective(int n_objective, int value);
    void setDistance(int n_obj, float n_dist);
    int getValue() const;
    int getObjective(int n_obj) const;
    float getDistance(int n_obj) const;
    float getCombination() const;
    float getSomethingToSort(const SORTING_TYPES sort) const;
    bool operator==(const ObjectiveValues& rhs) const;
    bool operator<(const ObjectiveValues& rhs) const;
    bool operator<=(const ObjectiveValues& rhs) const;
    bool operator>(const ObjectiveValues& rhs) const;
    bool operator>=(const ObjectiveValues& rhs) const;
    Dom dominance(const ObjectiveValues& rhs) const;
    void print();
};

#endif /* ObjectiveValues_hpp */
