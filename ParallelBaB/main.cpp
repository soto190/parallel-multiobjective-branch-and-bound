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
#include "myutils.hpp"
#include "GridContainer.hpp"


/**
 * argv[1] = problem: TSP, HCSP, VRP, and FJSSP.
 * argv[2] = First input file of instance.
 * argv[3] = Second input file of instance.
 * argv[4] = output folder for Pareto front, the file name is given by the instance name.
 */

int main(int argc, const char * argv[]) {
    
    Problem * problem;
    
    if(strcmp(argv[1], "TSP") == 0){
        printf("Problem: TSP\n");
        problem = new ProblemTSP(2, 1);
    }else if(strcmp(argv[1], "HCSP") == 0){
        printf("Problem: HCSP\n");
        problem = new ProblemHCSP(2, 1);
    }else if(strcmp(argv[1], "VRP") == 0){
        printf("Problem: VRP\n");
        problem = new ProblemVRP(2, 1);
    }else if(strcmp(argv[1], "FJSSP") == 0){
        printf("Problem: FJSSP\n");
        problem =  new ProblemFJSSP(2, 1);
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
    
    
    /** Creating the B&B. **/
    BranchAndBound BaB(problem);
    BaB.setParetoFrontFile(outputFile.c_str());
    BaB.setSummarizeFile(summarizeFile.c_str());
    
    /** Testing the Intervals. **/
 
    int * last_branch = new int[8];
    int * branch = new int[8];

    
    int starting_level = 0;

    int level_to_split = 0;
    
    BaB.computeLastBranch(starting_level, last_branch);
    BaB.splitInterval(level_to_split, last_branch);
    BaB.splitInterval(level_to_split + 1, last_branch);
    BaB.splitInterval(level_to_split + 2, last_branch);

    BaB.solve(starting_level, last_branch);
    
    
    delete [] last_branch;
    delete [] branch;
    
    return 0;
}
