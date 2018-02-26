//
//  BranchAndBound.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

/**
 * TODO: Decide if the grid is shared or each B&B has their own grid.
 *
 **/
#include "BranchAndBound.hpp"
SubproblemsPool globalPool;  /** intervals are the pending branches/subproblems/partialSolutions to be explored. **/
HandlerContainer paretoContainer;
tbb::atomic<int> sleeping_bb;
tbb::atomic<int> there_is_more_work;

BranchAndBound::BranchAndBound(const BranchAndBound& toCopy):
node_rank(toCopy.getNodeRank()),
bb_rank(toCopy.getBBRank()),
currentLevel(toCopy.getCurrentLevel()),
problem(toCopy.getProblem()),
fjssp_data(toCopy.getFJSSPdata()),
incumbent_s(toCopy.getIncumbentSolution()),
ivm_tree(toCopy.getIVMTree()) {
    number_of_shared_works.store(toCopy.getSharedWork());
    number_of_tree_levels.store(toCopy.getNumberOfLevels());
    number_of_nodes.store(toCopy.getNumberOfNodes());
    number_of_nodes_created.store(toCopy.getNumberOfNodesCreated());
    number_of_nodes_explored.store(toCopy.getNumberOfNodesExplored());
    number_of_reached_leaves.store(toCopy.getNumberOfReachedLeaves());
    number_of_nodes_unexplored.store(toCopy.getNumberOfNodesUnexplored());
    number_of_nodes_pruned.store(toCopy.getNumberOfNodesPruned());
    number_of_calls_to_prune.store(toCopy.getNumberOfCallsToPrune());
    number_of_calls_to_branch.store(toCopy.getNumberOfCallsToBranch());
    number_of_updates_in_lower_bound.store(toCopy.getNumberOfUpdatesInLowerBound());
    start = toCopy.start;
    elapsed_time = getElapsedTime();
    
    branches_to_move = problem.getUpperBound(0) * size_to_share;
    limit_level_to_share = number_of_tree_levels * deep_limit_share;
    
    std::strcpy(pareto_file, toCopy.pareto_file);
    std::strcpy(summarize_file, toCopy.summarize_file);
}

BranchAndBound::BranchAndBound(int node_rank, int rank, const ProblemFJSSP& problemToCopy, const Interval & branch):
node_rank(node_rank),
bb_rank(rank),
currentLevel(branch.getBuildUpTo()),
problem(problemToCopy),
fjssp_data(problemToCopy.getNumberOfJobs(),
           problemToCopy.getNumberOfOperations(),
           problemToCopy.getNumberOfMachines()),
incumbent_s(problemToCopy.getNumberOfObjectives(), problemToCopy.getNumberOfVariables()),
ivm_tree(problemToCopy.getNumberOfVariables(), problemToCopy.getUpperBound(0) + 1),
interval_to_solve(branch),
elapsed_time(0) {
    number_of_shared_works.store(0);
    number_of_tree_levels.store(problemToCopy.getNumberOfVariables());
    number_of_nodes.store(0);
    number_of_nodes_created.store(0);
    number_of_nodes_explored.store(0);
    number_of_reached_leaves.store(0);
    number_of_nodes_unexplored.store(0);
    number_of_nodes_pruned.store(0);
    number_of_calls_to_prune.store(0);
    number_of_calls_to_branch.store(0);
    number_of_updates_in_lower_bound.store(0);
    
    start = std::clock();
    t1 = std::chrono::high_resolution_clock::now();
    t2 = std::chrono::high_resolution_clock::now();
    
    branches_to_move = problemToCopy.getUpperBound(0) * size_to_share;
    limit_level_to_share = number_of_tree_levels * deep_limit_share;
    
    number_of_nodes.fetch_and_store(computeTotalNodes(problemToCopy.getNumberOfVariables()));
    
    ivm_tree.setOwnerId(rank);
}

BranchAndBound& BranchAndBound::operator()(int node_rank_new, int rank_new, const ProblemFJSSP &problem_to_copy, const Interval &branch) {
    t1 = std::chrono::high_resolution_clock::now();
    t2 = std::chrono::high_resolution_clock::now();
    
    start = std::clock();
    node_rank = node_rank_new;
    bb_rank = rank_new;
    problem = problem_to_copy;
    fjssp_data(problem.getNumberOfJobs(), problem.getNumberOfOperations(), problem.getNumberOfMachines());
    
    currentLevel = 0;
    number_of_tree_levels = 0;
    number_of_nodes = 0;
    number_of_nodes_created = 0;
    number_of_nodes_explored = 0;
    number_of_reached_leaves = 0;
    number_of_nodes_unexplored = 0;
    number_of_nodes_pruned = 0;
    number_of_calls_to_prune = 0;
    number_of_calls_to_branch = 0;
    number_of_updates_in_lower_bound = 0;
    elapsed_time = 0;
    number_of_shared_works = 0;
    
    interval_to_solve = branch; /** Copy the branch. **/
    
    int numberOfObjectives = problem.getNumberOfObjectives();
    int numberOfVariables = problem.getNumberOfVariables();
    
    incumbent_s(numberOfObjectives, numberOfVariables);
    problem.createDefaultSolution(incumbent_s);
    
    ivm_tree(problem.getNumberOfVariables(), problem.getUpperBound(0) + 1);
    ivm_tree.setOwnerId(bb_rank);
    
    branches_to_move = problem.getUpperBound(0) * size_to_share;
    limit_level_to_share = number_of_tree_levels * deep_limit_share;
    
    return *this;
}

BranchAndBound::~BranchAndBound() {
    pareto_front.clear();
}

void BranchAndBound::initialize(int starts_tree) {
    start = std::clock();
    
    if (starts_tree == -1)
        currentLevel = 0;
    else
        currentLevel = starts_tree;
    number_of_tree_levels = problem.getFinalLevel();
    number_of_nodes_created = 0;
    number_of_nodes_explored = 0;
    number_of_nodes_unexplored = 0;
    number_of_nodes_pruned = 0;
    number_of_calls_to_prune = 0;
    number_of_calls_to_branch = 0;
    number_of_updates_in_lower_bound = 0;
    number_of_nodes = computeTotalNodes(number_of_tree_levels);
    number_of_shared_works = 0;

    Solution sample_solution(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.createDefaultSolution(sample_solution);
    problem.evaluate(sample_solution);

    /*
     NSGA_II nsgaii_algorithm(problem);
     nsgaii_algorithm.setSampleSolution(sample_solution);
     nsgaii_algorithm.setCrossoverRate(0.90);
     nsgaii_algorithm.setMutationRate(0.90);
     nsgaii_algorithm.setMaxPopulationSize(50);
     nsgaii_algorithm.setMaxNumberOfGenerations(50);
     ParetoFront algorithms_pf = nsgaii_algorithm.solve();

     MOSA mosa_algorithm(problem);
     mosa_algorithm.setSampleSolution(sample_solution);
     mosa_algorithm.setCoolingRate(0.96);
     mosa_algorithm.setMaxMetropolisIterations(16);
     mosa_algorithm.setInitialTemperature(1000);
     mosa_algorithm.setFinalTemperature(0.001);
     mosa_algorithm.setPerturbationRate(0.950);
     algorithms_pf += mosa_algorithm.solve();

     printf("Bounds PF:\n");
     algorithms_pf.print();

     for (unsigned long solution_pf = 0; solution_pf < algorithms_pf.size(); ++solution_pf)
     updateBoundsWithSolution(algorithms_pf.at(solution_pf));
     */
    problem.createDefaultSolution(incumbent_s);
    updateBoundsWithSolution(incumbent_s);
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
int BranchAndBound::initGlobalPoolWithInterval(const Interval & branch_init) {
    
    Interval branch_to_split(branch_init);
    
    problem.createDefaultSolution(incumbent_s);
    Solution temp(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.getSolutionWithLowerBoundInObj(1, temp);
    temp.print();
    incumbent_s.print();
    updateParetoGrid(incumbent_s);
    updateParetoGrid(temp);
    updateBoundsWithSolution(temp);
    
    int row = 0;
    int split_level = branch_to_split.getBuildUpTo() + 1;
    int branches_created = 0;
    int num_elements = problem.getTotalElements();
    int map = 0;
    int toAdd = 0;
    
    fjssp_data.reset(); /** This function call is not necesary because the structurs are empty.**/
    fjssp_data.setMinTotalWorkload(problem.getSumOfMinPij());
    for (int m = 0; m < problem.getNumberOfMachines(); ++m) {
        fjssp_data.setBestWorkloadInMachine(m, problem.getBestWorkload(m));
        fjssp_data.setTempBestWorkloadInMachine(m, problem.getBestWorkload(m));
    }
    
    for (row = 0; row <= branch_to_split.getBuildUpTo(); ++row) {
        map = branch_to_split.getValueAt(row);
        incumbent_s.setVariable(row, map);
        problem.evaluateDynamic(incumbent_s, fjssp_data, row);
    }
    
    for (int element = 0; element < num_elements; ++element)
        if (fjssp_data.getNumberOfOperationsAllocatedFromJob(element) < problem.getTimesThatValueCanBeRepeated(element)) {
            int op = problem.getOperationInJobIsNumber(element, fjssp_data.getNumberOfOperationsAllocatedFromJob(element));
            unsigned long machines_aviable = problem.getNumberOfMachinesAvaibleForOperation(op);
            for (int machine = 0; machine < machines_aviable; ++machine) {
                int new_machine = problem.getMachinesAvaibleForOperation(op, machine);
                toAdd = problem.getEncodeMap(element, new_machine);
                incumbent_s.setVariable(split_level, toAdd);
                problem.evaluateDynamic(incumbent_s, fjssp_data, split_level);
                increaseExploredNodes();
                if (improvesTheGrid(incumbent_s)) {
                    branch_to_split.setValueAt(split_level, toAdd);
                    branch_to_split.setDistance(0, distanceToObjective(fjssp_data.getMakespan(), problem.getLowerBoundInObj(0)));
                    branch_to_split.setDistance(1, distanceToObjective(fjssp_data.getMaxWorkload(), problem.getLowerBoundInObj(1)));
                    setPriorityTo(branch_to_split);
                    
                    globalPool.push(branch_to_split); /** The vector adds a copy of interval. **/
                    increaseSharedWorks();
                    branch_to_split.removeLastValue();
                    branches_created++;
                } else
                    increasePrunedNodes();
                problem.evaluateRemoveDynamic(incumbent_s, fjssp_data, split_level);
            }
        }
    increaseNumberOfNodesCreated(branches_created);
    return branches_created;
}

int BranchAndBound::intializeIVM_data(Interval& branch_init, IVMTree& tree) {
    
    int col = 0;
    int row = 0; /** Counter level.**/
    int build_value = 0;
    int build_up_to = branch_init.getBuildUpTo();
    currentLevel = build_up_to;
    
    tree.setRootRow(build_up_to);/** root row of this tree**/
    tree.setStartingRow(build_up_to + 1); /** Level/row with the first branches of the tree. **/
    tree.setActiveRow(build_up_to);
    
    fjssp_data.setMinTotalWorkload(problem.getSumOfMinPij());
    for (int m = 0; m < problem.getNumberOfMachines(); ++m) {
        fjssp_data.setBestWorkloadInMachine(m, problem.getBestWorkload(m));
        fjssp_data.setTempBestWorkloadInMachine(m, problem.getBestWorkload(m));
    }
    
    fjssp_data.reset();
    
    for (row = 0; row <= build_up_to; ++row) {
        for (col = 0; col < tree.getNumberOfCols(); ++col)
            tree.setNodeValueAt(row, col, -1);
        build_value = branch_init.getValueAt(row);
        tree.setStartExploration(row, build_value);
        tree.setEndExploration(row, build_value);
        tree.setNumberOfNodesAt(row, 1);
        tree.setActiveColAtRow(row, build_value);
        tree.setNodeValueAt(row, build_value, build_value);
        
        /** The interval is equivalent to the solution. **/
        incumbent_s.setVariable(row, build_value);
        problem.evaluateDynamic(incumbent_s, fjssp_data, row);
    }
    
    for (row = build_up_to + 1; row <= number_of_tree_levels; ++row) {
        tree.setStartExploration(row, -1);
        tree.setActiveColAtRow(row, -1);
        tree.resetNumberOfNodesAt(row);
        incumbent_s.setVariable(row, -1);
    }
    
    incumbent_s.setBuildUpTo(build_up_to);
    int branches_created = branch(incumbent_s, build_up_to);
    
    /** Send intervals to global_pool. **/
    int branches_to_move_to_global_pool = branches_created * getSizeToShare();

    if (branches_created > branches_to_move_to_global_pool && branch_init.getBuildUpTo() <= getLimitLevelToShare())
        for (int moved = 0; moved < branches_to_move_to_global_pool; ++moved) {
            int val = tree.removeLastNodeAtRow(build_up_to + 1);
            branch_init.setValueAt(build_up_to + 1, val);

            incumbent_s.setVariable(build_up_to + 1, val);
            problem.evaluateDynamic(incumbent_s, fjssp_data, currentLevel + 1);
            
            branch_init.setDistance(0, distanceToObjective(fjssp_data.getMakespan(), problem.getLowerBoundInObj(0)));
            branch_init.setDistance(1, distanceToObjective(fjssp_data.getMaxWorkload(), problem.getLowerBoundInObj(1)));
            setPriorityTo(branch_init);
            globalPool.push(branch_init);
            number_of_shared_works++;
            
            branch_init.removeLastValue();
            problem.evaluateRemoveDynamic(incumbent_s, fjssp_data, currentLevel + 1);
        }
    
    return 0;
}

tbb::task* BranchAndBound::execute() {
    t1 = std::chrono::high_resolution_clock::now();
    initialize(interval_to_solve.getBuildUpTo());
    while (!globalPool.empty() || thereIsMoreWork()) /** While the pool has intervals or there are more work on other nodes. **/
        if(globalPool.try_pop(interval_to_solve))
            solve(interval_to_solve);
    
    sleeping_bb++;
    printf("[Worker-%03d:B&B-%03d] No more intervals in global pool. Going to sleep. [ET: %6.6f sec.]\n", node_rank, bb_rank, getElapsedTime());
    return NULL;
}

bool BranchAndBound::thereIsMoreWork() const {
    return there_is_more_work;
}

void BranchAndBound::solve(Interval& branch_to_solve) {
    
    double timeUp = 0;
    intializeIVM_data(branch_to_solve, ivm_tree);
    while (theTreeHasMoreNodes() && !timeUp) {
        
        explore(incumbent_s);
        problem.evaluateDynamic(incumbent_s, fjssp_data, currentLevel);
        if (!aLeafHasBeenReached() && theTreeHasMoreNodes()) {
            if (improvesTheGrid(incumbent_s))
                branch(incumbent_s, currentLevel);
            else
                prune(incumbent_s, currentLevel);
        }else {
            increaseReachedLeaves();
            if (updateParetoGrid(incumbent_s)) {
                increaseUpdatesInLowerBound();
                /*printf("[B&B-%03d] ", bb_rank);
                 printCurrentSolution();
                 printf(" + [%6lu] \n", paretoContainer.getSize());*/
            }
            updateBounds(incumbent_s, fjssp_data);
            ivm_tree.pruneActiveNode();  /** Go back and prepare to remove the evaluations. **/
        }
        
        /** If the branching operator doesnt creates branches or the prune function was called then we need to remove the evaluations. Also if a leave has been reached. **/
        for (int l = currentLevel; l >= ivm_tree.getActiveRow(); --l)
            problem.evaluateRemoveDynamic(incumbent_s, fjssp_data, l);
        
        if(theTreeHasMoreNodes())
            shareWorkAndSendToGlobalPool(branch_to_solve);
    }
}

double BranchAndBound::getElapsedTime() {
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    elapsed_time = time_span.count();
    return elapsed_time;
}

/**
 *  Gets the next node to explore.
 * Modifies the solution.
 **/
int BranchAndBound::explore(Solution & solution) {
    currentLevel = ivm_tree.getActiveRow();
    solution.setVariable(currentLevel, ivm_tree.getActiveNode());
    return 0;
}

/**
 * Modifies the variable at built_up_to + 1 of the solution.
 *
 * return the number of branches created.
 */
int BranchAndBound::branch(Solution& solution, int currentLevel) {
    number_of_calls_to_branch++;
    int isInPermut = 0;
    int row = 0;
    int levelStarting= 0;
    int toAdd = 0;
    int nodes_created = 0;
    
    float distance_error_to_best[2];
    
    SortedVector sorted_elements;
    ObjectiveValues obj_values;
    
    int best_values_found[2];
    for (int obj = 0; obj < 2; ++obj)
        best_values_found[obj] = paretoContainer.getBestValueFoundIn(obj);
    
    switch (problem.getType()) {
            
        case ProblemType::permutation:
            levelStarting = problem.getStartingRow();
            
            for (int element = problem.getUpperBound(0); element >= problem.getLowerBound(0); --element) {
                isInPermut = 0;
                for (row = levelStarting; row <= currentLevel; ++row)
                    if (solution.getVariable(row) == element) {
                        isInPermut = 1;
                        row = currentLevel + 1;
                    }
                
                if (isInPermut == 0) {
                    ivm_tree.addNodeToRow(currentLevel + 1, element);
                    number_of_nodes_created++;
                    nodes_created++;
                }
            }
            
            break;
            
        case ProblemType::permutation_with_repetition_and_combination:
            /** TODO: test to add only nodes with feasible solutions. **/
            for (int element = 0; element < problem.getTotalElements(); ++element)
                if (fjssp_data.getNumberOfOperationsAllocatedFromJob(element) < problem.getTimesThatValueCanBeRepeated(element)) {
                    int op = problem.getOperationInJobIsNumber(element, fjssp_data.getNumberOfOperationsAllocatedFromJob(element));
                    unsigned long machines_aviable = problem.getNumberOfMachinesAvaibleForOperation(op);
                    for (int machine = 0; machine < machines_aviable; ++machine) {
                        int new_machine = problem.getMachinesAvaibleForOperation(op, machine);
                        toAdd = problem.getEncodeMap(element, new_machine);
                        
                        solution.setVariable(currentLevel + 1, toAdd);
                        problem.evaluateDynamic(solution, fjssp_data, currentLevel + 1);
                        increaseExploredNodes();
                        
                        distance_error_to_best[0] = distanceToObjective(fjssp_data.getMakespan(), best_values_found[0]);
                        distance_error_to_best[1] = distanceToObjective(fjssp_data.getMaxWorkload(), best_values_found[1]);
                        /** If distance in obj1 is better  or distance in ob2 is better then it can produce an improvement. **/
                        if ((distance_error_to_best[0] <= 0 || distance_error_to_best[1] <= 0) && improvesTheGrid(solution)) {
                            
                            /** TODO: Here we can use a Fuzzy method to give priority to branches at the top or less priority to branches at bottom also considering the error or distance to the lower bound.**/
                            obj_values.setValue(toAdd);
                            obj_values.setObjective(0, fjssp_data.getMakespan());
                            obj_values.setObjective(1, fjssp_data.getMaxWorkload());
                            
                            obj_values.setDistance(0, distanceToObjective(obj_values.getObjective(0), problem.getLowerBoundInObj(0)));
                            obj_values.setDistance(1, distanceToObjective(obj_values.getObjective(1), problem.getLowerBoundInObj(1)));

                            sorted_elements.push(obj_values, SORTING_TYPES::DIST_1);/** sorting the nodes to give priority to promising nodes. **/
                            
                            //                            ivm_tree.addNodeToRow(currentLevel + 1, toAdd);
                            nodes_created++;
                        } else
                            increasePrunedNodes();
                        problem.evaluateRemoveDynamic(solution, fjssp_data, currentLevel + 1);
                    }
                }

            increaseNumberOfNodesCreated(nodes_created);
            if (nodes_created > 0) {

                for (std::deque<ObjectiveValues>::iterator it = sorted_elements.begin(); it != sorted_elements.end(); ++it)
                    ivm_tree.addNodeToRow(currentLevel + 1, (*it).getValue());

                ivm_tree.moveToNextRow();
                ivm_tree.setActiveColAtRow(ivm_tree.getActiveRow(), 0);
                ivm_tree.setThereAreMoreBranches();
            } else  /** If no branches were created then move to the next node. **/
                ivm_tree.pruneActiveNode();
            break;
            
        case ProblemType::combination:
            for (int element = problem.getUpperBound(0); element >= problem.getLowerBound(0); --element) {
                ivm_tree.addNodeToRow(currentLevel + 1, element);
                nodes_created++;
            }
            number_of_nodes_created += nodes_created;
            
            break;
            
        case ProblemType::XD:
            break;
    }
    
    return nodes_created;
}

void BranchAndBound::prune(Solution & solution, int currentLevel) {
    number_of_calls_to_prune++;
    increasePrunedNodes();
    ivm_tree.pruneActiveNode();
}

bool BranchAndBound::aLeafHasBeenReached() const {
    return (currentLevel == number_of_tree_levels) ? true : false;
}

/**
 * This function shares part of the ivm tree. The part corresponding to the root_row + 1 is sended to the global_pool if the next
 * conditions are reached:
 * 1) if the global pool has less than the indicated size.
 * 2) if the root_row + 1 is less than deep_share and it has more than 1 element.
 *
 * All the remanent of root_row + 1 is moved to the global pool.
 *
 ***/
void BranchAndBound::shareWorkAndSendToGlobalPool(const Interval & branch_to_solve) {
    
    int next_row = ivm_tree.getRootRow() + 1;
    unsigned long branches_to_move_to_global_pool = ivm_tree.getActiveRow() - ivm_tree.getNumberOfPendingNodes() - 1;
    
    /**
     * To start sharing we have to consider:
     * - If the global pool has enough subproblems to keep feeding the other threads.
     * - If the level at which we are going to share is not too deep.
     * - If we have branches to share.
     */
    if (globalPool.isEmptying() && next_row < getLimitLevelToShare() && branches_to_move_to_global_pool > 1) {
        
        Solution temp(incumbent_s.getNumberOfObjectives(), incumbent_s.getNumberOfVariables());
        FJSSPdata data(fjssp_data);
        Interval branch_to_send(branch_to_solve);
        
        data.reset();
        for (int l = 0; l <= ivm_tree.getRootRow() ; ++l) {
            temp.setVariable(l, incumbent_s.getVariable(l));
            problem.evaluateDynamic(temp, data, l);
        }
        
        /* In case we need to sort the intervals.
         * std::vector<Interval> intervals_to_send;
         */
        int total_moved = 0;
        while(globalPool.isEmptying()
              && next_row < getLimitLevelToShare()
              && next_row <= ivm_tree.getActiveRow()
              && total_moved < ivm_tree.getActiveRow() - ivm_tree.getNumberOfPendingNodes() - 1) {
            
            branches_to_move_to_global_pool = ivm_tree.getNumberOfNodesAt(next_row) - 1;
            for(int moved = 0; moved < branches_to_move_to_global_pool; ++moved) {
                int value = ivm_tree.removeLastNodeAtRow(next_row);
                
                branch_to_send.setValueAt(next_row, value);
                temp.setVariable(next_row, value);
                problem.evaluateDynamic(temp, data, next_row);
                
                branch_to_send.setDistance(0, distanceToObjective(data.getMakespan(), problem.getLowerBoundInObj(0)));
                branch_to_send.setDistance(1, distanceToObjective(data.getMaxWorkload(), problem.getLowerBoundInObj(1)));
                
                setPriorityTo(branch_to_send);
                globalPool.push(branch_to_send); /** This stores a copy.**/
                number_of_shared_works++;
                
                branch_to_send.removeLastValue();
                problem.evaluateRemoveDynamic(temp, data, next_row);
            }
            
            /** Resets / Clears the interval. **/
            if (next_row > ivm_tree.getRootRow() && next_row <= ivm_tree.getActiveRow() ) {
                branch_to_send.setValueAt(next_row, ivm_tree.getNodeValueAt(next_row, ivm_tree.getActiveColAt(next_row)));
                temp.setVariable(next_row, ivm_tree.getNodeValueAt(next_row, ivm_tree.getActiveColAt(next_row)));
                problem.evaluateDynamic(temp, data, next_row);
            }
            next_row++;
        }
    }
}

/**
 * Check if the ivm has pending branches to be explored.
 */
bool BranchAndBound::theTreeHasMoreNodes() const {
    return ivm_tree.thereAreMoreBranches();
}

bool BranchAndBound::updateParetoGrid(const Solution & solution) {
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
 */
bool BranchAndBound::improvesTheGrid(const Solution & solution) const {
    return paretoContainer.improvesTheGrid(solution);
}

unsigned long BranchAndBound::permut(unsigned long n, unsigned long i) const {
    unsigned long result = 1;
    for (long j = n; j > n - i; --j)
        result *= j;
    return result;
}

/**
 * This functions compute the number of nodes.
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
            deepest_level = number_of_tree_levels + 1;
            n_nodes = (pow(nodes_per_branch, deepest_level + 1) - 1) / (nodes_per_branch - 1);
            break;
            
        case ProblemType::permutation_with_repetition_and_combination:
            /** TODO: Design the correct computaiton of the number of nodes. **/
            /** Permutación without repetition: n!
             Permutation with repetition of each element is:
             n! / (a! * b! * c!)
             where n is the number of elements to use. And a is the number of times that a appears in the permutation.
             **/
            nodes_per_branch = (problem.getUpperBound(0) + 1) - problem.getLowerBound(0);
            deepest_level = number_of_tree_levels + 1;
            n_nodes = (pow(nodes_per_branch, deepest_level + 1) - 1) / (nodes_per_branch - 1);
            
            break;
            
        case ProblemType::XD:
            break;
    }
    
    return n_nodes;
}

void BranchAndBound::updateBounds(const Solution& sol, FJSSPdata& data) {
    
    if (data.getMakespan() < problem.getBestMakespanFound())
        problem.updateBestMakespanSolution(data);
    
    if (data.getMaxWorkload() < problem.getBestWorkloadFound())
        problem.updateBestMaxWorkloadSolution(data);
}

void BranchAndBound::updateBoundsWithSolution(const Solution & solution) {
    
    if (solution.getObjective(0) < problem.getBestMakespanFound())
        problem.updateBestMakespanSolutionWith(solution);
    
    if (solution.getObjective(1) < problem.getBestWorkloadFound())
        problem.updateBestMaxWorkloadSolutionWith(solution);
}
/**
 *
 * The priority needs to consider the Deep of the branch and the distance to the lower bound.
 *
 **/
void BranchAndBound::setPriorityTo(Interval& interval) const {
    /** TODO: This can be replaced by a Fuzzy Logic Controller. **/
    const float close = 0.25f; /** If it is less than 0.333f then it is close. **/
    const float half = 0.50f;   /** If it is more than 0.25f and less than 0.5f then it is at half distance. **/
    const float far = 0.75f;   /** If it is bigger than 0.75f then it is far. **/
    
    switch (interval.getDeep()) {
        case Deep::TOP:
            interval.setHighPriority();
            if (interval.getDistance(0) <= close || interval.getDistance(1) <= close) /** Good distance. **/
                interval.setHighPriority();
            else if(interval.getDistance(0) <= half || interval.getDistance(1) <= half) /** Moderate distance. **/
                interval.setMediumPriority();
            break;
            
        case Deep::MID:
            interval.setMediumPriority();
            
            if (interval.getDistance(0) <= close || interval.getDistance(1) <= close) /** Good distance. **/
                interval.setHighPriority();
            else if(interval.getDistance(0) >= far || interval.getDistance(1) >= far) /** Bad distance. **/
                interval.setLowPriority();
            break;
            
        case Deep::BOTTOM:
            interval.setLowPriority();
            if (interval.getDistance(0) <= close || interval.getDistance(1) <= close) /** Good distance. **/
                interval.setHighPriority();
            break;
            
        default:
            break;
    }
}

/** Returns the proximity to the given objective. When minimizing objectives, if it is less than 0 then it produces an improvement.
 *  other distance: (objective - value) / objective;
 ***/
float BranchAndBound::distanceToObjective(int value, int objective) {
    return (value - objective) / value;
}

int BranchAndBound::getNodeRank() const {
    return bb_rank;
}

int BranchAndBound::getBBRank() const {
    return bb_rank;
}

int BranchAndBound::getCurrentLevel() const {
    return currentLevel;
}

unsigned long BranchAndBound::getNumberOfLevels() const {
    return number_of_tree_levels;
}

unsigned long BranchAndBound::getNumberOfNodes( ) const {
    return number_of_nodes;
}

unsigned long BranchAndBound::getNumberOfNodesCreated( ) const {
    return number_of_nodes_created;
}

unsigned long BranchAndBound::getNumberOfNodesExplored( ) const {
    return number_of_nodes_explored;
}

unsigned long BranchAndBound::getNumberOfCallsToBranch( ) const {
    return number_of_calls_to_branch;
}

unsigned long BranchAndBound::getNumberOfReachedLeaves( ) const {
    return number_of_reached_leaves;
}

unsigned long BranchAndBound::getNumberOfNodesUnexplored( ) const {
    return number_of_nodes_unexplored;
}

unsigned long BranchAndBound::getNumberOfNodesPruned( ) const {
    return number_of_nodes_pruned;
}

unsigned long BranchAndBound::getNumberOfCallsToPrune( ) const {
    return number_of_calls_to_prune;
}

unsigned long BranchAndBound::getNumberOfUpdatesInLowerBound( ) const {
    return number_of_updates_in_lower_bound;
}

unsigned long BranchAndBound::getSharedWork() const {
    return number_of_shared_works;
}

void BranchAndBound::increaseNumberOfNodesExplored(unsigned long value) {
    number_of_nodes_explored.fetch_and_add(value);
}

void BranchAndBound::increaseNumberOfCallsToBranch(unsigned long value) {
    number_of_calls_to_branch.fetch_and_add(value);
}

void BranchAndBound::increaseNumberOfNodesCreated(unsigned long value) {
    number_of_nodes_created.fetch_and_add(value);
}

void BranchAndBound::increaseNumberOfCallsToPrune(unsigned long value) {
    number_of_calls_to_prune.fetch_and_add(value);
}

void BranchAndBound::increaseNumberOfNodesPruned(unsigned long value) {
    number_of_nodes_pruned.fetch_and_add(value);
}

void BranchAndBound::increaseNumberOfReachedLeaves(unsigned long value) {
    number_of_reached_leaves.fetch_and_add(value);
}

void BranchAndBound::increaseNumberOfUpdatesInLowerBound(unsigned long value) {
    number_of_updates_in_lower_bound.fetch_and_add(value);
}

void BranchAndBound::increaseSharedWork(unsigned long value) {
    number_of_shared_works.fetch_and_add(value);
}

void BranchAndBound::increaseExploredNodes() {
    number_of_nodes_explored++;
}

void BranchAndBound::increasePrunedNodes() {
    number_of_nodes_pruned++;
}

void BranchAndBound::increaseNodesCreated() {
    number_of_nodes_created++;
}

void BranchAndBound::increaseReachedLeaves() {
    number_of_reached_leaves++;
}

void BranchAndBound::increaseUpdatesInLowerBound() {
    number_of_updates_in_lower_bound++;
}

void BranchAndBound::increaseSharedWorks() {
    number_of_shared_works++;
}

const Solution& BranchAndBound::getIncumbentSolution() const {
    return incumbent_s;
}

const IVMTree& BranchAndBound::getIVMTree() const {
    return ivm_tree;
}

const Interval& BranchAndBound::getStartingInterval() const {
    return interval_to_solve;
}

const ProblemFJSSP& BranchAndBound::getProblem() const {
    return problem;
}

const FJSSPdata& BranchAndBound::getFJSSPdata() const {
    return fjssp_data;
}

int BranchAndBound::getLimitLevelToShare() const {
    return limit_level_to_share;
}

float BranchAndBound::getDeepLimitToShare() const {
    return deep_limit_share;
}

float BranchAndBound::getSizeToShare() const {
    return size_to_share;
}

std::vector<Solution>& BranchAndBound::getParetoFront() {
    pareto_front = paretoContainer.getParetoFront();
    return pareto_front;
}

void BranchAndBound::setParetoFrontFile(const char setOutputFile[255]) {
    std::strcpy(pareto_file, setOutputFile);
}

void BranchAndBound::setSummarizeFile(const char outputFile[255]) {
    std::strcpy(summarize_file, outputFile);
}

void BranchAndBound::setPoolFile(const char *outputFile) {
    std::strcpy(pool_file, outputFile);
}

void BranchAndBound::setParetoFront(const std::vector<Solution> &front) {
    pareto_front = front;
}

void BranchAndBound::buildOutputFiles() {
    
    std::vector<std::string> paths;
    std::vector<std::string> name_file;
    paths = split(summarize_file, '/');
    
    unsigned long int sizeOfElems = paths.size();
    name_file = split(paths[sizeOfElems - 1], '.');
    
    std::string output_file_pool = "/";
    std::string output_file_ivm = "/";
    for (int p = 0; p < paths.size() - 1; ++p)
        output_file_ivm += paths.at(p) + "/";
    output_file_pool = output_file_ivm;
    long long node_rank_longlong = static_cast<long long>(node_rank);
    output_file_ivm += name_file[0] + "-node" + std::to_string(node_rank_longlong) + "-ivm" + std::to_string(static_cast<long long>(bb_rank)) + ".txt";
    std::strcpy(ivm_file, output_file_ivm.c_str());
    
    output_file_pool += name_file[0] + "-node" + std::to_string(node_rank_longlong) + "-pool.txt";
    std::strcpy(pool_file, output_file_pool.c_str());
}


void BranchAndBound::saveCurrentState() const {
    /*
     * saveGlobalPool();
     * saveIVM();
     * saveParetoFront();
     */
}

void BranchAndBound::saveIVM() const {
    ivm_tree.saveToFile(ivm_file);
}

void BranchAndBound::saveGlobalPool() const {
    /** TODO: globalPoolFile is saved by the container B&B (bb_rank = 0).**/
    std::ofstream myfile(pool_file);
    if (myfile.is_open()) {
        Interval interval(problem.getNumberOfVariables());
        
        /** TODO: this needs a mutex. **/
        myfile << "pool_size: " << globalPool.unsafe_size() << endl;
        for (unsigned long element = 0; element < globalPool.unsafe_size(); ++element)
            if(globalPool.try_pop(interval)) {
                
                myfile << interval.getBuildUpTo() << " ";
                for (int index_var = 0; index_var <= interval.getBuildUpTo(); ++index_var)
                    myfile << interval.getValueAt(index_var) << " ";
                
                for (int index_var = interval.getBuildUpTo() + 1; index_var < problem.getNumberOfVariables(); ++index_var)
                    myfile << "- ";
                myfile << endl;
            }
        
        myfile.close();
    } else
        printf("[Worker-%03d:B&B-%03d] Unable to write global pool: %s\n", node_rank, bb_rank, pool_file);
}

int BranchAndBound::saveSummarize() {
    
    printf("[Worker-%03d:B&B-%03d] ---Summarize---\n", getNodeRank(), getBBRank());
    printf("Pareto front size:   %ld\n", pareto_front.size());
    printf("Total nodes:         %ld\n", getNumberOfNodes());
    printf("Explored nodes:      %ld\n", getNumberOfNodesExplored());
    printf("Eliminated nodes:    %ld\n", getNumberOfNodes() - getNumberOfNodesExplored());
    printf("Calls to branching:  %ld\n", getNumberOfCallsToBranch());
    printf("Created nodes:       %ld\n", getNumberOfNodesCreated());
    printf("Calls to prune:      %ld\n", getNumberOfCallsToPrune());
    printf("Pruned nodes:        %ld\n", getNumberOfNodesPruned());
    printf("Leaves reached:      %ld\n", getNumberOfReachedLeaves());
    printf("Updates in PF:       %ld\n", getNumberOfUpdatesInLowerBound());
    printf("Total time:          %f\n", getElapsedTime());
    printf("Shared work: %ld\n", getSharedWork());
    printf("Grid data:\n");
    printf("\tGrid dimension:    %d x %d\n", paretoContainer.getCols(), paretoContainer.getRows());
    printf("\tnon-dominated buckets:    %ld\n", paretoContainer.getNumberOfActiveBuckets());
    printf("\tdominated buckets:  %ld\n", paretoContainer.getNumberOfDisabledBuckets());
    printf("\tunexplored buckets:%ld\n", paretoContainer.getNumberOfUnexploredBuckets());
    printf("\tNumber of elements in: %ld\n", paretoContainer.getSize());
    
    std::ofstream myfile(summarize_file);
    if (myfile.is_open()) {
        printf("[Worker-%03d:B&B-%03d] Saving summarize in file %s\n", node_rank, bb_rank, summarize_file);
        
        myfile << "---Summarize---\n";
        //myfile << "Number of threads:   " << getNumberOfBB() << endl;
        myfile << "Sharing size:        " << getSizeToShare() << endl;
        myfile << "Deep limit to share: " << getDeepLimitToShare() << endl;
        myfile << "Pareto front size:   " << pareto_front.size() << endl;
        myfile << "Total nodes:         " << number_of_nodes << endl;
        myfile << "Explored nodes:      " << number_of_nodes_explored << endl;
        myfile << "Eliminated nodes:    " << number_of_nodes - number_of_nodes_explored << endl;
        myfile << "Calls to branching:  " << number_of_calls_to_branch << endl;
        myfile << "Created nodes:       " << number_of_nodes_created << endl;
        myfile << "Calls to prune:      " << number_of_calls_to_prune << endl;
        myfile << "Pruned nodes:        " << number_of_nodes_pruned << endl;
        myfile << "Leaves reached:      " << number_of_reached_leaves << endl;
        myfile << "Updates in PF:       " << number_of_updates_in_lower_bound << endl;
        myfile << "Shared work:         " << number_of_shared_works << endl;
        myfile << "Total time:          " << elapsed_time << endl;
        
        myfile << "Grid data:\n";
        myfile << "\tdimension:         \t" << paretoContainer.getCols() << " x " << paretoContainer.getRows() << endl;
        myfile << "\tnon-dominated:     \t" << paretoContainer.getNumberOfActiveBuckets() << endl;
        myfile << "\tdominated:         \t" << paretoContainer.getNumberOfDisabledBuckets() << endl;
        myfile << "\tunexplored:        \t" << paretoContainer.getNumberOfUnexploredBuckets() << endl;
        myfile << "\tnumber of elements:\t" << paretoContainer.getSize() << endl;
        myfile << "The pareto front found is: \n";
        
        int numberOfObjectives = problem.getNumberOfObjectives();
        int numberOfVariables = problem.getNumberOfVariables();
        
        int nObj = 0;
        int nVar = 0;
        
        int counterSolutions = 0;
        
        std::vector<Solution>::iterator it;
        for (it = pareto_front.begin(); it != pareto_front.end(); ++it) {
            
            myfile << std::fixed << std::setw(6) << std::setfill(' ') << ++counterSolutions << " ";
            for (nObj = 0; nObj < numberOfObjectives; ++nObj)
                myfile << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill(' ') << (*it).getObjective(nObj) << " ";
            myfile << " | ";
            
            for (nVar = 0; nVar < numberOfVariables; ++nVar)
                myfile << std::fixed << std::setw(4) << std::setfill(' ') << (*it).getVariable(nVar) << " ";
            myfile << " |\n";
        }
        
        myfile.close();
    } else
        printf("[Worker-%03d:B&B-%03d] Unable to write on summarize file: %s\n", node_rank, bb_rank, summarize_file);
    
    return 0;
}

int BranchAndBound::saveParetoFront() {
    
    pareto_front = paretoContainer.getParetoFront();
    
    std::ofstream myfile(pareto_file);
    if (myfile.is_open()) {
        printf("[Worker-%03d:B&B-%03d] Saving in file...\n", node_rank, bb_rank);
        int numberOfObjectives = problem.getNumberOfObjectives();
        
        std::vector<Solution>::iterator it;
        
        for (it = pareto_front.begin(); it != pareto_front.end(); ++it) {
            for (int nObj = 0; nObj < numberOfObjectives - 1; ++nObj)
                myfile << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill(' ') << (*it).getObjective(nObj) << " ";
            myfile << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill(' ') << (*it).getObjective(numberOfObjectives - 1) << "\n";
        }
        myfile.close();
    } else
        printf("[Worker-%03d:B&B-%03d] Unable to write on pareto front file: %s\n", node_rank, bb_rank, pareto_file);
    return 0;
}

void BranchAndBound::saveEvery(double timeInSeconds) {
    
    if (((std::clock() - start) / (double) CLOCKS_PER_SEC) > timeInSeconds) {
        start = std::clock();
        
        pareto_front = paretoContainer.getParetoFront();
        
        t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        elapsed_time = time_span.count();
        
        printf("The pareto front found is: \n");
        printParetoFront(1);
        saveParetoFront();
        saveSummarize();
        saveGlobalPool();
    }
}

void BranchAndBound::printCurrentSolution(int withVariables) {
    problem.printPartialSolution(incumbent_s, currentLevel);
}

void BranchAndBound::printParetoFront(int withExtraInfo) {
    
    int counterSolutions = 0;
    std::vector<Solution>::iterator it;
    
    for (it = pareto_front.begin(); it != pareto_front.end(); ++it) {
        printf("[%6d] ", ++counterSolutions);
        problem.printSolution(*it);
        printf("\n");
        if (withExtraInfo == 1) {
            problem.printSolutionInfo(*it);
            printf("\n");
        }
    }
}

void BranchAndBound::printDebug() {
    printf("\nSTART-DEBUG-INFO\n");
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
    printf("END-DEBUG-INFO\n");
}
