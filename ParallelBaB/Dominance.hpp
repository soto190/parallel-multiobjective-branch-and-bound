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

enum DominanceRelation {
	Dominates = 1, Dominated = -1, Nondominated = 0, Equals = 11
};

DominanceRelation dominanceOperator(Solution & legtSolution,
		Solution & rightSolution);
int updateFront(Solution & solution, std::vector<Solution>& front);
void extractParetoFront(std::vector<Solution>& front);

#endif /* Dominance_hpp */
