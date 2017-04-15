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
interval(nullptr){}

Interval::Interval(int max_size):
max_size(max_size),
build_up_to(-1),
interval(new int[max_size]){
}

Interval::Interval(const Interval &toCopy):
max_size(toCopy.getSize()),
interval(new int[toCopy.getSize()]),
build_up_to(toCopy.getBuildUpTo()){
    
    int index = 0;
    for (index = 0; index < max_size; ++index)
        interval[index] = toCopy.getValueAt(index);

}

Interval& Interval::operator()(int size){
    
    build_up_to = -1;
    max_size = size;
    
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

void Interval::setSize(int size){ max_size = size; }
void Interval::setValueAt(int index, int value){ interval[index] = value; }
void Interval::setBuildUpTo(int newBuild){ build_up_to = newBuild; }
int Interval::increaseBuildUpTo(){ return build_up_to++; }

void Interval::print() const{
    
    int index_var = 0;
    char sep = '-';
    
    printf("[");
    for (index_var = 0; index_var <= build_up_to; ++index_var)
        printf("%3d ", interval[index_var]);
    
    for (index_var = build_up_to + 1; index_var < max_size; ++index_var)
        printf("%3c ", sep);
    
    printf("]\n");
    
}
