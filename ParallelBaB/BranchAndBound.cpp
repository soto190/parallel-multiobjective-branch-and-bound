//
//  BranchAndBound.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

/**
 * TODO: Sort the branches considering the objective values.
 * TODO: Create a structure to store the objective values in each level of the tree (in the solution class or in the B&B class?).
 * TODO: Implements methods to share the information.
 * TODO: Decide if the grid is shared or each B&B has their own grid.
 * TODO: If the grid is shared, then only block the cell which is updated, because to verify the improvement only requieres read acces.
 *
 **/
#include "BranchAndBound.hpp"



BranchAndBound::BranchAndBound(){
    
}

BranchAndBound::BranchAndBound(Problem * problem){
    
    
    this->problem = problem;
    
    this->paretoContainer = new HandlerContainer();
    
    this->ivm_tree = new IVMTree(this->problem->totalVariables, this->problem->getUpperBound(0) + 1);
    
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
    delete this->bestObjectivesFound;
    
    this->paretoFront.clear();
    
    delete this->ivm_tree;
}

void BranchAndBound::initialize(int starts_tree, int * branch){
    
    this->start = std::clock();
    this->paretoFront.clear();

    int numberOfObjectives = this->problem->getNumberOfObjectives();
    int numberOfVariables = this->problem->getNumberOfVariables();
    
    this->currentLevel = starts_tree - 1;
    this->totalLevels = this->problem->getFinalLevel();
    this->branches = 0;
    this->exploredNodes = 0;
    this->unexploredNodes = 0;
    this->prunedNodes = 0;
    this->prunedBranches = 0;
    this->callsToBranch = 0;
    this->totalUpdatesInLowerBound = 0;
    this->totalNodes = this->computeTotalNodes(totalLevels);

    
    this->initializeExplorationInterval(starts_tree, branch);

    this->currentSolution = new Solution(numberOfObjectives, numberOfVariables);
    this->bestObjectivesFound = new Solution(numberOfObjectives, numberOfVariables);
    this->problem->createDefaultSolution(this->currentSolution);

    this->bestObjectivesFound = new Solution(numberOfObjectives, numberOfVariables);
    
    Solution * bestInObj1 = this->problem->getSolutionWithLowerBoundInObj(1);
    Solution * bestInObj2 = this->problem->getSolutionWithLowerBoundInObj(2);
    
    int nObj = 0;
    for (nObj = 0; nObj < numberOfObjectives; nObj++)
        this->bestObjectivesFound->setObjective(nObj, this->currentSolution->getObjective(nObj));

    this->bestObjectivesFound->setObjective(1, bestInObj1->getObjective(1));
    
    double obj1 = this->currentSolution->getObjective(0);
    double obj2 = this->currentSolution->getObjective(1);

    this->paretoContainer = new HandlerContainer(100, 100, obj1, obj2);
    
    printf("Ranges: %f %f \n Initial solution:\n", obj1, obj2);
    this->problem->printSolution(this->currentSolution);
    printf("\n");
    
    printf("Best max workload:\n");
    this->problem->printSolution(bestInObj1);
    printf("\nBest total workload:\n");
    this->problem->printSolution(bestInObj2);
    printf("\n");
    
    this->updateParetoGrid(bestInObj1);
    this->updateParetoGrid(bestInObj2);
    this->updateParetoGrid(this->currentSolution);
    

    delete bestInObj1;
    delete bestInObj2;

    int value = 0;
    
    for (value = 0; value < starts_tree; value++)
        this->currentSolution->setVariable(value, this->ivm_tree->start_exploration[value]);
    
    this->problem->evaluatePartial(this->currentSolution, this->currentLevel);
    
    switch(this->problem->getType()){
        case ProblemType::permutation_with_repetition_and_combination:
            if(this->currentLevel == 0)
                for (value = this->problem->getLowerBound(0); value <= this->problem->getUpperBound(0); value++) {
                    this->ivm_tree->setNode(this->currentLevel, value);
                    this->branches++;
                }
            else
                this->branch(this->currentSolution, this->currentLevel);
            break;
            
        case ProblemType::permutation:
            for (value = this->problem->getLowerBound(0); value <= this->problem->getUpperBound(0); value++) {
                this->ivm_tree->setNode(this->currentLevel, value);
                this->branches++;
            }
            break;
            
        case ProblemType::combination:
            for (value = this->problem->getLowerBound(0); value <= this->problem->getUpperBound(0); value++) {
                this->ivm_tree->setNode(this->currentLevel, value);
                this->branches++;
            }
            break;
            
        case ProblemType::XD:
            break;
    }
}

void BranchAndBound::solve(int starting_level, int * branch){
    
    printf("Starting Branch and Bound...\nTotal levels %d...\nStarting at level %d\nReceived branch: [", totalLevels, starting_level);
    
    int index = 0;
    for (index = 0; index <  this->problem->getNumberOfVariables(); index++)
        printf("%3d ", branch[index]);
    
    printf("]\n");

    this->initialize(starting_level, branch);
    double timeUp = 0;
    int updated = 0;
    
    this->t1 = std::chrono::high_resolution_clock::now();
    this->t2 = std::chrono::high_resolution_clock::now();
    
    while(theTreeHasMoreBranches() == 1 && timeUp == 0){

        this->explore(this->currentSolution);
        this->problem->evaluatePartial(this->currentSolution, this->currentLevel);
        
        if (aLeafHasBeenReached() == 0)
            if(improvesTheGrid(this->currentSolution) == 1)
                this->branch(this->currentSolution, this->currentLevel);
            else
                this->prune(this->currentSolution, this->currentLevel);
        else{
            
            this->leaves++;
            
            updated = this->updateParetoGrid(this->currentSolution);
            this->totalUpdatesInLowerBound += updated;
       
            if (updated == 1){
                printCurrentSolution();
                printf(" + [%6lu] \n", this->paretoContainer->getSize());
            }
        }
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
 *  Gets the next node to explore.
 **/
int BranchAndBound::explore(Solution * solution){
    
    this->exploredNodes++;
    
    /** If the active node is a leaf the we need to go up. **/
    if(this->aLeafHasBeenReached())
        this->ivm_tree->pruneActiveNode();
    
    int level = this->ivm_tree->getCurrentLevel();
    int element = this->ivm_tree->getActiveNode();
    
    solution->setVariable(level, element);
    this->currentLevel = level;
    
    return 0;
}

void BranchAndBound::branch(Solution* solution, int currentLevel){
    
    this->callsToBranch++;
    int variable = 0;
    int isInPermut = 0;
    int level = 0;
    int levelStarting= 0;
    
    int varInPos = 0;
    
    int * numberOfRepetitionsAllowed = problem->getElemensToRepeat();
    int timesRepeated [problem->getTotalElements()];
    int map = 0;
    int jobToCheck = 0;
    int jobAllocated = 0;
    int toAdd = 0;
    int machine = 0;
    
    switch (this->problem->getType()) {
            
        case ProblemType::permutation:
            levelStarting = this->problem->getStartingLevel();
            
            for (variable = this->problem->getUpperBound(0); variable >= this->problem->getLowerBound(0); variable--) {
                isInPermut = 0;
                for (level = levelStarting; level <= currentLevel && isInPermut == 0; level++)
                    if (solution->getVariable(level) == variable) {
                        isInPermut = 1;
                        level = currentLevel;
                    }
                
                if (isInPermut == 0) {
                    this->ivm_tree->setNode(currentLevel + 1, variable);
                    this->branches++;
                }
            }

            break;
            
        case ProblemType::permutation_with_repetition_and_combination:
            
            /** TODO: Sort the branches considering the objective value. **/
            for (variable = 0; variable < problem->getTotalElements(); variable++) {
                isInPermut = 0;
                jobToCheck = variable;
                timesRepeated[jobToCheck] = 0;
                
                for (varInPos = this->problem->getStartingLevel(); varInPos <= currentLevel; varInPos ++){
                    map = solution->getVariable(varInPos);
                    jobAllocated = this->problem->getMapping(map, 0);
                    if (jobToCheck == jobAllocated) {
                        timesRepeated[jobToCheck]++;
                        if(timesRepeated[jobToCheck] == numberOfRepetitionsAllowed[jobToCheck]){
                            isInPermut = 1;
                            varInPos = currentLevel + 1;
                        }
                    }
                }
                
                if (isInPermut == 0){
                    for(machine = 0; machine < this->problem->getTimesValueIsRepeated(0); machine++){
                        toAdd = this->problem->getMappingOf(jobToCheck, machine);
                        this->ivm_tree->setNode(currentLevel + 1, toAdd);
                        this->branches++;
                    }
                }
            }
            
            this->ivm_tree->moveToNextLevel();
            this->ivm_tree->active_nodes[this->ivm_tree->active_level] = 0;

            break;
            
        case ProblemType::combination:
            for (variable = this->problem->getUpperBound(0); variable >= this->problem->getLowerBound(0); variable--) {
                this->ivm_tree->setNode(currentLevel + 1, variable);
                this->branches++;
            }
            break;
            
        case ProblemType::XD:
            break;
    }
}

void BranchAndBound::prune(Solution * solution, int currentLevel){
    
    this->prunedBranches++;
    this->ivm_tree->pruneActiveNode();
    
}

int BranchAndBound::aLeafHasBeenReached(){
    if (this->currentLevel == this->totalLevels)
            return 1;
    return 0;
}

/**
 * If the Stack is empty then it does not have more branches to explore.
 */
int BranchAndBound::theTreeHasMoreBranches(){
    return this->ivm_tree->hasPendingBranches();
}

int BranchAndBound::updateParetoGrid(Solution * solution){
    
    /** TODO: Call a mutex in this method. **/
    int nObj = 0;
    for (nObj = 0; nObj < this->problem->totalObjectives; nObj++)
        if (solution->getObjective(nObj) < this->bestObjectivesFound->getObjective(nObj))
            this->bestObjectivesFound->objective[nObj] = solution->getObjective(nObj);

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
    if (paretoFrontSize > 0)
        for (index = 0; index < paretoFrontSize; index++) {
            domination = dominanceOperator(solution, this->paretoFront.at(index));
            if(domination == DominanceRelation::Dominated || domination == DominanceRelation::Equals){
                improves = 0;
                index = paretoFrontSize;
            }
        }
    
    return improves;
}

int BranchAndBound::improvesTheGrid(Solution * solution){
    int bucketCoordinate[2];
    paretoContainer->checkCoordinate(solution, bucketCoordinate);
    
    int stateOfBucket = this->paretoContainer->getStateOf(bucketCoordinate[0], bucketCoordinate[1]);
    
    int improveIt = 0;
    
    switch (stateOfBucket) {
            
        case BucketState::dominated:
            //improveIt = 0;
            break;
            
        case BucketState::unexplored:
            improveIt = 1;
            break;
            
        case BucketState::nondominated:
            vector<Solution *> bucketFront = this->paretoContainer->get(bucketCoordinate[0], bucketCoordinate[1]);
            improveIt = this->improvesTheBucket(solution, bucketFront);
            break;
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

/**
 * level indicates the position to start to build.
 *
 **/
int BranchAndBound::initializeExplorationInterval(int level, int * partialBranch){
    
    /** This is only for the FJSSP. **/
    int job = 0;
    int isIn = 0;
    int varInPos = 0;
    int * numberOfRepetitionsAllowed = problem->getElemensToRepeat();
    int timesRepeated [problem->getTotalElements()];
    int map = 0;
    int jobToCheck = 0;
    int jobAllocated = 0;
    int cl = 0; /** Current level.**/
    
    
    this->ivm_tree->starting_level = level;
    this->ivm_tree->active_level = level - 1;
    
    for (cl = 0; cl < level; cl++) {
        this->ivm_tree->start_exploration[cl] = partialBranch[cl];
        this->ivm_tree->end_exploration[cl] = partialBranch[cl];
        this->ivm_tree->max_nodes_in_level[cl] = 1;
        this->ivm_tree->active_nodes[cl] = partialBranch[cl];
        this->ivm_tree->ivm[cl][partialBranch[cl]] = partialBranch[cl];
    }
    
    for (cl = level; cl <= this->totalLevels; cl++){
        this->ivm_tree->start_exploration[cl] = 0;
        this->ivm_tree->max_nodes_in_level[cl] = 0;
    }
    
    if (level == 0) {
        this->ivm_tree->start_exploration[0] = 0;
        this->ivm_tree->end_exploration[0] = this->problem->getUpperBound(0);
    }

    /** For each level search the job to allocate.**/
    for (cl = level - 1; cl < this->totalLevels; cl++)
        for (job = problem->getTotalElements() - 1; job >= 0; job--) {
            isIn = 0;
            jobToCheck = job;
            timesRepeated[jobToCheck] = 0;
            
            for (varInPos = 0; varInPos <= cl && isIn == 0; varInPos++){
                map = this->ivm_tree->end_exploration[varInPos];
                jobAllocated = this->problem->getMapping(map, 0);
                if (jobToCheck == jobAllocated) {
                    timesRepeated[jobToCheck]++;
                    if(timesRepeated[jobToCheck] == numberOfRepetitionsAllowed[jobToCheck]){
                        isIn = 1;
                        varInPos = cl;
                    }
                }
            }
            
            if (isIn == 0){
                this->ivm_tree->end_exploration[cl + 1] = this->problem->getMappingOf(jobToCheck, this->problem->getTimesValueIsRepeated(0) - 1);
                /** To finish the loop. **/
                job = 0;
            }
        }
    
    return 0;
}

void BranchAndBound::computeLastBranch(int level, int * branch){
    /** This is only for the FJSSP. **/
    int totalLevels = this->problem->getFinalLevel();
    int job = 0;
    int isIn = 0;
    int varInPos = 0;
    int * numberOfRepetitionsAllowed = problem->getElemensToRepeat();
    int timesRepeated [problem->getTotalElements()];
    int map = 0;
    int jobToCheck = 0;
    int jobAllocated = 0;
    int cl = 0; /** Current level.**/
    
    if (level == 0)
        branch[0] = this->problem->getUpperBound(0);
    
    /** For each level search the job to allocate.**/
    for (cl = level - 1; cl < totalLevels; cl++)
        for (job = problem->getTotalElements() - 1; job >= 0; job--) {
            isIn = 0;
            jobToCheck = job;
            timesRepeated[jobToCheck] = 0;
            
            for (varInPos = 0; varInPos <= cl; varInPos++){
                map = branch[varInPos];
                jobAllocated = this->problem->getMapping(map, 0);
                if (jobToCheck == jobAllocated) {
                    timesRepeated[jobToCheck]++;
                    if(timesRepeated[jobToCheck] == numberOfRepetitionsAllowed[jobToCheck]){
                        isIn = 1;
                        varInPos = cl + 1;
                    }
                }
            }
            
            if (isIn == 0){
                branch[cl + 1] = this->problem->getMappingOf(jobToCheck, this->problem->getTimesValueIsRepeated(0) - 1);
                /** To finish the loop. **/
                job = 0;
            }
        }
}

/** Generates an interval for each possible value in the given level of the branch_to_split.
 *  Level: 4
 *  Branch: [ 8 8 4 3 ...]
 *  Generates the intervals:
 *  Interval_1: [8 8 4 0]
 *  Interval_2: [8 8 4 1]
 *  Interval_3: [8 8 4 2]
 *  Interval_4: [8 8 4 3]
 *
 *   Remeber to avoid to split the intervals in the last levels.
 **/
void BranchAndBound::splitInterval(int level, int * branch_to_split){
    
    int index_interval = 0;
    int index_var = 0;
    int number_of_variables = this->problem->totalVariables;
    int sections = branch_to_split[level] + 1;
    
    /** TODO: Instead of a matrix store the intervals in a vector. **/
    int intervals [sections][number_of_variables];

   
    /** From level + 1 to the last position are initialized at 0. **/
    for (index_interval = 0; index_interval < sections; index_interval++)
        for (index_var = level + 1; index_var < number_of_variables; index_var++)
            intervals[index_interval][index_var] = -1;

    /** Copy the first variables of the branch_to_split. **/
    for (index_interval = 0; index_interval < sections; index_interval++)
        for (index_var = 0; index_var < level; index_var++)
            intervals[index_interval][index_var] = branch_to_split[index_var];

    /** The first interval intervals[0] starts at 0. **/
    intervals[0][level] = 0;
    for (index_interval = 1; index_interval < sections; index_interval++)
        intervals[index_interval][level] = intervals[index_interval - 1][level] + 1;
    
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

/**
 * This functions compute the number of nodes.
 *
 */
unsigned long BranchAndBound::computeTotalNodes(int totalVariables) {
    long totalNodes = 0;
    long nodes_per_branch = 0;
    long deepest_level;
    
    switch (this->problem->getType()) {
            
        case ProblemType::permutation:
            for (int i = 0; i < totalVariables; i++)
                totalNodes += (totalVariables - i) * permut(totalVariables, i);
            break;
            
        case ProblemType::combination:
            nodes_per_branch = (this->problem->getUpperBound(0) + 1) - this->problem->getLowerBound(0);
            deepest_level = this->totalLevels + 1;
            totalNodes = (pow(nodes_per_branch, deepest_level + 1) - 1) / (nodes_per_branch - 1);
            break;
            
        case ProblemType::permutation_with_repetition_and_combination:
            
            break;
            
        case ProblemType::XD:
            break;
            
    }
    
    return totalNodes;
}

void BranchAndBound::printCurrentSolution(int withVariables){
    this->problem->printPartialSolution(this->currentSolution, this->currentLevel);
}

/**
 * This function prints the pareto front found.
 **/
void BranchAndBound::printParetoFront(int withVariables){
    
    int counterSolutions = 0;
    std::vector<Solution* >::iterator it;
    
    for (it = this->paretoFront.begin(); it != this->paretoFront.end(); it++) {
        printf("[%6d]\n", ++counterSolutions);
        this->problem->printSolution((*it));
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
