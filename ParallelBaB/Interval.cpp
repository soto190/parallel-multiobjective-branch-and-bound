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

Interval::Interval(){
}

Interval::Interval(int max_size){
    this->interval = new int[max_size];
    this->build_up_to = -1;
    this->max_size = max_size;
}

Interval::Interval(const Interval &toCopy){
    this->max_size = toCopy.max_size;
    this->build_up_to = toCopy.build_up_to;
    this->interval = new int[max_size];
    int index = 0;
    
    for (index = 0; index < toCopy.max_size; index++)
        this->interval[index] = toCopy.interval[index];
    
}

Interval::~Interval(){
    //delete [] interval;
}

Interval& Interval::operator=(const Interval &toCopy){
    this->max_size = toCopy.max_size;
    this->build_up_to = toCopy.build_up_to;
    this->interval = new int[max_size];
    int index = 0;
    
    for (index = 0; index < toCopy.max_size; index++)
        this->interval[index] = toCopy.interval[index];
    
    
    return *this;
}

void Interval::showInterval() const{
    
    int index_var = 0;
    char sep = '-';
    
    printf("[");
    for (index_var = 0; index_var <= this->build_up_to; index_var++)
        printf("%3d ", this->interval[index_var]);
    
    for (index_var = this->build_up_to + 1; index_var < this->max_size; index_var++)
        printf("%3c ", sep);
    
    printf("]\n");
    
}

Interval& Interval::operator()(int size){
    
    this->interval = new int[size];
    this->build_up_to = -1;
    this->max_size = size;
    
    return *this;
}
