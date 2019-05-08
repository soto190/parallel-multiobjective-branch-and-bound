//
//  ParallelBranchAndBound.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 06/02/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//
/** Dummy commit**/
#include "ParallelBranchAndBound.hpp"

ParallelBranchAndBound::ParallelBranchAndBound(int rank, int n_threads, const ProblemFJSSP& problem):
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

    time_start = std::chrono::high_resolution_clock::now();
    time_end = std::chrono::high_resolution_clock::now();
}

ParallelBranchAndBound::~ParallelBranchAndBound() {
    
}

tbb::task * ParallelBranchAndBound::execute() {

    initSharedParetoFront();
    initSharedPool(branch_init);

    set_ref_count(number_of_bbs + 1);
    
    tbb::task_list tl; /** When task_list is destroyed it doesn't calls the destructor. **/
    vector<BranchAndBound *> bb_threads;
    sleeping_bb = 0;
    int n_bb = 0;
    time_start = std::chrono::high_resolution_clock::now();
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
        number_of_reached_leaves += bb_in->getNumberOfReachedLeaves();
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
    /*Solution temp_1(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.createDefaultSolution(temp_1);
    sharedParetoFront.push_back(temp_1);
    problem.updateBestMakespanSolutionWith(temp_1);

    Solution temp(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.getSolutionWithLowerBoundInObj(1, temp);
    sharedParetoFront.push_back(temp);
    problem.updateBestMaxWorkloadSolutionWith(temp);

    Solution temp_2(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.getSolutionWithLowerBoundInObj(2, temp_2);
    sharedParetoFront.push_back(temp_2);

    problem.updateBestBoundsWith(temp_1);
    problem.updateBestBoundsWith(temp);
    problem.updateBestBoundsWith(temp_2);

    NSGA_II nsgaii_algorithm(problem);
    nsgaii_algorithm.setSampleSolution(temp_1);
    nsgaii_algorithm.setCrossoverRate(0.90);
    nsgaii_algorithm.setMutationRate(0.10);
    nsgaii_algorithm.setMaxPopulationSize(100);
    nsgaii_algorithm.setMaxNumberOfGenerations(300);
    ParetoFront algorithms_pf = nsgaii_algorithm.solve();
    for (const auto& n_sol : algorithms_pf) {
        problem.updateBestBoundsWith(n_sol);
        sharedParetoFront.push_back(n_sol);
    }
    //std::cout << temp_1 << temp << temp_2 << std::endl;
    std::cout << "NSGA-II: " << std::endl << algorithms_pf << std::endl;
*/
    sharedParetoFront.print();
}

int ParallelBranchAndBound::initSharedPool(const Interval & branch_init) {

    sharedPool.setSizeEmptying((unsigned long) (number_of_bbs * 2)); /** If the global pool reach this size then the B&B starts sending part of their work to the global pool. **/

    Interval branch_to_split(branch_init);
    Solution incumbent_s(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.createDefaultSolution(incumbent_s);

    int split_level = branch_to_split.getBuildUpTo() + 1;
    int nodes_created = 0;
    int num_elements = problem.getTotalElements();
    int code = 0;
    int toAdd = 0;

    FJSSPdata fjssp_data (problem.getNumberOfJobs(), problem.getNumberOfOperations(), problem.getNumberOfMachines()); /** This function call is not necesary because the structurs are empty.**/
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

double ParallelBranchAndBound::getElapsedTime() {
    time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
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

void ParallelBranchAndBound::setOutputPath(const std::string outputPath) {
    output_path = outputPath;
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

    time_t now = time(0);
    tm *ltm = localtime(&now);

    std::ostringstream stream_console;

    printf("[Worker-%03d:B&B-%03d] Saving summarize in file %s\n", 0, 0, summarize_file);

    stream_console << "---Summarize---";
    stream_console << 1900 + ltm->tm_year << '-' << ltm->tm_mon << '-' << ltm->tm_mday << ' ' << std::endl;
    stream_console << ltm->tm_hour << ':' << ltm->tm_min << ':' << ltm->tm_sec << std::endl;
    stream_console << "Problem name:        " << problem.getName() << std::endl;
    stream_console << "Number of threads:   " << number_of_bbs << std::endl;
    stream_console << "Sub-problems to share:" << problem.getNumberOfVariables() * size_to_share << std::endl;
    stream_console << "Deep limit to share: " << deep_limit_share << std::endl;
    stream_console << "Pareto front size:   " << sharedParetoFront.getSize() << std::endl;
    stream_console << "Total nodes:         " << number_of_nodes << std::endl;
    stream_console << "Eliminated nodes:    " << number_of_nodes - number_of_nodes_explored << std::endl;
    stream_console << "Evaluated nodes:     " << number_of_nodes_explored << std::endl;
    stream_console << "Branched nodes:       " << number_of_nodes_created << std::endl;
    stream_console << "Fathomed nodes:      " << number_of_nodes_pruned << std::endl;
    stream_console << "Leaves reached:      " << number_of_reached_leaves << std::endl;
    stream_console << "Calls to branching:  " << number_of_calls_to_branch << std::endl;
    stream_console << "Calls to prune:      " << number_of_calls_to_prune << std::endl;
    stream_console << "Updates in PF:       " << number_of_updates_in_lower_bound << std::endl;
    stream_console << "Shared work:         " << number_of_shared_works << std::endl;
    stream_console << "Total time:          " << elapsed_time << std::endl;
    stream_console << "Grid data:\n";
    stream_console << "\tnon-dominated:     \t" << number_of_non_dominated_buckets << std::endl;
    stream_console << "\tdominated:         \t" << number_of_dominated_buckets << std::endl;
    stream_console << "\tunexplored:        \t" << number_of_unexplored_buckets << std::endl;
    stream_console << "The pareto front found is:" << std::endl;

    stream_console << pareto_front;
    std::cout << stream_console.str();

    std::ofstream myfile(summarize_file);
    if (myfile.is_open()) {
        myfile << stream_console.str();
        myfile.close();
    } else
        printf("[Worker-%03d:B&B-%03d] Unable to write on summarize file: %s\n", 0, 0, summarize_file);

    
    std::string summarize_file = output_path +  "Summarize.csv";
    std::cout << "Summarize file is: " << summarize_file << std::endl;
    std::ofstream output_summarize(summarize_file, std::ios_base::app);

    if (output_summarize.is_open()) {
        /** Header of the output:
         * Date and time, Instance, threads, grid, sorting, priority queue, time(sec), evaluated, branched, fathomed, leaves, updates, shared sub-problems, %branched, %fathomed
         **/
        std::ostringstream output_stream_row;
        output_stream_row << 1900 + ltm->tm_year << '-';
        output_stream_row << ltm->tm_mon << '-';
        output_stream_row << ltm->tm_mday << ' ';
        output_stream_row << ltm->tm_hour << ':';
        output_stream_row << ltm->tm_min << ':';
        output_stream_row << ltm->tm_sec << ',';

        output_stream_row << problem.getName() << ',';
        output_stream_row << number_of_bbs << ',';
        output_stream_row << isGridEnable() << ',';
        output_stream_row << isSortingEnable() << ',';
        output_stream_row << isPriorityEnable() << ',';
        output_stream_row << elapsed_time << ',';
        output_stream_row << number_of_nodes_explored << ',';
        output_stream_row << number_of_calls_to_branch << ',';
        output_stream_row << number_of_nodes_pruned << ',';
        output_stream_row << number_of_reached_leaves << ',';
        output_stream_row << number_of_updates_in_lower_bound << ',';
        output_stream_row << number_of_shared_works << ',';

        output_stream_row << ((number_of_calls_to_branch * 1.0) / number_of_nodes_explored * 100.0) << ',';
        output_stream_row << ((number_of_nodes_pruned * 1.0) / number_of_nodes_explored * 100.0) << std::endl;

        output_summarize << output_stream_row.str(); /** For writing in one flush to the output file.**/
        output_summarize.close();
    } else
        std::cout << "Unable to write data to file " << "Summarize.csv" << std::endl;

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
