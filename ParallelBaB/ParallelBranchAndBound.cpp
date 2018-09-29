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
    sharedParetoFront.print();

    vector<Solution> v_pf = sharedParetoFront.getVector();
    for (auto solution = v_pf.begin(); solution != v_pf.end(); ++solution)
        pareto_front.push_back(*solution);

    saveSummarize();
    saveParetoFront();

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
    problem.getSolutionWithLowerBoundInObj(1, temp);
    sharedParetoFront.push_back(temp);
    problem.updateBestSolutionInObjectiveWith(1, temp_1);

    Solution temp_2(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.getSolutionWithLowerBoundInObj(2, temp_2);
    sharedParetoFront.push_back(temp_2);

    problem.updateBestBoundsWith(temp_1);
    problem.updateBestBoundsWith(temp);
    problem.updateBestBoundsWith(temp_2);

    std::cout << temp_1 << temp << temp_2;
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

    VRPTWdata fjssp_data (problem.getNumberOfCustomers(), problem.getMaxNumberOfVehicles(), problem.getMaxVehicleCapacity());

    for (int row = 0; row <= branch_to_split.getBuildUpTo(); ++row) {
        code = branch_to_split.getValueAt(row);
        incumbent_s.setVariable(row, code);
        problem.evaluateDynamic(incumbent_s, fjssp_data, row);
    }

    for (int element = 1; element <= num_elements; ++element)
        if (fjssp_data.getTimesThatElementAppears(element) < problem.getTimesThatValueCanBeRepeated(element)) {

            toAdd = element;
            incumbent_s.setVariable(split_level, toAdd);
            problem.evaluateDynamic(incumbent_s, fjssp_data, split_level);

            number_of_nodes_explored++;
            if (fjssp_data.isFeasible() && sharedParetoFront.produceImprovement(incumbent_s)) {
                branch_to_split.setValueAt(split_level, toAdd);

                sharedPool.push(branch_to_split);
                branch_to_split.removeLastValue();
                nodes_created++;

                number_of_shared_works++;
                number_of_nodes_created++;
            } else
                number_of_nodes_pruned++;

            problem.evaluateRemoveDynamic(incumbent_s, fjssp_data, split_level);
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

int ParallelBranchAndBound::getRank() const {
    return rank;
}

void ParallelBranchAndBound::saveSummarize() {

    printf("[Worker-%03d:B&B-%03d] ---Summarize---\n", 0, 0);
    printf("Pareto front size:   %ld\n", sharedParetoFront.getSize());
    printf("Total time:          %f\n",  elapsed_time);
    printf("Total nodes:         %ld\n", number_of_nodes);
    printf("Eliminated nodes:    %ld\n", number_of_nodes - number_of_nodes_explored);
    printf("Explored nodes:      %ld\n", number_of_nodes_explored);
    printf("Created nodes:       %ld\n", number_of_nodes_created);
    printf("Pruned nodes:        %ld\n", number_of_nodes_pruned);
    printf("Leaves reached:      %ld\n", number_of_reached_leaves);
    printf("Calls to branching:  %ld\n", number_of_calls_to_branch);
    printf("Calls to prune:      %ld\n", number_of_calls_to_prune);
    printf("Updates in PF:       %ld\n", number_of_updates_in_lower_bound);
    printf("Shared work: %ld\n", number_of_shared_works);
    printf("Grid data:\n");
    //myfile << "\tdimension:         \t" << paretoContainer.getCols() << " x " << paretoContainer.getRows() << endl;
    printf("\tnon-dominated:\t%ld\n", number_of_non_dominated_buckets);
    printf("\tdominated:\t%ld\n", number_of_dominated_buckets);
    printf("\tunexplored:\t%ld\n", number_of_unexplored_buckets);

    std::ofstream myfile(summarize_file);
    if (myfile.is_open()) {
        printf("[Worker-%03d:B&B-%03d] Saving summarize in file %s\n", 0, 0, summarize_file);

        myfile << "---Summarize---\n";
        myfile << "Number of threads:   " << number_of_bbs << endl;
        myfile << "Sharing size:        " << problem.getNumberOfVariables() * size_to_share << endl;
        myfile << "Deep limit to share: " << deep_limit_share << endl;
        myfile << "Pareto front size:   " << sharedParetoFront.getSize() << endl;
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
        //myfile << "\tdimension:         \t" << paretoContainer.getCols() << " x " << paretoContainer.getRows() << endl;
        myfile << "\tnon-dominated:     \t" << number_of_non_dominated_buckets << endl;
        myfile << "\tdominated:         \t" << number_of_dominated_buckets << endl;
        myfile << "\tunexplored:        \t" << number_of_unexplored_buckets << endl;
        myfile << "The pareto front found is: \n";

        int numberOfObjectives = problem.getNumberOfObjectives();
        int numberOfVariables = problem.getNumberOfVariables();

        int nObj = 0;
        int nVar = 0;

        int counterSolutions = 0;
        for (auto it = pareto_front.begin(); it != pareto_front.end(); ++it) {

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
        printf("[Worker-%03d:B&B-%03d] Unable to write on summarize file: %s\n", 0, 0, summarize_file);

}

void ParallelBranchAndBound::saveParetoFront() {

    std::ofstream myfile(pareto_file);
    if (myfile.is_open()) {
        printf("[Worker-%03d:B&B-%03d] Saving in file...\n", 0, 0);
        int numberOfObjectives = problem.getNumberOfObjectives();


        for (auto it = pareto_front.begin(); it != pareto_front.end(); ++it) {
            for (int nObj = 0; nObj < numberOfObjectives - 1; ++nObj)
                myfile << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill(' ') << (*it).getObjective(nObj) << " ";
            myfile << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill(' ') << (*it).getObjective(numberOfObjectives - 1) << "\n";
        }

        myfile.close();
    } else
        printf("[Worker-%03d:B&B-%03d] Unable to write on pareto front file: %s\n", 0, 0, pareto_file);
}
