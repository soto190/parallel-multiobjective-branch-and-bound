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
    int default_size = 1;
    this->interval = new int[default_size];
	this->build_up_to = -1;
	this->max_size = default_size;

	int index = 0;
	for (index = 0; index < default_size; index++)
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
    this->build_up_to = toCopy.getBuildUpTo();
    this->interval = new int[toCopy.getSize()];
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
    this->build_up_to = toCopy.getBuildUpTo();
    this->interval = new int[max_size];
    int index = 0;
    
    for (index = 0; index < this->max_size; index++)
        this->interval[index] = toCopy.getValueAt(index);
    
    return *this;
}

int Interval::getSize() const{ return  this->max_size;}
int Interval::getBuildUpTo() const{ return this->build_up_to;}
int Interval::getValueAt(int position) const{ return  this->interval[position];}

void Interval::setSize(int size){ this->max_size = size; }
void Interval::setValueAt(int index, int value){ this->interval[index] = value; }
void Interval::setBuildUpTo(int newBuild){ this->build_up_to = newBuild; }
int Interval::increaseBuildUpTo(){ return this->build_up_to++; }

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
