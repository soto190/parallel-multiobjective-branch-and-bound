//
//  SortedVector.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef SortedVector_hpp
#define SortedVector_hpp

#include <stdio.h>
#include <iterator>
#include <deque>
#include "myutils.hpp"
#include "ObjectiveValues.hpp"

class SortedVector{
    deque<ObjectiveValues> m_data;
    
public:
    SortedVector();
    ~SortedVector();
    
    int push(const ObjectiveValues & objValues, const SORTING_TYPES sort_type);
    int push_dist1(const ObjectiveValues& objValues);
    int push_dist2(const ObjectiveValues& objValues);
    int push_dist_comb(const ObjectiveValues& objValues);
    int push_dominance(const ObjectiveValues& objValues);
    
    std::deque<ObjectiveValues>::iterator begin();
    std::deque<ObjectiveValues>::iterator end();
    
    unsigned long binarySearchIncrement(const ObjectiveValues & objValues, const SORTING_TYPES sort_type);
    unsigned long binarySearchDecrement(const ObjectiveValues & objValues, const SORTING_TYPES sort_type);
    void print();
};
#endif /* SortedVector_hpp */
