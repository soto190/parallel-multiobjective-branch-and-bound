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
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/task_group.h>
#include "Solution.hpp"
#include "Problem.hpp"
#include "ProblemHCSP.hpp"
#include "ProblemTSP.hpp"
#include "ProblemVRP.hpp"
#include "ProblemVRPTW.hpp"
#include "ProblemFJSSP.hpp"
#include "BranchAndBound.hpp"
#include "ParallelBranchAndBound.hpp"
#include "myutils.hpp"
#include "ConcurrentGridContainer.hpp"
#include "MasterWorkerPBB.hpp"
#include "NSGA_II.hpp"
#include "MOSA.hpp"

using namespace std;

void print_usage() {
    printf("Usage:\n\tbranchandbound: -t num -l time_sec -g [enable|disable] -q [enable|disable] -s [enable|disable] -p problem_type [FJSSP|TSP|HCSP|VRP] -i instance_file -o output_file\n");
}

void generateGantt(int argc, char* argv[]){
    int index;
    int iarg = 0;

    double time_limit = 0;
    int number_of_threads = 1;
    string problem_type = "not especified";
    string instance_path = "not especified";
    string output_path = "not especified";
    bool is_grid_enable = false;
    bool is_priority_enable = false;
    bool is_sorting_enable = false;

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
                if (optarg) {
                    string tmp = optarg;
                    if (tmp == "enable")
                        is_priority_enable = true;
                }
                std::cout << (is_priority_enable ? "enable" : "disable") << std::endl;
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
    ProblemFJSSP problem (3, 4);
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

    const int t_sol = 1, n_var = 4;
    FJSSPdata data(problem.getNumberOfJobs(), problem.getNumberOfOperations(), problem.getNumberOfMachines());
    Solution solution(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    int values[t_sol][n_var] = {
        {0, 2, 3, 0}
       // {3, 3, 7, 7, 7, 5, 0, 0, 2},
//        {0, 7, 7, 3, 0, 3, 2, 8, 5},
//        {0, 6, 8, 3, 0, 3, 5, 1, 7},
//        {6, 7, 7, 3, 3, 5, 0, 0, 1}
    };

    for (unsigned int n_sol = 0; n_sol < t_sol; ++n_sol) {
        data.reset();
        for (unsigned int n_op = 0; n_op < problem.getNumberOfOperations(); ++n_op) {
            solution.setVariable(n_op, values[n_sol][n_op]);
            problem.evaluateDynamic(solution, data, n_op);
        }
        problem.printSolutionInfo(solution);
    }
}

void testVRPSolution(ProblemVRPTW& problem) {
    //ProblemVRPTW problem(3, 0);

    Solution test_solution (3, problem.getNumberOfVariables());
    VRPTWdata test_data(problem.getNumberOfCustomers(), problem.getMaxNumberOfVehicles(), problem.getMaxVehicleCapacity());
    test_data.reset();

    unsigned int values[20] = /*{
        20,  3, 25,  8, 15, 11,  9,  6,  4, 22,
         1, 26,  5, 24,  7, 10, 16, 14, 12, 27,
        28, 29, 13, 17, 18, 23,  2, 21, 30, 31};*/

    {   9,    6,    4,    2,    1,   11,    5,    3,    7,    8,   10,    0,    0,    0,    0,    0,    0,    0,    0,    0,  };
    for (unsigned int variable = 0; variable < 11; ++variable) {
        test_solution.push_back(values[variable]);
        problem.evaluateDynamic(test_solution, test_data, variable);
        cout << "Dynamic: " << test_solution << ' ' << test_solution.isFeasible();
    }

    /** Dynamic evaluation. **/
    /*for (unsigned int var = 0; var < problem.getNumberOfVariables(); ++var) {
        problem.evaluateDynamic(test_solution, test_data, var);
    }*/

    /** Dynamic deallocation of evaluation. **/
//    printf("Removing variables. %d\n", test_data.getPosition());
//    for (unsigned int n_var = test_data.getPosition() + 1; n_var > 0; --n_var) {
//        printf("idx: %d var: %d\n", n_var - 1, test_solution.getVariable(n_var - 1));
//        problem.evaluateRemoveDynamic(test_solution, test_data, n_var - 1);
//        test_data.print();
//    }

    problem.evaluate(test_solution);
    cout << "Evaluate: "  << test_solution;
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
    bool is_priority_enable = false;
    bool is_sorting_enable = false;

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
                if (optarg) {
                    string tmp = optarg;
                    if (tmp == "enable")
                        is_priority_enable = true;
                }
                std::cout << (is_priority_enable ? "enable" : "disable") << std::endl;
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

    ProblemVRPTW problem (2, 1);

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
    /** End **/

//    testVRPSolution(problem);

    /** Preparing output files:
     * - CSV: contains the Pareto front.
     * - TXT: contains a log file.
     **/
    std::string outputFile = output_path + splited[0] + ".csv";
    std::string summarizeFile = output_path + splited[0] + ".txt";

    try {

        tbb::task_scheduler_init init(number_of_threads);

        ParallelBranchAndBound * pbb = new (tbb::task::allocate_root()) ParallelBranchAndBound(0, number_of_threads, problem);

        pbb->setTimeLimit(time_limit);

        if (is_grid_enable)
            pbb->enableGrid();

        if (is_sorting_enable)
            pbb->enableSortingNodes();
        
        if (is_priority_enable)
            pbb->enablePriorityQueue();

        pbb->setParetoFrontFile(outputFile.c_str());
        pbb->setSummarizeFile(summarizeFile.c_str());
        
        printf("Spawning root...\n");
        tbb::task::spawn_root_and_wait(*pbb);

    } catch (tbb::tbb_exception& e) {
        std::cerr << "Intercepted exception:\n" << e.name();
        std::cerr << "Reason is:\n" << e.what();
    }
    
    printf("Done.\n");
}

void mpi_launch_nodes(int argc, char* argv[]) {
    int rank = MPI::COMM_WORLD.Get_rank();
    int size_world = MPI::COMM_WORLD.Get_size();

    int index;
    int iarg = 0;

    double time_limit = 0;
    int number_of_threads = 1;
    string problem_type = "not especified";
    string instance_path = "not especified";
    string output_path = "not especified";
    bool is_grid_enable = false;
    bool is_priority_enable = false;
    bool is_sorting_enable = false;

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
                if (optarg) {
                    string tmp = optarg;
                    if (tmp == "enable")
                        is_priority_enable = true;
                }
                std::cout << (is_priority_enable ? "enable" : "disable") << std::endl;
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

    printf("Deep sharing: %0.2f\n", deep_limit_share);

    char files [2][255], extension[10];
    std::strcpy(files[0], instance_path.c_str());

    /** Get name of the instance. **/
    std::vector<std::string> elemens;
    std::vector<std::string> splited;
    elemens = split(instance_path.c_str(), '/');

    unsigned long int sizeOfElems = elemens.size();
    splited = split(elemens[sizeOfElems - 1], '.');
    std::cout << "Name: " << splited[0].c_str() << std::endl;
    std::cout << "File extension: " << splited[1].c_str() << std::endl;
    std::strcpy(extension, splited[1].c_str());

    std::cout << "Files:" << std::endl << '\t' << files[0] << std::endl;

    /** End **/

    /** Preparing output files:
     * - CSV: contains the Pareto front.
     * - TXT: contains a log file.
     **/
    std::string outputFile = output_path + splited[0] + ".csv";
    std::string summarizeFile = output_path + splited[0] + ".txt";

    try {
        tbb::task_scheduler_init init(number_of_threads);
        MasterWorkerPBB *  mw_pbb = new (tbb::task::allocate_root()) MasterWorkerPBB (
                                                                                      size_world,
                                                                                      number_of_threads,
                                                                                      instance_path.c_str(),
                                                                                      output_path.c_str());

        mw_pbb->setTimeLimit(time_limit);

        if (is_grid_enable)
            mw_pbb->enableGrid();

        if (is_sorting_enable)
            mw_pbb->enableSortingNodes();

        if (is_priority_enable)
            mw_pbb->enablePriorityQueue();

        mw_pbb->setParetoFrontFile(outputFile.c_str());
        mw_pbb->setSummarizeFile(summarizeFile.c_str());

        tbb::task::spawn_root_and_wait(*mw_pbb);
    } catch (tbb::tbb_exception& e) {
        std::cerr << "Intercepted exception:\n" << e.name();
        std::cerr << "Reason is:\n" << e.what();
    }
    printf("[Node-%02d] Done.\n", rank);
}
/**
 * argv[1] = number of threads.
 * argv[2] = problem: TSP, HCSP, VRP, and FJSSP.
 * argv[3] = First input file of instance.
 * argv[4] = Second input file of instance.
 * argv[5] = output folder for Pareto front, the file name is given by the instance name.
 */
int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);

    if (MPI::COMM_WORLD.Get_rank() == 0)
        printf("[Master] Number of nodes: %3d\n", MPI::COMM_WORLD.Get_size());

    if (MPI::COMM_WORLD.Get_size() == 1) { /** MPI disable or one node request: shared memory version. **/
        one_node(argc, argv);
    } else {/** MPI enable: Distributed memory. **/
        mpi_launch_nodes(argc, argv);
    }

    MPI_Finalize();
}
