//
//  main.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include <iostream>
#include <cstring>
#include "Solution.hpp"
#include "Problem.hpp"
#include "ProblemHCSP.hpp"
#include "ProblemTSP.hpp"
#include "ProblemVRP.hpp"
#include "ProblemFJSSP.hpp"
#include "BranchAndBound.hpp"
#include "ParallelBranchAndBound.hpp"
#include "myutils.hpp"
#include "GridContainer.hpp"
#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/task_group.h>


/**
 * argv[1] = problem: TSP, HCSP, VRP, and FJSSP.
 * argv[2] = First input file of instance.
 * argv[3] = Second input file of instance.
 * argv[4] = output folder for Pareto front, the file name is given by the instance name.
 */

int main(int argc, const char * argv[]) {
    
   std::shared_ptr<Problem> problem;
    
    if(strcmp(argv[1], "TSP") == 0){
        printf("Problem: TSP\n");
        problem = std::make_shared<ProblemTSP>(2, 1);
    }else if(strcmp(argv[1], "HCSP") == 0){
        printf("Problem: HCSP\n");
        problem = std::make_shared<ProblemHCSP>(2, 1);
    }else if(strcmp(argv[1], "VRP") == 0){
        printf("Problem: VRP\n");
        problem = std::make_shared<ProblemVRP>(2, 1);
    }else if(strcmp(argv[1], "FJSSP") == 0){
        printf("Problem: FJSSP\n");
        problem = std::make_shared<ProblemFJSSP>(2, 1);
    }
    else{
        printf("Problem no found.\n");
        return 0;
    }

    char *files[2];
    files[0] = new char[255];
    files[1] = new char[255];
    std::strcpy(files[0], argv[2]);
    std::strcpy(files[1], argv[3]);
    
    /** Get name of the instance. **/
    std::vector<std::string> elemens;
    std::vector<std::string> splited;
    elemens = split(argv[2], '/');
    
    unsigned long int sizeOfElems = elemens.size();
    splited = split(elemens[sizeOfElems - 1], '.' );
    printf("Name: %s\n", splited[0].c_str());
    
    printf("Files:\n\t%s\n\t%s\n", files[0],  files[1]);
    problem->setName(splited[0].c_str());
    problem->loadInstance(files);
    problem->printProblemInfo();
    /** End **/

    /** Preparing output files: 
     * - CSV: contains the Pareto front.
     * - TXT: contains a log file.
     **/
    std::string outputFile = argv[4] + splited[0] + ".csv";
    std::string summarizeFile = argv[4] + splited[0] + ".txt";
    
    
    
    tbb::task_scheduler_init init(tbb::task_scheduler_init::automatic);
    
    ParallelBranchAndBound * pbb = new(tbb::task::allocate_root()) ParallelBranchAndBound();
    pbb->setInstanceFile(files);
    pbb->setParetoFrontFile(outputFile.c_str());
    pbb->setSummarizeFile(summarizeFile.c_str());
    pbb->setProblem(problem);
    
    tbb::task::spawn_root_and_wait(*pbb);

    return 0;
}
