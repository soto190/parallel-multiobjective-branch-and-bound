//
//  SubproblemsPool.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/6/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "SubproblemsPool.hpp"

SubproblemsPool::SubproblemsPool() {
    size.store(0);
}

void SubproblemsPool::setSizeEmptying(unsigned long size) {
    size_emptying = size;
}

unsigned long SubproblemsPool::getSizeEmptying() const {
    return size_emptying;
}

unsigned long SubproblemsPool::unsafe_size() const {
    return size;
}

bool SubproblemsPool::empty() const {
    return size > 0 ? false : true;
}

bool SubproblemsPool::isEmptying() const {
    return size < size_emptying ? true : false;
}

void SubproblemsPool::push(const Interval & subproblem) {
    priority_queues[subproblem.getPriority()].push(subproblem);
    size.fetch_and_increment();
}

bool SubproblemsPool::try_pop(Interval & interval) {
    for(int i = P_High; i <= P_Low; ++i) // Scan queues in priority order for a subproblem.
        if(priority_queues[i].try_pop(interval)) {
            size.fetch_and_decrement();
            return true;
        }
    return false;
}

void SubproblemsPool::print() const {
    unsigned long total_size = unsafe_size();
    printf("T:%4lu\t[H:%4lu\tM:%4lu\tL:%4lu\t]\n", total_size, priority_queues[P_High].unsafe_size(), priority_queues[P_Medium].unsafe_size(), priority_queues[P_Low].unsafe_size());
}
