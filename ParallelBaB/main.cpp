//
//  main.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include <iostream>
#include <cstring>
#include <exception>
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
 * argv[1] = number of threads.
 * argv[2] = problem: TSP, HCSP, VRP, and FJSSP.
 * argv[3] = First input file of instance.
 * argv[4] = Second input file of instance.
 * argv[5] = output folder for Pareto front, the file name is given by the instance name.
 */

int main(int argc, const char * argv[]) {

    const int arg_num_threads = 1;
    //const int arg_problem = 2;
    const int arg_input_file1 = 3;
    const int arg_input_file2 = 4;
    const int arg_output = 5;
    
	ProblemFJSSP  problem (2, 1);

//    if (strcmp(argv[1], "TSP") == 0) {
//		printf("Problem: TSP\n");
//        problem = new ProblemTSP(2, 1);
//	} else if (strcmp(argv[arg_problem], "HCSP") == 0) {
//		printf("Problem: HCSP\n");
//		problem = new ProblemHCSP(2, 1);
//	} else if (strcmp(argv[arg_problem], "VRP") == 0) {
//		printf("Problem: VRP\n");
//		problem = new ProblemVRP(2, 1);
//	} else if (strcmp(argv[arg_problem], "FJSSP") == 0) {
//		printf("Problem: FJSSP\n");
//		problem = new ProblemFJSSP(2, 1);
//	} else {
//		printf("Problem no found.\n");
//		return 0;
//	}

    
	char **files = new char*[2];
	files[0] = new char[255];
	files[1] = new char[255];
	std::strcpy(files[0], argv[arg_input_file1]);
	std::strcpy(files[1], argv[arg_input_file2]);

	/** Get name of the instance. **/
	std::vector<std::string> elemens;
	std::vector<std::string> splited;
	elemens = split(argv[arg_input_file1], '/');

	unsigned long int sizeOfElems = elemens.size();
	splited = split(elemens[sizeOfElems - 1], '.');
	printf("Name: %s\n", splited[0].c_str());

	printf("Files:\n\t%s\n\t%s\n", files[0], files[1]);
	problem.setName(splited[0].c_str());
	problem.loadInstance(files);
	problem.printProblemInfo();
	/** End **/

	/** Preparing output files:
	 * - CSV: contains the Pareto front.
	 * - TXT: contains a log file.
	 **/
	std::string outputFile = argv[arg_output] + splited[0] + ".csv";
	std::string summarizeFile = argv[arg_output] + splited[0] + ".txt";


    try {
        Solution solution (problem.getNumberOfObjectives(), problem.getNumberOfVariables());
        problem.createDefaultSolution(solution);
        
        int number_of_threads = stoi(argv[arg_num_threads]);//tbb::task_scheduler_init::default_num_threads();
		tbb::task_scheduler_init init(number_of_threads);

		ParallelBranchAndBound * pbb = new (tbb::task::allocate_root()) ParallelBranchAndBound(problem);
		pbb->setNumberOfThreads(number_of_threads);
		pbb->setParetoFrontFile(outputFile.c_str());
		pbb->setSummarizeFile(summarizeFile.c_str());

		printf("Spawning root...\n");
		tbb::task::spawn_root_and_wait(*pbb);

	} catch (tbb::tbb_exception& e) {

		std::cerr << "Intercepted exception\n" << e.name();

		std::cerr << "Reason is\n" << e.what();

	}
    
    printf("Done\n");
    
    delete [] files[0];
    delete [] files[1];
    delete [] files;

	return 0;
}
