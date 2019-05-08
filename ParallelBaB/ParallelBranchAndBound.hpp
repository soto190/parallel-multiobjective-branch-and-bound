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
    std::string output_path;
    char pareto_file[255];
    char summarize_file[255];
    bool is_grid_enable = false;
    bool is_sorting_enable = false;
    bool is_priority_enable = false;
    double time_limit;

    unsigned long number_of_nodes;
    unsigned long number_of_nodes_created;
    unsigned long number_of_nodes_pruned;
    unsigned long number_of_nodes_explored;
    unsigned long number_of_nodes_unexplored;
    unsigned long number_of_calls_to_branch;
    unsigned long number_of_reached_leaves;
    unsigned long number_of_calls_to_prune;
    unsigned long number_of_updates_in_lower_bound;
    unsigned long number_of_tree_levels;
    unsigned long number_of_shared_works;

    unsigned long number_of_non_dominated_buckets;
    unsigned long number_of_unexplored_buckets;
    unsigned long number_of_dominated_buckets;
    
    ProblemFJSSP problem;
    Interval branch_init;
    ParetoFront pareto_front;

    double elapsed_time;
    std::chrono::high_resolution_clock::time_point time_start;
    std::chrono::high_resolution_clock::time_point time_end;

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
    void setOutputPath(const std::string outputPath);
    void setParetoFrontFile(const char outputFile[255]);
    void setSummarizeFile(const char outputFile[255]);
    void setBranchInit(const Interval& interval);
    void setBranchInitPayload(const Payload_interval& payload);
    
    int getRank() const;

    void saveSummarize();
    void saveParetoFront();
private:
    int initSharedPool(const Interval& initial);
    void initSharedParetoFront();
    double getElapsedTime();

};
#endif /* ParallelBranchAndBound_hpp */
