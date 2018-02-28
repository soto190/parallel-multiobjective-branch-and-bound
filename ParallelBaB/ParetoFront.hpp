//
//  ParetoFront.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 2/2/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef ParetoFront_hpp
#define ParetoFront_hpp

#include <stdio.h>
#include "Solution.hpp"
#include "Dominance.hpp"
#include "myutils.hpp"

enum FrontState {
    Unexplored = 0, NonDominated = 1, dominated = 2
};

class ParetoFront {
    
private:
    FrontState state;
    std::vector<Solution> m_vec;
    
public:
    ParetoFront();
    ParetoFront(const ParetoFront& toCopy);
    ParetoFront(const std::vector<Solution>& set_of_solutions);
    ~ParetoFront();
    
    ParetoFront& operator=(const ParetoFront& rhs);
    ParetoFront& operator+=(const ParetoFront& rhs);
    ParetoFront& operator+(const ParetoFront& rhs);

    FrontState getState() const;
    void setUnexplored();
    void setNonDominated();
    void setDominated();

    std::vector<Solution>& getVector();
    const std::vector<Solution>& getVectorToCopy() const;
    bool produceImprovement(const Solution& obj);
    bool push_back(const Solution& obj);
    void join(const ParetoFront& to_join);
    const Solution at(unsigned long position) const;
    const Solution back() const;
    const Solution front() const;
    unsigned long size() const;
    bool empty() const;
    void clear();
    void print() const;
};
#endif /* ParetoFront_hpp */
