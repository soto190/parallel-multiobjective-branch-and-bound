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
//#include <memory.h> /** For the Ehecatl wich uses GCC 4.4.7, this activates the shared_ptr. **/

class ParallelBranchAndBound: public tbb::task {
    
public:
    int rank;
    int number_of_bbs;
    char outputParetoFile[255];
    char summarizeFile[255];
    bool is_grid_enable = false;
    bool is_sorting_enable = false;
    bool is_priority_enable = false;
    double time_limit;

    ProblemFJSSP problem;
    Interval branch_init;


    ParallelBranchAndBound(int rank, int n_threads, const ProblemFJSSP& problem);
    ~ParallelBranchAndBound();
    tbb::task* execute();

    double getTimeLimit() const;
    void setTimeLimit(double time_sec);
    
    void enableGrid();
    void enableSortingNodes();
    void enablePriorityQueue();
    bool isGridEnable() const;
    bool isSortingEnable() const;
    bool isPriorityEnable() const;
    
    void setNumberOfThreads(int number_of_threads);
    void setParetoFrontFile(const char outputFile[255]);
    void setSummarizeFile(const char outputFile[255]);
    void setBranchInit(const Interval& interval);
    void setBranchInitPayload(const Payload_interval& payload);
    
    int getRank() const;
};
#endif /* ParallelBranchAndBound_hpp */
