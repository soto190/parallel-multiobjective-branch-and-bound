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
#include "BranchAndBound.hpp"
#include "myutils.hpp"


/**
 * argv[1] = problem: TSP, HCSP, and VRP.
 * argv[2] = instances files.
 */

int main(int argc, const char * argv[]) {

    std::cout << "Hello!\n";
    
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
    
    
    printf("Files:\n\t%s\n\t%s\n", files[0],  files[1]);
    problem->loadInstance(files);
    problem->printProblemInfo();
    
    BranchAndBound BaB(problem);
    BaB.start();
    
    return 0;
}
