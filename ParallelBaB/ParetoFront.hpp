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

class ParetoFront{
private:
    std::vector<Solution> m_vec;
    
public:
    ParetoFront();
    ParetoFront(const ParetoFront& toCopy);
    ~ParetoFront();
    
    std::vector<Solution>& getVector() ;
    const std::vector<Solution>& getVectorToCopy() const ;
    int produceImprovement(const Solution& obj);
    int push_back(const Solution& obj);
    unsigned long size();
    bool empty();
    void clear();
    void print() const;
};

#endif /* ParetoFront_hpp */
