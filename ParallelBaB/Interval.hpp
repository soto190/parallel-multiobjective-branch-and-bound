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
#include <iostream>
#include <iomanip>

/**
 *
 * Interval represents an interval/branch to be explored.
 * interval is a pointer to an array
 * level indicates the level of the interval.
 *
 **/
const float large_branch  = 0.666f;
const float short_branch = 0.333f;

enum Priority {P_High = 0, P_Normal = 1, P_Low = 2};
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
    float distance[3];
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

    friend std::ostream &operator<<(std::ostream& stream, const Interval& interval);

    int getSize() const;
    int getBuildUpTo() const;
    int getValueAt(int position) const;
    Priority getPriority() const;
    Deep getDeep() const;
    float getDistance(int n_dim) const;
    int increaseBuildUpTo();
    void setValueAt(int index, int value);
    void setLowPriority();
    void setHighPriority();
    void setNormalPriority();
    void setDistance(int n_dim, float n_val);
    void removeLastValue();
    void print() const;
    bool verify() const;
    
private:
    void setBuildUpTo(int newBuild);
    bool isShortBranch() const;
    bool isMediumBranch() const;
    bool isLargeBranch() const;
};
#endif /* Interval_hpp */
