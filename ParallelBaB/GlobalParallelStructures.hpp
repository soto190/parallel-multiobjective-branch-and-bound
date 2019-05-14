//
//  GlobalParallelStructures.h
//  ParallelBaB
//
//  Created by Carlos Soto on 5/14/19.
//  Copyright © 2019 Carlos Soto. All rights reserved.
//

#ifndef GlobalParallelStructures_h
#define GlobalParallelStructures_h

#include "SubproblemsPool.hpp"
#include "ConcurrentHandlerContainer.hpp"
#include "tbb/atomic.h"

const float size_to_share = 0.2f; /** We share two percent of the row. **/
const float deep_limit_share = 0.80f;

extern SubproblemsPool sharedPool;  /** intervals are the pending branches/subproblems/partialSolutions to be explored. **/
//extern ConcurrentHandlerContainer sharedParetoContainer;
extern ParetoBucket sharedParetoFront;
extern tbb::atomic<int> sleeping_bb;
extern tbb::atomic<int> there_is_more_work;

#endif /* GlobalParallelStructures_h */
