//
//  Interval.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 01/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#ifndef Interval_hpp
#define Interval_hpp

#include <stdio.h>

/**
 *
 * Interval represents an interval/branch to be explored.
 * interval is a pointer to an array
 * level indicates the level of the interval.
 *
 **/

class Interval{
   
public:
    int build_up_to = -1;
    int * interval;
    
    Interval(int max_size);
    ~Interval();
};


#endif /* Interval_hpp */
