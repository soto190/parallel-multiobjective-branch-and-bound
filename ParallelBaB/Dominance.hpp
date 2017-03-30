//
//  Dominance.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/10/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#ifndef Dominance_hpp
#define Dominance_hpp

#include <stdio.h>
#include <vector>
#include "Solution.hpp"

DominanceRelation dominanceOperator(Solution & leftSolution, Solution & rightSolution);
int updateFront(Solution & solution, std::vector<Solution>& front);
void extractParetoFront(std::vector<Solution>& front);

#endif /* Dominance_hpp */
