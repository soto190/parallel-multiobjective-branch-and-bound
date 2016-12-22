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
    
    this->paretoContainer = new HandlerContainer();

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
    this->levelOfTree.clear();
}

void BranchAndBound::initialize(){
    this->start = std::clock();;
    
    this->treeOnAStack.resize(1000);
    this->levelOfTree.resize(1000);
    this->paretoFront.resize(1000);
    
    this->treeOnAStack.clear();
    this->levelOfTree.clear();
    this->paretoFront.clear();
    
    int numberOfObjectives = this->problem->getNumberOfObjectives();
    int numberOfVariables = this->problem->getNumberOfVariables();
    
    this->currentSolution = new Solution(numberOfObjectives, numberOfVariables);
    this->problem->createDefaultSolution(this->currentSolution);

    Solution * bestInObj1 = this->problem->getSolutionWithLowerBoundInObj(1);
    Solution * bestInObj2 = this->problem->getSolutionWithLowerBoundInObj(2);
    
    this->bestObjectivesFound = new Solution(numberOfObjectives, numberOfVariables);
    
    int nObj = 0;
    for (nObj = 0; nObj < numberOfObjectives; nObj++)
        this->bestObjectivesFound->setObjective(nObj, this->currentSolution->getObjective(nObj));

   
    this->bestObjectivesFound->setObjective(1, bestInObj1->getObjective(1));
    
    this->paretoContainer = new HandlerContainer(100, 100, this->currentSolution->getObjective(0), this->currentSolution->getObjective(1));
    
    printf("Ranges: %f %f \n Initial solution:\n", currentSolution->getObjective(0), currentSolution->getObjective(1));
    this->problem->printSolution(this->currentSolution);
    printf("\n");
    
    printf("Best max workload:\n");
    this->problem->printSolution(bestInObj1);
    printf("\n");
    printf("Best total workload:\n");
    this->problem->printSolution(bestInObj2);
    printf("\n");
    
    updateParetoGrid(bestInObj1);
    updateParetoGrid(bestInObj2);
    updateParetoGrid(this->currentSolution);
    
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
    
    if (this->problem->getType() == ProblemType::permutation_with_repetition_and_combination) {
        int interVar = 0;
        for (variable = this->problem->getUpperBound(0); variable >= this->problem->getLowerBound(0); variable--)
            for (interVar = this->problem->getUpperBound(1); interVar >= this->problem->getLowerBound(1); interVar--) {
                
                this->treeOnAStack.push_back(variable);
                this->levelOfTree.push_back(this->problem->getStartingLevel());
                this->branches++;
                
                this->treeOnAStack.push_back(interVar);
                this->levelOfTree.push_back(this->problem->getStartingLevel());
                this->branches++;
            }
    }
    else
        for (variable = this->problem->getUpperBound(0); variable >= this->problem->getLowerBound(0); variable--) {
            this->treeOnAStack.push_back(variable);
            this->levelOfTree.push_back(this->problem->getStartingLevel());
            this->branches++;
        }

}

void BranchAndBound::solve(){
    
    this->initialize();
    double timeUp = 0;
    int updated = 0;
    
    printf("Starting Branch and Bound...\n");
    printf("Total levels %d...\n", totalLevels);
    
    this->t1 = std::chrono::high_resolution_clock::now();
    this->t2 = std::chrono::high_resolution_clock::now();
    
    while(theTreeHasMoreBranches() == 1 && timeUp == 0){
        
        this->explore(this->currentSolution);
        this->problem->evaluatePartial(this->currentSolution, this->currentLevel);
        
        //printCurrentSolution();
        
        if (aLeafHasBeenReached() == 0)
            if(improvesTheGrid(this->currentSolution) == 1) // if(improvesTheLowerBound(this->currentSolution) == 1)
                this->branch(this->currentSolution, this->currentLevel);
            else
                /**
                 * The branch is pruned, no nodes are added to the stack, and the partial evaluation is reduced.
                 **/
                this->prunedBranches++;
                //this->prune(this->currentSolution, this->currentLevel);
        else{
            
            this->problem->evaluateLastLevel(this->currentSolution);
            this->leaves++;
            
            updated = this->updateParetoGrid(this->currentSolution);
//            updated = this->updateLowerBound(this->currentSolution);
            this->totalUpdatesInLowerBound += updated;
       
            if (updated == 1){
                printCurrentSolution();
                printf(" + [%6lu] \n", this->paretoContainer->getSize());
            }
        }
        // printf("\n");

        this->saveEvery(3600);
    }

    this->paretoFront = paretoContainer->getParetoFront();
    
    this->t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    this->totalTime = time_span.count();

    printf("The pareto front found is: \n");
    this->printParetoFront(1);
    this->saveParetoFront();
    this->saveSummarize();
    
    this->problem->printSolutionInfo(this->paretoFront.back());
}

/**
 * The same as stackPop();
 *  Gets an element form the stack and removes the level corresponding to the element.
 **/
int BranchAndBound::explore(Solution * solution){

    if(this->problem->getType() == ProblemType::permutation_with_repetition_and_combination){
        /** If permutation with combination. **/
        this->exploredNodes++;
        int subLevel = this->levelOfTree.back();
        int element = this->treeOnAStack.back();
        this->treeOnAStack.pop_back();
        this->levelOfTree.pop_back();
        
        solution->setVariable((subLevel * 2) + 1, element);
        
        /****/
        element = this->treeOnAStack.back();
        this->currentLevel = this->levelOfTree.back();
        this->treeOnAStack.pop_back();
        this->levelOfTree.pop_back();
        
        solution->setVariable(this->currentLevel * 2, element);
    }
    else{
        int element = this->treeOnAStack.back();
        this->currentLevel = this->levelOfTree.back();
        this->treeOnAStack.pop_back();
        this->levelOfTree.pop_back();
        
        solution->setVariable(this->currentLevel, element);
    }
    
    return 0;
}

void BranchAndBound::branch(Solution* solution, int currentLevel){
    
    this->callsToBranch++;
    int variable = 0;
    
    /**If permutation**/
    if(problem->getType() == ProblemType::permutation){
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
    if(problem->getType() == ProblemType::combination)
        for (variable = this->problem->getUpperBound(0); variable >= this->problem->getLowerBound(0); variable--) {
            this->levelOfTree.push_back(currentLevel + 1);
            this->treeOnAStack.push_back(variable);
            this->branches++;
        }
    
    /** If permutation and combination **/
    if(problem->getType() == ProblemType::permutation_with_repetition_and_combination){
        
        int belongs = 0;
        int varInPos = 0;
        int intVariable = 0;
        int levelStarting = this->problem->getStartingLevel();
        
        int numberOfElements = problem->getTotalElements();
        int * numberOfRepetitions = problem->getElemensToRepeat();
        int timesRepeated [numberOfElements];
        
        for (variable = numberOfElements - 1; variable >= 0; variable--) {
            belongs = 0;
            timesRepeated[variable] = 0;
            for (varInPos = levelStarting; varInPos <= currentLevel && belongs == 0; varInPos ++)
                if (solution->getVariable(varInPos * 2) == variable) {
                    timesRepeated[variable]++;
                    if(timesRepeated[variable] == numberOfRepetitions[variable]){
                        belongs = 1;
                        varInPos = currentLevel;
                    }
                }
            
            if (belongs == 0)
                for (intVariable = this->problem->getUpperBound(1); intVariable >= this->problem->getLowerBound(1); intVariable--) {
                    this->treeOnAStack.push_back(variable);
                    this->levelOfTree.push_back(currentLevel + 1);
                    
                    this->treeOnAStack.push_back(intVariable);
                    this->levelOfTree.push_back(currentLevel + 1);
                    this->branches++;
                }
        }
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

int BranchAndBound::updateParetoGrid(Solution * solution){
    
    
    int nObj = 0;
    for (nObj = 0; nObj < this->problem->totalObjectives; nObj++)
        if (solution->getObjective(nObj) < this->bestObjectivesFound->getObjective(nObj)) {
            this->bestObjectivesFound->objective[nObj] = solution->getObjective(nObj);
        }

    int bucketCoord [2];
    paretoContainer->checkCoordinate(solution, bucketCoord);
    int updated = this->paretoContainer->set(solution, bucketCoord[0], bucketCoord[1]);
    return updated;
}


/**
 * Adds the new solution to the Pareto front and removes the dominated solutions.
 */
int BranchAndBound::updateLowerBound(Solution * solution){
    return updateFront(solution, this->paretoFront);
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
    DominanceRelation domination;
    unsigned long index = 0;
    int improves = 1;
    if (paretoFrontSize > 0) {
        
        //#pragma omp parallel for if (paretoFrontSize >= 50) shared(solution) private(domination, index)
        for (index = 0; index < paretoFrontSize; index++) {
            
            domination = dominanceOperator(solution, this->paretoFront.at(index));
            if(domination == DominanceRelation::Dominated || domination == DominanceRelation::Equals){
                improves = 0;
                index = paretoFrontSize;
            }
        }
    }

    return improves;
}

int BranchAndBound::improvesTheGrid(Solution * solution){
        int bucketCoordinate[2];
        paretoContainer->checkCoordinate(solution, bucketCoordinate);
        
        int stateOfBucket = this->paretoContainer->getStateOf(bucketCoordinate[0], bucketCoordinate[1]);
        
        int improveIt = 0;
        if(stateOfBucket == BucketState::dominated)
            improveIt = 0;
        else if(stateOfBucket == BucketState::unexplored)
            improveIt = 1;
        else{
            vector<Solution *> bucketFront = this->paretoContainer->get(bucketCoordinate[0], bucketCoordinate[1]);
            improveIt = this->improvesTheBucket(solution, bucketFront);
        }
        return improveIt;

}

int BranchAndBound::improvesTheBucket(Solution *solution, vector<Solution *>& bucketFront){
    
    unsigned long paretoFrontSize = bucketFront.size();
    DominanceRelation domination;
    unsigned long index = 0;
    int improves = 1;
    if (paretoFrontSize > 0)
        for (index = 0; index < paretoFrontSize && improves == 1; index++) {
            domination = dominanceOperator(solution, bucketFront.at(index));
            if(domination == DominanceRelation::Dominated || domination == DominanceRelation::Equals)
                improves = 0;
        }
    
    return improves;
}

long BranchAndBound::permut(int n, int i) {
    long result = 1;
    for (int j = n; j > n - i; j--)
        result *= j;
    return result;
}

int BranchAndBound::getUpperBound(int objective){
    return 0;
}

int BranchAndBound::getLowerBoundInObj(int nObj){
   return this->problem->getLowerBoundInObj(nObj);
}

unsigned long BranchAndBound::computeTotalNodes(int totalVariables) {
    long totalNodes = 0;
    
    if(this->problem->getType() == ProblemType::permutation)
        for (int i = 0; i < totalVariables; i++)
            totalNodes += (totalVariables - i) * permut(totalVariables, i);
    
    else if(this->problem->getType() == ProblemType::combination){
        long nodes_per_branch = (this->problem->getUpperBound(0) + 1) - this->problem->getLowerBound(0);
        long deepest_level = this->totalLevels + 1;
        totalNodes = (pow(nodes_per_branch, deepest_level + 1) - 1) / (nodes_per_branch - 1);
    }else if(this->problem->getType() == ProblemType::permutation_with_repetition_and_combination){
        /** TODO: Implements the correct computation of total nodes. **/
//        for (int i = 0; i < totalVariables; i++)
//            totalNodes += (totalVariables - i) * permut(totalVariables, i) * ((this->problem->getUpperBound(1) + 1) - this->problem->getLowerBound(1));
        
    }
    
    return totalNodes;
}

void BranchAndBound::printCurrentSolution(int withVariables){
    
    this->problem->printPartialSolution(this->currentSolution, this->currentLevel);
/*
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
 */
}

/**
 * This function prints the pareto front found.
 * If receives value of 0 or no parameters then the variables of the solution won't be printed.
 * If receives value of 1 the variables of the solutioj will be printed.
 **/
void BranchAndBound::printParetoFront(int withVariables){
    
    int counterSolutions = 0;
    std::vector<Solution* >::iterator it;
    
    for (it = this->paretoFront.begin(); it != this->paretoFront.end(); it++) {
        printf("[%6d]\n", ++counterSolutions);
        this->problem->printSolution((*it));
/*
        for (indexVar = 0; indexVar < numberOfObjectives; indexVar++)
            printf("%26.16f ", (*it)->getObjective(indexVar));
        
        if(printVariables == 1){
            printf("| ");
            
            for (indexVar = 0; indexVar < numberOfVariables; indexVar++)
                printf("%3d ", (*it)->getVariable(indexVar));
        }
 */
 printf("\n");
        
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
    printf("Grid data:\n");
    printf("\tGrid dimension:    %d x %d\n", this->paretoContainer->getCols(), this->paretoContainer->getRows());
    printf("\tnon-dominated buckets:    %ld\n", this->paretoContainer->activeBuckets);
    printf("\tdominated buckets:  %ld\n", this->paretoContainer->disabledBuckets);
    printf("\tunexplored buckets:%ld\n", this->paretoContainer->unexploredBuckets);
    printf("\tTotal elements in: %ld\n", this->paretoContainer->getSize());
    
    std::ofstream myfile(this->summarizeFile);
    if (myfile.is_open()){
        printf("Saving summarize in file...\n");
        
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
        
        myfile <<"Grid data:\n";
        myfile <<"\tgrid dimension:" << this->paretoContainer->getCols() << " x " << this->paretoContainer->getRows() << "\n";

        myfile <<"\tnon-dominated buckets:\t" << this->paretoContainer->activeBuckets << "\n";
        myfile <<"\tdominated buckets:   " << this->paretoContainer->disabledBuckets << "\n";
        myfile <<"\tunexplored buckets: " << this->paretoContainer->unexploredBuckets << "\n";
        myfile <<"\ttotal elements in:  " << this->paretoContainer->getSize() << "\n";


        myfile << "The pareto front found is: \n";
        
        int numberOfObjectives = this->problem->getNumberOfObjectives();
        int numberOfVariables = this->problem->getNumberOfVariables();

        int nObj = 0;
        int nVar = 0;
        
        int counterSolutions = 0;

        std::vector<Solution* >::iterator it;
        
        for (it = this->paretoFront.begin(); it != this->paretoFront.end(); it++) {
           
            myfile << std::fixed << std::setw(6) << std::setfill(' ') << ++counterSolutions << " ";
 
            for (nObj = 0; nObj < numberOfObjectives; nObj++)
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

void BranchAndBound::saveEvery(double timeInSeconds){
    
    if(((std::clock() - this->start ) / (double) CLOCKS_PER_SEC) > timeInSeconds){
        this->start = std::clock();
        
        this->paretoFront = paretoContainer->getParetoFront();
        
        this->t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        this->totalTime = time_span.count();
        
        printf("The pareto front found is: \n");
        this->printParetoFront(1);
        this->saveParetoFront();
        this->saveSummarize();
        
    }
}
