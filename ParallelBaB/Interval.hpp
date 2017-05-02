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
#include "tbb/concurrent_queue.h"

/**
 *
 * Interval represents an interval/branch to be explored.
 * interval is a pointer to an array
 * level indicates the level of the interval.
 *
 **/
const float low_priority  = 0.666f;
const float high_priority = 0.333f;

enum Priority {
    P_High,     /** Deeper branches.        **/
    P_Medium,   /** Branches at middle.     **/
    P_Low       /** Brancheas near to root. **/
};

class Interval {
private:
    int build_up_to = -1;
    int * interval;
    int max_size = 0;
    Priority priority;
    
    int low;
    int high;
public:
    
    Interval();
    Interval(int max_size);
    Interval(const Interval &toCopy);
    ~Interval();
    
    Interval& operator=(const Interval& toCopy);
    Interval& operator()(int size);
    
    int increaseBuildUpTo();
    int getSize() const;
    int getBuildUpTo() const;
    int getValueAt(int position) const;
    int getLowSize() const;
    int getHighSize() const;
    
    Priority getPriority() const;
    
    void setBuildUpTo(int newBuild);
    void setSize(int size);
    void setValueAt(int index, int value);
    void setLowPriority();
    void setHighPriority();
    void setMediumPriority();
    
    void removeLastValue();
    void print() const;
    bool verify() const;
};

class ReadySubproblems {
    // One queue for each priority level
    tbb::concurrent_queue<Interval> level[P_Low + 1];
    tbb::atomic<unsigned int> size;
    unsigned int size_empty;
    
public:
    ReadySubproblems(){size.store(0);}
    void setSizeEmpty(int size){ size_empty = size;}
    unsigned int getSizeEmpty() const{return size_empty;}
    unsigned long unsafe_size() const{ return size; }
    bool empty() const{ return size > 0?false:true;}
    
    void push(const Interval & subproblem) {
        level[subproblem.getPriority()].push(subproblem);
        size.fetch_and_increment();
        /**
         TODO: Test the next line to launch one thread per subproblem.
         tbb::task::enqueue(*new(tbb::task::allocate_root()) RunWorkItem);
         **/
    }
    
    bool try_pop(Interval & interval) {
        // Scan queues in priority order for a subproblem.
        for(int i = P_High; i <= P_Low; ++i)
            if(level[i].try_pop(interval)){
                size.fetch_and_decrement();
                return true;
            }
        return false;
    }
    
    void print() const{
        unsigned long total_size = unsafe_size();
        printf("T:%4lu\t[H:%4lu\tM:%4lu\tL:%4lu\t]\n", total_size, level[P_High].unsafe_size(), level[P_Medium].unsafe_size(), level[P_Low].unsafe_size());
    }
};
#endif /* Interval_hpp */
