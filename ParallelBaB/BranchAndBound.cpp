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

SubproblemsPool sharedPool;  /** intervals are the pending branches/subproblems/partialSolutions to be explored. **/
//ConcurrentHandlerContainer sharedParetoContainer;
ParetoBucket sharedParetoFront;
tbb::atomic<int> sleeping_bb;
tbb::atomic<int> there_is_more_work;

BranchAndBound::BranchAndBound(int node_rank, int rank, const ProblemVRPTW& problemToCopy, const Interval & branch):
is_grid_enable(false),
is_sorting_enable(false),
is_priority_enable(false),
node_rank(node_rank),
bb_rank(rank),
local_update_version(0),
currentLevel(branch.getBuildUpTo()),
problem(problemToCopy),
data_solution(problemToCopy.getNumberOfCustomers(),
           problemToCopy.getMaxNumberOfVehicles(),
           problemToCopy.getMaxVehicleCapacity()),
incumbent_s(problemToCopy.getNumberOfObjectives(), problemToCopy.getNumberOfVariables()),
ivm_tree(problemToCopy.getNumberOfVariables(), problemToCopy.getUpperBound(0) + 1),
interval_to_solve(branch),
elapsed_time(0) {
    number_of_shared_works = 0;
    number_of_tree_levels = problemToCopy.getNumberOfVariables();
    number_of_nodes = 0;
    number_of_nodes_created = (0);
    number_of_nodes_explored = (0);
    number_of_reached_leaves = (0);
    number_of_nodes_unexplored = (0);
    number_of_nodes_pruned = (0);
    number_of_calls_to_prune = (0);
    number_of_calls_to_branch = (0);
    number_of_updates_in_lower_bound = (0);
    
    time_start = std::clock();
    t1 = std::chrono::high_resolution_clock::now();
    t2 = std::chrono::high_resolution_clock::now();
    
    branches_to_move = problemToCopy.getUpperBound(0) * size_to_share;
    limit_level_to_share = number_of_tree_levels * deep_limit_share;
    
    number_of_nodes = computeTotalNodes(problemToCopy.getNumberOfVariables());
    
    ivm_tree.setOwnerId(rank);
}

BranchAndBound::BranchAndBound(const BranchAndBound& toCopy):
local_update_version(0),
is_grid_enable(toCopy.isGridEnable()),
is_sorting_enable(toCopy.isSortingEnable()),
is_priority_enable(toCopy.isPriorityEnable()),
node_rank(toCopy.getNodeRank()),
bb_rank(toCopy.getBBRank()),
currentLevel(toCopy.getCurrentLevel()),
problem(toCopy.getProblem()),
data_solution(toCopy.getVRPTWdata()),
incumbent_s(toCopy.getIncumbentSolution()),
ivm_tree(toCopy.getIVMTree()),
paretoContainer(toCopy.getParetoContainer()),
pareto_front(toCopy.getParetoFront()) {
    number_of_shared_works = (toCopy.getSharedWork());
    number_of_tree_levels = (toCopy.getNumberOfLevels());
    number_of_nodes = (toCopy.getNumberOfNodes());
    number_of_nodes_created = (toCopy.getNumberOfNodesCreated());
    number_of_nodes_explored = (toCopy.getNumberOfNodesExplored());
    number_of_reached_leaves = (toCopy.getNumberOfReachedLeaves());
    number_of_nodes_unexplored = (toCopy.getNumberOfNodesUnexplored());
    number_of_nodes_pruned = (toCopy.getNumberOfNodesPruned());
    number_of_calls_to_prune = (toCopy.getNumberOfCallsToPrune());
    number_of_calls_to_branch = (toCopy.getNumberOfCallsToBranch());
    number_of_updates_in_lower_bound = (toCopy.getNumberOfUpdatesInLowerBound());
    time_start = toCopy.time_start;
    elapsed_time = getElapsedTime();

    branches_to_move = problem.getUpperBound(0) * size_to_share;
    limit_level_to_share = number_of_tree_levels * deep_limit_share;

    std::strcpy(pareto_file, toCopy.pareto_file);
    std::strcpy(summarize_file, toCopy.summarize_file);
}

BranchAndBound& BranchAndBound::operator=(const BranchAndBound &toCopy) {
    local_update_version = toCopy.getPFVersion();
    is_grid_enable = toCopy.isGridEnable();
    is_sorting_enable = toCopy.isSortingEnable();
    is_priority_enable = toCopy.isPriorityEnable();
    node_rank = toCopy.getNodeRank();
    bb_rank = toCopy.getBBRank();
    currentLevel = toCopy.getCurrentLevel();
    problem = toCopy.getProblem();
    data_solution = toCopy.getVRPTWdata();
    incumbent_s = toCopy.getIncumbentSolution();
    ivm_tree = toCopy.getIVMTree();
    paretoContainer = toCopy.getParetoContainer();
    pareto_front = toCopy.getParetoFront();

    number_of_shared_works = (toCopy.getSharedWork());
    number_of_tree_levels = (toCopy.getNumberOfLevels());
    number_of_nodes = (toCopy.getNumberOfNodes());
    number_of_nodes_created = (toCopy.getNumberOfNodesCreated());
    number_of_nodes_explored = (toCopy.getNumberOfNodesExplored());
    number_of_reached_leaves = (toCopy.getNumberOfReachedLeaves());
    number_of_nodes_unexplored = (toCopy.getNumberOfNodesUnexplored());
    number_of_nodes_pruned = (toCopy.getNumberOfNodesPruned());
    number_of_calls_to_prune = (toCopy.getNumberOfCallsToPrune());
    number_of_calls_to_branch = (toCopy.getNumberOfCallsToBranch());
    number_of_updates_in_lower_bound = (toCopy.getNumberOfUpdatesInLowerBound());
    time_start = toCopy.time_start;
    elapsed_time = getElapsedTime();

    branches_to_move = problem.getUpperBound(0) * size_to_share;
    limit_level_to_share = number_of_tree_levels * deep_limit_share;

    std::strcpy(pareto_file, toCopy.pareto_file);
    std::strcpy(summarize_file, toCopy.summarize_file);

    return *this;
}

BranchAndBound& BranchAndBound::operator()(int node_rank_new, int rank_new, const ProblemVRPTW &problem_to_copy, const Interval &branch) {
    local_update_version = 0;
    t1 = std::chrono::high_resolution_clock::now();
    t2 = std::chrono::high_resolution_clock::now();
    
    time_start = std::clock();
    node_rank = node_rank_new;
    bb_rank = rank_new;
    problem = problem_to_copy;
    data_solution(problem.getNumberOfCustomers(), problem.getMaxNumberOfVehicles(), problem.getMaxVehicleCapacity());

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
    paretoContainer.clear();
    pareto_front.clear();
}

void BranchAndBound::initialize(int starts_tree) {
    time_start = std::clock();

    if (isGridEnable())
        paretoContainer(10, 10, problem.getFmax(0), problem.getFmax(1), problem.getFmin(0), problem.getFmin(1));
    else
        paretoContainer(1, 1, problem.getFmax(0), problem.getFmax(1), problem.getFmin(0), problem.getFmin(1));
    
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

     for (size_t solution_pf = 0; solution_pf < algorithms_pf.size(); ++solution_pf)
     updateBoundsWithSolution(algorithms_pf.at(solution_pf));
     */
    problem.createDefaultSolution(incumbent_s);
    updateParetoContainer(incumbent_s);
    updateBoundsWithSolution(incumbent_s);

    Solution temp(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.getSolutionWithLowerBoundInObj(1, temp);
    updateParetoContainer(temp);
    updateBoundsWithSolution(temp);

    Solution temp_2(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.getSolutionWithLowerBoundInObj(2, temp_2);
    updateParetoContainer(temp_2);
    updateBoundsWithSolution(temp_2);
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
 */
int BranchAndBound::initGlobalPoolWithInterval(const Interval & branch_init) {
    
    Interval branch_to_split(branch_init);
    
    problem.createDefaultSolution(incumbent_s);
    Solution temp(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.getSolutionWithLowerBoundInObj(1, temp);
    temp.print();
    incumbent_s.print();
    updateParetoContainer(incumbent_s);
    updateParetoContainer(temp);
    updateBoundsWithSolution(temp);
    
    int split_level = branch_to_split.getBuildUpTo() + 1;
    int nodes_created = 0;
    int num_elements = problem.getTotalElements();
    int code = 0;
    int toAdd = 0;

    data_solution.reset();
    for (int row = 0; row <= branch_to_split.getBuildUpTo(); ++row) {
        code = branch_to_split.getValueAt(row);
        incumbent_s.setVariable(row, code);
        problem.evaluateDynamic(incumbent_s, data_solution, row);
    }
    
    for (int element = 1; element <= num_elements; ++element)
        if (data_solution.getTimesThatElementAppears(element) < problem.getTimesThatValueCanBeRepeated(element)) {

            incumbent_s.setVariable(split_level, toAdd);
            problem.evaluateDynamic(incumbent_s, data_solution, split_level);
            increaseExploredNodes();
            if (data_solution.isFeasible() &&
                improvesTheParetoContainer(incumbent_s)) {

                branch_to_split.setValueAt(split_level, toAdd);

                branch_to_split.setDistance(0, minMaxNormalization(data_solution.getObjective(0), problem.getFmin(0), problem.getFmax(0)));

                branch_to_split.setDistance(1, minMaxNormalization(data_solution.getObjective(1), problem.getFmin(1), problem.getFmax(1)));

                setPriorityTo(branch_to_split);

                sharedPool.push(branch_to_split);
                increaseSharedWorks();
                branch_to_split.removeLastValue();
                nodes_created++;
            } else
                increasePrunedNodes();
            problem.evaluateRemoveDynamic(incumbent_s, data_solution, split_level);
        }

    increaseNumberOfNodesCreated(nodes_created);
    return nodes_created;
}

int BranchAndBound::intializeIVM_data(Interval& branch_init, IVMTree& tree) {

    int build_value = 0;
    int build_up_to = branch_init.getBuildUpTo();
    currentLevel = build_up_to;
    
    tree.setRootRow(build_up_to);/** root row of this tree. **/
    tree.setStartingRow(build_up_to + 1); /** Level/row with the first branches of the tree. **/
    tree.setActiveRow(build_up_to);

    data_solution.reset();
    for (int row = 0; row <= build_up_to; ++row) {
        for (int col = 0; col < tree.getNumberOfCols(); ++col)
            tree.setNodeValueAt(row, col, 0);
        build_value = branch_init.getValueAt(row);
        tree.setStartExploration(row, build_value);
        tree.setEndExploration(row, build_value);
        tree.setNumberOfNodesAt(row, 1);
        tree.setActiveColAtRow(row, build_value);
        tree.setNodeValueAt(row, build_value - 1, build_value);
        
        /** The interval is equivalent to the solution. **/
        incumbent_s.setVariable(row, build_value);
        problem.evaluateDynamic(incumbent_s, data_solution, row);
    }
    
    for (int row = build_up_to + 1; row <= number_of_tree_levels; ++row) {
        tree.setStartExploration(row, 0);
        tree.setActiveColAtRow(row, -1);
        tree.resetNumberOfNodesAt(row);
        incumbent_s.setVariable(row, -1);
    }

    tree.print();
    incumbent_s.setBuildUpTo(build_up_to);
    int branches_created = branchFromInterval(incumbent_s, build_up_to);
    
    /** Send intervals to global_pool. **/
    int branches_to_move_to_global_pool = branches_created * getSizeToShare();

    if (!sharedPool.isMaxLimitReached() &&
        branches_created > branches_to_move_to_global_pool &&
        branch_init.getBuildUpTo() <= getLimitLevelToShare())

        for (int moved = 0; moved < branches_to_move_to_global_pool; ++moved) {
            int val = tree.removeLastNodeAtRow(build_up_to + 1);
            branch_init.setValueAt(build_up_to + 1, val);

            incumbent_s.setVariable(build_up_to + 1, val);
            problem.evaluateDynamic(incumbent_s, data_solution, currentLevel + 1);
            
            branch_init.setDistance(0, minMaxNormalization(data_solution.getObjective(0), problem.getFmin(0), problem.getFmax(0)));
            branch_init.setDistance(1, minMaxNormalization(data_solution.getObjective(1), problem.getFmin(1), problem.getFmax(1)));
            setPriorityTo(branch_init);
            sharedPool.push(branch_init);
            number_of_shared_works++;
            
            branch_init.removeLastValue();
            problem.evaluateRemoveDynamic(incumbent_s, data_solution, currentLevel + 1);
        }
    
    return 0;
}

tbb::task* BranchAndBound::execute() {

    t1 = std::chrono::high_resolution_clock::now();
    initialize(interval_to_solve.getBuildUpTo());
    unsigned long number_of_sub_problems_popped = 0;
    while (!sharedPool.empty() || thereIsMoreWork())/** While the pool has intervals or there are more work on other nodes. **/
        if(sharedPool.try_pop(interval_to_solve)) {
            number_of_sub_problems_popped++;
            updateLocalPF();
            solve(interval_to_solve);
        }

    sleeping_bb++;
    updateLocalPF();
    pareto_front = paretoContainer.generateParetoFront();

    printf("[Worker-%03d:B&B-%03d] No more intervals in global pool. Sub-problems popped from queue: %ld. Going to sleep. [ET: %6.6f sec.]\n", node_rank, bb_rank, number_of_sub_problems_popped, getElapsedTime());
    return NULL;
}

void BranchAndBound::solve(Interval& branch_to_solve) {
    
    intializeIVM_data(branch_to_solve, ivm_tree);
    while (theTreeHasMoreNodes() && thereIsMoreTime()) {
        updateLocalPF();
        explore(incumbent_s);
        problem.evaluateDynamic(incumbent_s, data_solution, currentLevel);
        if (!aLeafHasBeenReached() && theTreeHasMoreNodes()) {
            if (improvesTheParetoContainer(incumbent_s))
                branch(incumbent_s, currentLevel);
            else
                prune(incumbent_s, currentLevel);
        } else {
            increaseReachedLeaves();
            if (updateParetoContainer(incumbent_s)) {
                printf("[Worker-%03d:B&B-%03d] New solution found:\n", node_rank, bb_rank);
                cout << incumbent_s;

                increaseUpdatesInLowerBound();
                updateGlobalPF(incumbent_s);
            }
            updateBounds(incumbent_s, data_solution);
            ivm_tree.pruneActiveNode();  /** Go back and prepare to remove the evaluations. **/
        }
        
        /** If the branching operator doesnt creates branches or the prune function was called then we need to remove the evaluations. Also if a leave has been reached. **/
        for (int l = currentLevel; l >= ivm_tree.getActiveRow(); --l)
            problem.evaluateRemoveDynamic(incumbent_s, data_solution, l);
        
        if(theTreeHasMoreNodes())
            shareWorkAndSendToGlobalPool(branch_to_solve);
    }
}

bool BranchAndBound::thereIsMoreWork() const {
    return there_is_more_work;
}

unsigned long BranchAndBound::getPFVersion() const {
    return local_update_version;
}

bool BranchAndBound::isLocalPFversionOutdated() const {
    return local_update_version < sharedParetoFront.getVersionUpdate();
}

void BranchAndBound::updateLocalPF() {
    if (isLocalPFversionOutdated()) {
        unsigned long global_version = sharedParetoFront.getVersionUpdate();

        std::vector<Solution> global_pf = sharedParetoFront.getVector();
        for (const auto& element : global_pf)
            paretoContainer.add(element);

        local_update_version = global_version;
    }
}

void BranchAndBound::updateGlobalPF(const Solution& local_solution) {
    sharedParetoFront.push_back(local_solution);
}

double BranchAndBound::getTimeLimit() const {
    return time_limit;
}

void BranchAndBound::setTimeLimit(double limit_sec) {
    time_limit = limit_sec;
}

bool BranchAndBound::isTimeLimitEnable() const {
    return time_limit > 0;
}

bool BranchAndBound::thereIsMoreTime() {
    return isTimeLimitEnable() ? getElapsedTime() <= time_limit : true;
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
int BranchAndBound::explore(Solution& solution) {
    currentLevel = ivm_tree.getActiveRow();
    solution.setVariable(currentLevel, ivm_tree.getActiveNode());
    return 0;
}

/**
  *Modifies the variable at built_up_to + 1 of the solution. The branch operator generates the new row from the pending values to be added to IVM structure.*

  - Parameters:

   - solution: The solution/sub-problem to branch.

 - currentLevel: The level or deep at which the branch will be performed.

 - Returns: The number of branches created.
 */
int BranchAndBound::branchFromInterval(Solution& solution, int currentLevel) {
    number_of_calls_to_branch++;

    int toAdd = 0;
    int nodes_created = 0;
    
    double ub_normalized[3];
    
    SortedVector sorted_elements;
    ObjectiveValues obj_values;

    for (int element = 1; element <= problem.getTotalElements(); ++element)

        if (data_solution.getTimesThatElementAppears(element) < problem.getTimesThatValueCanBeRepeated(element)) {

                toAdd = element;

                solution.setVariable(currentLevel + 1, toAdd);
                problem.evaluateDynamic(solution, data_solution, currentLevel + 1);
                increaseExploredNodes();

                bool is_improving = false;
                for (unsigned int objc = 0; objc < problem.getNumberOfObjectives(); ++objc) {
                    ub_normalized[objc] = minMaxNormalization(data_solution.getObjective(objc), problem.getBestObjectiveFoundIn(objc), problem.getFmax(objc));

                    if (ub_normalized[objc] <= 0)
                        is_improving = true;
                }
                /** If distance in obj1 is better  or distance in obj2 is better then it can produce an improvement. **/
                if (data_solution.isFeasible() && is_improving && improvesTheParetoContainer(solution)) {

                    /** TODO: Here we can use a Fuzzy method to give priority to branches at the top or less priority to branches at bottom also considering the error or distance to the lower bound.**/
                    if(isSortingEnable()) {
                        obj_values.setValue(toAdd);

                        for (unsigned int objc = 0; objc < problem.getNumberOfObjectives(); ++objc) {
                            obj_values.setObjective(objc, data_solution.getObjective(objc));
                            obj_values.setDistance(objc, minMaxNormalization(obj_values.getObjective(objc), problem.getFmin(objc), problem.getFmax(objc)));
                        }

                        sorted_elements.push(obj_values, SORTING_TYPES::DIST_1); //** sorting the nodes to give priority to promising nodes.
                    } else
                        ivm_tree.addNodeToRow(currentLevel + 1, toAdd);

                    nodes_created++;
                } else {
                    ivm_tree.addNodeToRow(currentLevel + 1, toAdd);
                    ivm_tree.pruneFirstNodeAtRow(currentLevel + 1);
                    increasePrunedNodes();
                }
                problem.evaluateRemoveDynamic(solution, data_solution, currentLevel + 1);
        }

    increaseNumberOfNodesCreated(nodes_created);
    if (nodes_created > 0) {
        if (isSortingEnable())
            for (const auto& it : sorted_elements)
                ivm_tree.addNodeToRow(currentLevel + 1, it.getValue());

        ivm_tree.moveToNextRow();
        ivm_tree.setActiveColAtRow(ivm_tree.getActiveRow(), ivm_tree.getStartExploration(ivm_tree.getActiveRow()));
        ivm_tree.setThereAreMoreBranches();

    } else  /** If no branches were created then move to the next node. **/
        ivm_tree.pruneActiveNode();

    return nodes_created;
}

/**
 *Modifies the variable at built_up_to + 1 of the solution. The branch operator copies the last row from the IVM structure.*

 - Parameters:

 - solution: The solution/sub-problem to branch.

 - currentLevel: The level or deep at which the branch will be performed. The new nodes will be in currentLevel + 1.

 - Returns: The number of branches created.
 */
int BranchAndBound::branch(Solution& solution, int currentLevel) {
    number_of_calls_to_branch++;

    int nodes_created = 0;

    double ub_normalized[3];

    SortedVector sorted_elements;
    ObjectiveValues obj_values;


    for (unsigned int node = 0; node < ivm_tree.getNumberOfCols(); ++node) {
        int current_node = abs(ivm_tree.getNodeValueAt(currentLevel, node));

        if (current_node == 0) /** It is an empty cell. **/
            break;

        /** Validates that the new node can be added. In problems like FJSSP some nodes can be repeated a number of times.
         If not the next line does the work:
            if (current_node != ivm_tree.getActiveNode())
         **/
        if (data_solution.getTimesThatElementAppears(current_node) < problem.getTimesThatValueCanBeRepeated(current_node)) {

            solution.setVariable(currentLevel + 1, current_node);
            problem.evaluateDynamic(solution, data_solution, currentLevel + 1);
            increaseExploredNodes();

            bool is_improving = false;
            for (unsigned int objc = 0; objc < problem.getNumberOfObjectives(); ++objc) {
                ub_normalized[objc] = minMaxNormalization(data_solution.getObjective(objc), problem.getBestObjectiveFoundIn(objc), problem.getFmax(objc));

                if (ub_normalized[objc] <= 0)
                    is_improving = true;
            }

            if (data_solution.isFeasible() && is_improving && improvesTheParetoContainer(solution)) {

                /** TODO: Here we can use a Fuzzy method to give priority to branches at the top or less priority to branches at bottom also considering the error or distance to the lower bound.**/
                if(isSortingEnable()) {
                    obj_values.setValue(current_node);

                    for (unsigned int objc = 0; objc < problem.getNumberOfObjectives(); ++objc) {
                        obj_values.setObjective(objc, data_solution.getObjective(objc));
                        obj_values.setDistance(objc, minMaxNormalization(obj_values.getObjective(objc), problem.getFmin(objc), problem.getFmax(objc)));
                    }

                    sorted_elements.push(obj_values, SORTING_TYPES::DIST_1); //** sorting the nodes to give priority to promising nodes.
                } else
                    ivm_tree.addNodeToRow(currentLevel + 1, current_node);

                nodes_created++;
            } else {
                ivm_tree.addNodeToRow(currentLevel + 1, current_node);
                ivm_tree.pruneLastNodeAtRow(currentLevel + 1);
                increasePrunedNodes();
            }
            problem.evaluateRemoveDynamic(solution, data_solution, currentLevel + 1);
            nodes_created++;
        }
    }

    increaseNumberOfNodesCreated(nodes_created);
    if (nodes_created > 0) {
        if (isSortingEnable())
            for (const auto& it : sorted_elements)
                ivm_tree.addNodeToRow(currentLevel + 1, it.getValue());

        ivm_tree.moveToNextRow();
        ivm_tree.setActiveColAtRow(ivm_tree.getActiveRow(), ivm_tree.getStartExploration(ivm_tree.getActiveRow()));
        ivm_tree.setThereAreMoreBranches();

    } else  /** If no branches were created then move to the next node. **/
        ivm_tree.pruneActiveNode();

    return nodes_created;
}

void BranchAndBound::prune(Solution & solution, int currentLevel) {
    number_of_calls_to_prune++;
    ivm_tree.pruneActiveNode();
    increasePrunedNodes();
}

bool BranchAndBound::aLeafHasBeenReached() const {
    return (currentLevel == number_of_tree_levels);
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
    if (sharedPool.isEmptying() && !sharedPool.isMaxLimitReached() && next_row < getLimitLevelToShare() && branches_to_move_to_global_pool > 1) {
        
        Solution temp(incumbent_s.getNumberOfObjectives(), incumbent_s.getNumberOfVariables());
        VRPTWdata data(data_solution);
        Interval branch_to_send(branch_to_solve);
        
        //data.reset();
        for (int l = 0; l <= ivm_tree.getRootRow() ; ++l) {
            temp.setVariable(l, incumbent_s.getVariable(l));
            problem.evaluateDynamic(temp, data, l);
        }
        
        /* In case we need to sort the intervals.
         * std::vector<Interval> intervals_to_send;
         */
        int total_moved = 0;
        while(sharedPool.isEmptying()
              && next_row < getLimitLevelToShare()
              && next_row <= ivm_tree.getActiveRow()
              && total_moved < ivm_tree.getActiveRow() - ivm_tree.getNumberOfPendingNodes() - 1) {
            
            branches_to_move_to_global_pool = ivm_tree.getNumberOfNodesAt(next_row) - 1;
            for(int moved = 0; moved < branches_to_move_to_global_pool; ++moved) {
                int value = ivm_tree.removeLastNodeAtRow(next_row);
                
                branch_to_send.setValueAt(next_row, value);
                temp.setVariable(next_row, value);
                problem.evaluateDynamic(temp, data, next_row);


                branch_to_send.setDistance(0, minMaxNormalization(data.getObjective(0), problem.getBestObjectiveFoundIn(0), problem.getFmax(0)));
                branch_to_send.setDistance(1, minMaxNormalization(data.getObjective(1), problem.getBestObjectiveFoundIn(1), problem.getFmax(1)));
                
                setPriorityTo(branch_to_send);
                sharedPool.push(branch_to_send); /** This stores a copy.**/
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

bool BranchAndBound::updateParetoContainer(const Solution & solution) {
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
bool BranchAndBound::improvesTheParetoContainer(const Solution & solution) {
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

void BranchAndBound::updateBounds(const Solution& sol, VRPTWdata& data) {
    
   /* if (data.getMakespan() < problem.getBestMakespanFound())
        problem.updateBestMakespanSolution(data);
    
    if (data.getMaxWorkload() < problem.getBestWorkloadFound())
        problem.updateBestMaxWorkloadSolution(data);
    */
}

void BranchAndBound::updateBoundsWithSolution(const Solution & solution) {
    
  /*  if (solution.getObjective(0) < problem.getBestMakespanFound())
        problem.updateBestMakespanSolutionWith(solution);
    
    if (solution.getObjective(1) < problem.getBestWorkloadFound())
        problem.updateBestMaxWorkloadSolutionWith(solution);
*/

}
/**
 *
 * The priority needs to consider the Deep of the branch and the distance to the lower bound.
 *
 **/
void BranchAndBound::setPriorityTo(Interval& interval) const {
    /** TODO: This can be replaced by a Fuzzy Logic Controller. **/
    if (isPriorityEnable()) {

        const float close = 0.25f; /** If it is less than 0.333f then it is close. **/
        const float half = 0.50f;  /** If it is more than 0.25f and less than 0.5f then it is at half distance. **/
        const float far = 0.75f;   /** If it is bigger than 0.75f then it is far. **/

        switch (interval.getDeep()) {
            case Deep::TOP:
                interval.setHighPriority();
                if (interval.getDistance(0) <= close || interval.getDistance(1) <= close) /** Good distance. **/
                    interval.setHighPriority();
                else if(interval.getDistance(0) <= half || interval.getDistance(1) <= half) /** Moderate distance. **/
                    interval.setNormalPriority();
                break;

            case Deep::MID:
                interval.setNormalPriority();

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
    } else
        interval.setHighPriority();
}

/** Returns the proximity to the given objective. When minimizing objectives, if it is less than 0 then it produces an improvement.
 *  other distance: (objective - value) / objective;
 */
float BranchAndBound::distanceToObjective(int value, int objective) {
    return (value - objective) / value;
}

double BranchAndBound::minMaxNormalization(int value, int min, int max) const{
    return ((value - min) * 1.0 )/ (max - min);
}

void BranchAndBound::enableGrid() {
    is_grid_enable = true;
}

void BranchAndBound::enableSortingNodes() {
    is_sorting_enable = true;
}

void BranchAndBound::enablePriorityQueue() {
    is_priority_enable = true;
}

bool BranchAndBound::isGridEnable() const {
    return is_grid_enable;
}

bool BranchAndBound::isSortingEnable() const {
    return is_sorting_enable;
}

bool BranchAndBound::isPriorityEnable() const {
    return is_priority_enable;
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

unsigned long BranchAndBound::getNumberOfDominatedContainers() const {
    return paretoContainer.getNumberOfDisabledBuckets();
}

unsigned long BranchAndBound::getNumberOfNonDominatedContainers() const {
    return paretoContainer.getNumberOfActiveBuckets();
}

unsigned long BranchAndBound::getNumberOfUnexploredContainers() const {
    return paretoContainer.getNumberOfUnexploredBuckets();
}

void BranchAndBound::increaseNumberOfNodesExplored(unsigned long value) {
    number_of_nodes_explored += value;
}

void BranchAndBound::increaseNumberOfCallsToBranch(unsigned long value) {
    number_of_calls_to_branch += value;
}

void BranchAndBound::increaseNumberOfNodesCreated(unsigned long value) {
    number_of_nodes_created += value;
}

void BranchAndBound::increaseNumberOfCallsToPrune(unsigned long value) {
    number_of_calls_to_prune += value;
}

void BranchAndBound::increaseNumberOfNodesPruned(unsigned long value) {
    number_of_nodes_pruned += value;
}

void BranchAndBound::increaseNumberOfReachedLeaves(unsigned long value) {
    number_of_reached_leaves += value;
}

void BranchAndBound::increaseNumberOfUpdatesInLowerBound(unsigned long value) {
    number_of_updates_in_lower_bound += value;
}

void BranchAndBound::increaseSharedWork(unsigned long value) {
    number_of_shared_works += value;
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

const ProblemVRPTW& BranchAndBound::getProblem() const {
    return problem;
}

const VRPTWdata& BranchAndBound::getVRPTWdata() const {
    return data_solution;
}

const HandlerContainer& BranchAndBound::getParetoContainer() const {
    return paretoContainer;
}

const ParetoFront& BranchAndBound::getParetoFront() const {
    return pareto_front;
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
    std::vector<Interval> to_restore;
    std::ofstream myfile(pool_file);
    if (myfile.is_open()) {
        Interval interval(problem.getNumberOfVariables());
        
        /** TODO: this needs a mutex. **/
        myfile << "pool_size: " << sharedPool.unsafe_size() << endl;
        for (size_t element = 0; element < sharedPool.unsafe_size(); ++element)
            if(sharedPool.try_pop(interval)) {
                to_restore.push_back(interval);

                myfile << interval.getBuildUpTo() << " ";
                for (int index_var = 0; index_var <= interval.getBuildUpTo(); ++index_var)
                    myfile << interval.getValueAt(index_var) << " ";
                
                for (int index_var = interval.getBuildUpTo() + 1; index_var < problem.getNumberOfVariables(); ++index_var)
                    myfile << "- ";
                myfile << endl;
            }

        /** This have to be used only when the progress is saved and then we have to recover the data to continue with the exploration. **/
        for (const auto& sub_problem : to_restore)
            sharedPool.push(sub_problem);

        myfile.close();
    } else
        printf("[Worker-%03d:B&B-%03d] Unable to write global pool: %s\n", node_rank, bb_rank, pool_file);
}

int BranchAndBound::saveSummarize() {
    
    printf("[Worker-%03d:B&B-%03d] ---Summarize---\n", getNodeRank(), getBBRank());
    printf("Pareto front size:   %ld\n", pareto_front.size());
    printf("Total time:          %f\n", getElapsedTime());
    printf("Total nodes:         %ld\n", getNumberOfNodes());
    printf("Eliminated nodes:    %ld\n", getNumberOfNodes() - getNumberOfNodesExplored());
    printf("Explored nodes:      %ld\n", getNumberOfNodesExplored());
    printf("Created nodes:       %ld\n", getNumberOfNodesCreated());
    printf("Pruned nodes:        %ld\n", getNumberOfNodesPruned());
    printf("Leaves reached:      %ld\n", getNumberOfReachedLeaves());
    printf("Calls to branching:  %ld\n", getNumberOfCallsToBranch());
    printf("Calls to prune:      %ld\n", getNumberOfCallsToPrune());
    printf("Updates in PF:       %ld\n", getNumberOfUpdatesInLowerBound());
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
        myfile << "Eliminated nodes:    " << number_of_nodes - number_of_nodes_explored << endl;
        myfile << "Explored nodes:      " << number_of_nodes_explored << endl;
        myfile << "Created nodes:       " << number_of_nodes_created << endl;
        myfile << "Pruned nodes:        " << number_of_nodes_pruned << endl;
        myfile << "Leaves reached:      " << number_of_reached_leaves << endl;
        myfile << "Calls to branching:  " << number_of_calls_to_branch << endl;
        myfile << "Calls to prune:      " << number_of_calls_to_prune << endl;
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
        for (const auto& it : pareto_front) {

            myfile << std::fixed << std::setw(6) << std::setfill(' ') << ++counterSolutions << " ";
            for (nObj = 0; nObj < numberOfObjectives; ++nObj)
                myfile << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill(' ') << it.getObjective(nObj) << " ";
            myfile << " | ";
            
            for (nVar = 0; nVar < numberOfVariables; ++nVar)
                myfile << std::fixed << std::setw(4) << std::setfill(' ') << it.getVariable(nVar) << " ";
            myfile << " |\n";
        }
        
        myfile.close();
    } else
        printf("[Worker-%03d:B&B-%03d] Unable to write on summarize file: %s\n", node_rank, bb_rank, summarize_file);
    
    return 0;
}

int BranchAndBound::saveParetoFront() {

    std::ofstream myfile(pareto_file);
    if (myfile.is_open()) {
        printf("[Worker-%03d:B&B-%03d] Saving in file...\n", node_rank, bb_rank);
        int numberOfObjectives = problem.getNumberOfObjectives();

        for (const auto& it : pareto_front) {
            for (int nObj = 0; nObj < numberOfObjectives - 1; ++nObj)
                myfile << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill(' ') << it.getObjective(nObj) << " ";
            myfile << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill(' ') << it.getObjective(numberOfObjectives - 1) << "\n";
        }

        myfile.close();
    } else
        printf("[Worker-%03d:B&B-%03d] Unable to write on pareto front file: %s\n", node_rank, bb_rank, pareto_file);
    return 0;
}

void BranchAndBound::saveEvery(double timeInSeconds) {
    
    if (((std::clock() - time_start) / (double) CLOCKS_PER_SEC) > timeInSeconds) {
        time_start = std::clock();
        
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

    for (const auto& it : pareto_front) {
        printf("[%6d] ", ++counterSolutions);
        problem.printSolution(it);
        printf("\n");
        if (withExtraInfo == 1) {
            problem.printSolutionInfo(it);
            printf("\n");
        }
    }
}

void BranchAndBound::print() const {
    printf("[B&B %d %d %lu %f]\n", getNodeRank(), getBBRank(), pareto_front.size(), elapsed_time);
}

void BranchAndBound::printDebug() {
    printf("\nSTART-DEBUG-INFO\n");
    printf("GlobalPool:\n");
    sharedPool.print();
    printf("Subproblem/interval:\n");
    interval_to_solve.print();
    printf("Incumbent solution at level: %3d\n", currentLevel);
    incumbent_s.print();
    printf("IVM Tree:\n");
    ivm_tree.print();
    printf("FJSSP Data:\n");
    data_solution.print();
    printf("END-DEBUG-INFO\n");
}
