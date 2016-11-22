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
        problem =  new ProblemHCSP(2, 1);
    }else if(strcmp(argv[1], "VRP") == 0){
        printf("Problem: VRP\n");
        problem =  new ProblemVRP(2, 1);
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
    
    
    /**Experimental section**/
    
    std::vector<std::string> elemens;
    std::vector<std::string> splited;
    elemens = split(argv[2], '/');
    
    unsigned long int sizeOfElems = elemens.size();
    splited = split(elemens[sizeOfElems - 1], '.' );
    printf("Name: %s\n", splited[0].c_str());
    
    /**Ends experimental section***/
    
    printf("Files:\n\t%s\n\t%s\n", files[0],  files[1]);
    problem->setName(splited[0].c_str());
    problem->loadInstance(files);
    problem->printProblemInfo();
    
    /* Eliminar despues
    Solution * solution = new Solution(2, 24);
    
    solution->setVariable(0, 0);
    solution->setVariable(0 + 12, 3);
    
    solution->setVariable(1, 0);
    solution->setVariable(1 + 12, 1);
    
    solution->setVariable(2, 0);
    solution->setVariable(2 + 12, 3);
    
    solution->setVariable(3, 1);
    solution->setVariable(3 + 12, 0);
    
    solution->setVariable(4, 1);
    solution->setVariable(4 + 12, 4);
    
    solution->setVariable(5, 1);
    solution->setVariable(5 + 12, 0);
    
    solution->setVariable(6, 2);
    solution->setVariable(6 + 12, 2);
    
    solution->setVariable(7, 2);
    solution->setVariable(7 + 12, 1);
    
    solution->setVariable(8, 2);
    solution->setVariable(8 + 12, 3);
    
    solution->setVariable(9, 2);
    solution->setVariable(9 + 12, 3);
    
    solution->setVariable(10, 3);
    solution->setVariable(10 + 12, 0);
    
    solution->setVariable(11, 3);
    solution->setVariable(11 + 12, 1);
    
    
    problem->evaluatePartial(solution, 5);
    
    printf("%f %f\n", solution->getObjective(0), solution->getObjective(1));
    */
    
    /** Preparing output files: 
     * - CSV: contains the Pareto front.
     * - TXT: contains a log file.
     **/
    std::string outputFile = argv[4] + splited[0] + ".csv";
    std::string summarizeFile = argv[4] + splited[0] + ".txt";
    
    BranchAndBound BaB(problem);
    BaB.setParetoFrontFile(outputFile.c_str());
    BaB.setSummarizeFile(summarizeFile.c_str());
    BaB.start();
    
    return 0;
}
