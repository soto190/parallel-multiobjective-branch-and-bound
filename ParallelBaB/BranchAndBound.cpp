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
 * TODO: Decide if the grid is shared or each B&B has their own grid.
 *
 **/
#include "BranchAndBound.hpp"

ReadySubproblems globalPool;  /** intervals are the pending branches/subproblems/partialSolutions to be explored. **/
HandlerContainer paretoContainer;

BranchAndBound::BranchAndBound(const BranchAndBound& toCopy):
    rank(toCopy.getRank()),
    problem(toCopy.getProblem()),
    fjssp_data(toCopy.getFJSSPdata()),
    incumbent_s(toCopy.getIncumbentSolution()),
    ivm_tree(toCopy.getIVMTree()),
    currentLevel(toCopy.getCurrentLevel()),
    totalLevels(toCopy.getNumberOfLevels()),
    totalNodes(toCopy.getNumberOfNodes()),
    branches(toCopy.getNumberOfBranches()),
    exploredNodes(toCopy.getNumberOfExploredNodes()),
    reachedLeaves(toCopy.getNumberOfReachedLeaves()),
    unexploredNodes(toCopy.getNumberOfUnexploredNodes()),
    prunedNodes(toCopy.getNumberOfPrunedNodes()),
    callsToPrune(toCopy.getNumberOfCallsToPrune()),
    callsToBranch(toCopy.getNumberOfCallsToBranch()),
    totalUpdatesInLowerBound(toCopy.getNumberOfUpdatesInLowerBound()){
        
        start = toCopy.start;
        outputFile = new char[255];
        summarizeFile = new char[255];
        
        branches_to_move = problem.getUpperBound(0) * to_share;
        deep_to_share = totalLevels * deep_limit_share;
    
        std::strcpy(outputFile, toCopy.outputFile);
        std::strcpy(summarizeFile, toCopy.summarizeFile);
}

BranchAndBound::BranchAndBound(int rank, const ProblemFJSSP& problemToCopy, const Interval & branch):
rank(rank),
problem(problemToCopy),
fjssp_data(problemToCopy.getNumberOfJobs(),
           problemToCopy.getNumberOfOperations(),
           problemToCopy.getNumberOfMachines()),
interval_to_solve(branch),
currentLevel(0),
totalLevels(problemToCopy.getNumberOfVariables()),
totalNodes(0),
branches(0),
exploredNodes(0),
reachedLeaves(0),
unexploredNodes(0),
prunedNodes(0),
callsToPrune(0),
callsToBranch(0),
totalUpdatesInLowerBound(0),
totalTime(0){
    
    start = std::clock();
    t1 = std::chrono::high_resolution_clock::now();
    t2 = std::chrono::high_resolution_clock::now();
    
    branches_to_move = problem.getUpperBound(0) * to_share;
    deep_to_share = totalLevels * deep_limit_share;
    
    int numberOfObjectives = problem.getNumberOfObjectives();
    int numberOfVariables = problem.getNumberOfVariables();
    
    totalNodes.fetch_and_store(computeTotalNodes(numberOfVariables));
    
    bestObjectivesFound(numberOfObjectives, numberOfVariables);
    incumbent_s(numberOfObjectives, numberOfVariables);
    problem.createDefaultSolution(incumbent_s);
    
    int nObj = 0;
    for (nObj = 0; nObj < numberOfObjectives; ++nObj)
        bestObjectivesFound.setObjective(nObj, incumbent_s.getObjective(nObj));
    
    ivm_tree(problem.getNumberOfVariables(), problem.getUpperBound(0) + 1);
    ivm_tree.setOwner(rank);
}

BranchAndBound& BranchAndBound::operator()(int rank_new, const ProblemFJSSP &problem_to_copy, const Interval &branch){
    t1 = std::chrono::high_resolution_clock::now();
    t2 = std::chrono::high_resolution_clock::now();
    
    start = std::clock();
    rank = rank_new;
    problem = problem_to_copy;
    fjssp_data(problem.getNumberOfJobs(), problem.getNumberOfOperations(), problem.getNumberOfMachines());
    
    currentLevel = 0;
    totalLevels = 0;
    totalNodes = 0;
    branches = 0;
    exploredNodes = 0;
    reachedLeaves = 0;
    unexploredNodes = 0;
    prunedNodes = 0;
    callsToPrune = 0;
    callsToBranch = 0;
    totalUpdatesInLowerBound = 0;
    totalTime = 0;
    
    interval_to_solve = branch; /** Copy the branch. **/
    
    int numberOfObjectives = problem.getNumberOfObjectives();
    int numberOfVariables = problem.getNumberOfVariables();
    
    incumbent_s(numberOfObjectives, numberOfVariables);
    bestObjectivesFound(numberOfObjectives, numberOfVariables);
    problem.createDefaultSolution(incumbent_s);
    
    int nObj = 0;
    for (nObj = 0; nObj < numberOfObjectives; ++nObj)
        bestObjectivesFound.setObjective(nObj, incumbent_s.getObjective(nObj));

    ivm_tree(problem.getNumberOfVariables(), problem.getUpperBound(0) + 1);
    ivm_tree.setOwner(rank);
    
    branches_to_move = problem.getUpperBound(0) * to_share;
    deep_to_share = totalLevels * deep_limit_share;
    
    outputFile = new char[255];
    summarizeFile = new char[255];
    
    return *this;
}

BranchAndBound::~BranchAndBound() {
	delete[] outputFile;
	delete[] summarizeFile;

    paretoFront.clear();
}

void BranchAndBound::initialize(int starts_tree) {

	start = std::clock();

	int numberOfObjectives = problem.getNumberOfObjectives();
	int numberOfVariables = problem.getNumberOfVariables();

	if (starts_tree == -1)
		currentLevel = 0;
	else
		currentLevel = starts_tree;
	totalLevels = problem.getFinalLevel();
	branches = 0;
	exploredNodes = 0;
	unexploredNodes = 0;
	prunedNodes = 0;
	callsToPrune = 0;
	callsToBranch = 0;
	totalUpdatesInLowerBound = 0;
	totalNodes = computeTotalNodes(totalLevels);

	bestObjectivesFound(numberOfObjectives, numberOfVariables);

	int nObj = 0;
	for (nObj = 0; nObj < numberOfObjectives; ++nObj)
		bestObjectivesFound.setObjective(nObj, incumbent_s.getObjective(nObj));

}

int BranchAndBound::intializeIVM_data(Interval& branch_init, IVMTree& tree){
    
    int col = 0;
    int row = 0; /** Counter level.**/
    int build_value = 0;
    int build_up_to = branch_init.getBuildUpTo();
    currentLevel = build_up_to;
    
    tree.setRootRow(build_up_to);/** root row of this tree**/
    tree.setStartingRow(build_up_to + 1); /** Level/row with the first branches of the tree. **/
    tree.setActiveRow(build_up_to);
    
    fjssp_data.setMinTotalWorkload(problem.getSumOfMinPij());
    for (int m = 0; m < problem.getNumberOfMachines(); ++m){
        fjssp_data.setBestWorkloadInMachine(m, problem.getBestWorkload(m));
        fjssp_data.setTempBestWorkloadInMachine(m, problem.getBestWorkload(m));
    }
    
    fjssp_data.reset();

    for (row = 0; row <= build_up_to; ++row) {
        for (col = 0; col < tree.getNumberOfCols(); ++col)
            tree.setIVMValueAt(row, col, -1);
        build_value = branch_init.getValueAt(row);
        tree.setStartExploration(row, build_value);
        tree.setEndExploration(row, build_value);
        tree.setNumberOfNodesAt(row, 1);
        tree.setActiveNodeAt(row, build_value);
        tree.setIVMValueAt(row, build_value, build_value);
        
        /** TODO: Check this part. The interval is equivalent to the solution?. **/
        incumbent_s.setVariable(row, build_value);
        problem.evaluateDynamic(incumbent_s, fjssp_data, row);
    }
    
    for (row = build_up_to + 1; row <= totalLevels; ++row) {
        tree.setStartExploration(row, -1);
        tree.resetNumberOfNodesAt(row);
        incumbent_s.setVariable(row, -1);
    }
    incumbent_s.setBuildUpTo(build_up_to);
    int branches_created = branch(incumbent_s, build_up_to);
    
    /** Send intervals to global_pool. **/
    int branches_to_move_to_global_pool = branches_created * to_share;
    if (rank > 0
        && branches_to_move_to_global_pool > 0
        && branches_created > branches_to_move_to_global_pool
        && branch_init.getBuildUpTo() <= deep_to_share) {
        
        for (int moved = 0; moved < branches_to_move_to_global_pool; ++moved) {
            branch_init.setValueAt(build_up_to + 1, ivm_tree.removeLastNodeAtRow(build_up_to + 1));
            globalPool.push(branch_init);
            branch_init.removeLastValue();
        }
    }

    return 0;
}

tbb::task* BranchAndBound::execute() {

    initialize(interval_to_solve.getBuildUpTo());
    while (!globalPool.empty())
        if(globalPool.try_pop(interval_to_solve))
            //printf("[B&B-%03d] Picking from global pool. Pool size is %lu\n", rank, globalPool.unsafe_size());
            solve(interval_to_solve);
    
    printf("[B&B-%03d] No more intervals in global pool. Going to sleep.\n", rank);
    
    return NULL;
}

void BranchAndBound::solve(Interval& branch_to_solve) {

    double timeUp = 0;
    int updated = 0;

    intializeIVM_data(branch_to_solve, ivm_tree);
    while (theTreeHasMoreBranches() && !timeUp) {
    
        explore(incumbent_s);
        problem.evaluateDynamic(incumbent_s, fjssp_data, currentLevel);
      
        if (!aLeafHasBeenReached() && theTreeHasMoreBranches()){
            if (improvesTheGrid(incumbent_s))
                branch(incumbent_s, currentLevel);
            else
                prune(incumbent_s, currentLevel);
        }else {
            reachedLeaves++;
            updated = updateParetoGrid(incumbent_s);
            totalUpdatesInLowerBound += updated;
            
            /*if (updated) {
                printf("[B&B-%03d] ", rank);
                printCurrentSolution();
                printf(" + [%6lu] \n", paretoContainer.getSize());
            }*/
            
            /** Go back and prepare to remove the evaluations. **/
            ivm_tree.pruneActiveNode();
        }
        
        /** If the branching operator doesnt creates branches or the prune
         function was called then we need to remove the evaluations.
         Also if a leave has been reached. **/
        for (int l = currentLevel; l >= ivm_tree.getActiveRow(); --l)
            problem.evaluateRemoveDynamic(incumbent_s, fjssp_data, l);
        
        
        /** Sharing part of the IVM tree to the global pool. This is done after branching and pruning any pending work of the three. **/
        if(ivm_tree.hasPendingBranches())
            shareWorkAndSendToGlobalPool(branch_to_solve);
        
        
    }
    
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    totalTime = time_span.count();
}

double BranchAndBound::getTotalTime() {
	t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> time_span = std::chrono::duration_cast<
			std::chrono::milliseconds>(t2 - t1);
	totalTime = time_span.count();
	return totalTime;
}

/**
 *  Gets the next node to explore.
 * Modifies the solution.
 **/
int BranchAndBound::explore(Solution & solution) {

	exploredNodes++;
	currentLevel = ivm_tree.getActiveRow();
	solution.setVariable(currentLevel, ivm_tree.getActiveNode());
	return 0;
}

/**
 * Modifies the variable at built_up_to + 1 of the solution.
 *
 *
 */
int BranchAndBound::branch(Solution& solution, int currentLevel) {

	callsToBranch++;
    
    int element = 0;
    int isInPermut = 0;
    int row = 0;
    int levelStarting= 0;
    int toAdd = 0;
    int machine = 0;
    
    int branches_created = 0;
    //SortedVector elements_sorted;
    //Data3 data;

    switch (problem.getType()) {
            
        case ProblemType::permutation:
            levelStarting = problem.getStartingRow();
            
            for (element = problem.getUpperBound(0); element >= problem.getLowerBound(0); --element) {
                isInPermut = 0;
                for (row = levelStarting; row <= currentLevel; ++row)
                    if (solution.getVariable(row) == element) {
                        isInPermut = 1;
                        row = currentLevel + 1;
                    }
                
                if (isInPermut == 0) {
                    ivm_tree.setNode(currentLevel + 1, element);
                    branches++;
                    branches_created++;
                }
            }
            
            break;
            
        case ProblemType::permutation_with_repetition_and_combination:
            
            /** TODO: TEST parallel for?, solution is shared and each thread needs their own copy. **/
            for (element = 0; element < problem.getTotalElements(); ++element)
                if (fjssp_data.getNumberOfOperationsAllocatedInJob(element) < problem.getTimesValueIsRepeated(element))
                    for (machine = 0; machine < problem.getNumberOfMachines(); ++machine) {
                        toAdd = problem.getCodeMap(element, machine);
                        
                        solution.setVariable(currentLevel + 1, toAdd);
                        problem.evaluateDynamic(solution, fjssp_data, currentLevel + 1);
                        
                        //data.setValue(toAdd);
                        //data.setObjective(0, fjssp_data.getMakespan());
                        //data.setObjective(1, fjssp_data.getMaxWorkload());
                        if (improvesTheGrid(solution)) {
                            //elements_sorted.push(data);
                            ivm_tree.setNode(currentLevel + 1, toAdd);
                            branches_created++;
                        } else
                            prunedNodes++;
                        problem.evaluateRemoveDynamic(solution, fjssp_data, currentLevel + 1);
                    }
            branches += branches_created;

            if (branches_created > 0) { /** If a branch was created. **/
                ivm_tree.moveToNextRow();
                ivm_tree.setActiveNodeAt(ivm_tree.getActiveRow(), 0);
                ivm_tree.setHasBranches(1);
            } else { /** If no branches were created then move to the next node. **/
                ivm_tree.pruneActiveNode();
                prunedNodes++;
            }
            break;
            
        case ProblemType::combination:
            for (element = problem.getUpperBound(0); element >= problem.getLowerBound(0); --element) {
                ivm_tree.setNode(currentLevel + 1, element);
                branches++;
                branches_created++;
            }
            break;
            
        case ProblemType::XD:
            break;
    }
    
    return branches_created;
}

void BranchAndBound::prune(Solution & solution, int currentLevel) {
	callsToPrune++;
	ivm_tree.pruneActiveNode();
}

int BranchAndBound::aLeafHasBeenReached() const { return (currentLevel == totalLevels)?1:0;}

/** 
 * This function shares part of the ivm tree. The part corresponding to the root_row + 1 is sended to the global_pool if the next
 * conditions are reached:
 * 1) if the global pool has less than the indicated size.
 * 2) if the root_row + 1 is less than deep_share and it has more than 1 element.
 * 
 * All the remanent of root_row + 1 is moved to the global pool.
 *
 * TODO: Send the next pending nodes to global pool, not only root_row + 1.
 ***/
void BranchAndBound::shareWorkAndSendToGlobalPool(Interval & branch_to_solve){
    /** Testing code. **/
    int next_row = ivm_tree.getRootRow() + 1;
    if (globalPool.unsafe_size() < 120 && next_row < deep_to_share)
        while(ivm_tree.getNumberOfNodesAt(next_row) > 1){
            branch_to_solve.setValueAt(next_row, ivm_tree.removeLastNodeAtRow(next_row));
            if(!branch_to_solve.verify())
                printDebug();
            globalPool.push(branch_to_solve);
            branch_to_solve.removeLastValue();
            if (!branch_to_solve.verify())
                printDebug();
            
        }
    /** End testing code. **/
}

/**
 * Check if the ivm has pending branches to be explored.
 */
int BranchAndBound::theTreeHasMoreBranches() const { return ivm_tree.hasPendingBranches(); }
int BranchAndBound::updateParetoGrid(const Solution & solution) { return paretoContainer.add(solution); }

/**
 * The solution improves the lowe bound if:
 *  1- The solution is no-dominated by the lower bound or the solution dominates one of the solutions in the lower bound then it is said that the solution improves the lower bound.
 *  2- The solution dominates one solution in the front.
 *  3- The solution is non-dominated by all the solutions in the front.
 *  4- Any solution in the front dominates dominates the solution.
 *  5- It is not repeated.
 *  6- It is non-dominated.
 *
 */
int BranchAndBound::improvesTheGrid(const Solution & solution) const { return paretoContainer.improvesTheGrid(solution); }

/**
 *
 * TODO: method not used, delete later.
 *
 * The branch must contains all the nodes before the indicated level.
 *
 **/
void BranchAndBound::computeLastBranch(Interval & branch_to_compute) {
	/** This is only for the FJSSP. **/
	int level = branch_to_compute.getBuildUpTo();
	int totalLevels = branch_to_compute.getBuildUpTo() + 1;
	int job = 0;
	int isIn = 0;
	int varInPos = 0;
	int * numberOfRepetitionsAllowed = problem.getElemensToRepeat();
	int timesRepeated[problem.getTotalElements()];
	int map = 0;
	int jobToCheck = 0;
	int jobAllocated = 0;

	if (level == -1) {
        branch_to_compute.setValueAt(0, problem.getUpperBound(0));
        branch_to_compute.setBuildUpTo(0);
	} else
		/** For each level search the job to allocate.**/
		for (job = problem.getTotalElements() - 1; job >= 0; --job) {
			isIn = 0;
			jobToCheck = job;
			timesRepeated[jobToCheck] = 0;

			for (varInPos = 0; varInPos < totalLevels; ++varInPos) {
                map = branch_to_compute.getValueAt(varInPos);// branch.interval[varInPos];
				jobAllocated = problem.getDecodeMap(map, 0);
				if (jobToCheck == jobAllocated) {
					timesRepeated[jobToCheck]++;
					if (timesRepeated[jobToCheck]
							== numberOfRepetitionsAllowed[jobToCheck]) {
						isIn = 1;
						varInPos = totalLevels + 1;
					}
				}
			}

			if (isIn == 0) {
                branch_to_compute.setValueAt(totalLevels, problem.getCodeMap(jobToCheck, problem.getTimesValueIsRepeated(0) - 1));
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
void BranchAndBound::initGlobalPoolWithInterval(Interval & branch_to_split) {
    
    Solution solution (problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.createDefaultSolution(solution);
    paretoContainer(100, 100, solution.getObjective(0), solution.getObjective(1));
    
    Solution temp(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.getSolutionWithLowerBoundInObj(1, temp);
    temp.print();
    solution.print();
    updateParetoGrid(solution);
    updateParetoGrid(temp);
    
    int row = 0;
    int split_level = branch_to_split.getBuildUpTo() + 1;
    int branches_created = 0;
    int num_elements = problem.getTotalElements();
    int map = 0;
    int element = 0;
    int machine = 0;
    int toAdd = 0;
    //fjssp_data.reset(); /** This function call is not necesary because the structuras starts empty.**/
    
    for (row = 0; row <= branch_to_split.getBuildUpTo(); ++row) {
        map = branch_to_split.getValueAt(row);
        incumbent_s.setVariable(row, map);
        problem.evaluateDynamic(incumbent_s, fjssp_data, row);
    }
    
    for (element = 0; element < num_elements; ++element)
        if (fjssp_data.getNumberOfOperationsAllocatedInJob(element) < problem.getTimesValueIsRepeated(element))
            for (machine = 0; machine < problem.getNumberOfMachines(); ++machine) {
                
                toAdd = problem.getCodeMap(element, machine);
                incumbent_s.setVariable(split_level, toAdd);
                problem.evaluateDynamic(incumbent_s, fjssp_data, split_level);
                
                if (improvesTheGrid(incumbent_s)) {
                    /** Gets the branch to add. */
                    branch_to_split.setValueAt(split_level, toAdd);
                    /**Add it to pending intervals. **/
                    if (rank == 0)
                        globalPool.push(branch_to_split); /** The vector adds a copy of interval. **/
                    branch_to_split.removeLastValue();
                    branches_created++;
                } else
                    prunedNodes++;
                problem.evaluateRemoveDynamic(incumbent_s, fjssp_data, split_level);
            }
    
    branches += branches_created;
}

unsigned long BranchAndBound::permut(unsigned long n, unsigned long i) const {
	unsigned long result = 1;
	for (long j = n; j > n - i; --j)
		result *= j;
	return result;
}

/**
 * This functions compute the number of nodes.
 *
 */
unsigned long BranchAndBound::computeTotalNodes(unsigned long totalVariables) const {
	long n_nodes = 0;
    long nodes_per_branch = 0;
    long deepest_level;

    switch (problem.getType()) {
            
        case ProblemType::permutation:
            for (int i = 0; i < totalVariables; ++i)
                n_nodes += (totalVariables - i) * permut(totalVariables, i);
            break;
            
        case ProblemType::combination:
            nodes_per_branch = (problem.getUpperBound(0) + 1) - problem.getLowerBound(0);
            deepest_level = totalLevels + 1;
            n_nodes = (pow(nodes_per_branch, deepest_level + 1) - 1) / (nodes_per_branch - 1);
            break;
            
        case ProblemType::permutation_with_repetition_and_combination:
            /** TODO: Design the correct computaiton of the number of nodes. **/
            nodes_per_branch = (problem.getUpperBound(0) + 1) - problem.getLowerBound(0);
            deepest_level = totalLevels + 1;
            n_nodes = (pow(nodes_per_branch, deepest_level + 1) - 1) / (nodes_per_branch - 1);
            
            break;
            
        case ProblemType::XD:
            break;
    }

	return n_nodes;
}

void BranchAndBound::printCurrentSolution(int withVariables) {
	problem.printPartialSolution(incumbent_s, currentLevel);
}

/**
 * This function prints the pareto front found.
 **/
void BranchAndBound::printParetoFront(int withVariables) {

	int counterSolutions = 0;
	std::vector<Solution>::iterator it;

	for (it = paretoFront.begin(); it != paretoFront.end(); ++it) {
		printf("[%6d] ", ++counterSolutions);
        problem.printSolution(*it);
        printf("\n");
		problem.printSolutionInfo(*it);
		printf("\n");
	}
}

int BranchAndBound::setParetoFrontFile(const char * setOutputFile) {
	outputFile = new char[255];
	std::strcpy(outputFile, setOutputFile);
	return 0;
}

int BranchAndBound::setSummarizeFile(const char * outputFile) {
	summarizeFile = new char[255];
	std::strcpy(summarizeFile, outputFile);
	return 0;
}

void BranchAndBound::setParetoFront(const std::vector<Solution> &front){
    paretoFront = front;
}

int BranchAndBound::getRank() const{ return rank; }
int BranchAndBound::getCurrentLevel() const{ return currentLevel;}
unsigned long BranchAndBound::getNumberOfLevels() const{ return totalLevels;}
unsigned long BranchAndBound::getNumberOfNodes( ) const{ return totalNodes; }
unsigned long BranchAndBound::getNumberOfBranches( ) const{ return branches; }
unsigned long BranchAndBound::getNumberOfExploredNodes( ) const{ return exploredNodes; }
unsigned long BranchAndBound::getNumberOfCallsToBranch( ) const{ return callsToBranch; }
unsigned long BranchAndBound::getNumberOfReachedLeaves( ) const{ return reachedLeaves; }
unsigned long BranchAndBound::getNumberOfUnexploredNodes( ) const{ return unexploredNodes; }
unsigned long BranchAndBound::getNumberOfPrunedNodes( ) const{ return prunedNodes; }
unsigned long BranchAndBound::getNumberOfCallsToPrune( ) const{ return callsToPrune; }
unsigned long BranchAndBound::getNumberOfUpdatesInLowerBound( ) const{ return totalUpdatesInLowerBound; }

void BranchAndBound::increaseNumberOfExploredNodes(unsigned long value){ exploredNodes.fetch_and_add(value); }
void BranchAndBound::increaseNumberOfCallsToBranch(unsigned long value){ callsToBranch.fetch_and_add(value);}
void BranchAndBound::increaseNumberOfBranches(unsigned long value){ branches.fetch_and_add(value); }
void BranchAndBound::increaseNumberOfCallsToPrune(unsigned long value){ callsToPrune.fetch_and_add(value); }
void BranchAndBound::increaseNumberOfPrunedNodes(unsigned long value){ prunedNodes.fetch_and_add(value); }
void BranchAndBound::increaseNumberOfReachedLeaves(unsigned long value){ reachedLeaves.fetch_and_add(value); }
void BranchAndBound::increaseNumberOfUpdatesInLowerBound(unsigned long value){ totalUpdatesInLowerBound.fetch_and_add(value); }

const Solution& BranchAndBound::getIncumbentSolution() const { return incumbent_s;}
const IVMTree& BranchAndBound::getIVMTree() const { return ivm_tree;}
const Interval& BranchAndBound::getStartingInterval() const { return interval_to_solve;}
const ProblemFJSSP& BranchAndBound::getProblem() const { return problem;}
const FJSSPdata& BranchAndBound::getFJSSPdata() const { return fjssp_data;}

std::vector<Solution>& BranchAndBound::getParetoFront(){
    paretoFront = paretoContainer.getParetoFront();
    return paretoFront;
}

int BranchAndBound::saveSummarize() {

	printf("---Summarize---\n");
	printf("Pareto front size:   %ld\n", paretoFront.size());
	printf("Total nodes:         %ld\n", getNumberOfNodes());
	printf("Explored nodes:      %ld\n", getNumberOfExploredNodes());
	printf("Eliminated nodes:    %ld\n",
			getNumberOfNodes() - getNumberOfExploredNodes());
	printf("Calls to branching:  %ld\n", getNumberOfCallsToBranch());
	printf("Created branches:    %ld\n", getNumberOfBranches());
	printf("Calls to prune:      %ld\n", getNumberOfCallsToPrune());
	printf("Pruned nodes:        %ld\n", getNumberOfPrunedNodes());
	printf("Leaves reached:      %ld\n", getNumberOfReachedLeaves());
	printf("Updates in PF:       %ld\n", getNumberOfUpdatesInLowerBound());
	printf("Total time:          %f\n", getTotalTime());
	printf("Grid data:\n");
	printf("\tGrid dimension:    %d x %d\n", paretoContainer.getCols(),
			paretoContainer.getRows());
	printf("\tnon-dominated buckets:    %ld\n",
			paretoContainer.getNumberOfActiveBuckets());
	printf("\tdominated buckets:  %ld\n",
			paretoContainer.getNumberOfDisabledBuckets());
	printf("\tunexplored buckets:%ld\n",
			paretoContainer.getNumberOfUnexploredBuckets());
	printf("\tTotal elements in: %ld\n", paretoContainer.getSize());
    
    /*
    problem.printSolution(paretoFront.front());
    printf("\n");
    problem.printSchedule(paretoFront.front());
    */
	
    std::ofstream myfile(summarizeFile);
	if (myfile.is_open()) {
		printf("Saving summarize in file...\n");

		myfile << "---Summarize---\n";
		myfile << "Pareto front size:   " << paretoFront.size() << "\n";
		myfile << "Total nodes:         " << totalNodes << "\n";
		myfile << "Explored nodes:      " << exploredNodes << "\n";
		myfile << "Eliminated nodes:    "
				<< totalNodes - exploredNodes << "\n";
		myfile << "Calls to branching:  " << callsToBranch << "\n";
		myfile << "Created branches:    " << branches << "\n";
		myfile << "Calls to prune:      " << callsToPrune << "\n";
		myfile << "Leaves reached:      " << reachedLeaves << "\n";
		myfile << "Updates in PF:       " << totalUpdatesInLowerBound
				<< "\n";
		myfile << "Total time:          " << totalTime << "\n";

		myfile << "Grid data:\n";
		myfile << "\tdimension:         \t" << paretoContainer.getCols()
				<< " x " << paretoContainer.getRows() << "\n";
		myfile << "\tnon-dominated:     \t"
				<< paretoContainer.getNumberOfActiveBuckets() << "\n";
		myfile << "\tdominated:         \t"
				<< paretoContainer.getNumberOfDisabledBuckets() << "\n";
		myfile << "\tunexplored:        \t"
				<< paretoContainer.getNumberOfUnexploredBuckets() << "\n";
		myfile << "\tnumber of elements:\t" << paretoContainer.getSize()
				<< "\n";

		myfile << "The pareto front found is: \n";

		int numberOfObjectives = problem.getNumberOfObjectives();
		int numberOfVariables = problem.getNumberOfVariables();

		int nObj = 0;
		int nVar = 0;

		int counterSolutions = 0;

		std::vector<Solution>::iterator it;

		for (it = paretoFront.begin(); it != paretoFront.end(); ++it) {

			myfile << std::fixed << std::setw(6) << std::setfill(' ')
					<< ++counterSolutions << " ";

			for (nObj = 0; nObj < numberOfObjectives; ++nObj)
				myfile << std::fixed << std::setw(26) << std::setprecision(16)
						<< std::setfill(' ') << (*it).getObjective(nObj) << " ";

			myfile << " | ";

			for (nVar = 0; nVar < numberOfVariables; ++nVar)
				myfile << std::fixed << std::setw(4) << std::setfill(' ')
						<< (*it).getVariable(nVar) << " "; //printf("%3d ", (*it)->getVariable(nVar));

			myfile << " |\n";
		}

		myfile.close();
	} else
		printf("Unable to open file...\n");

	return 0;
}

int BranchAndBound::saveParetoFront() {

	paretoFront = paretoContainer.getParetoFront();

	std::ofstream myfile(outputFile);
	if (myfile.is_open()) {
		printf("[B&B-%03d] Saving in file...\n", rank);
		int numberOfObjectives = problem.getNumberOfObjectives();
		int nObj = 0;

		std::vector<Solution>::iterator it;

		for (it = paretoFront.begin(); it != paretoFront.end(); ++it) {
			for (nObj = 0; nObj < numberOfObjectives - 1; ++nObj)
				myfile << std::fixed << std::setw(26) << std::setprecision(16)
						<< std::setfill(' ') << (*it).getObjective(nObj)
						<< ", ";
			myfile << std::fixed << std::setw(26) << std::setprecision(16)
					<< std::setfill(' ')
					<< (*it).getObjective(numberOfObjectives - 1) << "\n";
		}
		myfile.close();
	} else
		printf("[B&B-%03d] Unable to open file...\n", rank);
	return 0;
}

void BranchAndBound::saveEvery(double timeInSeconds) {

	if (((std::clock() - start) / (double) CLOCKS_PER_SEC)
			> timeInSeconds) {
		start = std::clock();

		paretoFront = paretoContainer.getParetoFront();

		t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> time_span = std::chrono::duration_cast<
				std::chrono::milliseconds>(t2 - t1);
		totalTime = time_span.count();

		printf("The pareto front found is: \n");
		printParetoFront(1);
		saveParetoFront();
		saveSummarize();
	}
}

void BranchAndBound::printDebug(){
    printf("DEBUG\n");
    printf("GlobalPool:\n");
    globalPool.print();
    printf("Subproblem/interval:\n");
    interval_to_solve.print();
    printf("Incumbent solution at level: %3d\n", currentLevel);
    incumbent_s.print();
    printf("IVM Tree:\n");
    ivm_tree.print();
    printf("FJSSP Data:\n");
    fjssp_data.print();
    printf("DEBUG\n");
}
