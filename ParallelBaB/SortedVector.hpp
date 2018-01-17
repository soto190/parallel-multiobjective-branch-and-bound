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
    /**
     * The elements are sorted by dominance. It also stores the dominated solutions at the end.
     * returns 1.
     **/
    int push(const ObjectiveValues & data, const SORTING_TYPES sort_type);
    int push_dist1(const ObjectiveValues& data);
    int push_dist2(const ObjectiveValues& data);
    int push_dist_comb(const ObjectiveValues& data);
    int push_dominance(const ObjectiveValues& data);
    
    std::deque<ObjectiveValues>::iterator begin();
    std::deque<ObjectiveValues>::iterator end();
    
    unsigned long binarySearchIncrement(const ObjectiveValues & data, const SORTING_TYPES sort);
    unsigned long binarySearchDecrement(const ObjectiveValues & data, const SORTING_TYPES sort);
    void print();
};

#endif /* SortedVector_hpp */
