//
//  BranchAndBound.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "BranchAndBound.hpp"


BranchAndBound::BranchAndBound(){
    
}

BranchAndBound::BranchAndBound(Problem * problem){
    this->problem = problem;
    this->currentLevel = 0;
    this->totalLevels = 0;
    this->totalNodes = 0;
    this->branches = 0;
    this->exploredNodes = 0;
    this->leaves = 0;
    this->unexploredNodes = 0;
    this->prunedNodes = 0;
    this->prunedBranches = 0;
    this->callsToBranch = 0;
    this->totalUpdatesInLowerBound = 0;
    this->totalTime = 0;
}

BranchAndBound::~BranchAndBound(){
    
    delete [] this->outputFile;
    delete [] this->summarizeFile;
    delete this->problem;
    
    Solution * pd;
    for(std::vector<Solution *>::iterator it = paretoFront.begin(); it != paretoFront.end(); ++it) {
        pd = * it;
        delete pd;
    }
    
    delete this->currentSolution;
    
    this->paretoFront.clear();
    this->treeOnAStack.clear();
    this->levelOfTree .clear();
}

void BranchAndBound::initialize(){
    
    
    this->treeOnAStack.resize(1000);
    this->levelOfTree.resize(1000);
    this->paretoFront.resize(1000);
    
    this->treeOnAStack.clear();
    this->levelOfTree.clear();
    this->paretoFront.clear();
    
    int numberOfObjectives = this->problem->getNumberOfObjectives();
    int numberOfVariables = this->problem->getNumberOfVariables();
    this->currentSolution = new Solution(numberOfObjectives, numberOfVariables);
    
    this->currentLevel = this->problem->getStartingLevel();
    this->totalLevels = this->problem->getFinalLevel();
    this->branches = 0;
    this->exploredNodes = 0;
    this->unexploredNodes = 0;
    this->prunedNodes = 0;
    this->prunedBranches = 0;
    this->callsToBranch = 0;
    this->totalUpdatesInLowerBound = 0;
    this->totalNodes = this->computeTotalNodes(totalLevels);
    
    
    int variable = 0;
    for (variable = this->problem->getUpperBound(0); variable >= this->problem->getLowerBound(0); variable--) {
        this->treeOnAStack.push_back(variable);
        this->levelOfTree.push_back(this->problem->getStartingLevel());
        this->branches++;
    }
    
}

void BranchAndBound::start(){
    
    /**
     * set maxTime as parameter.
     */
    //int maxTime = 10;
    this->initialize();
    
    double timeUp = 0;
    //double saveTime = 3600;
    // double maxTime = 0;
    int updated = 0;
    
    printf("Starting Branch and Bound...\n");
    printf("Total levels %d...\n", totalLevels);
    
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    
    while(theTreeHasMoreBranches() == 1 && timeUp == 0){
        
        this->explore(this->currentSolution);
        this->problem->evaluatePartial(this->currentSolution, this->currentLevel);

        if (aLeafHasBeenReached() == 0)
            if(improvesTheLowerBound(this->currentSolution) == 1)
                this->branch(this->currentSolution, this->currentLevel);
            else{
                /**
                 * The branch is pruned, no nodes are added to the stack, and the partial evaluation is reduced.
                 **/
                this->prunedBranches++;
                //this->prune(this->currentSolution, this->currentLevel);
            }
        else{
            
            this->problem->evaluatePartial(this->currentSolution, this->totalLevels);
            //this->problem->evaluateLastLevel(this->currentSolution);
            this->leaves++;
            
            updated = this->updateLowerBound(this->currentSolution);
            this->totalUpdatesInLowerBound += updated;
    /*
            if (updated == 1) {
                printf("[%6lu] ", this->paretoFront.size());
                printCurrentSolution(1);
                printf("+\n");
            }
    */
            //int nextLevel = this->levelOfTree.back();
            //this->problem->removeLastLevelEvaluation(this->currentSolution, nextLevel);
        }
        
        /*std::chrono::seconds sec(maxTime);
        if (time_span.count() > sec.count()) {
         //timeUp = 1;
        }
         */
    }
    
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    this->totalTime = time_span.count();

    printf("The pareto front found is: \n");
    this->printParetoFront(1);
    this->saveParetoFront();
    this->saveSummarize();
}

/**
 * The same as stackPop();
 *  Gets an element form the stack and removes the level corresponding to the element.
 **/
int BranchAndBound::explore(Solution * solution){
    this->exploredNodes++;
    
    int element = this->treeOnAStack.back();
    this->currentLevel = this->levelOfTree.back();
    
    this->treeOnAStack.pop_back();
    this->levelOfTree.pop_back();
    
    solution->setVariable(this->currentLevel, element);
    return element;
}

void BranchAndBound::branch(Solution* solution, int currentLevel){
    
    this->callsToBranch++;
    int variable = 0;
    
    /**If permutation**/
    if(problem->getType() == 0){
        int belongs = 0;
        int level = 0;
        int levelStarting = this->problem->getStartingLevel();
        
        for (variable = this->problem->getUpperBound(0); variable >= this->problem->getLowerBound(0); variable--) {
            belongs = 0;
            for (level = levelStarting; level <= currentLevel && belongs == 0; level++)
                if (solution->getVariable(level) == variable) {
                    belongs = 1;
                    level = currentLevel;
                }
            
            if (belongs == 0) {
                this->treeOnAStack.push_back(variable);
                this->levelOfTree.push_back(currentLevel + 1);
                this->branches++;
            }
        }
    }
    /**If combination**/
    if(problem->getType() == 1)
        for (variable = this->problem->getUpperBound(0); variable >= this->problem->getLowerBound(0); variable--) {
            this->levelOfTree.push_back(currentLevel + 1);
            this->treeOnAStack.push_back(variable);
            this->branches++;
        }
}

void BranchAndBound::prune(Solution * solution, int currentLevel){
    this->prunedBranches++;
    int nextExploration = this->levelOfTree.back();
    this->problem->removeLastEvaluation(solution, currentLevel, nextExploration);
}

int BranchAndBound::aLeafHasBeenReached(){
    if (this->currentLevel == totalLevels)
        return 1;
    return 0;
}

/**
 * If the Stack is empty then it does not have more branches to explore.
 */
int BranchAndBound::theTreeHasMoreBranches(){
    if(this->treeOnAStack.empty() == 1)
        return 0;
    return 1;
}

/**
 * Adds the new solution to the Pareto front and removes the dominated solutions.
 */
int BranchAndBound::updateLowerBound(Solution * solution){
    
    int saveMemory = 0;
    
    unsigned int * status = new unsigned int[4];
    status[0] = 0;
    status[1] = 0;
    status[2] = 0;
    status[3] = 0;
    
    std::vector<Solution *>::iterator begin = this->paretoFront.begin();
    
    unsigned int nSol = 0;
    int domination = 0;
    
    if(this->paretoFront.size() > 0)
        for(nSol = 0; nSol < this->paretoFront.size(); nSol++){
            
            domination = dominanceTest(solution, this->paretoFront.at(nSol));
            
            if(domination == 1){
                this->paretoFront.erase(begin + nSol);
                status[0]++;
                nSol--;
            }
            else if(domination == 0)
                status[1]++;
            else if(domination == -1){
                status[2]++;
                nSol = (int) this->paretoFront.size();
            }
            else if(domination == 11)
                status[3] = 1;
        }
    
    /**
     * status[3] is to avoid to add solutions with the same objective values in the front, remove it if repeated objective values are requiered.
     */
    //if(status[0] > 0 || status[1] == this->paretoFront.size() || status[2] == 0){
    if((status[3] == 0) && (this->paretoFront.size() == 0 || status[0] > 0 || status[1] == this->paretoFront.size() || status[2] == 0)){
       
        int index = 0;
        Solution * copyOfSolution;
        if (saveMemory == 1)
            copyOfSolution = new Solution(this->problem->getNumberOfObjectives(), 1);
        else
            copyOfSolution = new Solution(this->problem->getNumberOfObjectives(), this->problem->getNumberOfVariables());
        
        for (index = 0; index < this->problem->getNumberOfObjectives(); index++)
            copyOfSolution->setObjective(index, solution->getObjective(index));
        
        if (saveMemory == 0)
            for (index = 0; index < this->problem->getNumberOfVariables(); index++)
                copyOfSolution->setVariable(index, solution->getVariable(index));
        
        this->paretoFront.push_back(copyOfSolution);
        
        delete[] status;
        return 1;
    }
    
    delete [] status;
    return  0;
}

/**
 * The solution improves the lowe bound if:
 *  1- The solution is no-dominated by the lower bound or the solution dominates one of the solutions in the lower bound then it is said that the solution improves the lower bound.
 *  2- The solution dominates one solution in the front.
 *  3- The solution is non-dominated by all the solutions in the front.
 *  4- Any solution in the front dominates dominates the solution.
 *  5- It is not repeated.
 *  6- It is non-dominated.
 */
int BranchAndBound::improvesTheLowerBound(Solution * solution){
    
    unsigned long paretoFrontSize = this->paretoFront.size();
    int domination = 0;
    unsigned long index = 0;
    int improves = 1;
    if (paretoFrontSize > 0) {
        
        //#pragma omp parallel for if (paretoFrontSize >= 50) shared(solution) private(domination, index)
        for (index = 0; index < paretoFrontSize; index++) {
            
            domination = dominanceTest(solution, this->paretoFront.at(index));
            if(domination == -1 || domination == 11 ){
                improves = 0;
                index = paretoFrontSize;
            }
        }
    }

    return improves;
}

/**
 * Retunrs the domination Status with respect to Pareto front found. Status have four values:
 *  [0] -> The total solutions which are dominated by the current solution.
 *  [1] -> The total solutions which are no-dominated.
 *  [2] -> The total solutions which dominated the current solution.
 *  [3] -> 1 if the objective values from solution are in the pareto front.
 **/
unsigned int * BranchAndBound::getDominationStatus(Solution * solution){
    unsigned int * status = new unsigned int[4];
    status[0] = 0;
    status[1] = 0;
    status[2] = 0;
    status[3] = 0;
    
    unsigned long paretoFrontSize = this->paretoFront.size();
    int domination = 0;
    int index = 0;
    if (paretoFrontSize > 0) {
        
        /**
         * Note: With small size fronts B&B takes more time.
         **/
        //#pragma omp parallel for if (paretoFrontSize >= 100) shared(status, solution) private(domination, index)
        for (index = 0; index < paretoFrontSize; index++) {
            
                domination = dominanceTest(solution, this->paretoFront.at(index));
            
                if(domination == 1)
                    status[0]++;
                else if(domination == 0)
                    status[1]++;
                else if(domination == -1)
                    status[2]++;
                else if(domination == 11)
                    status[3] = 1;
            }
    }
    else
        status[0] = 1;
    return status;
}

/**
 *
 * Dominance test computes the domination relationship between two solutions.
 * returns
 *  1: solution A dominates solution B.
 *  0: non-dominated solutions.
 * -1: solution B dominates solution A.
 * 11: solution A and solution B are equals in all objectives.
 *
 **/
int BranchAndBound::dominanceTest(Solution * solutionA, Solution * solutionB){
    
    int objective = 0;
    int solAIsBetterIn = 0;
    int solBIsBetterIn = 0;
    int equals = 1;
    
    /**
     * For more objectives consider
     * if (solAIsBetterIn > 0 and solBIsBetterIn > 0) break the FOR because the solutions are non-dominated.
     **/
    for (objective = 0; objective < this->problem->getNumberOfObjectives(); objective++) {
        double objA = solutionA->getObjective(objective);
        double objB = solutionB->getObjective(objective);
        
        if(objA < objB){
            solAIsBetterIn++;
            equals = 0;
        }
        else if(objB < objA){
            solBIsBetterIn++;
            equals = 0;
        }
    }
    
    if(equals == 1)
        return 11;
    else if (solAIsBetterIn > 0 && solBIsBetterIn == 0)
        return 1;
    else if (solBIsBetterIn > 0 && solAIsBetterIn == 0)
        return -1;
    else
        return 0;
    
    return 190;
}

long BranchAndBound::permut(int n, int i) {
    long result = 1;
    for (int j = n; j > n - i; j--)
        result *= j;
    return result;
}

unsigned long BranchAndBound::computeTotalNodes(int totalVariables) {
    long totalNodes = 0;
    
    if(this->problem->getType() == 0)
        for (int i = 0; i < totalVariables; i++)
            totalNodes += (totalVariables - i) * permut(totalVariables, i);
    
    else if(this->problem->getType() == 1){
        long nodes_per_branch = (this->problem->getUpperBound(0) + 1) - this->problem->getLowerBound(0);
        long deepest_level = this->totalLevels + 1;
        totalNodes = (pow(nodes_per_branch, deepest_level + 1) - 1) / (nodes_per_branch - 1);
    }
    return totalNodes;
}

void BranchAndBound::printCurrentSolution(int withVariables){
    
    int indexVar = 0;
    
    for (indexVar = 0; indexVar < this->problem->getNumberOfObjectives(); indexVar++)
        printf("%26.16f ", this->currentSolution->getObjective(indexVar));
    
    if (withVariables == 1) {
        
        printf(" | ");
        
        for (indexVar = 0; indexVar <= this->currentLevel; indexVar++)
            printf("%3d ", this->currentSolution->getVariable(indexVar));
        
        for (indexVar = currentLevel + 1; indexVar < this->problem->getNumberOfVariables(); indexVar++)
            printf("  - ");
        
        printf("|");
    }
}

/**
 * This function prints the pareto front found.
 * If receives value of 0 or no parameters then the variables of the solution won't be printed.
 * If receives value of 1 the variables of the solutioj will be printed.
 **/
void BranchAndBound::printParetoFront(int withVariables){
    
    int numberOfVariables = this->problem->getNumberOfVariables();
    int numberOfObjectives = this->problem->getNumberOfObjectives();
    int indexVar = 0;
    int counterSolutions = 0;
    
    int printVariables = withVariables;
    
    std::vector<Solution* >::iterator it;
    
    for (it = this->paretoFront.begin(); it != this->paretoFront.end(); it++) {
        printf("%6d ", ++counterSolutions);
        for (indexVar = 0; indexVar < numberOfObjectives; indexVar++)
            printf("%26.16f ", (*it)->getObjective(indexVar));
        
        if(printVariables == 1){
            printf("| ");
            
            for (indexVar = 0; indexVar < numberOfVariables; indexVar++)
                printf("%3d ", (*it)->getVariable(indexVar));
        }
        printf("|\n");
    }
}

int BranchAndBound::setParetoFrontFile(const char * outputFile){
  
    this->outputFile = new char[255];
    std::strcpy(this->outputFile, outputFile);
    return 0;
}

int BranchAndBound::setSummarizeFile(const char * outputFile){
    this->summarizeFile = new char[255];
    std::strcpy(this->summarizeFile, outputFile);
    return 0;
}

int BranchAndBound::saveSummarize(){
    
    printf("---Summarize---\n");
    printf("Pareto front size:   %ld\n", this->paretoFront.size());
    printf("Total nodes:         %ld\n", this->totalNodes);
    printf("Explored nodes:      %ld\n", this->exploredNodes);
    printf("Eliminated nodes:    %ld\n", this->totalNodes - this->exploredNodes);
    printf("Calls to branching:  %ld\n", this->callsToBranch);
    printf("Created branches:    %ld\n", this->branches);
    printf("Pruned branches:     %ld\n", this->prunedBranches);
    printf("Leaves reached:      %ld\n", this->leaves);
    printf("Leaves updated in PF:%ld\n", this->totalUpdatesInLowerBound);
    printf("Total time:          %f\n" , this->totalTime);
    
    std::ofstream myfile(this->summarizeFile);
    if (myfile.is_open()){
        printf("Saving in summarize in file...\n");
        
        myfile << "---Summarize---\n";
        myfile << "Pareto front size:   " << this->paretoFront.size() << "\n";
        myfile << "Total nodes:         " << this->totalNodes << "\n";
        myfile << "Explored nodes:      " << this->exploredNodes << "\n";
        myfile << "Eliminated nodes:    " << this->totalNodes - this->exploredNodes << "\n";
        myfile << "Calls to branching:  " << this->callsToBranch << "\n";
        myfile << "Created branches:    " << this->branches << "\n";
        myfile << "Pruned branches:     " << this->prunedBranches << "\n";
        myfile << "Leaves reached:      " << this->leaves << "\n";
        myfile << "Leaves updated in PF:" << this->totalUpdatesInLowerBound << "\n";
        myfile << "Total time:          " << this->totalTime << "\n";

        myfile << "The pareto front found is: \n";
        
        int numberOfObjectives = this->problem->getNumberOfObjectives();
        int numberOfVariables = this->problem->getNumberOfVariables();

        int nObj = 0;
        int nVar = 0;
        
        int counterSolutions = 0;

        std::vector<Solution* >::iterator it;
        
        for (it = this->paretoFront.begin(); it != this->paretoFront.end(); it++) {
           
            myfile << std::fixed << std::setw(6) << std::setfill(' ') << ++counterSolutions << " ";
 
            for (nObj = 0; nObj < numberOfObjectives - 1; nObj++)
                myfile << std::fixed << std::setw(26) << std::setprecision(16) << std::setfill(' ') << (*it)->getObjective(nObj) << " ";
            
            myfile << " | ";
            
            for (nVar = 0; nVar < numberOfVariables; nVar++)
                myfile << std::fixed << std::setw(4) << std::setfill(' ') << (*it)->getVariable(nVar) << " "; //printf("%3d ", (*it)->getVariable(nVar));
            
            myfile << " |\n";
        }
        
        myfile.close();
    }
    else printf("Unable to open file...\n");
    
    return 0;
}

int BranchAndBound::saveParetoFront(){
    std::ofstream myfile(this->outputFile);
    if (myfile.is_open()){
        printf("Saving in file...\n");
        int numberOfObjectives = this->problem->getNumberOfObjectives();
        int nObj = 0;
        
        std::vector<Solution* >::iterator it;
        
        for (it = this->paretoFront.begin(); it != this->paretoFront.end(); it++) {
            for (nObj = 0; nObj < numberOfObjectives - 1; nObj++)
                myfile << std::fixed << std::setw(26) << std::setprecision(16) << std::setfill(' ') << (*it)->getObjective(nObj) << ", ";
            myfile << std::fixed << std::setw(26) << std::setprecision(16) << std::setfill(' ') << (*it)->getObjective(numberOfObjectives - 1) << "\n";
        }
        myfile.close();
    }
    else printf("Unable to open file...\n");
    return 0;
}
