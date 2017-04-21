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
Interval::Interval():
max_size(1),
build_up_to(-1),
interval(nullptr),
low(0),
high(0),
priority(Priority::P_Low){}

Interval::Interval(int max_size):
max_size(max_size),
build_up_to(-1),
interval(new int[max_size]),
low(max_size * low_priority),
high(max_size * high_priority),
priority(Priority::P_Low){}

Interval::Interval(const Interval &toCopy):
max_size(toCopy.getSize()),
interval(new int[toCopy.getSize()]),
build_up_to(toCopy.getBuildUpTo()),
priority(toCopy.getPriority()),
low(toCopy.getLowSize()),
high(toCopy.getHighSize()){
    
    int index = 0;
    for (index = 0; index < max_size; ++index)
        interval[index] = toCopy.getValueAt(index);

}

Interval& Interval::operator()(int size){
    
    build_up_to = -1;
    max_size = size;
    priority = Priority::P_Low;
    
    
    int index = 0;
    
    delete [] interval; /** Freeing previously used memory. **/
    interval = new int[size];
    for (index = 0; index < max_size; ++index)
        interval[index] = -1;
    
    return *this;
}

Interval& Interval::operator=(const Interval &toCopy){
    
    if (this == &toCopy) return *this;
    
    max_size = toCopy.getSize();
    build_up_to = toCopy.getBuildUpTo();
    priority = toCopy.getPriority();
    low = toCopy.getLowSize();
    high = toCopy.getHighSize();
    
    if(interval != nullptr)
        delete [] interval; /** Freeing previously used memory. **/
    interval = new int[max_size];
    int index = 0;
    
    for (index = 0; index < max_size; ++index)
        interval[index] = toCopy.getValueAt(index);
    
    return *this;
}

Interval::~Interval(){
    delete [] interval;
}

int Interval::getSize() const{ return  max_size;}
int Interval::getBuildUpTo() const{ return build_up_to;}
int Interval::getValueAt(int position) const{ return interval[position];}
Priority Interval::getPriority() const {return priority;}
int Interval::getLowSize() const{return low;}
int Interval::getHighSize() const{return high;}

void Interval::setLowPriority(){priority = Priority::P_Low;}
void Interval::setHighPriority(){priority = Priority::P_High;}
void Interval::setMediumPriority(){priority = Priority::P_Medium;}

void Interval::setSize(int size){ max_size = size; }
void Interval::setValueAt(int index, int value){
    interval[index] = value;
    build_up_to = index;
    
    if (build_up_to <= high)
        priority = Priority::P_High;
    else if (build_up_to >= low)
        priority = Priority::P_Low;
    else
        priority = Priority::P_Medium;
}

void Interval::setBuildUpTo(int newBuild){ build_up_to = newBuild; }
void Interval::removeLastValue(){interval[build_up_to] = -1; build_up_to--;}
int Interval::increaseBuildUpTo(){ return build_up_to++; }

bool Interval::verify() const{
    for (int in = build_up_to; in >= 0; --in)
        if (interval[in] == -1) return false;
    return true;
}

void Interval::print() const{
    int index_var = 0;
    char sep = '-';
    
    printf("[%3d][", build_up_to);
    for (index_var = 0; index_var <= build_up_to; ++index_var)
        printf("%3d ", interval[index_var]);
    
    for (index_var = build_up_to + 1; index_var < max_size; ++index_var)
        printf("%3c ", sep);
    
    printf("]\n");
}
