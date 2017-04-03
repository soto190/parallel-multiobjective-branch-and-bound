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

BranchAndBound::BranchAndBound(const BranchAndBound& toCopy):
    rank(toCopy.getRank()),
    globalPool(toCopy.getGlobalPool()),
    problem(toCopy.getProblem()),
    paretoContainer(toCopy.getParetoGrid()),
    currentSolution(toCopy.getIncumbentSolution()),
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
        
        branches_to_move = problem.getUpperBound(0) * percent_to_move;
        deep_to_share = totalLevels * percent_deep;
    
        std::strcpy(outputFile, toCopy.outputFile);
        std::strcpy(summarizeFile, toCopy.summarizeFile);
}

BranchAndBound::BranchAndBound(int rank, const ProblemFJSSP& problemToCopy, const Interval & branch, GlobalPool &globa_pool, HandlerContainer& pareto_container):
    rank(rank),
    problem(problemToCopy),
    globalPool(globa_pool),
    starting_interval(branch),
    paretoContainer(pareto_container),
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
        

        branches_to_move = problem.getUpperBound(0) * percent_to_move;
        deep_to_share = totalLevels * percent_deep;
        
	int numberOfObjectives = problem.getNumberOfObjectives();
	int numberOfVariables = problem.getNumberOfVariables();

    totalNodes.fetch_and_store(computeTotalNodes(numberOfVariables));

	bestObjectivesFound(numberOfObjectives, numberOfVariables);
    currentSolution(numberOfObjectives, numberOfVariables);
	problem.createDefaultSolution(currentSolution);
    
	int nObj = 0;
	for (nObj = 0; nObj < numberOfObjectives; ++nObj)
		bestObjectivesFound.setObjective(nObj, currentSolution.getObjective(nObj));

	ivm_tree(problem.getNumberOfVariables(), problem.getUpperBound(0) + 1);
	ivm_tree.setOwner(rank);
}

BranchAndBound& BranchAndBound::operator()(int rank_new, const ProblemFJSSP &problem_to_copy, const Interval &branch){
    t1 = std::chrono::high_resolution_clock::now();
    t2 = std::chrono::high_resolution_clock::now();
    
    start = std::clock();
    rank = rank_new;
    problem = problem_to_copy;
    
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
    
    starting_interval = branch; /** Copy the branch. **/
    
    int numberOfObjectives = problem.getNumberOfObjectives();
    int numberOfVariables = problem.getNumberOfVariables();
    
    currentSolution(numberOfObjectives, numberOfVariables);
    bestObjectivesFound(numberOfObjectives, numberOfVariables);
    problem.createDefaultSolution(currentSolution);
    
    int nObj = 0;
    for (nObj = 0; nObj < numberOfObjectives; ++nObj)
        bestObjectivesFound.setObjective(nObj, currentSolution.getObjective(nObj));

    ivm_tree(problem.getNumberOfVariables(), problem.getUpperBound(0) + 1);
    ivm_tree.setOwner(rank);
    
    branches_to_move = problem.getUpperBound(0) * percent_to_move;
    deep_to_share = totalLevels * percent_deep;
    
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

	currentSolution(numberOfObjectives, numberOfVariables);
	bestObjectivesFound(numberOfObjectives, numberOfVariables);
	problem.createDefaultSolution(currentSolution);

	int nObj = 0;
	for (nObj = 0; nObj < numberOfObjectives; ++nObj)
		bestObjectivesFound.setObjective(nObj, currentSolution.getObjective(nObj));

    //updateParetoGrid(bestInObj1);
    //updateParetoGrid(bestInObj2);
	updateParetoGrid(currentSolution);

}

/**
 * This modifies the IVMTree.
 *
 * - branch.build_up_to.
 *
 **/
int BranchAndBound::initializeExplorationInterval(const Interval & branch_to_init, IVMTree& tree) {

	int col = 0;
	int row = 0; /** Counter level.**/
    int builded_value = 0;
    int builded_up_to = branch_to_init.getBuildUpTo();
    tree.setRootNode(builded_up_to);/** root node of this tree**/
    tree.setStartingLevel(builded_up_to); /** Level with the first branches of the tree. **/
    tree.setActiveLevel(builded_up_to);

    /** Copy the built part. TODO: Probably this part can be removed, considering the root node.**/
	for (row = 0; row <= builded_up_to; ++row) {
		for (col = 0; col < tree.getNumberOfCols(); ++col)
            tree.setIVMValueAt(row, col, -1);
        builded_value = branch_to_init.getValueAt(row);
        tree.setStartExploration(row, builded_value);
        tree.setEndExploration(row, builded_value);
        tree.setNumberOfNodesAt(row, 1);
        tree.setActiveNodeAt(row, builded_value);
        tree.setIVMValueAt(row, builded_value, builded_value);

		/** TODO: Check this part. The interval is equivalent to the solution?. **/
		currentSolution.setVariable(row, builded_value);
	}

    for (row = builded_up_to + 1; row <= totalLevels; ++row) {
        tree.setStartExploration(row, 0);
        tree.resetNumberOfNodesAt(row);
	}
    
	currentSolution.build_up_to = builded_up_to;
	int branches_created = branch(currentSolution, builded_up_to);
    tree.setActiveNodeAt(tree.getActiveLevel(), 0);
    tree.setActiveNodeAt(tree.getActiveLevel(), tree.getStartExploration(tree.getActiveLevel()));

    if (branches_created > 0)
        tree.setHasBranches(1);
    else
        tree.setHasBranches(0);

	return 0;
}

tbb::task* BranchAndBound::execute() {

	solve(starting_interval);
    return NULL;

}

void BranchAndBound::solve(const Interval& branch_to_solve) {

	double timeUp = 0;
	int updated = 0;
	int working = 1;

	Interval branchFromGlobal = branch_to_solve;
	Interval activeBranch(problem.getNumberOfVariables());

	initialize(branchFromGlobal.getBuildUpTo());

	while (working > 0) {

		if (!globalPool.empty()) {
			globalPool.try_pop(branchFromGlobal);
			working++;
			printf("[B&B-%03d] Picking from global pool. Pool size is %lu\n", rank, globalPool.unsafe_size());
			branchFromGlobal.showInterval();
            splitInterval(branchFromGlobal);

		} else
            working = 0;

		while (!localPool.empty()) {

			activeBranch = localPool.front();
			localPool.pop();
            
            initializeExplorationInterval(activeBranch, ivm_tree);
            
			while (theTreeHasMoreBranches() == 1 && timeUp == 0) {
				
                explore(currentSolution);
                problem.evaluatePartial(currentSolution, currentLevel);

				if (aLeafHasBeenReached() == 0 && theTreeHasMoreBranches()) {
					if (improvesTheGrid(currentSolution))
						branch(currentSolution, currentLevel);
					else
						prune(currentSolution, currentLevel);
				} else {

					reachedLeaves++;

					updated = updateParetoGrid(currentSolution);
					totalUpdatesInLowerBound += updated;

					if (updated == 1) {
						printf("[B&B-%03d] ", rank);
						printCurrentSolution();
						printf(" + [%6lu] \n", paretoContainer.getSize());
					}
				}
			}
		}

		t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		totalTime = time_span.count();
	}
    printf("[B&B-%03d] No more intervals in global pool. Going to sleep.\n", rank);
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

    if (aLeafHasBeenReached()){ /** If the active node is a leaf then we need to go up. **/
        ivm_tree.pruneActiveNode();
    }
    
	int level = ivm_tree.getCurrentLevel();
	int element = ivm_tree.getActiveNode();

	solution.setVariable(level, element);
	currentLevel = level;
	currentSolution.build_up_to = level;

	return 0;
}

/**
 * Modifies the variable at built_up_to + 1 of the solution.
 *
 * TODO: When branching insert some branches in the global pool.
 * TODO: When branching consider copy the last row.
 *
 */
int BranchAndBound::branch(Solution& solution, int currentLevel) {

	callsToBranch++;
    
    int element = 0;
    int isInPermut = 0;
    int level = 0;
    int levelStarting= 0;
    int varInPos = 0;
    int timesRepeated [problem.getTotalElements()];
    int toAdd = 0;
    int machine = 0;
    
    int branches_created = 0;
	//vector<double [3]> elements_sorted;

    switch (problem.getType()) {
            
        case ProblemType::permutation:
            levelStarting = problem.getStartingLevel();
            
            for (element = problem.getUpperBound(0); element >= problem.getLowerBound(0); --element) {
                isInPermut = 0;
                for (level = levelStarting; level <= currentLevel; ++level)
                    if (solution.getVariable(level) == element) {
                        isInPermut = 1;
                        level = currentLevel + 1;
                    }
                
                if (isInPermut == 0) {
                    ivm_tree.setNode(currentLevel + 1, element);
                    branches++;
                    branches_created++;
                }
            }
            
            break;
            
        case ProblemType::permutation_with_repetition_and_combination:
            
            for (element = 0; element < problem.getTotalElements(); ++element)
                timesRepeated[element] = 0;
            
            for (varInPos = 0; varInPos <= currentLevel; ++varInPos) {
                element = problem.getMapping(solution.getVariable(varInPos), 0);
                timesRepeated[element]++;
            }
            
            for (element = 0; element < problem.getTotalElements(); ++element) {
                if (timesRepeated[element] < problem.getTimesValueIsRepeated(element)) {
                    /** TODO: sort the branches. **/
                    for (machine = 0; machine < problem.getNumberOfMachines(); ++machine) {
                        toAdd = problem.getMappingOf(element, machine);
                        
                        solution.setVariable(currentLevel + 1, toAdd);
                        problem.evaluatePartial(solution, currentLevel + 1);
                        
                        if (improvesTheGrid(solution)) {
                            /*  double element[3];
                             element[0] = solution->getObjective(0);
                             element[1] = solution->getObjective(1);
                             element[2] = toAdd;
                             
                             elements_sorted.push_back(element);
                             */
                            ivm_tree.setNode(currentLevel + 1, toAdd);
                            branches++;
                            branches_created++;
                        } else
                            prunedNodes++;
                    }
                }
            }
            
            if (branches_created > 0) { /** If a branched was created. **/
                ivm_tree.moveToNextLevel();
                ivm_tree.setActiveNodeAt(ivm_tree.getActiveLevel(), 0);
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

int BranchAndBound::aLeafHasBeenReached() const {
	if (currentLevel == totalLevels)
		return 1;
	return 0;
}

/**
 * Check if the ivm has pending branches to be explored.
 */
int BranchAndBound::theTreeHasMoreBranches() const {
	return ivm_tree.hasPendingBranches();
}

int BranchAndBound::updateParetoGrid(const Solution & solution) {
    return paretoContainer.add(solution);
}

/**
 * The solution improves the lowe bound if:
 *  1- The solution is no-dominated by the lower bound or the solution dominates one of the solutions in the lower bound then it is said that the solution improves the lower bound.
 *  2- The solution dominates one solution in the front.
 *  3- The solution is non-dominated by all the solutions in the front.
 *  4- Any solution in the front dominates dominates the solution.
 *  5- It is not repeated.
 *  6- It is non-dominated.
 *
 *  NOTE: This doesnt modifies the solution. Can be const & solution.
 */
int BranchAndBound::improvesTheGrid(const Solution & solution) const {
	return paretoContainer.improvesTheGrid(solution);
}

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
				jobAllocated = problem.getMapping(map, 0);
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
                branch_to_compute.setValueAt(totalLevels, problem.getMappingOf(jobToCheck, problem.getTimesValueIsRepeated(0) - 1));

                branch_to_compute.increaseBuildUpTo();

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
void BranchAndBound::splitInterval(Interval & branch_to_split) {
    
	int index_var = 0;
	int level_to_split = branch_to_split.getBuildUpTo() + 1;
	int branches_created = 0;
    int num_elements = problem.getTotalElements();
	int timesRepeated[num_elements];
	int map = 0;
	int element = 0;
    int machine = 0;
    int toAdd = 0;

	Solution sol_test(2, branch_to_split.getSize());

    for (element = 0; element < num_elements; ++element)
        timesRepeated[element] = 0;
    
	for (index_var = 0; index_var <= branch_to_split.getBuildUpTo(); ++index_var) {
        map = branch_to_split.getValueAt(index_var);
		sol_test.setVariable(index_var, map);
        timesRepeated[problem.getMapping(map, 0)]++;
	}

	for (element = 0; element < num_elements; ++element)
        if (timesRepeated[element] < problem.getTimesValueIsRepeated(element))
            for (machine = 0; machine < problem.getNumberOfMachines(); ++machine) {

				toAdd = problem.getMappingOf(element, machine);
				sol_test.setVariable(level_to_split, toAdd);
				problem.evaluatePartial(sol_test, level_to_split);

				if (improvesTheGrid(sol_test) == 1) {
                    /** Gets the branch to add. */
                    branch_to_split.setValueAt(level_to_split, toAdd);
                    branch_to_split.setBuildUpTo(level_to_split);

					/**Add it to Intervals. **/
					if (rank == 0)
                        globalPool.push(branch_to_split); /** The vector adds a copy of interval. **/
                    else
						localPool.push(branch_to_split);
					branches_created++;
				} else
					prunedNodes++;
			}
	
    branches += branches_created;
    
	/** TODO: Design something to decide when to add something to the global pool. **/
	if (rank > 0
        && branches_created > branches_to_move
        && branch_to_split.getBuildUpTo() < deep_to_share) {
        
        int moved = 0;
        for (moved = 0; moved < branches_to_move; ++moved) {
            globalPool.push(localPool.front());
            localPool.pop();
        }
	}
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
	problem.printPartialSolution(currentSolution, currentLevel);
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

const Solution& BranchAndBound::getIncumbentSolution() const { return currentSolution;}
const IVMTree& BranchAndBound::getIVMTree() const { return ivm_tree;}
const Interval& BranchAndBound::getStartingInterval() const { return starting_interval;}
const ProblemFJSSP& BranchAndBound::getProblem() const { return problem;}

HandlerContainer& BranchAndBound::getParetoGrid() const { return paretoContainer;}
HandlerContainer& BranchAndBound::getParetoContainer() {return paretoContainer;}
GlobalPool& BranchAndBound::getGlobalPool() const { return globalPool;}

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
