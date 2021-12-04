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
#include <getopt.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#include "Solution.hpp"
#include "Problem.hpp"
#include "ProblemHCSP.hpp"
#include "ProblemTSP.hpp"
#include "ProblemVRP.hpp"
#include "ProblemFJSSP.hpp"
#include "BranchAndBound.hpp"
#include "ParallelBranchAndBound_FJSSP.hpp"
#include "myutils.hpp"
#include "MasterWorkerPBB_FJSSP.hpp"

using namespace std;

void print_usage() {
    printf("Usage:\n\tbranchandbound: -t num -l time_sec -g [enable|disable] -q [enable|disable] -s [enable|disable] -p problem_type [FJSSP|TSP|HCSP|VRP] -i instance_file -o output_file\n");
}

void one_node(int argc, char* argv[]) {
    int index;
    int iarg = 0;

    double time_limit = 0;
    int number_of_threads = 1;
    string problem_type = "not especified";
    string instance_path = "not especified";
    string output_path = "not especified";
    bool is_grid_enable = false;
    bool is_sorting_enable = false;
    unsigned int is_priority_enable = 0;


    const struct option longopts[] = {
        {"threads", required_argument,  0, 't'},
        {"time-limit", optional_argument,  0, 'l'},
        {"grid",    required_argument,  0, 'g'},
        {"queue",   required_argument,  0, 'q'},
        {"sorting", required_argument,  0, 's'},
        {"problem", required_argument,  0, 'p'},
        {"instance",required_argument,  0, 'i'},
        {"output",  required_argument,  0, 'o'},
        {"AppleLanguages", optional_argument, 0, 'A'},
        {"AppleTextDirection", optional_argument, 0, 'A'},
        {"AppleLocale", optional_argument, 0, 'A'},
        {0,0,0,0},
    };

    while(iarg != -1) {
        iarg = getopt_long(argc, argv, "t:l:g:q:s:p:i:o:A:", longopts, &index);
        switch (iarg) {
            case 't':
                if(optarg)
                    number_of_threads = stoi(optarg);

                std::cout << "Number of threads: " << number_of_threads << std::endl;
                break;

            case 'l':
                if(optarg)
                    time_limit = stod(optarg);
                if (time_limit > 0)
                    std::cout << "Time limit (sec): " << time_limit << std::endl;
                else
                    std::cout << "Time limit (sec): disable" << std::endl;
                break;

            case 'g':
                std::cout << "Grid: ";
                if (optarg) {
                    string tmp = optarg;
                    if (tmp == "enable")
                        is_grid_enable = true;
                }
                std::cout << (is_grid_enable ? "enable" : "disable") << std::endl;
                break;

            case 'q':
                std::cout << "Priority queue: ";
                if (optarg)
                    is_priority_enable = stoi(optarg);;

                std::cout << is_priority_enable << std::endl;
                break;

            case 's':
                std::cout << "Sorting nodes: ";
                if (optarg) {
                    string tmp = optarg;
                    if (tmp == "enable")
                        is_sorting_enable = true;
                }
                std::cout << (is_sorting_enable ? "enable" : "disable") << std::endl;

                break;

            case 'p':
                if (optarg)
                    problem_type = optarg;
                std::cout << "Problem: " << problem_type << std::endl;
                break;

            case 'i':
                if (optarg)
                    instance_path = optarg;
                std::cout << "Instance file: " << instance_path << std::endl;
                break;

            case 'o':
                if (optarg)
                    output_path = optarg;
                std::cout << "Output file: " << output_path << std::endl;
                break;

            case 'A':
                break;

            case -1:
                break;
                
            default:
                print_usage();
                break;
        }
    }

    ProblemFJSSP problem (3, 1);

    printf("Deep sharing: %0.2f\n", deep_limit_share);
    
    char files [2][255], extension[10];
    std::strcpy(files[0], instance_path.c_str());

    /** Get name of the instance. **/
    std::vector<std::string> elemens;
    std::vector<std::string> splited;
    elemens = split(instance_path.c_str(), '/');
    
    unsigned long int sizeOfElems = elemens.size();
    splited = split(elemens[sizeOfElems - 1], '.');
    printf("Name: %s\n", splited[0].c_str());
    printf("File extension: %s\n", splited[1].c_str());
    std::strcpy(extension, splited[1].c_str());
    
    printf("Files:\n\t%s\n", files[0]);
    problem.setName(splited[0].c_str());
    problem.loadInstance(files, extension);
    problem.printProblemInfo();
    //std:exit(1);

    /** End **/
    
    /** Preparing output files:
     * - CSV: contains the Pareto front.
     * - TXT: contains a log file.
     **/
    //std::string outputFile = output_path + splited[0] + ".csv";
    std::string summarizeFile = output_path + splited[0] + ".txt";
    std::string outputFile;
    Solution temp_1(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.createDefaultSolution(temp_1);

    for(unsigned int run = 1; run <= 30; ++run) {
        outputFile = output_path + splited[0] + "-r" + std::to_string(run) + ".csv";

        NSGA_II nsgaii_algorithm(problem);
        nsgaii_algorithm.setSampleSolution(temp_1);
        nsgaii_algorithm.setCrossoverRate(0.9);
        nsgaii_algorithm.setMutationRate(0.10);
        nsgaii_algorithm.setMaxPopulationSize(100);
        nsgaii_algorithm.setMaxNumberOfGenerations(200);
        ParetoFront algorithms_pf = nsgaii_algorithm.solve();

        //std::cout << temp_1 << temp << temp_2 << std::endl;
        std::cout << "NSGA-II : " << std::endl << algorithms_pf << std::endl;

        std::ofstream myfile(outputFile.c_str());

        if (myfile.is_open()) {
            myfile << algorithms_pf << std::endl;
            myfile.close();
        }
    }
    try {
        
        tbb::task_scheduler_init init(number_of_threads);

        ParallelBranchAndBound_FJSSP * pbb = new (tbb::task::allocate_root()) ParallelBranchAndBound_FJSSP(0, number_of_threads, problem);

        pbb->setTimeLimit(time_limit);

        if (is_grid_enable)
            pbb->enableGrid();

        if (is_sorting_enable)
            pbb->enableSortingNodes();
        
        if (is_priority_enable)
            pbb->enablePriorityQueue(is_priority_enable);

        pbb->setParetoFrontFile(outputFile.c_str());
        pbb->setSummarizeFile(summarizeFile.c_str());
        pbb->setOutputPath(output_path.c_str());
        
        printf("Spawning root...\n");
        tbb::task::spawn_root_and_wait(*pbb);

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
    } else {/** MPI enable: Distributed memory. **/
        
        try {
            tbb::task_scheduler_init init(stoi(argv[arg_num_threads]));
            MasterWorkerPBB_FJSSP *  mw_pbb = new (tbb::task::allocate_root()) MasterWorkerPBB_FJSSP (size_world, stoi(argv[arg_num_threads]), argv[arg_input_file], argv[arg_ouput_path]);
            tbb::task::spawn_root_and_wait(*mw_pbb);
        } catch (tbb::tbb_exception& e) {
            std::cerr << "Intercepted exception:\n" << e.name();
            std::cerr << "Reason is:\n" << e.what();
        }
        printf("[Node-%02d] Done.\n", rank);
    }
    MPI_Finalize();
}
