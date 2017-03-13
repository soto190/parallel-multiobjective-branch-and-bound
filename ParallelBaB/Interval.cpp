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
	this->interval = new int[1];
	this->build_up_to = -1;
	this->max_size = 1;

	int index = 0;
	for (index = 0; index < this->max_size; index++)
		this->interval[index] = -1;
}

Interval::Interval(int max_size){
    this->interval = new int[max_size];
    this->build_up_to = -1;
    this->max_size = max_size;
    
    int index = 0;
    for (index = 0; index < max_size; index++)
        this->interval[index] = -1;
}

Interval::Interval(const Interval &toCopy){
    this->max_size = toCopy.getSize();
    this->build_up_to = toCopy.getBuidUpTo();
    this->interval = new int[max_size];
    int index = 0;
    
    for (index = 0; index < this->max_size; index++)
        this->interval[index] = toCopy.getValueAt(index);

}

Interval& Interval::operator()(int size){
    
    this->interval = new int[size];
    this->build_up_to = -1;
    this->max_size = size;
    
    int index = 0;
    for (index = 0; index < max_size; index++)
        this->interval[index] = -1;
    
    return *this;
}

Interval::~Interval(){
    delete [] interval;
}

Interval& Interval::operator=(const Interval &toCopy){
    this->max_size = toCopy.getSize();
    this->build_up_to = toCopy.getBuidUpTo();
    this->interval = new int[max_size];
    int index = 0;
    
    for (index = 0; index < this->max_size; index++)
        this->interval[index] = toCopy.getValueAt(index);
    
    return *this;
}

int Interval::getSize() const{ return  this->max_size;}
int Interval::getBuidUpTo() const{ return this->build_up_to;}
int Interval::getValueAt(int position) const{ return  this->interval[position];}

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
