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
#include <exception>
#include "BranchAndBound.hpp"
#include "Interval.hpp"
#include "ProblemFJSSP.hpp"
#include "tbb/task.h"
#include "tbb/mutex.h"
//#include <memory.h> /** For the Ehecatl wich uses GCC 4.4.7, this activates the shared_ptr. **/

class ParallelBranchAndBound: public tbb::task{

public:
    int number_of_threads;
    char * outputParetoFile;
    char * summarizeFile;
    char * path[2];
        
    ProblemFJSSP problem;
    
    tbb::task* execute();
    
    void setNumberOfThreads(int number_of_threads);
    void setInstanceFile(char * path[]);
    void setProblem(const ProblemFJSSP&  problem);
    void setParetoFrontFile(const char * outputFile);
    void setSummarizeFile(const char * outputFile);
};

#endif /* ParallelBranchAndBound_hpp */
