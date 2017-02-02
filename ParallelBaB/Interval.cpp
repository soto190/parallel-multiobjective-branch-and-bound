//
//  Interval.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 01/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "Interval.hpp"


/**
 *
 * size is equals to the number of variables.
 *
 **/
Interval::Interval(int max_size){
    this->interval = new int[max_size];
    this->build_up_to = -1;
}

Interval::~Interval(){
    delete [] interval;
}
