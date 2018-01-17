//
//  ObjectiveValues.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "ObjectiveValues.hpp"


ObjectiveValues::ObjectiveValues(){
}

ObjectiveValues::ObjectiveValues(int var_value, int obj1, int obj2): value(var_value){
    obj[0] = obj1;
    obj[1] = obj2;
}

ObjectiveValues::ObjectiveValues(const ObjectiveValues & toCopy){
    value = toCopy.getValue();
    for (int n_obj = 0; n_obj < 2; ++n_obj){
        obj[n_obj] = toCopy.getObjective(n_obj);
        distance[n_obj] = toCopy.getDistance(n_obj);
    }
}

void ObjectiveValues::setValue(int n_value){
    value = n_value;
}

void ObjectiveValues::setObjective(int n_objective, int value){
    obj[n_objective] = value;
}

void ObjectiveValues::setDistance(int n_obj, float n_dist){
    distance[n_obj] = n_dist;
}

int ObjectiveValues::getValue() const{
    return value;
}

int ObjectiveValues::getObjective(int n_obj) const{
    return obj[n_obj];
}

float ObjectiveValues::getDistance(int n_obj) const{
    return distance[n_obj];
}

float ObjectiveValues::getCombination() const{
    return distance[0] + distance[1];
}

float ObjectiveValues::getSomethingToSort(const SORTING_TYPES sort) const{
    
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
            return distance[0] + distance[1];
            break;
            
        default:
            return 0;
            break;
    }
}

bool ObjectiveValues::operator==(const ObjectiveValues& rhs) const{
    for (int n_obj = 0; n_obj < 2; ++n_obj)
        if (obj[n_obj] != rhs.getObjective(n_obj))
            return false;
    return true;
}

bool ObjectiveValues::operator<(const ObjectiveValues& rhs) const{
    for (int n_obj = 0; n_obj < 2; ++n_obj)
        if (obj[n_obj] > rhs.getObjective(n_obj))
            return false;
    return true;
}

bool ObjectiveValues::operator<=(const ObjectiveValues& rhs) const{
    for (int n_obj = 0; n_obj < 2; ++n_obj)
        if (obj[n_obj] > rhs.getObjective(n_obj))
            return false;
    return true;
}

bool ObjectiveValues::operator>(const ObjectiveValues& rhs) const{
    for (int n_obj = 0; n_obj < 2; ++n_obj)
        if (obj[n_obj] < rhs.getObjective(n_obj))
            return false;
    return true;
}

bool ObjectiveValues::operator>=(const ObjectiveValues& rhs) const{
    for (int n_obj = 0; n_obj < 2; ++n_obj)
        if (obj[n_obj] <= rhs.getObjective(n_obj))
            return false;
    return true;
}

Dom ObjectiveValues::dominance(const ObjectiveValues& rhs) const{
    int nObj = 0, objA = 0, objB = 0, localSolIsBetterIn = 0, exterSolIsBetterIn = 0, equals = 1;
    
    for (nObj = 0; nObj < 2; ++nObj) {
        objA = obj[nObj];
        objB = rhs.getObjective(nObj);
        
        if (objA < objB) {
            localSolIsBetterIn++;
            equals = 0;
        } else if (objB < objA) {
            exterSolIsBetterIn++;
            equals = 0;
        }
    }
    
    if (equals == 1)
        return Dom::Eq;
    else if (localSolIsBetterIn > 0 && exterSolIsBetterIn == 0)
        return Dom::Domtes;
    else if (exterSolIsBetterIn > 0 && localSolIsBetterIn == 0)
        return Dom::Domted;
    else
        return Dom::Nondom;
}

void ObjectiveValues::print(){
    printf("%3d | %3d %3d | %3.3f %3.3f |\n", value, obj[0], obj[1], distance[0], distance[1]);
}
