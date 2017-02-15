//
//  ParallelBranchAndBound.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 06/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#ifndef ParallelBranchAndBound_hpp
#define ParallelBranchAndBound_hpp

#include <stdio.h>
#include "BranchAndBound.hpp"
#include "Interval.hpp"
#include "ProblemFJSSP.hpp"
#include "tbb/task.h"
#include "tbb/mutex.h"

class ParallelBranchAndBound: public tbb::task{

public:
    int number_of_threads;
    char * outputParetoFile;
    char * summarizeFile;
    char * path[2];
        
    std::shared_ptr<Problem> problem;
    
    tbb::task* execute();
    
    void setNumberOfThreads(int number_of_threads);
    void setInstanceFile(char * path[]);
    void setProblem(std::shared_ptr<Problem> problem);
    void setParetoFrontFile(const char * outputFile);
    void setSummarizeFile(const char * outputFile);
};

#endif /* ParallelBranchAndBound_hpp */
