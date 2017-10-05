/*
 * MasterWorker.cpp
 *
 *  Created on: 22/09/2017
 *      Author: carlossoto
 */

#include "MasterWorkerPBB.hpp"

MasterWorkerPBB::MasterWorkerPBB() {
    rank = 0;
    n_workers = 0;
    threads_per_node = 0;
}

MasterWorkerPBB::MasterWorkerPBB(int num_nodes, int num_threads, const char file_instance[]) :
n_workers(num_nodes - 1),
threads_per_node(num_threads) {
    rank = 0;
    std::strcpy(file, file_instance);
}

MasterWorkerPBB::~MasterWorkerPBB() {
}

void MasterWorkerPBB::run() {
    
    if (isMaster())
        loadInstance(payload_problem, file);
    
    preparePayloadProblemPart1(payload_problem, datatype_problem);
    MPI::COMM_WORLD.Bcast(&payload_problem, 1, datatype_problem, MASTER_RANK); /* The MASTER_NODE broadcast the part 1 of payload and the slaves nodes receives it. */
    
    unpack_payload_part1(payload_problem, payload_interval); /** Each node initialize their structures. The MASTER_NODE has initialized them from the load_instance. **/
    preparePayloadProblemPart2(payload_problem, datatype_problem); /** Prepares the second part (processing times).**/
    MPI::COMM_WORLD.Bcast(&payload_problem, 1, datatype_problem, MASTER_RANK);
    
    unpack_payload_part2(payload_problem);
    preparePayloadInterval(payload_interval, datatype_interval); /** Each node prepares the payload for receiving intervals. **/
    
}

tbb::task * MasterWorkerPBB::execute() {
    rank = MPI::COMM_WORLD.Get_rank();
    n_workers = MPI::COMM_WORLD.Get_size() - 1;
    
    printf("Size world: %d\n", n_workers);
    run(); /** Call the MPI initialiation functions. **/
    
    if (isMaster())
        runMasterProcess();
    else
        runWorkerProcess();
    
    MPI_Type_free(&datatype_problem);
    MPI_Type_free(&datatype_interval);

    return NULL;
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
        
        printf("[Master] Sending: %d %d %d %d %f %f\n",
               payload_interval.priority,
               payload_interval.deep,
               payload_interval.build_up_to,
               payload_interval.max_size,
               payload_interval.distance[0],
               payload_interval.distance[1]);
        
        printf("[Master] ");
        for (int element = 0; element < payload_interval.max_size; ++element)
            printf("%d ", payload_interval.interval[element]);
        printf("\n");
        MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, worker_dest, TAG_INTERVAL);
        
        n_intervals++;
        if (n_intervals == max_number_of_mappings)
            worker_dest = n_workers;
    }
    
    /*sleeping_workers = 0;
    printf("[Master] Sleeping_workers: %3d.\n", (int) sleeping_workers);
    while (sleeping_workers < n_workers) {
        printf("[Master] Waiting for request.\n");
        MPI_Recv(&payload_interval, 1, datatype_interval, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        switch (status.MPI_TAG) {
            case TAG_REQUEST_MORE_WORK:
                if (n_intervals < max_number_of_mappings) {
                    payload_interval.interval[0] = n_intervals;
                    
                    printf("[Master] Sending [%03d]: %d %d %d %d %f %f\n", n_intervals, payload_interval.priority, payload_interval.deep, payload_interval.build_up_to, payload_interval.max_size, payload_interval.distance[0], payload_interval.distance[1]);
                    
                    printf("[Master] ");
                    for (int element = 0; element < payload_interval.max_size; ++element)
                        printf("%d ", payload_interval.interval[element]);
                    printf("\n");
                    
                    MPI_Send(&payload_interval, 1, datatype_interval, status.MPI_SOURCE, TAG_INTERVAL, MPI_COMM_WORLD);
                    n_intervals++;
                }else{
                    printf("[Interval number %3d of intervals %3d] \n", n_intervals, max_number_of_mappings);
                    MPI_Send(&payload_interval, 1, datatype_interval, status.MPI_SOURCE, TAG_NO_MORE_WORK, MPI_COMM_WORLD);
                    sleeping_workers++;
                }
                break;
                
            default:
                break;
        }
     
    }*/
    printf("[Master] No more work.\n");
}

/**
 * The worker receives the seeding then initalize all the necessary structures and creates in each thread one B&B.
 *
 */
void MasterWorkerPBB::runWorkerProcess() {
    
    int source = MASTER_RANK;
    sleeping_bb = 0;
    problem.loadInstancePayload(payload_problem);
    
    printf("[WorkerPBB-%03d] Waiting for interval.\n", rank);
    MPI_Recv(&payload_interval, 1, datatype_interval, source, TAG_INTERVAL, MPI_COMM_WORLD, &status);
    branch_init(payload_interval);
    branch_init.print();
    
//    globalPool.setSizeEmptying((unsigned long) (threads_per_node * 2)); /** If the global pool reach this size then the B&B tasks starts sending part of their work to the global pool. **/
//
//    BranchAndBound BB_container(rank, 0, problem, branch_init);
//    BB_container.initGlobalPoolWithInterval(branch_init);
//
//    printf("[WorkerPBB-%03d] Container initialized.\n", rank);
//    set_ref_count(threads_per_node + 1);
//
//    tbb::task_list tl; /** When task_list is destroyed it doesn't calls the destructor. **/
//    vector<BranchAndBound *> bb_threads;
//    int n_bb = 0;
//    while (n_bb++ < threads_per_node) {
//
//        BranchAndBound * BaB_task = new (tbb::task::allocate_child()) BranchAndBound(rank, n_bb, problem, branch_init);
//
//        bb_threads.push_back(BaB_task);
//        tl.push_back(*BaB_task);
//    }
//
//    printf("[WorkerPBB-%03d] Spawning the swarm...\n", rank);
//    tbb::task::spawn_and_wait_for_all(tl);
//
//    while (sleeping_bb < threads_per_node) {
//        if (globalPool.isEmptying()) {
//            MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, MASTER_RANK, TAG_REQUEST_MORE_WORK);
//            MPI_Recv(&payload_interval, 1, datatype_interval, source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//            switch (status.MPI_TAG) {
//                case TAG_INTERVAL:
//                    printf("[WorkerPBB-%03d] Receiving: %d %d %d %d %f %f\n",
//                           rank,
//                           payload_interval.priority,
//                           payload_interval.deep,
//                           payload_interval.build_up_to,
//                           payload_interval.max_size,
//                           payload_interval.distance[0],
//                           payload_interval.distance[1]);
//
//                    printf("[WorkerPBB-%03d] ", rank);
//                    for (int element = 0; element < payload_interval.max_size; ++element)
//                        printf("%d ", payload_interval.interval[element]);
//                    printf("\n");
//                    branch_init(payload_interval);
//                    globalPool.push(branch_init);
//
//                    break;
//
//                case TAG_NO_MORE_WORK:
//                    printf("[WorkerPBB-%03d] No more work. %03d B&B inactives.\n", rank, (int) sleeping_bb);
//                    break;
//                default:
//                    break;
//            }
//        }
//    }
//
//    /** Recollects the data. **/
//    BB_container.getTotalTime();
//    BranchAndBound* bb_in;
//    while (!bb_threads.empty()) {
//
//        bb_in = bb_threads.back();
//        bb_threads.pop_back();
//
//        BB_container.increaseNumberOfExploredNodes(bb_in->getNumberOfExploredNodes());
//        BB_container.increaseNumberOfCallsToBranch(bb_in->getNumberOfCallsToBranch());
//        BB_container.increaseNumberOfBranches(bb_in->getNumberOfBranches());
//        BB_container.increaseNumberOfCallsToPrune(bb_in->getNumberOfCallsToPrune());
//        BB_container.increaseNumberOfPrunedNodes(bb_in->getNumberOfPrunedNodes());
//        BB_container.increaseNumberOfReachedLeaves(bb_in->getNumberOfReachedLeaves());
//        BB_container.increaseNumberOfUpdatesInLowerBound(bb_in->getNumberOfUpdatesInLowerBound());
//        BB_container.increaseSharedWork(bb_in->getSharedWork());
//    }
//
//    //    BB_container.setParetoFrontFile(outputParetoFile);
//    //    BB_container.setSummarizeFile(summarizeFile);
//
//    BB_container.getParetoFront();
//    BB_container.printParetoFront(0);
//    //    BB_container.saveParetoFront();
//    //    BB_container.saveSummarize();
//    bb_threads.clear();
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
    std::vector<std::string> splited;
    
    elemens = split(filePath, '/');
    
    unsigned long int sizeOfElems = elemens.size();
    splited = split(elemens[sizeOfElems - 1], '.');
    printf("Name: %s\n", splited[0].c_str());
    printf("File extension: %s\n", splited[1].c_str());
    std::strcpy(extension, splited[1].c_str());
    
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

void MasterWorkerPBB::preparePayloadSolution(const Payload_solution &solution, MPI_Datatype &datatype_solution) {
    const int n_blocks = 6;
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

void MasterWorkerPBB::unpack_payload_part1(Payload_problem_fjssp& problem, Payload_interval& interval) {
    if (isWorker()) {
        problem.release_times = new int[problem.n_jobs];
        problem.n_operations_in_job = new int[problem.n_jobs];
        problem.processing_times = new int[problem.n_operations * problem.n_machines];
    }
    
    interval.max_size = problem.n_operations;
    interval.interval = new int[problem.n_operations];
}

void MasterWorkerPBB::unpack_payload_part2(Payload_problem_fjssp& problem) {
    printf("[%d] Jobs: %d Operations: %d Machines: %d\n", rank, problem.n_jobs, problem.n_operations,
           problem.n_machines);
    
    printf("[%d] ", rank);
    for (int n_job = 0; n_job < problem.n_jobs; ++n_job)
        printf("%d ", problem.release_times[n_job]);
    printf("\n");
    
    printf("[%d] ", rank);
    for (int n_job = 0; n_job < problem.n_jobs; ++n_job)
        printf("%d ", problem.n_operations_in_job[n_job]);
    printf("\n");
    
    printf("[%d] ", rank);
    for (int proc_t = 0; proc_t < problem.n_operations * problem.n_machines; ++proc_t)
        printf("%d ", problem.processing_times[proc_t]);
    printf("\n");
}

int MasterWorkerPBB::getRank() {
    return rank;
}

int MasterWorkerPBB::getSizeWorkers() {
    return n_workers;
}

int MasterWorkerPBB::isMaster() {
    if (rank == MASTER_RANK)
        return 1;
    return 0;
}

int MasterWorkerPBB::isWorker() {
    if (rank > MASTER_RANK)
        return 1;
    return 0;
}

