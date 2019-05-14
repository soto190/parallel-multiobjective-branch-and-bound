//
//  GlobalParallelStructures.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 5/14/19.
//  Copyright © 2019 Carlos Soto. All rights reserved.
//

#include <stdio.h>
#include "GlobalParallelStructures.hpp"

SubproblemsPool sharedPool;
//ConcurrentHandlerContainer sharedParetoContainer;
ParetoBucket sharedParetoFront;
tbb::atomic<int> sleeping_bb;
tbb::atomic<int> there_is_more_work;
