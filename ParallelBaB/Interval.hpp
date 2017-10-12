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
const float large_branch  = 0.666f;
const float short_branch = 0.333f;

enum Priority {
    P_High = 0,     /** Deeper branches | branches at bottom | large branches. **/
    P_Medium = 1,   /** Branches at middle | medium size branches. **/
    P_Low = 2       /** Brancheas near to root | branches at top | short branches. **/
};

enum Deep{TOP = 0, MID = 1, BOTTOM = 2};

typedef struct {
    int priority;
    int deep;
    int build_up_to;
    int max_size;
    float distance[2];
    int * interval;
} Payload_interval;

class Interval {
private:
    Priority priority;
    Deep deep;
    
    int build_up_to = -1;
    int max_size = 0;
    
    float distance[2];
    int * interval;
    
public:
    
    Interval();
    Interval(int max_size);
    Interval(const Interval &toCopy);
    Interval(const Payload_interval& payload);
    virtual ~Interval();
    
    Interval& operator=(const Interval& toCopy);
    Interval& operator()(int size);
    Interval& operator()(const Payload_interval& payload);
    
    int increaseBuildUpTo();
    int getSize() const;
    int getBuildUpTo() const;
    int getValueAt(int position) const;
    int getLowSize() const;
    int getHighSize() const;
    
    float getDistance(int n_dim) const;
    
    Priority getPriority() const;
    Deep getDeep() const;
    void setBuildUpTo(int newBuild);
    void setSize(int size);
    void setValueAt(int index, int value);
    
    void setLowPriority();
    void setHighPriority();
    void setMediumPriority();
    
    void setDistance(int n_dim, float n_val);
    
    void removeLastValue();
    void print() const;
    bool verify() const;
};

class ReadySubproblems {
    // One queue for each priority level
    tbb::concurrent_queue<Interval> level[P_Low + 1];
    tbb::atomic<unsigned int> size;
    unsigned long size_emptying = 10;
    
public:
    ReadySubproblems(){size.store(0);}
    void setSizeEmptying(unsigned long size){ size_emptying = size;}
    unsigned long getSizeEmptying() const{return size_emptying;}
    unsigned long unsafe_size() const{ return size; }
    bool empty() const{ return size > 0 ? false : true;}
    bool isEmptying() const{ return size < size_emptying ? true : false;}
    
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
