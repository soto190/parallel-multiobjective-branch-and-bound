//
//  ParallelBranchAndBound.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 06/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//
/** Dummy commit**/
#include "ParallelBranchAndBound.hpp"

ParallelBranchAndBound::ParallelBranchAndBound(int rank, int n_threads, const ProblemVRPTW& problem):
time_limit(0),
is_grid_enable(false),
is_sorting_enable(false),
is_priority_enable(false),
rank(rank),
number_of_bbs(n_threads),
problem(problem),
branch_init(problem.getNumberOfVariables()) {
    number_of_nodes = 0;
    number_of_nodes_created = 0;
    number_of_nodes_pruned = 0;
    number_of_nodes_explored = 0;
    number_of_nodes_unexplored = 0;
    number_of_calls_to_branch = 0;
    number_of_reached_leaves = 0;
    number_of_calls_to_prune = 0;
    number_of_updates_in_lower_bound = 0;
    number_of_tree_levels = 0;
    number_of_shared_works = 0;

    number_of_non_dominated_buckets = 0;
    number_of_unexplored_buckets = 0;
    number_of_dominated_buckets = 0;

    time_start = std::clock();
    t1 = std::chrono::high_resolution_clock::now();
    t2 = std::chrono::high_resolution_clock::now();
}

ParallelBranchAndBound::~ParallelBranchAndBound() {
    
}

tbb::task * ParallelBranchAndBound::execute() {

    initSharedParetoFront();
    initSharedPool(branch_init);

    set_ref_count(number_of_bbs + 1);
    
    tbb::task_list tl; /** When task_list is destroyed it doesn't calls the destructor. **/
    vector<BranchAndBound *> bb_threads;
    int n_bb = 0;
    while (n_bb++ < number_of_bbs) {
        BranchAndBound * BaB_task = new (tbb::task::allocate_child()) BranchAndBound(rank, n_bb,  problem, branch_init);
        if (isGridEnable())
            BaB_task->enableGrid();

        if (isSortingEnable())
            BaB_task->enableSortingNodes();

        if (isPriorityEnable())
            BaB_task->enablePriorityQueue();

        BaB_task->setTimeLimit(getTimeLimit());
        BaB_task->setSummarizeFile(summarize_file);
        BaB_task->setParetoFrontFile(pareto_file);
        BaB_task->setPoolFile(pool_file);
        bb_threads.push_back(BaB_task);
        tl.push_back(*BaB_task);
    }
    
    printf("[Worker-%03d] Spawning the swarm...\nWaiting for all...\n", rank);
    tbb::task::spawn_and_wait_for_all(tl);
    printf("[Worker-%03d] Job done...\n", rank);
    
    /** Recollects the data. **/
    getElapsedTime();
    BranchAndBound* bb_in;
    while (!bb_threads.empty()) {
        
        bb_in = bb_threads.back();
        bb_threads.pop_back();
        number_of_nodes = bb_in->getNumberOfNodes();
        number_of_nodes_created += bb_in->getNumberOfNodesCreated();
        number_of_nodes_pruned += bb_in->getNumberOfNodesPruned();
        number_of_nodes_explored += bb_in->getNumberOfNodesExplored();
        number_of_calls_to_branch += bb_in->getNumberOfCallsToBranch();
        number_of_reached_leaves += bb_in->getNumberOfNodesExplored();
        number_of_calls_to_prune += bb_in->getNumberOfCallsToPrune();
        number_of_updates_in_lower_bound += bb_in->getNumberOfUpdatesInLowerBound();
        number_of_shared_works += bb_in->getSharedWork();

        number_of_non_dominated_buckets += bb_in->getNumberOfNonDominatedContainers();
        number_of_dominated_buckets += bb_in->getNumberOfDominatedContainers();
        number_of_unexplored_buckets += bb_in->getNumberOfUnexploredContainers();
    }

    printf("[Worker-%03d] Parallel Branch And Bound front.\n", rank);
    cout << sharedParetoFront;

    vector<Solution> v_pf = sharedParetoFront.getVector();
    for (auto solution = v_pf.begin(); solution != v_pf.end(); ++solution)
        pareto_front.push_back(*solution);

    saveSummarize();
    saveParetoFront();
    saveParetoFrontInLaTex();

    bb_threads.clear();
    printf("[Worker-%03d] Parallel Branch And Bound ended.\n", rank);
    return NULL;
}

void ParallelBranchAndBound::initSharedParetoFront() {
    Solution temp_1(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.createDefaultSolution(temp_1);
    sharedParetoFront.push_back(temp_1);
    problem.updateBestSolutionInObjectiveWith(0, temp_1);

    Solution temp(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.heuristic(temp);
    sharedParetoFront.push_back(temp);
    problem.updateBestSolutionInObjectiveWith(0, temp_1);

    Solution temp_2(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.heuristic_min_dist(temp_2);
    sharedParetoFront.push_back(temp_2);
    problem.updateBestSolutionInObjectiveWith(0, temp_2);

    //problem.updateBestBoundsWith(temp_1);
    //problem.updateBestBoundsWith(temp);
    //problem.updateBestBoundsWith(temp_2);

    std::cout << temp_1 << sharedParetoFront;
}
/*
 This is for the FJSSP.
int ParallelBranchAndBound::initSharedPool(const Interval & branch_init) {

    sharedPool.setSizeEmptying((unsigned long) (number_of_bbs * 2));
    Interval branch_to_split(branch_init);
    Solution incumbent_s(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.createDefaultSolution(incumbent_s);

    int split_level = branch_to_split.getBuildUpTo() + 1;
    int nodes_created = 0;
    int num_elements = problem.getTotalElements();
    int code = 0;
    int toAdd = 0;

    FJSSPdata fjssp_data (problem.getNumberOfJobs(), problem.getNumberOfOperations(), problem.getNumberOfMachines());
    fjssp_data.setMinTotalWorkload(problem.getSumOfMinPij());
    for (int m = 0; m < problem.getNumberOfMachines(); ++m) {
        fjssp_data.setBestWorkloadInMachine(m, problem.getBestWorkload(m));
        fjssp_data.setTempBestWorkloadInMachine(m, problem.getBestWorkload(m));
    }

    for (int row = 0; row <= branch_to_split.getBuildUpTo(); ++row) {
        code = branch_to_split.getValueAt(row);
        incumbent_s.setVariable(row, code);
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

                number_of_nodes_explored++;
                if (sharedParetoFront.produceImprovement(incumbent_s)) {
                    branch_to_split.setValueAt(split_level, toAdd);

                    sharedPool.push(branch_to_split);
                    branch_to_split.removeLastValue();
                    nodes_created++;
                    problem.evaluateRemoveDynamic(incumbent_s, fjssp_data, split_level);

                    number_of_shared_works++;
                    number_of_nodes_created++;
                } else
                    number_of_nodes_pruned++;
            }
        }
    return nodes_created;
}
 */

/**
 *Function for the VRPTW.*

 - Parameter branch_init: initial branch from where the pool will be initialized.

 */
int ParallelBranchAndBound::initSharedPool(const Interval & branch_init) {
    sharedPool.setSizeEmptying((unsigned long) (number_of_bbs * 2));
    Interval branch_to_split(branch_init);
    Solution incumbent_s(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.createDefaultSolution(incumbent_s);

    int split_level = branch_to_split.getBuildUpTo() + 1;
    int nodes_created = 0;
    int num_elements = problem.getTotalElements();
    int code = 0;
    int toAdd = 0;

    VRPTWdata solution_data (problem.getNumberOfCustomers(), problem.getMaxNumberOfVehicles(), problem.getMaxVehicleCapacity());

    for (int row = 0; row <= branch_to_split.getBuildUpTo(); ++row) {
        code = branch_to_split.getValueAt(row);
        incumbent_s.setVariable(row, code);
        problem.evaluateDynamic(incumbent_s, solution_data, row);
    }

    for (int element = 1; element <= num_elements; ++element)
        if (solution_data.getTimesThatElementAppears(element) < problem.getTimesThatValueCanBeRepeated(element)) {

            toAdd = element;
            incumbent_s.setVariable(split_level, toAdd);
            problem.evaluateDynamic(incumbent_s, solution_data, split_level);

            number_of_nodes_explored++;
            if (solution_data.isFeasible() && sharedParetoFront.produceImprovement(incumbent_s)) {
                branch_to_split.setValueAt(split_level, toAdd);

                sharedPool.push(branch_to_split);
                branch_to_split.removeLastValue();
                nodes_created++;

                number_of_shared_works++;
                number_of_nodes_created++;
            } else
                number_of_nodes_pruned++;

            problem.evaluateRemoveDynamic(incumbent_s, solution_data, split_level);
        }
    return nodes_created;
}

double ParallelBranchAndBound::getElapsedTime() {
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    elapsed_time = time_span.count();
    return elapsed_time;
}

double ParallelBranchAndBound::getTimeLimit() const {
    return time_limit;
}

void ParallelBranchAndBound::setTimeLimit(double time_sec) {
    time_limit = time_sec;
}

void ParallelBranchAndBound::enableGrid() {
    is_grid_enable = true;
}

void ParallelBranchAndBound::enableSortingNodes() {
    is_sorting_enable = true;
}

void ParallelBranchAndBound::enablePriorityQueue() {
    is_priority_enable = true;
}

bool ParallelBranchAndBound::isGridEnable() const {
    return is_grid_enable;
}

bool ParallelBranchAndBound::isSortingEnable() const {
    return is_sorting_enable;
}

bool ParallelBranchAndBound::isPriorityEnable() const {
    return is_priority_enable;
}

void ParallelBranchAndBound::setBranchInitPayload(const Payload_interval& payload) {
    branch_init(payload);
}

void ParallelBranchAndBound::setBranchInit(const Interval &interval) {
    branch_init = interval;
}

void ParallelBranchAndBound::setNumberOfThreads(int n_number_of_threads) {
    number_of_bbs = n_number_of_threads;
}

void ParallelBranchAndBound::setParetoFrontFile(const char outputFile[255]) {
    std::strcpy(pareto_file, outputFile);
}

void ParallelBranchAndBound::setSummarizeFile(const char outputFile[255]) {
    std::strcpy(summarize_file, outputFile);
}

void ParallelBranchAndBound::setPoolFile(const char outputFile[255]) {
    std::strcpy(pool_file, outputFile);
}


int ParallelBranchAndBound::getRank() const {
    return rank;
}

void ParallelBranchAndBound::saveSummarize() {

    time_t now = time(0);
    tm *ltm = localtime(&now);

    std::ostringstream stream_console;

    printf("[Worker-%03d:B&B-%03d] Saving summarize in file %s\n", 0, 0, summarize_file);

    stream_console << "---Summarize---";
    stream_console << 1900 + ltm->tm_year << '-' << ltm->tm_mon << '-' << ltm->tm_mday << ' ' << std::endl;
    stream_console << ltm->tm_hour << ':' << ltm->tm_min << ':' << ltm->tm_sec << std::endl;

    stream_console << "Number of threads:   " << number_of_bbs << endl;
    stream_console << "Sharing size:        " << problem.getNumberOfVariables() * size_to_share << endl;
    stream_console << "Deep limit to share: " << deep_limit_share << endl;
    stream_console << "Pareto front size:   " << sharedParetoFront.getSize() << endl;
    stream_console << "Total nodes:         " << number_of_nodes << endl;
    stream_console << "Eliminated nodes:    " << number_of_nodes - number_of_nodes_explored << endl;
    stream_console << "Explored nodes:      " << number_of_nodes_explored << endl;
    stream_console << "Created nodes:       " << number_of_nodes_created << endl;
    stream_console << "Pruned nodes:        " << number_of_nodes_pruned << endl;
    stream_console << "Leaves reached:      " << number_of_reached_leaves << endl;
    stream_console << "Calls to branching:  " << number_of_calls_to_branch << endl;
    stream_console << "Calls to prune:      " << number_of_calls_to_prune << endl;
    stream_console << "Updates in PF:       " << number_of_updates_in_lower_bound << endl;
    stream_console << "Shared work:         " << number_of_shared_works << endl;
    stream_console << "Total time:          " << elapsed_time << endl;
    stream_console << "Grid data:\n";
    stream_console << "\tnon-dominated:     \t" << number_of_non_dominated_buckets << endl;
    stream_console << "\tdominated:         \t" << number_of_dominated_buckets << endl;
    stream_console << "\tunexplored:        \t" << number_of_unexplored_buckets << endl;
    stream_console << "The pareto front found is:" << endl;

    stream_console << pareto_front;
    std::cout << stream_console.str();

    std::ofstream myfile(summarize_file);
    if (myfile.is_open()) {
        myfile << stream_console.str();
        myfile.close();
    } else
        printf("[Worker-%03d:B&B-%03d] Unable to write on summarize file: %s\n", 0, 0, summarize_file);

}

void ParallelBranchAndBound::saveParetoFront() {

    std::ofstream myfile(pareto_file);
    if (myfile.is_open()) {
        printf("[Worker-%03d:B&B-%03d] Saving in file...\n", 0, 0);

        myfile << pareto_front;

        myfile.close();
    } else
        printf("[Worker-%03d:B&B-%03d] Unable to write on pareto front file: %s\n", 0, 0, pareto_file);
}

void ParallelBranchAndBound::saveParetoFrontInLaTex() {

}
