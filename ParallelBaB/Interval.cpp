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
priority(Priority::P_High),
deep(Deep::TOP),
build_up_to(-1),
max_size(0),
interval(nullptr) {
    distance[0] = 0;
    distance[1] = 0;
    distance[2] = 0;
}

Interval::Interval(int max_size):
priority(Priority::P_High),
deep(Deep::TOP),
build_up_to(-1),
max_size(max_size),
interval(new int[max_size]) {
    distance[0] = 0;
    distance[1] = 0;
    distance[2] = 0;
}

Interval::Interval(const Interval &toCopy):
priority(toCopy.getPriority()),
deep(toCopy.getDeep()),
build_up_to(toCopy.getBuildUpTo()),
max_size(toCopy.getSize()),
interval(new int[toCopy.getSize()]) {
    
    distance[0] = toCopy.getDistance(0);
    distance[1] = toCopy.getDistance(1);
    distance[2] = toCopy.getDistance(2);
    
    for (int index = 0; index < max_size; ++index)
        interval[index] = toCopy.getValueAt(index);
    
}

Interval::Interval(const Payload_interval& payload) {
    distance[0] = 0;
    distance[1] = 0;
    distance[2] = 0;

    build_up_to = payload.build_up_to;
    max_size = payload.max_size;
    
    priority = (Priority) payload.priority;
    deep = (Deep) payload.deep;
    
    interval = new int[max_size];
    for (int index = 0; index < max_size; ++index)
        interval[index] = payload.interval[index];
}

Interval::~Interval() {
    delete [] interval;
}

Interval& Interval::operator()(int size) {
    distance[0] = 0;
    distance[1] = 0;
    distance[2] = 0;

    build_up_to = -1;
    max_size = size;
    priority = Priority::P_Low;
    deep = Deep::TOP;
    
    delete [] interval; /** Freeing previously used memory. **/
    interval = new int[size];
    for (int index = 0; index < max_size; ++index)
        interval[index] = -1;
    
    return *this;
}

Interval& Interval::operator()(const Payload_interval& payload) {
    build_up_to = payload.build_up_to;
    max_size = payload.max_size;
    
    priority = (Priority) payload.priority;
    deep = (Deep) payload.deep;
    
    distance[0] = payload.distance[0];
    distance[1] = payload.distance[1];
//    distance[2] = payload.distance[2];

    if(interval != nullptr)
        delete [] interval;
    
    interval = new int[max_size];
    for (int index = 0; index < max_size; ++index)
        interval[index] = payload.interval[index];
    return *this;
}

Interval& Interval::operator=(const Interval &toCopy) {
    
    if (this == &toCopy) return *this;
    
    max_size = toCopy.getSize();
    build_up_to = toCopy.getBuildUpTo();
    priority = toCopy.getPriority();
    deep = toCopy.getDeep();
    
    distance[0] = toCopy.getDistance(0);
    distance[1] = toCopy.getDistance(1);
    distance[2] = toCopy.getDistance(2);
    
    if(interval != nullptr)
        delete [] interval; /** Freeing previously used memory. **/
    interval = new int[max_size];
    
    for (int index = 0; index < max_size; ++index)
        interval[index] = toCopy.getValueAt(index);
    
    return *this;
}

std::ostream &operator<<(std::ostream& stream, const Interval& interval) {
    stream << "[" << interval.getBuildUpTo() << "] [";
    for (int index_var = 0; index_var <= interval.getBuildUpTo(); ++index_var)
        stream << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill(' ') << interval.getValueAt(index_var);

    for (int index_var = interval.getBuildUpTo() + 1; index_var < interval.getSize(); ++index_var)
        stream << std::fixed << std::setw(6) << std::setfill(' ') << '-';

    for (int index_dist = 0; index_dist < 3; ++index_dist)
        stream << ']' << '[' << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill(' ') << interval.getDistance(index_dist);
    stream << ']' << ' ' <<'[' << interval.getDeep() << ' ' << interval.getPriority() << ']' << std::endl;
    return stream;
}

int Interval::getSize() const {
    return max_size;
}

int Interval::getBuildUpTo() const {
    return build_up_to;
}

int Interval::getValueAt(int position) const {
    return interval[position];
}

Priority Interval::getPriority() const {
    return priority;
}

Deep Interval::getDeep() const {
    return deep;
}

float Interval::getDistance(int n_dim) const {
    return distance[n_dim];
}

void Interval::setLowPriority() {
    priority = Priority::P_Low;
}

void Interval::setHighPriority() {
    priority = Priority::P_High;
}

void Interval::setNormalPriority() {
    priority = Priority::P_Normal;
}

void Interval::setDistance(int n_dim, float n_val) {
    distance[n_dim] = n_val;
}

void Interval::setValueAt(int index, int value) {
    interval[index] = value;
    setBuildUpTo(index);
}

void Interval::setBuildUpTo(int n_build) {
    build_up_to = n_build;
    if (isShortBranch())
        deep = Deep::TOP;
    else if(isMediumBranch())
        deep = Deep::MID;
    else
        deep = Deep::BOTTOM;
}

bool Interval::isShortBranch() const {
    return build_up_to < max_size * short_branch;
}

bool Interval::isMediumBranch() const { /** (short_size < branch_size < large_size)**/
    return  max_size * short_branch < build_up_to && build_up_to < max_size * large_branch;
}

bool Interval::isLargeBranch() const {
    return build_up_to > max_size * large_branch;
}

/** TODO: Verify if we try to remove a value when it doesn't has values. **/
void Interval::removeLastValue() {
    interval[build_up_to] = -1;
    build_up_to--;
}

int Interval::increaseBuildUpTo() {
    return build_up_to++;
}

bool Interval::verify() const {
    for (int in = 0; in <= build_up_to; ++in)
        if (interval[in] == -1)
            return false;
    return true;
}

void Interval::print() const {
    const char sep = '-';
    
    printf("[%3d][", build_up_to);
    for (int index_var = 0; index_var <= build_up_to; ++index_var)
        printf("%3d ", interval[index_var]);
    
    for (int index_var = build_up_to + 1; index_var < max_size; ++index_var)
        printf("%3c ", sep);
    
    printf("] [%3.3f, %3.3f, %3.3f] [%1d, %1d]\n", distance[0], distance[1], distance[2], deep, priority);
}
