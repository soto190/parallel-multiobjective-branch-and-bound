//
//  ObjectiveValues.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "ObjectiveValues.hpp"


ObjectiveValues::ObjectiveValues() {
    objective[0] = 0;
    objective[1] = 0;
    objective[2] = 0;
}

ObjectiveValues::ObjectiveValues(int var_value, int obj1, int obj2): variable_value(var_value) {
    objective[0] = obj1;
    objective[1] = obj2;
    objective[2] = 0;
}

ObjectiveValues::ObjectiveValues(const ObjectiveValues & toCopy) {
    variable_value = toCopy.getValue();
    for (int n_obj = 0; n_obj < MAX_OBJECTIVES; ++n_obj) {
        objective[n_obj] = toCopy.getObjective(n_obj);
        distance[n_obj] = toCopy.getDistance(n_obj);
    }
}

ObjectiveValues::~ObjectiveValues() {
}

void ObjectiveValues::setValue(int n_value) {
    variable_value = n_value;
}

void ObjectiveValues::setObjective(int n_objective, int value) {
    objective[n_objective] = value;
}

void ObjectiveValues::setDistance(int n_obj, float n_dist) {
    distance[n_obj] = n_dist;
}

int ObjectiveValues::getValue() const {
    return variable_value;
}

int ObjectiveValues::getObjective(int n_obj) const {
    return objective[n_obj];
}

float ObjectiveValues::getDistance(int n_obj) const {
    return distance[n_obj];
}

float ObjectiveValues::getCombination() const {
    return distance[0] + distance[1] + distance[2];
}

float ObjectiveValues::getSomethingToSort(const SORTING_TYPES sort) const {
    
    switch (sort) {
        case SORTING_TYPES::DOMINANCE:
            return 0; /** DOESNT APPLY**/
            break;
            
        case SORTING_TYPES::DIST_1:
            return distance[0];
            break;
            
        case SORTING_TYPES::DIST_2:
            return distance[1];
            break;
            
        case SORTING_TYPES::DIST_COMB: /** This part can be used to combine the distances. **/
            return distance[0] + distance[1] + distance[2];
            break;
            
        default:
            return 0;
            break;
    }
}

bool ObjectiveValues::operator==(const ObjectiveValues& rhs) const {
    for (int n_obj = 0; n_obj < MAX_OBJECTIVES; ++n_obj)
        if (objective[n_obj] != rhs.getObjective(n_obj))
            return false;
    return true;
}

bool ObjectiveValues::operator<(const ObjectiveValues& rhs) const {
    for (int n_obj = 0; n_obj < MAX_OBJECTIVES; ++n_obj)
        if (objective[n_obj] > rhs.getObjective(n_obj))
            return false;
    return true;
}

bool ObjectiveValues::operator<=(const ObjectiveValues& rhs) const {
    for (int n_obj = 0; n_obj < MAX_OBJECTIVES; ++n_obj)
        if (objective[n_obj] > rhs.getObjective(n_obj))
            return false;
    return true;
}

bool ObjectiveValues::operator>(const ObjectiveValues& rhs) const {
    for (int n_obj = 0; n_obj < MAX_OBJECTIVES; ++n_obj)
        if (objective[n_obj] < rhs.getObjective(n_obj))
            return false;
    return true;
}

bool ObjectiveValues::operator>=(const ObjectiveValues& rhs) const {
    for (int n_obj = 0; n_obj < MAX_OBJECTIVES; ++n_obj)
        if (objective[n_obj] <= rhs.getObjective(n_obj))
            return false;
    return true;
}

Dom ObjectiveValues::dominance(const ObjectiveValues& rhs) const {
    int n_obj = 0, obj_A = 0, obj_B = 0, local_is_better = 0, extern_is_better = 0, bot_are_equals = 1;
    
    for (n_obj = 0; n_obj < MAX_OBJECTIVES; ++n_obj) {
        obj_A = objective[n_obj];
        obj_B = rhs.getObjective(n_obj);
        
        if (obj_A < obj_B) {
            local_is_better++;
            bot_are_equals = 0;
        } else if (obj_B < obj_A) {
            extern_is_better++;
            bot_are_equals = 0;
        }
    }
    
    if (bot_are_equals == 1)
        return Dom::Eq;
    else if (local_is_better > 0 && extern_is_better == 0)
        return Dom::Domtes;
    else if (extern_is_better > 0 && local_is_better == 0)
        return Dom::Domted;
    else
        return Dom::Nondom;
}

void ObjectiveValues::print() {
    printf("%3d | %3d %3d %3d | %3.3f %3.3f %3.3f |\n", variable_value, objective[0], objective[1], objective[2], distance[0], distance[1], distance[2]);
}
