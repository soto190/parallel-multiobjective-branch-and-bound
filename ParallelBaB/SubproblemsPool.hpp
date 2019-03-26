//
//  SubproblemsPool.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/6/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#ifndef SubproblemsPool_hpp
#define SubproblemsPool_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include "Interval.hpp"
#include "tbb/concurrent_queue.h"
#include "tbb/queuing_rw_mutex.h"

typedef tbb::queuing_rw_mutex Lock;

class SubproblemsPool {

private:
    tbb::concurrent_queue<Interval> priority_queues[P_Low + 1];   // One queue for each priority level.
    tbb::atomic<unsigned int> size;
    unsigned long size_emptying;
    unsigned long max_limit_size;
    
public:
    SubproblemsPool();

    Lock updating_lock;

    friend std::ostream &operator<<(std::ostream& stream, SubproblemsPool& interval);
    void setSizeEmptying(unsigned long size);
    unsigned long getSizeEmptying() const;
    unsigned long unsafe_size() const;
    bool isMaxLimitReached() const;
    bool empty() const;
    bool isEmptying() const;
    void push(const Interval & subproblem);
    bool try_pop(Interval & interval);
    void print() const;
};
#endif /* SubproblemsPool_hpp */
