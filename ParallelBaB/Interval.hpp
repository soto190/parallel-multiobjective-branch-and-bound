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

/**
 *
 * Interval represents an interval/branch to be explored.
 * interval is a pointer to an array
 * level indicates the level of the interval.
 *
 **/

class Interval {
private:
    int build_up_to = -1;
    int * interval;
    int max_size = 0;

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
    
    void setBuildUpTo(int newBuild);
    void setSize(int size);
    void setValueAt(int index, int value);

    void removeLastValue();
	void print() const;
};

#endif /* Interval_hpp */
