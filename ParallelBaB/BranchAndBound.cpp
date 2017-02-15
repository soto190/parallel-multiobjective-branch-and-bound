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

BranchAndBound::BranchAndBound(int rank, std::shared_ptr<Problem> problem){
    
    this->t1 = std::chrono::high_resolution_clock::now();
    this->t2 = std::chrono::high_resolution_clock::now();
    
    this->rank = rank;
    this->problem = problem;
    
    this->paretoContainer = std::make_shared<HandlerContainer>();
    
    this->ivm_tree = new IVMTree(this->problem->totalVariables, this->problem->getUpperBound(0) + 1);
    this->localPool = std::make_shared<vector<Interval>>();
    
    this->localPool->reserve(100);
    this->globalPool = std::make_shared<vector<Interval>>();

    
    this->currentLevel = 0;
    this->totalLevels = 0;
    this->totalNodes = 0;
    this->branches = 0;
    this->exploredNodes = 0;
    this->leaves = 0;
    this->unexploredNodes = 0;
    this->prunedNodes = 0;
    this->callsToPrune = 0;
    this->callsToBranch = 0;
    this->totalUpdatesInLowerBound = 0;
    this->totalTime = 0;
    
}

BranchAndBound::BranchAndBound(int rank, std::shared_ptr<Problem> problem, const Interval & branch){
    
    this->t1 = std::chrono::high_resolution_clock::now();
    this->t2 = std::chrono::high_resolution_clock::now();
    
    this->rank = rank;
    this->problem = problem;
    
    
    this->ivm_tree = new IVMTree(this->problem->totalVariables, this->problem->getUpperBound(0) + 1);
    this->localPool = make_shared<vector<Interval>>();
    this->localPool->reserve(100);
    this->globalPool = std::make_shared<vector<Interval>>();

    this->currentLevel = 0;
    this->totalLevels = 0;
    this->totalNodes = 0;
    this->branches = 0;
    this->exploredNodes = 0;
    this->leaves = 0;
    this->unexploredNodes = 0;
    this->prunedNodes = 0;
    this->callsToPrune = 0;
    this->callsToBranch = 0;
    this->totalUpdatesInLowerBound = 0;
    this->totalTime = 0;
    
    this->starting_interval = new Interval(this->problem->getNumberOfVariables());
    
    int val = 0;
    for (val = 0; val < this->problem->getNumberOfVariables(); val++) {
        this->starting_interval->interval[val] = branch.interval[val];
    }
    
    this->starting_interval->build_up_to = branch.build_up_to;
    
    int numberOfObjectives = this->problem->getNumberOfObjectives();
    int numberOfVariables = this->problem->getNumberOfVariables();
    
    this->currentSolution = new Solution(numberOfObjectives, numberOfVariables);
    this->bestObjectivesFound = new Solution(numberOfObjectives, numberOfVariables);
    this->problem->createDefaultSolution(this->currentSolution);
    
    this->bestObjectivesFound = new Solution(numberOfObjectives, numberOfVariables);
    
    Solution * bestInObj1 = this->problem->getSolutionWithLowerBoundInObj(1);
    //Solution * bestInObj2 = this->problem->getSolutionWithLowerBoundInObj(2);
    
    int nObj = 0;
    for (nObj = 0; nObj < numberOfObjectives; nObj++)
        this->bestObjectivesFound->setObjective(nObj, this->currentSolution->getObjective(nObj));
    
    this->bestObjectivesFound->setObjective(1, bestInObj1->getObjective(1));
    
    double obj1 = this->currentSolution->getObjective(0);
    double obj2 = this->currentSolution->getObjective(1);
    
    this->paretoContainer = std::make_shared<HandlerContainer> (100, 100, obj1, obj2);
        
}

BranchAndBound::~BranchAndBound(){
    
    delete [] this->outputFile;
    delete [] this->summarizeFile;
    //delete this->problem;
    
    Solution * pd;
    for(std::vector<Solution *>::iterator it = paretoFront.begin(); it != paretoFront.end(); ++it) {
        pd = * it;
        delete pd;
    }
    
    delete this->currentSolution;
    delete this->bestObjectivesFound;
    
    this->paretoFront.clear();
    this->localPool->clear();
    
    //delete this->ivm_tree;
    //delete this->starting_interval;
}

void BranchAndBound::initialize(int starts_tree){
    
    this->start = std::clock();

    int numberOfObjectives = this->problem->getNumberOfObjectives();
    int numberOfVariables = this->problem->getNumberOfVariables();
    
    if (starts_tree == -1)
        this->currentLevel = 0;
    else
        this->currentLevel = starts_tree;
    this->totalLevels = this->problem->getFinalLevel();
    this->branches = 0;
    this->exploredNodes = 0;
    this->unexploredNodes = 0;
    this->prunedNodes = 0;
    this->callsToPrune = 0;
    this->callsToBranch = 0;
    this->totalUpdatesInLowerBound = 0;
    this->totalNodes = this->computeTotalNodes(totalLevels);

    
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
    
    this->updateParetoGrid(bestInObj1);
    this->updateParetoGrid(bestInObj2);
    this->updateParetoGrid(this->currentSolution);

    /*
    double obj1 = this->currentSolution->getObjective(0);
    double obj2 = this->currentSolution->getObjective(1);

    // this->paretoContainer = new HandlerContainer(100, 100, obj1, obj2);
    printf("Ranges: %f %f \n Initial solution:\n", obj1, obj2);
    this->problem->printSolution(this->currentSolution);
    printf("\n");
    
    printf("Best max workload:\n");
    this->problem->printSolution(bestInObj1);
    printf("\nBest total workload:\n");
    this->problem->printSolution(bestInObj2);
    printf("\n");
    */
    

    //delete bestInObj1;
    //delete bestInObj2;
}

/**
 * This modifies the IVMTree.
 *
 * - branch.build_up_to.
 *
 **/
int BranchAndBound::initializeExplorationInterval(const Interval & branch, IVMTree *tree){
    
    /** This is only for the FJSSP. **/
    int job = 0;
    int isIn = 0;
    int varInPos = 0;
    int * numberOfRepetitionsAllowed = problem->getElemensToRepeat();
    int timesRepeated [problem->getTotalElements()];
    int map = 0;
    int jobToCheck = 0;
    int jobAllocated = 0;
    int cl = 0; /** Counter level.**/
    tree->root_node = branch.build_up_to; /** root node of this tree**/
    tree->starting_level = branch.build_up_to + 1; /** Level with the first branches of the tree. **/
    tree->active_level = branch.build_up_to + 1;
    
    /** Copy the built part. **/
    for (cl = 0; cl <= branch.build_up_to; cl++) {
        
        for (varInPos = 0; varInPos < tree->getNumberOfCols(); varInPos++) {
            tree->ivm[cl][varInPos] = -1;
        }
        tree->start_exploration[cl] = branch.interval[cl];
        tree->end_exploration[cl] =  branch.interval[cl];
        tree->max_nodes_in_level[cl] = 1;
        tree->active_node[cl] = branch.interval[cl];
        tree->ivm[cl][branch.interval[cl]] = branch.interval[cl];
        

        /** TODO: Check this part. The interval is equivalent to the solution?. **/
        this->currentSolution->setVariable(cl, this->ivm_tree->start_exploration[cl]);
    }

    /** For each level search the job to allocate.**/
    for (cl = branch.build_up_to + 1; cl <= this->totalLevels; cl++){
        tree->start_exploration[cl] = 0;
        tree->max_nodes_in_level[cl] = 0;
    
        for (job = problem->getTotalElements() - 1; job >= 0; job--) {
            isIn = 0;
            jobToCheck = job;
            timesRepeated[jobToCheck] = 0;
            
            for (varInPos = 0; varInPos < cl; varInPos++){
                map = tree->end_exploration[varInPos];
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
                tree->end_exploration[cl] = this->problem->getMappingOf(jobToCheck, this->problem->getTimesValueIsRepeated(0) - 1);
                
                /** To finish the loop. **/
                job = -1;
            }
        }
    }
    this->currentSolution->build_up_to = branch.build_up_to;
    this->branch(this->currentSolution, branch.build_up_to);
    this->ivm_tree->active_level--;
    this->ivm_tree->active_node[this->ivm_tree->active_level] = 0;
    tree->active_node[tree->active_level] = tree->start_exploration[tree->active_level];
    this->ivm_tree->hasBranches = 1;

    return 0;
}


tbb::task* BranchAndBound::execute() {
   
    this->solve(*starting_interval);
    return NULL;

}

 
void BranchAndBound::setStartingInterval(Interval *branch){
    this->starting_interval = new Interval(this->problem->getNumberOfVariables());
    
    int val = 0;
    for (val = 0; val < this->problem->getNumberOfVariables(); val++) {
        this->starting_interval->interval[val] = branch->interval[val];
    }
    this->starting_interval->build_up_to = branch->build_up_to;
    
    /**branch the given interval and split it. **/
}

void BranchAndBound::solve(const Interval& branch){

    double timeUp = 0;
    int updated = 0;
    int working = 1;
    
    Interval branchInt = branch;
    this->initialize(branchInt.build_up_to);
    
    while (working > 0) {
        
        
        MutexToUpdateGrid.lock();
        if (this->globalPool->size() > 0) {
            branchInt = this->globalPool->back();
            this->globalPool->pop_back();
            working++;
            
            printf("[BB%d] Picking from global pool\n", this->rank);
//            branchInt.showInterval();
        }
        else{
            working = 0;
            printf("[BB%d] No more intervals in global pool, going to sleep.\n", this->rank);
        }
        MutexToUpdateGrid.unlock();
        
        if(working > 0){
            this->splitInterval(branchInt);
//            printf("[BB%d] Starting Branch and Bound...\n[BB%d] Total levels %d...\n[BB%d] Starting at level %d\n[BB%d] Received branch:", this->rank, this->rank, totalLevels, this->rank, branchInt.build_up_to, this->rank);
//            branchInt.showInterval();
        }
        
        Interval activeBranch (this->problem->getNumberOfVariables());
        
        while (localPool->size() > 0) {
            
            activeBranch = localPool->back();
            localPool->pop_back();
            
//            printf("[BB%d] Exploring interval: ", this-> rank);
//            activeBranch.showInterval();
            this->initializeExplorationInterval(activeBranch, this->ivm_tree);
            
            while(theTreeHasMoreBranches() == 1 && timeUp == 0){
                
                this->explore(this->currentSolution);
                this->problem->evaluatePartial(this->currentSolution, this->currentLevel);
                
                
                if (aLeafHasBeenReached() == 0){
                    if(improvesTheGrid(this->currentSolution) == 1)
                        this->branch(this->currentSolution, this->currentLevel);
                    else
                        this->prune(this->currentSolution, this->currentLevel);
                }else{
                    
                    this->leaves++;
                    
                    updated = this->updateParetoGrid(this->currentSolution);
                    this->totalUpdatesInLowerBound += updated;
                    
                    if (updated == 1){
                        printf("[BB%d] ", this->rank);
                        printCurrentSolution();
                        printf(" + [%6lu] \n", this->paretoContainer->getSize());
                    }
                }
                this->saveEvery(3600);
            }
        }
        
        this->t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        this->totalTime = time_span.count();
        
        printf("[BB%d] Exploration finished.\n", this->rank);
        
    }
}

double BranchAndBound::getTotalTime(){

    this->t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    this->totalTime = time_span.count();
    return this->totalTime;
}

/**
 *  Gets the next node to explore.
 **/
int BranchAndBound::explore(Solution * solution){
    
    this->exploredNodes++;
    
    /** If the active node is a leaf then we need to go up. **/
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
    
    int branched = 0;
    //vector<double [3]> elements_sorted;
    
    switch (this->problem->getType()) {
            
        case ProblemType::permutation:
            levelStarting = this->problem->getStartingLevel();
            
            for (variable = this->problem->getUpperBound(0); variable >= this->problem->getLowerBound(0); variable--) {
                isInPermut = 0;
                for (level = levelStarting; level <= currentLevel; level++)
                    if (solution->getVariable(level) == variable) {
                        isInPermut = 1;
                        level = currentLevel + 1;
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
                
                for (varInPos = 0; varInPos <= currentLevel; varInPos ++){
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
                    /** TODO: sort the branches. **/
                    for(machine = 0; machine < this->problem->getTimesValueIsRepeated(0); machine++){
                        toAdd = this->problem->getMappingOf(jobToCheck, machine);
                        
                        solution->setVariable(currentLevel + 1, toAdd);
                        this->problem->evaluatePartial(solution, currentLevel + 1);
                       
                        if (this->improvesTheGrid(solution)) {
                          /*  double element[3];
                            element[0] = solution->getObjective(0);
                            element[1] = solution->getObjective(1);
                            element[2] = toAdd;
                            
                            elements_sorted.push_back(element);
                            */
                            this->ivm_tree->setNode(currentLevel + 1, toAdd);
                            this->branches++;
                            branched++;
                        }
                        else{
                            this->prunedNodes++;
                        }
                    }
                }
            }
            
            
            /** Search for the next active node. **/
            if(branched > 0){
              
                this->ivm_tree->moveToNextLevel();
                this->ivm_tree->active_node[this->ivm_tree->active_level] = 0;
            }
            else
                this->ivm_tree->pruneActiveNode();

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
    
    this->callsToPrune++;
    this->ivm_tree->pruneActiveNode();
    
}

int BranchAndBound::aLeafHasBeenReached(){
    if (this->currentLevel == this->totalLevels)
            return 1;
    return 0;
}

/**
 * Check if the ivm has pending branches to be explored.
 */
int BranchAndBound::theTreeHasMoreBranches(){
    return this->ivm_tree->hasPendingBranches();
}

int BranchAndBound::updateParetoGrid(Solution * solution){

    int nObj = 0;
    for (nObj = 0; nObj < this->problem->totalObjectives; nObj++)
        if (solution->getObjective(nObj) < this->bestObjectivesFound->getObjective(nObj))
            this->bestObjectivesFound->objective[nObj] = solution->getObjective(nObj);

    
    MutexToUpdateGrid.lock();
    int bucketCoord [2];
    this->paretoContainer->checkCoordinate(solution, bucketCoord);
    int updated = this->paretoContainer->set(solution, bucketCoord[0], bucketCoord[1]);
    MutexToUpdateGrid.unlock();
    
    return updated;
}


/**
 * Adds the new solution to the Pareto front and removes the dominated solutions.
 */
int BranchAndBound::updateLowerBound(Solution * solution){
    //MutexToUpdateGrid.lock();
    int updated = updateFront(solution, this->paretoFront);
    //MutexToUpdateGrid.unlock();
    return updated;
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
    this->paretoContainer->checkCoordinate(solution, bucketCoordinate);
    BucketState stateOfBucket = this->paretoContainer->getStateOf(bucketCoordinate[0], bucketCoordinate[1]);
    int improveIt = 0;
    
    switch (stateOfBucket) {
            
        case BucketState::dominated:
            //improveIt = 0;
            break;
            
        case BucketState::unexplored:
            improveIt = 1;
            break;
            
        case BucketState::nondominated:
//            std::vector<Solution *> bucketFront = this->paretoContainer->get(bucketCoordinate[0], bucketCoordinate[1]);
            improveIt = this->improvesTheBucket(solution, this->paretoContainer->get(bucketCoordinate[0], bucketCoordinate[1]));
            break;
    }
    
    return improveIt;
}

int BranchAndBound::improvesTheBucket(Solution *solution, std::vector<Solution *>& bucketFront){
    
    unsigned long paretoFrontSize = bucketFront.size();
    DominanceRelation domination;
    unsigned long index = 0;
    int improves = 1;
    if (paretoFrontSize > 0)
        for (index = 0; index < paretoFrontSize; index++) {
            domination = dominanceOperator(solution, bucketFront.at(index));
            if(domination == DominanceRelation::Dominated || domination == DominanceRelation::Equals){
                improves = 0;
                index = paretoFrontSize + 1;
            }
        }
    
    return improves;
}

/**
 * TODO: method not used, delete later.
 *
 * The branch must contains all the nodes before the indicated level.
 *
 *
 *
 **/
void BranchAndBound::computeLastBranch(Interval *  branch){
    /** This is only for the FJSSP. **/
    int level = branch->build_up_to;
    int totalLevels = branch->build_up_to + 1;//this->problem->getFinalLevel();
    int job = 0;
    int isIn = 0;
    int varInPos = 0;
    int * numberOfRepetitionsAllowed = problem->getElemensToRepeat();
    int timesRepeated [problem->getTotalElements()];
    int map = 0;
    int jobToCheck = 0;
    int jobAllocated = 0;
    
    if (level == -1){
        branch->interval[0] = this->problem->getUpperBound(0);
        branch->build_up_to = 0;
    }else
    /** For each level search the job to allocate.**/
        for (job = problem->getTotalElements() - 1; job >= 0; job--) {
            isIn = 0;
            jobToCheck = job;
            timesRepeated[jobToCheck] = 0;
            
            for (varInPos = 0; varInPos < totalLevels; varInPos++){
                map = branch->interval[varInPos];
                jobAllocated = this->problem->getMapping(map, 0);
                if (jobToCheck == jobAllocated) {
                    timesRepeated[jobToCheck]++;
                    if(timesRepeated[jobToCheck] == numberOfRepetitionsAllowed[jobToCheck]){
                        isIn = 1;
                        varInPos = totalLevels + 1;
                    }
                }
            }
            
            if (isIn == 0){
                branch->interval[totalLevels] = this->problem->getMappingOf(jobToCheck, this->problem->getTimesValueIsRepeated(0) - 1);
                
                branch->build_up_to++;
                /** To finish the loop. **/
                job = 0;
            }
        }
}

/** Generates an interval for each possible value in the given level of the branch_to_split
 *  Level: 4
 *  Branch: [ 8 8 4 3 ...]
 *  Generates the intervals:
 *  Interval_1: [8 8 4 0]
 *  Interval_2: [8 8 4 1]
 *  Interval_3: [8 8 4 2]
 *  Interval_4: [8 8 4 3]
 *
 *  This adds branches to the pending intervals. 
 *
 *  NOTE: Remember to avoid to split the intervals in the last levels.
 **/
void BranchAndBound::splitInterval(const Interval & branch_to_split){
    
    int index_var = 0;
    int level_to_split = branch_to_split.build_up_to + 1;
    int number_of_variables = this->problem->totalVariables;
    
    int isIn = 0;
    int varInPos = 0;
    int * numberOfRepetitionsAllowed = problem->getElemensToRepeat();
    int timesRepeated [problem->getTotalElements()];
    int map = 0;
    int jobToCheck = 0;
    int jobAllocated = 0;
    
    Solution sol_test (2, branch_to_split.max_size);
    
    for (index_var = 0; index_var <= branch_to_split.build_up_to; index_var++) {
        sol_test.setVariable(index_var, branch_to_split.interval[index_var]);
    }
    
    for (jobToCheck = problem->getTotalElements() - 1; jobToCheck >= 0; jobToCheck--) {
        isIn = 0;
        timesRepeated[jobToCheck] = 0;
        
        for (varInPos = 0; varInPos < level_to_split; varInPos++){
            map = branch_to_split.interval[varInPos];
            jobAllocated = this->problem->getMapping(map, 0);
            if (jobToCheck == jobAllocated) {
                timesRepeated[jobToCheck]++;
                if(timesRepeated[jobToCheck] == numberOfRepetitionsAllowed[jobToCheck]){
                    isIn = 1;
                    varInPos = level_to_split + 1;
                }
            }
        }
        
        if (isIn == 0){
            int toAdd = 0;
            int machine = 0;
            for(machine = this->problem->getTimesValueIsRepeated(0) - 1; machine >= 0 ; machine--){
                
                toAdd = this->problem->getMappingOf(jobToCheck, machine);
                
                /** Creates a new Interval. **/
                Interval branch (number_of_variables);
                
                /** Copy the first variables of the branch_to_split-> **/
                for (index_var = 0; index_var < level_to_split; index_var++)
                    branch.interval[index_var] = branch_to_split.interval[index_var];
                
                /** From level + 1 to last position are initialized with -1. **/
                for (index_var = level_to_split + 1; index_var < number_of_variables; index_var++)
                    branch.interval[index_var] = -1;
                
                /** Gets the branch to add. */
                branch.interval[level_to_split] = toAdd;
                branch.build_up_to = level_to_split;
                
                sol_test.setVariable(level_to_split, toAdd);
                this->problem->evaluatePartial(&sol_test, level_to_split);
                
                if(this->improvesTheGrid(&sol_test) == 1){
                    /**Add it to Intervals. **/
                    this->localPool->push_back(branch);
                    this->branches++;
                }
                else
                    this->prunedNodes++;
                
            }
        }
    }

/*
    int index_branch = 0;
    printf("Received branch:  ");
    branch_to_split.showInterval();

    for (index_branch = 0; index_branch < this->intervals.size(); index_branch++){
        printf("Generated branch: ");
        this->intervals.at(index_branch).showInterval();
    }
 */
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
            /** TODO: **/
            nodes_per_branch = (this->problem->getUpperBound(0) + 1) - this->problem->getLowerBound(0);
            deepest_level = this->totalLevels + 1;
            totalNodes = (pow(nodes_per_branch, deepest_level + 1) - 1) / (nodes_per_branch - 1);
            
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
        printf("[%6d] ", ++counterSolutions);
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

void BranchAndBound::setParetoContainer(std::shared_ptr<HandlerContainer> paretoContainer){
    this->paretoContainer = paretoContainer;
}

std::shared_ptr<HandlerContainer> BranchAndBound::getParetoContainer(){
    return this->paretoContainer;
}

void BranchAndBound::setGlobalPool(std::shared_ptr<std::vector<Interval> > globalPool){
    this->globalPool = globalPool;
}

int BranchAndBound::saveSummarize(){
    
    printf("---Summarize---\n");
    printf("Pareto front size:   %ld\n", this->paretoFront.size());
    printf("Total nodes:         %ld\n", this->totalNodes);
    printf("Explored nodes:      %ld\n", this->exploredNodes);
    printf("Eliminated nodes:    %ld\n", this->totalNodes - this->exploredNodes);
    printf("Calls to branching:  %ld\n", this->callsToBranch);
    printf("Created branches:    %ld\n", this->branches);
    printf("Calls to prune:      %ld\n", this->callsToPrune);
    printf("Pruned nodes:        %ld\n", this->prunedNodes);
    printf("Leaves reached:      %ld\n", this->leaves);
    printf("Updates in PF:%ld\n", this->totalUpdatesInLowerBound);
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
        myfile << "Calls to prune:      " << this->callsToPrune << "\n";
        myfile << "Leaves reached:      " << this->leaves << "\n";
        myfile << "Updates in PF:       " << this->totalUpdatesInLowerBound << "\n";
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
    this->MutexToUpdateGrid.lock();
    this->paretoFront = this->paretoContainer->getParetoFront();
    this->MutexToUpdateGrid.unlock();
    
    std::ofstream myfile(this->outputFile);
    if (myfile.is_open()){
        printf("[BB%d] Saving in file...\n", this->rank);
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
    else printf("[BB%d] Unable to open file...\n", this->rank);
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
