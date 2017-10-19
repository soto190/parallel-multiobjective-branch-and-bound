/*
 * MasterWorker.cpp
 *
 *  Created on: 22/09/2017
 *      Author: carlossoto
 *  This class is inspired from:
 *  - https://github.com/coolis/MPI_Master_Worker
 *  - https://www.hpc.cam.ac.uk/using-clusters/compiling-and-development/parallel-programming-mpi-example
 *
 */

#include "MasterWorkerPBB.hpp"

MasterWorkerPBB::MasterWorkerPBB() {
    rank = 0;
    n_workers = 0;
    threads_per_node = 0;
    datatype_interval = 0;
    datatype_problem = 0;
    datatype_solution = 0;
}

MasterWorkerPBB::MasterWorkerPBB(int num_nodes, int num_threads, const char file_instance[]) :
n_workers(num_nodes - 1),
threads_per_node(num_threads) {
    rank = 0;
    std::strcpy(file, file_instance);
    datatype_interval = 0;
    datatype_problem = 0;
    datatype_solution = 0;
}

MasterWorkerPBB::~MasterWorkerPBB() {
}

tbb::task * MasterWorkerPBB::execute() {
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    rank = MPI::COMM_WORLD.Get_rank();
    n_workers = MPI::COMM_WORLD.Get_size() - 1;

    run(); /** Initializes the Payloads. **/
    
    if (isMaster())
        runMasterProcess();
    else
        runWorkerProcess();
    
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    
    if (isMaster())
        printf("[Master] Elapsed time: %6.6f\n", time_span.count());
    else
        printf("[WorkerPBB-%03d] Elapsed time: %6.6f\n", rank, time_span.count());
    /** Wait for all the workers to save the Pareto front. **/
    /*MPI::COMM_WORLD.Barrier();
    if (isMaster()) {
     // save pareto front and data.
    }*/
    
    MPI_Type_free(&datatype_problem);
    MPI_Type_free(&datatype_interval);
    MPI_Type_free(&datatype_solution);
    
    return NULL;
}

void MasterWorkerPBB::run() {
    if (isMaster())
        loadInstance(payload_problem, file);
    
    preparePayloadProblemPart1(payload_problem, datatype_problem);
    MPI::COMM_WORLD.Bcast(&payload_problem, 1, datatype_problem, MASTER_RANK); /* The MASTER_NODE broadcast the part 1 of payload and the slaves nodes receives it. */
    
    unpack_payload_part1(payload_problem, payload_interval, payload_solution); /** Each node initialize their structures. The MASTER_NODE has initialized them from the load_instance. **/
    preparePayloadProblemPart2(payload_problem, datatype_problem); /** Prepares the second part (processing times).**/
    MPI::COMM_WORLD.Bcast(&payload_problem, 1, datatype_problem, MASTER_RANK);
    
    unpack_payload_part2(payload_problem);
    preparePayloadInterval(payload_interval, datatype_interval); /** Each node prepares the payload for receiving intervals. **/
    preparePayloadSolution(payload_solution, datatype_solution);
    //preparePayloadSolutions(payload_solutions, datatyple_solutions);
}

/**
 * Seeds each worker with an Interval.
 */
void MasterWorkerPBB::runMasterProcess() {
    int worker_dest = 1;
    for (int element = 1; element < payload_interval.max_size; ++element)
        payload_interval.interval[element] = -1;
    
    int max_number_of_mappings = payload_problem.n_jobs * payload_problem.n_machines;
    int n_intervals = 0;
    
    payload_interval.priority = 0;
    payload_interval.deep = 0;
    payload_interval.build_up_to = 0;
    payload_interval.distance[0] = 0.9;
    payload_interval.distance[1] = 0.9;
    
    /** Seeding the workers with initial subproblems/intervals. **/
    for (worker_dest = 1; worker_dest <= n_workers; worker_dest++) {
        
        payload_interval.interval[0] = n_intervals; /** The first map of the interval. **/
        
        printf("[Master] Sending to Worker-%03d.\n", worker_dest);
        
        printPayloadInterval();
        MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, worker_dest, TAG_INTERVAL);
        
        n_intervals++;
        if (n_intervals == max_number_of_mappings)
            worker_dest = n_workers;
    }
    
    int sleeping_workers = 0;
    printf("[Master] Sleeping_workers: %3d.\n", (int) sleeping_workers);
    vector<Solution> new_solutions;
    Solution new_sol(2, payload_interval.max_size);
    
    while (sleeping_workers < n_workers) {
        printf("[Master] Waiting for request.\n");
        MPI_Recv(&payload_interval, 1, datatype_interval, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        switch (status.MPI_TAG) {
            case TAG_REQUEST_MORE_WORK:
                if (n_intervals < max_number_of_mappings) {
                    payload_interval.interval[0] = n_intervals;
                    
                    printf("[Master] Sending interval number %d to Worker-%03d.\n", n_intervals, status.MPI_SOURCE);
                    printPayloadInterval();
                    
                    MPI_Send(&payload_interval, 1, datatype_interval, status.MPI_SOURCE, TAG_INTERVAL, MPI_COMM_WORLD);
                    n_intervals++;
                }else{
                    /** TODO: Starts a process to request work from other nodes. **/
                    
                    /** TODO: If the other nodes doesnt have enough work to share then send a stop signal. **/
                    printf("[Master] Sending no more work signal to Worker-%03d.\n", status.MPI_SOURCE);
                    MPI_Send(&payload_interval, 1, datatype_interval, status.MPI_SOURCE, TAG_NO_MORE_WORK, MPI_COMM_WORLD);
                    sleeping_workers++;
                }
                break;
                
            case TAG_SOLUTION:
                printf("[Master] Receiving solution from Worker-%03d.\n", status.MPI_SOURCE);

                new_sol.setObjective(0, payload_interval.distance[0]);
                new_sol.setObjective(1, payload_interval.distance[1]);
                
                for (int sol = 0; sol < new_sol.getNumberOfVariables(); ++sol)
                    new_sol.setVariable(sol, payload_interval.interval[sol]);
                
                new_solutions.push_back(new_sol);
                
            default:
                break;
        }
    }
    printf("[Master] Number of solutions received: %03lu\n", new_solutions.size());
    printf("[Master] No more work.\n");
    
    /** TODO: Collects the final data. **/
}

/**
 * The worker receives the seeding then initalize all the necessary structures and creates in each thread one B&B.
 *
 */
void MasterWorkerPBB::runWorkerProcess() {
    
    int source = MASTER_RANK;
    sleeping_bb = 0;
    there_is_more_work = 1;
    problem.loadInstancePayload(payload_problem);
    
    printf("[WorkerPBB-%03d] Waiting for interval.\n", rank);
    MPI_Recv(&payload_interval, 1, datatype_interval, source, TAG_INTERVAL, MPI_COMM_WORLD, &status);
    branch_init(payload_interval);
    
    globalPool.setSizeEmptying((unsigned long) (threads_per_node * 2)); /** If the global pool reach this size then the B&B tasks starts sending part of their work to the global pool. **/
    
    Solution solution (problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.createDefaultSolution(solution);
    
    paretoContainer(25, 25, solution.getObjective(0), solution.getObjective(1), problem.getLowerBoundInObj(0), problem.getLowerBoundInObj(1));
    
    BranchAndBound BB_container(rank, 0, problem, branch_init);
    int branches_created = BB_container.initGlobalPoolWithInterval(branch_init);
    BB_container.getRank();
    printf("[WorkerPBB-%03d] Pool size: %3lu %3d\n", rank, globalPool.unsafe_size(), branches_created);
    
    set_ref_count(threads_per_node + 1);
    
    tbb::task_list tl; /** When task_list is destroyed it doesn't calls the destructor. **/
    vector<BranchAndBound *> bb_threads;
    int n_bb = 0;
    while (n_bb++ < threads_per_node) {
        BranchAndBound * BaB_task = new (tbb::task::allocate_child()) BranchAndBound(rank, n_bb, problem, branch_init);
        bb_threads.push_back(BaB_task);
        tl.push_back(*BaB_task);
    }
    
    printf("[WorkerPBB-%03d] Spawning the swarm...\n", rank);
    tbb::task::spawn(tl);
    /**TODO: Spawn the thread with the metaheuristic. **/
    int branches_in_loop = 0;
    
    vector<Solution> paretoFront;// = BB_container.getParetoFront();
    Solution temp;
    
    while (there_is_more_work == 1) {
        if (globalPool.isEmptying()) {
            MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, MASTER_RANK, TAG_REQUEST_MORE_WORK);
            MPI_Recv(&payload_interval, 1, datatype_interval, source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("[WorkerPBB-%03d] Waiting for more work.\n", rank);
            switch (status.MPI_TAG) {
                    
                case TAG_INTERVAL:
                    printf("[WorkerPBB-%03d] Receiving more work.\n", rank);
                    printPayloadInterval();
                    branch_init(payload_interval);
                    branches_in_loop = splitInterval(branch_init); /** Splits and push to GlobalPool. **/
                    branches_created += branches_in_loop;
                    printf("[WorkerPBB-%03d] Interval divided in %3d sub-intervals.\n", rank, branches_in_loop);
                    
                    /** TODO: Share the solutions found with other nodes. **/
                    paretoFront = BB_container.getParetoFront();
                    payload_interval.build_up_to = (int) paretoFront.size();
                    printf("[WorkerPBB-%03d] Sending solutions to Master %3d.\n", rank, (int) paretoFront.size());

                    for (int n_sol = 0; n_sol < paretoFront.size(); ++n_sol) {
                        temp =  paretoFront.at(n_sol);
                        payload_interval.distance[0] = temp.getObjective(0);
                        payload_interval.distance[1] = temp.getObjective(1);
                        
                        for (int n_var = 0; n_var < temp.getNumberOfVariables(); ++n_var)
                            payload_interval.interval[n_var] = temp.getVariable(n_var);
                        
                        MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, MASTER_RANK, TAG_SOLUTION);
                    }
                    break;
                    
                case TAG_NO_MORE_WORK:  /** There is no more work.**/
                    there_is_more_work = 0;
                    printf("[WorkerPBB-%03d] Waiting for %03d B&B to end.\n", rank, threads_per_node - (int) sleeping_bb);
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    while (sleeping_bb < threads_per_node) {
        /** Waiting for working B&Bs to end. **/
    }
    
    /** Recollects the data. **/
    BB_container.getTotalTime();
    BranchAndBound* bb_in;
    while (!bb_threads.empty()) {
        
        bb_in = bb_threads.back();
        bb_threads.pop_back();
        
        BB_container.increaseNumberOfExploredNodes(bb_in->getNumberOfExploredNodes());
        BB_container.increaseNumberOfCallsToBranch(bb_in->getNumberOfCallsToBranch());
        BB_container.increaseNumberOfBranches(bb_in->getNumberOfBranches());
        BB_container.increaseNumberOfCallsToPrune(bb_in->getNumberOfCallsToPrune());
        BB_container.increaseNumberOfPrunedNodes(bb_in->getNumberOfPrunedNodes());
        BB_container.increaseNumberOfReachedLeaves(bb_in->getNumberOfReachedLeaves());
        BB_container.increaseNumberOfUpdatesInLowerBound(bb_in->getNumberOfUpdatesInLowerBound());
        BB_container.increaseSharedWork(bb_in->getSharedWork());
    }
    
    //    BB_container.setParetoFrontFile(outputParetoFile);
    //    BB_container.setSummarizeFile(summarizeFile);
    
    /** TODO: Send solutions found to Master node. **/
    BB_container.getParetoFront();
    BB_container.printParetoFront(0);
    //    BB_container.saveParetoFront();
    //    BB_container.saveSummarize();
    bb_threads.clear();
    printf("[WorkerPBB-%03d] Data swarm recollected and saved.\n", rank);
    printf("[WorkerPBB-%03d] Parallel Branch And Bound ended.\n", rank);
}

void MasterWorkerPBB::loadInstance(Payload_problem_fjssp& problem, const char *filePath) {
    char extension[4];
    problem.n_objectives = 2;
    problem.n_jobs = 0;
    problem.n_machines = 0;
    problem.n_operations = 0;
    
    /** Get name of the instance. **/
    std::vector<std::string> elemens;
    std::vector<std::string> name_file_ext;
    
    elemens = split(filePath, '/');
    
    unsigned long int sizeOfElems = elemens.size();
    name_file_ext = split(elemens[sizeOfElems - 1], '.');
    printf("[Master] Name: %s\n", name_file_ext[0].c_str());
    printf("[Master] File extension: %s\n", name_file_ext[1].c_str());
    std::strcpy(extension, name_file_ext[1].c_str());
    
    std::ifstream infile(filePath);
    if (infile.is_open()) {
        std::string line;
        std::getline(infile, line);
        
        elemens = split(line, ' ');
        problem.n_jobs = std::stoi(elemens.at(0));
        problem.n_machines = std::stoi(elemens.at(1));
        
        if (problem.n_jobs > 0 && problem.n_machines > 0) {
            problem.release_times = new int[problem.n_jobs];
            problem.n_operations_in_job = new int[problem.n_jobs];
            
            std::string * job_line = new std::string[problem.n_jobs];
            for (int n_job = 0; n_job < problem.n_jobs; ++n_job) {
                std::getline(infile, job_line[n_job]);
                split(job_line[n_job], ' ', elemens);
                problem.n_operations_in_job[n_job] = std::stoi(elemens.at(0));
                problem.n_operations += problem.n_operations_in_job[n_job];
                problem.release_times[n_job] = 0;
            }
            
            problem.processing_times = new int[problem.n_operations * problem.n_machines];
            int op_counter = 0;
            for (int n_job = 0; n_job < problem.n_jobs; ++n_job) {
                int token = 1;
                split(job_line[n_job], ' ', elemens);
                for (int n_op_in_job = 0; n_op_in_job < problem.n_operations_in_job[n_job]; ++n_op_in_job) {
                    int op_can_be_proc_in_n_mach = std::stoi(elemens.at(token++));
                    
                    for (int n_machine = 0; n_machine < problem.n_machines; ++n_machine)
                        problem.processing_times[op_counter * problem.n_machines + n_machine] = INF_PROC_TIME;
                    
                    for (int n_mach = 0; n_mach < op_can_be_proc_in_n_mach; ++n_mach) {
                        int machine = std::stoi(elemens.at(token++)) - 1;
                        int proc_ti = std::stoi(elemens.at(token++));
                        problem.processing_times[op_counter * problem.n_machines + machine] = proc_ti;
                    }
                    op_counter++;
                }
            }
        }
        infile.close();
    }
}

void MasterWorkerPBB::preparePayloadProblemPart1(const Payload_problem_fjssp &problem, MPI_Datatype &datatype_problem) {
    const int n_blocks = 4;
    int blocks[n_blocks] = { 1, 1, 1, 1 };
    MPI_Aint displacements_interval[n_blocks];
    MPI_Datatype types[n_blocks] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT };
    MPI_Aint addr_base, addr_n_objectives, addr_n_jobs, addr_n_operations, addr_n_machines;
    
    MPI_Get_address(&problem, &addr_base);
    MPI_Get_address(&(problem.n_objectives), &addr_n_objectives);
    MPI_Get_address(&(problem.n_jobs), &addr_n_jobs);
    MPI_Get_address(&(problem.n_operations), &addr_n_operations);
    MPI_Get_address(&(problem.n_machines), &addr_n_machines);
    
    displacements_interval[0] = static_cast<MPI_Aint>(0);
    displacements_interval[1] = addr_n_jobs - addr_base;
    displacements_interval[2] = addr_n_operations - addr_base;
    displacements_interval[3] = addr_n_machines - addr_base;
    
    MPI_Type_create_struct(n_blocks, blocks, displacements_interval, types, &datatype_problem);
    MPI_Type_commit(&datatype_problem);
}

void MasterWorkerPBB::preparePayloadProblemPart2(const Payload_problem_fjssp &problem, MPI_Datatype &datatype_problem) {
    const int n_blocks = 7;
    int blocks[n_blocks] = { 1, 1, 1, 1, problem.n_jobs, problem.n_jobs, problem.n_operations
        * problem.n_machines };
    MPI_Aint displacements_interval[n_blocks];
    MPI_Datatype types[n_blocks] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT };
    MPI_Aint addr_base, addr_n_obj, addr_n_jobs, addr_n_operations, addr_n_machines, addr_release_times,
    addr_n_oper_in_job,
    addr_processing_times;
    
    MPI_Get_address(&problem, &addr_base);
    MPI_Get_address(&(problem.n_objectives), &addr_n_obj);
    MPI_Get_address(&(problem.n_jobs), &addr_n_jobs);
    MPI_Get_address(&(problem.n_operations), &addr_n_operations);
    MPI_Get_address(&(problem.n_machines), &addr_n_machines);
    MPI_Get_address(problem.release_times, &addr_release_times);
    MPI_Get_address(problem.n_operations_in_job, &addr_n_oper_in_job);
    MPI_Get_address(problem.processing_times, &addr_processing_times);
    
    displacements_interval[0] = static_cast<MPI_Aint>(0);
    displacements_interval[1] = addr_n_jobs - addr_base;
    displacements_interval[2] = addr_n_operations - addr_base;
    displacements_interval[3] = addr_n_machines - addr_base;
    displacements_interval[4] = addr_release_times - addr_base;
    displacements_interval[5] = addr_n_oper_in_job - addr_base;
    displacements_interval[6] = addr_processing_times - addr_base;
    
    MPI_Type_create_struct(n_blocks, blocks, displacements_interval, types, &datatype_problem);
    MPI_Type_commit(&datatype_problem);
}

void MasterWorkerPBB::preparePayloadInterval(const Payload_interval &interval, MPI_Datatype &datatype_interval) {
    const int n_blocks = 6;
    int blocks[n_blocks] = { 1, 1, 1, 1, 2, interval.max_size };
    MPI_Aint displacements_interval[n_blocks];
    MPI_Datatype types[n_blocks] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_FLOAT, MPI_INT };
    MPI_Aint addr_base, addr_priority, addr_deep, addr_build, addr_max_size, addr_distance, addr_interval;
    
    MPI_Get_address(&interval, &addr_base);
    MPI_Get_address(&(interval.priority), &addr_priority);
    MPI_Get_address(&(interval.deep), &addr_deep);
    MPI_Get_address(&(interval.build_up_to), &addr_build);
    MPI_Get_address(&(interval.max_size), &addr_max_size);
    MPI_Get_address(interval.distance, &addr_distance);
    MPI_Get_address(interval.interval, &addr_interval);
    
    displacements_interval[0] = static_cast<MPI_Aint>(0);
    displacements_interval[1] = addr_deep - addr_base;
    displacements_interval[2] = addr_build - addr_base;
    displacements_interval[3] = addr_max_size - addr_base;
    displacements_interval[4] = addr_distance - addr_base;
    displacements_interval[5] = addr_interval - addr_base;
    
    MPI_Type_create_struct(n_blocks, blocks, displacements_interval, types, &datatype_interval);
    MPI_Type_commit(&datatype_interval);
}
/*
void MasterWorkerPBB::preparePayloadSolutions(const Payload_interval * solutions, MPI_Datatype &datatype_solutions){
    const int n_blocks = 6;
    int blocks[n_blocks] = { 1, 1, 1, 1, 2, solutions[0].max_size };
    MPI_Aint displacements_interval[n_blocks];
    MPI_Datatype types[n_blocks] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_FLOAT, MPI_INT };
    MPI_Aint addr_base, addr_priority, addr_deep, addr_build, addr_max_size, addr_distance, addr_interval;
    
    MPI_Get_address(&solutions, &addr_base);
    MPI_Get_address(&(solutions[0].priority), &addr_priority);
    MPI_Get_address(&(solutions[0].deep), &addr_deep);
    MPI_Get_address(&(solutions[0].build_up_to), &addr_build);
    MPI_Get_address(&(solutions[0].max_size), &addr_max_size);
    MPI_Get_address(solutions[0].distance, &addr_distance);
    MPI_Get_address(solutions[0].interval, &addr_interval);
    
    displacements_interval[0] = static_cast<MPI_Aint>(0);
    displacements_interval[1] = addr_deep - addr_base;
    displacements_interval[2] = addr_build - addr_base;
    displacements_interval[3] = addr_max_size - addr_base;
    displacements_interval[4] = addr_distance - addr_base;
    displacements_interval[5] = addr_interval - addr_base;
    
    MPI_Type_create_struct(n_blocks, blocks, displacements_interval, types, &datatype_interval);
    MPI_Type_commit(&datatype_interval);
}
*/
void MasterWorkerPBB::preparePayloadSolution(const Payload_solution &solution, MPI_Datatype &datatype_solution) {
    const int n_blocks = 5;
    int blocks[n_blocks] = { 1, 1, 1, solution.n_objectives, solution.n_variables };
    MPI_Aint displacements_interval[n_blocks];
    MPI_Datatype types[n_blocks] = { MPI_INT, MPI_INT, MPI_INT, MPI_DOUBLE, MPI_INT };
    MPI_Aint addr_base, addr_n_obj, addr_n_var, addr_build, addr_obj, addr_var;
    
    MPI_Get_address(&solution, &addr_base);
    MPI_Get_address(&(solution.n_objectives), &addr_n_obj);
    MPI_Get_address(&(solution.n_variables), &addr_n_var);
    MPI_Get_address(&(solution.build_up_to), &addr_build);
    MPI_Get_address(solution.objective, &addr_obj);
    MPI_Get_address(solution.variable, &addr_var);
    
    displacements_interval[0] = static_cast<MPI_Aint>(0);
    displacements_interval[1] = addr_n_var - addr_base;
    displacements_interval[2] = addr_build - addr_base;
    displacements_interval[3] = addr_obj - addr_base;
    displacements_interval[4] = addr_var - addr_base;
    
    MPI_Type_create_struct(n_blocks, blocks, displacements_interval, types, &datatype_solution);
    MPI_Type_commit(&datatype_solution);
}

void MasterWorkerPBB::unpack_payload_part1(Payload_problem_fjssp& problem, Payload_interval& interval, Payload_solution & solution) {
    if (isWorker()) { /** Only the workers do this because the Master node initialized them in the loadInstance function. **/
        problem.release_times = new int[problem.n_jobs];
        problem.n_operations_in_job = new int[problem.n_jobs];
        problem.processing_times = new int[problem.n_operations * problem.n_machines];
    }
    
    /** All nodes initializes this. **/
    interval.max_size = problem.n_operations;
    interval.interval = new int[problem.n_operations];
    
    solution.n_objectives = problem.n_objectives;
    solution.n_variables = problem.n_operations;
    solution.build_up_to = -1;
    solution.objective =  new double[problem.n_objectives];
    solution.variable = new int[problem.n_operations];
    
    /** Calling the payload_solutions directly. **/
    /*for (int n_sol = 0; n_sol < 10; n_sol++) {
        payload_solutions[n_sol].distance[0] = 0;
        payload_solutions[n_sol].distance[1] = 0;
        payload_solutions[n_sol].max_size = problem.n_operations;
        payload_solutions[n_sol].interval = new int[problem.n_operations];
    }*/
}

void MasterWorkerPBB::unpack_payload_part2(Payload_problem_fjssp& problem) {

    printf("[Node-%02d] Jobs: %d Operations: %d Machines: %d\n", rank, problem.n_jobs, problem.n_operations,
           problem.n_machines);
    
    /*
    printf("[Node%02d] ", rank);
    for (int n_job = 0; n_job < problem.n_jobs; ++n_job)
        printf("%d ", problem.n_operations_in_job[n_job]);
    printf("\n");
    
    printf("[Node%d] ", rank);
    for (int n_job = 0; n_job < problem.n_jobs; ++n_job)
        printf("%d ", problem.release_times[n_job]);
    printf("\n");
    
    printf("[Node%d] ", rank);
    for (int proc_t = 0; proc_t < problem.n_operations * problem.n_machines; ++proc_t)
        printf("%d ", problem.processing_times[proc_t]);
    printf("\n");
    */
}

int MasterWorkerPBB::getRank() const{
    return rank;
}

int MasterWorkerPBB::getSizeWorkers() const{
    return n_workers;
}

int MasterWorkerPBB::isMaster() const{
    if (rank == MASTER_RANK)
        return 1;
    return 0;
}

int MasterWorkerPBB::isWorker() const{
    if (rank > MASTER_RANK)
        return 1;
    return 0;
}

int MasterWorkerPBB::splitInterval(Interval& branch_to_split){
    Solution temp(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    int row = 0;
    int split_level = branch_to_split.getBuildUpTo() + 1;
    int branches_created = 0;
    int num_elements = problem.getTotalElements();
    int map = 0;
    int element = 0;
    int machine = 0;
    int toAdd = 0;
    
    float distance_error[2];
    
    FJSSPdata fjssp_data(problem.getNumberOfJobs(), problem.getNumberOfOperations(), problem.getNumberOfMachines());
    fjssp_data.reset(); /** This function call is not necesary because the structurs are empty.**/
    
    fjssp_data.setMinTotalWorkload(problem.getSumOfMinPij());
    for (int m = 0; m < problem.getNumberOfMachines(); ++m){
        fjssp_data.setBestWorkloadInMachine(m, problem.getBestWorkload(m));
        fjssp_data.setTempBestWorkloadInMachine(m, problem.getBestWorkload(m));
    }
    
    for (row = 0; row <= branch_to_split.getBuildUpTo(); ++row) {
        map = branch_to_split.getValueAt(row);
        temp.setVariable(row, map);
        problem.evaluateDynamic(temp, fjssp_data, row);
    }
    
    for (element = 0; element < num_elements; ++element)
        if (fjssp_data.getNumberOfOperationsAllocatedInJob(element) < problem.getTimesValueIsRepeated(element))
            for (machine = 0; machine < problem.getNumberOfMachines(); ++machine) {
                
                toAdd = problem.getCodeMap(element, machine);
                temp.setVariable(split_level, toAdd);
                problem.evaluateDynamic(temp, fjssp_data, split_level);
                
                if (paretoContainer.improvesTheGrid(temp)) {
                    /** Gets the branch to add. */
                    branch_to_split.setValueAt(split_level, toAdd);
                    
                    distance_error[0] = (problem.getLowerBoundInObj(0) - fjssp_data.getMakespan()) / (float) problem.getLowerBoundInObj(0);
                    distance_error[1] = (problem.getLowerBoundInObj(1) - fjssp_data.getMaxWorkload()) / (float) problem.getLowerBoundInObj(1);
                    
                    branch_to_split.setDistance(0, distance_error[0]);
                    branch_to_split.setDistance(1, distance_error[1]);
                    branch_to_split.setHighPriority();
                    /** Add it to pending intervals. **/
                    globalPool.push(branch_to_split); /** The vector adds a copy of interval. **/
                    branch_to_split.removeLastValue();
                    branches_created++;
                } else
                    problem.evaluateRemoveDynamic(temp, fjssp_data, split_level);
            }
    return branches_created;
}

void MasterWorkerPBB::printPayloadInterval() const{
    
    if (isMaster())
        printf("[Master] [ ");
    else
        printf("[WorkerPBB-%03d] [ ", rank);
    
    for (int element = 0; element < payload_interval.max_size; ++element)
        if (payload_interval.interval[element] == -1)
            printf("- ");
        else
            printf("%d ", payload_interval.interval[element]);
    printf("]\n");
}
