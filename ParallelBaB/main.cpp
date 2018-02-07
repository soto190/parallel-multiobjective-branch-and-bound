//
//  main.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//
/**
 * Debugging and running MPI programs in Xcode:
 * - http://wp.yaonan.li/archives/1065
 **/

#include <mpi.h>
#include <iostream>
#include <cstring>
#include <exception>
#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/task_group.h>
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
#include "MasterWorkerPBB.hpp"
#include "MOSA.hpp"

using namespace std;

void one_node(int argc, char* argv[]){
    const int arg_num_threads = 1;
    //const int arg_problem = 2;
    const int arg_input_file1 = 3;
    const int arg_input_file2 = 4;
    const int arg_output = 5;
    ProblemFJSSP  problem (2, 1);

    int number_of_threads = stoi(argv[arg_num_threads]);
    printf("Number of threads:%3d\n", number_of_threads);
    printf("Deep sharing: %0.2f\n", deep_limit_share);
    
    char files [2][255], extension[10];
    std::strcpy(files[0], argv[arg_input_file1]);
    std::strcpy(files[1], argv[arg_input_file2]);
    
    /** Get name of the instance. **/
    std::vector<std::string> elemens;
    std::vector<std::string> splited;
    elemens = split(argv[arg_input_file1], '/');
    
    unsigned long int sizeOfElems = elemens.size();
    splited = split(elemens[sizeOfElems - 1], '.');
    printf("Name: %s\n", splited[0].c_str());
    printf("File extension: %s\n", splited[1].c_str());
    std::strcpy(extension, splited[1].c_str());
    
    printf("Files:\n\t%s\n\t%s\n", files[0], files[1]);
    problem.setName(splited[0].c_str());
    problem.loadInstance(files, extension);
    problem.printProblemInfo();
    /** End **/
    
    /** Preparing output files:
     * - CSV: contains the Pareto front.
     * - TXT: contains a log file.
     **/
    std::string outputFile = argv[arg_output] + splited[0] + ".csv";
    std::string summarizeFile = argv[arg_output] + splited[0] + ".txt";
    
    try {
        
        tbb::task_scheduler_init init(number_of_threads);
        
        Solution sample_solution(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
//        sample_solution.setVariable(0, 6);
//        sample_solution.setVariable(1, 7);
//        sample_solution.setVariable(2, 8);
//        sample_solution.setVariable(3, 3);
//        sample_solution.setVariable(4, 3);
//        sample_solution.setVariable(5, 5);
//        sample_solution.setVariable(6, 0);
//        sample_solution.setVariable(7, 1);
//        sample_solution.setVariable(8, 1);
        
        problem.createDefaultSolution(sample_solution);
        problem.evaluate(sample_solution);
        sample_solution.print();
        
        MOSA algorithm(problem);
        algorithm.setSampleSolution(sample_solution);
        algorithm.setCoolingRate(0.96);
        algorithm.setMaxMetropolisIterations(10);
        algorithm.setInitialTemperature(1000);
        algorithm.setFinalTemperature(0.01);
        algorithm.setPerturbationRate(0.950);
        algorithm.solve();
        
        /*
        ParallelBranchAndBound * pbb = new (tbb::task::allocate_root()) ParallelBranchAndBound(0, number_of_threads, problem);
        pbb->setParetoFrontFile(outputFile.c_str());
        pbb->setSummarizeFile(summarizeFile.c_str());
        
        printf("Spawning root...\n");
        tbb::task::spawn_root_and_wait(*pbb);
        */
    } catch (tbb::tbb_exception& e) {
        std::cerr << "Intercepted exception:\n" << e.name();
        std::cerr << "Reason is:\n" << e.what();
    }
    
    printf("Done.\n");
}

/**
 * argv[1] = number of threads.
 * argv[2] = problem: TSP, HCSP, VRP, and FJSSP.
 * argv[3] = First input file of instance.
 * argv[4] = Second input file of instance.
 * argv[5] = output folder for Pareto front, the file name is given by the instance name.
 */
int main(int argc, char* argv[]) {
    
    const int arg_num_threads = 1;
    const int arg_input_file = 3;
    const int arg_ouput_path = 5;

    MPI_Init(&argc, &argv);
    int rank = MPI::COMM_WORLD.Get_rank();
    int size_world = MPI::COMM_WORLD.Get_size();
    
    if (rank == 0)
        printf("[Master] Number of nodes: %3d\n", size_world);
    
    if (size_world == 1) { /** MPI disable or one node request: shared memory version. **/
        one_node(argc, argv);
    }else{/** MPI enable: Distributed memory. **/
        
        try {
            tbb::task_scheduler_init init(stoi(argv[arg_num_threads]));
            MasterWorkerPBB *  mw_pbb = new (tbb::task::allocate_root()) MasterWorkerPBB (size_world, stoi(argv[arg_num_threads]), argv[arg_input_file], argv[arg_ouput_path]);
            tbb::task::spawn_root_and_wait(*mw_pbb);
        } catch (tbb::tbb_exception& e) {
            std::cerr << "Intercepted exception:\n" << e.name();
            std::cerr << "Reason is:\n" << e.what();
        }
        printf("[Node-%02d] Done.\n", rank);
    }
    MPI_Finalize();
}
