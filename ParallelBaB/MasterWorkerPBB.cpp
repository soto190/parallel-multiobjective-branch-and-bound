/*
 * MasterWorker.cpp
 *
 *  Created on: 22/09/2017
 *      Author: carlossoto
 *  This class is inspired from:
 *  - https://github.com/coolis/MPI_Master_Worker
 *  - https://www.hpc.cam.ac.uk/using-clusters/compiling-and-development/parallel-programming-mpi-example
 *
 *  Hybrid parallel designs:
 *  - https://software.intel.com/en-us/articles/hybrid-parallelism-parallel-distributed-memory-and-shared-memory-computing
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

    /** All the nodes loads the text of tags for printing.**/
    strcpy(TAGS[0], "MASTER_RANK");
    strcpy(TAGS[1], "TAG_INTERVAL");
    strcpy(TAGS[2], "TAG_SOLUTION");
    strcpy(TAGS[3], "TAG_FINISH_WORK");
    strcpy(TAGS[4], "TAG_WORKER_READY");
    strcpy(TAGS[5], "TAG_NO_MORE_WORK");
    strcpy(TAGS[6], "TAG_REQUEST_MORE_WORK");
    strcpy(TAGS[7], "TAG_SHARE_WORK");
    
    run(); /** Initializes the Payloads. **/
    
    if (isMaster())
        runMasterProcess();
    else
        runWorkerProcess();
    
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    
    if (isMaster())
        printf("[Master] Barrier reached at time: %6.6f\n", time_span.count());
    else
        printf("[WorkerPBB-%03d] Barrier reached at time: %6.6f\n", rank, time_span.count());
    /** Wait for all the workers to save the Pareto front. **/
    MPI::COMM_WORLD.Barrier();
    if (isMaster()) {
        // save pareto front and data.
    }
    
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
    
    printf("[Master] Launching Master...\n");
    int max_number_of_mappings = payload_problem.n_jobs * payload_problem.n_machines;
    int n_intervals = 0;
    int sleeping_workers = 0;
    int requesting_worker = 0;
    int number_of_workers_with_work = n_workers;
    int number_of_works_received = 0;
    
    vector<Interval> pending_work;
    Interval work_to_send(problem.getNumberOfOperations());
    vector<Solution> received_solutions;
    Solution received_solution(2, payload_interval.max_size);
    
    int workers_with_work[n_workers + 1];
    int worker_at_right[n_workers + 1];
    worker_at_right[0] = 0; /** This position is not used. **/
    
    payload_interval.priority = 0;
    payload_interval.deep = 0;
    payload_interval.build_up_to = 0;
    payload_interval.distance[0] = 0.0;
    payload_interval.distance[1] = 0.0;
    for (int element = 1; element < payload_interval.max_size; ++element)
        payload_interval.interval[element] = -1;
    
    /** Seeding the workers with initial subproblems/intervals. **/
    for (int worker = 1; worker <= n_workers; worker++) {
        
        payload_interval.interval[0] = n_intervals; /** The first map/level of the interval/tree. **/
        printf("[Master] Sending to Worker-%03d.\n", worker);
        printPayloadInterval();
        MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, worker, TAG_INTERVAL);
        
        workers_with_work[worker] = 1;
        
        n_intervals++;
        if (n_intervals == max_number_of_mappings)
            worker = n_workers;
    }
    
    for (int worker = 1; worker < n_workers; ++worker)
        worker_at_right[worker] = worker + 1;
    worker_at_right[n_workers] = 1;
    
    while (sleeping_workers < n_workers) {
        printf("[Master] Waiting for workers request.\n");
        MPI_Recv(&payload_interval, 1, datatype_interval, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printMessageStatus(status.MPI_SOURCE, status.MPI_TAG);
        
        switch (status.MPI_TAG) {
            case TAG_REQUEST_MORE_WORK:
                if (n_intervals < max_number_of_mappings) {
                    payload_interval.interval[0] = n_intervals;
                    payload_interval.build_up_to = 0;
                    payload_interval.priority = 1;
                    payload_interval.deep = 0;
                    payload_interval.distance[0] = 0.0;
                    payload_interval.distance[1] = 0.0;
                    
                    for (int var = 1; var < payload_interval.max_size; ++var)
                        payload_interval.interval[var] = -1;
                    
                    printf("[Master] Sending interval number %d to WorkerPBB-%03d [B: %3d; D: (%4.4f, %4.4f)].\n", n_intervals, status.MPI_SOURCE, payload_interval.build_up_to, payload_interval.distance[0], payload_interval.distance[1]);
                    printPayloadInterval();
                    
                    MPI_Send(&payload_interval, 1, datatype_interval, status.MPI_SOURCE, TAG_INTERVAL, MPI_COMM_WORLD);
                    workers_with_work[status.MPI_SOURCE] = 1;
                    n_intervals++;
                } else if(pending_work.size() > 0){
                    /** TODO: Starts a process to request work from other nodes.
                     *  Strategy 1: Request work to the node who has received less works. Which means the worker has received a larger job or it is a slow worker. The workers whoes request more jobs are fastest or their job is easiest.
                     *  Strategy 2: Request work to all nodes except the source worker.
                     *  Strategy 3: The source worker request to the worker at their rigth to share work with him.
                     **/
                    work_to_send = pending_work.back();
                    pending_work.pop_back();
                    storesPayloadInterval(payload_interval, work_to_send);
                    
                    printf("[Master] Sending interval to WorkerPBB-%03d [B: %3d; D: (%4.4f, %4.4f)].\n", status.MPI_SOURCE, payload_interval.build_up_to, payload_interval.distance[0], payload_interval.distance[1]);
                    MPI_Send(&payload_interval, 1, datatype_interval, status.MPI_SOURCE, TAG_INTERVAL, MPI_COMM_WORLD);
                    
                    workers_with_work[status.MPI_SOURCE] = 1;
                    
                }else{
                    printf("[Master] Stack of work is empty.\n");
                    
                    requesting_worker = status.MPI_SOURCE; /** Saving the requesting worker. **/
                    workers_with_work[requesting_worker] = 0;  /** Requesting node doesn't has work. **/
                    for (int worker = 1; worker <= n_workers; ++worker)  /** Requesting all nodes to share one work except the source worker.**/
                        if(worker != requesting_worker && workers_with_work[worker] == 1){
                            printf("[Master] Requesting WorkerPBB-%03d to share job with Master.\n", worker);
                            MPI_Send(&payload_interval, 1, datatype_interval, worker, TAG_SHARE_WORK, MPI_COMM_WORLD);
                        }
                    
                    for (int worker = 1; worker <= n_workers; ++worker) /** Receives the response of each worker. **/
                        if(worker != requesting_worker && workers_with_work[worker] == 1){
                            printf("[Master] Waiting for work from WorkerPBB-%03d.\n", worker);
                            MPI_Recv(&payload_interval, 1, datatype_interval, worker, TAG_SHARE_WORK, MPI_COMM_WORLD, &status);
                            if (payload_interval.build_up_to > -1) {
                                number_of_works_received++;
                                work_to_send(payload_interval);
                                pending_work.push_back(work_to_send);
                                printf("[Master] Received work from WorkerPBB-%03d.\n", worker);
                                
                            }else{
                                workers_with_work[worker] = 0;
                                printf("[Master] WorkerPBB-%03d doesn't has more work.\n", worker);
                            }
                        }
                    
                    number_of_workers_with_work = 0; /** Verify if the workers have work.**/
                    for (int worker = 1; worker <= n_workers; ++worker)
                        if (workers_with_work[worker] == 1)
                            number_of_workers_with_work++;
                    
                    if (number_of_workers_with_work == 0) /** If no worker has work to share then send a stop signal to all workers. **/
                        for (int worker = 1; worker <= n_workers; ++worker) {
                            printf("[Master] Sending no more work signal to WorkerPBB-%03d.\n", worker);
                            MPI_Send(&payload_interval, 1, datatype_interval, worker, TAG_NO_MORE_WORK, MPI_COMM_WORLD);
                            sleeping_workers++;
                        }
                    else{
                        /** Re-send the received work to the requesting worker. **/
                        work_to_send = pending_work.back();
                        pending_work.pop_back();
                        storesPayloadInterval(payload_interval, work_to_send);
                        printf("[Master] Sending interval to WorkerPBB-%03d [B: %3d; D: (%4.4f, %4.4f)].\n", requesting_worker, payload_interval.build_up_to, payload_interval.distance[0], payload_interval.distance[1]);
                        MPI_Send(&payload_interval, 1, datatype_interval, requesting_worker, TAG_INTERVAL, MPI_COMM_WORLD);
                        workers_with_work[requesting_worker] = 1; /** The requesting worker now has work. **/
                    }
                }
                break;
                
            case TAG_SHARE_WORK:
                number_of_works_received++;
                work_to_send(payload_interval);
                pending_work.push_back(work_to_send);
                printf("[Master] Received work from WorkerPBB-%03d.\n", status.MPI_SOURCE);
                break;
                
            case TAG_FINISH_WORK:
                sleeping_workers++;
                break;
                
            case TAG_NO_MORE_WORK:
                number_of_workers_with_work = 0;
                workers_with_work[status.MPI_SOURCE] = 0;
                
                for (int worker = 1; worker <= n_workers; ++worker)
                    if (workers_with_work[worker] == 1)
                        number_of_workers_with_work++;
                
                if (number_of_workers_with_work == 0) /** If any worker has work to share then send a stop signal to all workers. **/
                    for (int worker = 1; worker <= n_workers; ++worker) {
                        printf("[Master] Sending no more work signal to WorkerPBB-%03d.\n", worker);
                        MPI_Send(&payload_interval, 1, datatype_interval, worker, TAG_NO_MORE_WORK, MPI_COMM_WORLD);
                        sleeping_workers++;
                    }
                break;
                
            case TAG_SOLUTION:
                /** This tag is not used in this implementation because each worker shares their solutions found to the node at right. **/
                printf("[Master] Receiving solution from WorkerPBB-%03d.\n", status.MPI_SOURCE);
                
                recoverSolutionFromPayload(payload_interval, received_solution);
                received_solutions.push_back(received_solution);
                
                /** Re-sending the solution to other workers. **/
                printf("[Master] Re-sending solution to workers.\n");
                for (int worker = 1; worker <= n_workers; ++worker)
                    if (worker != status.MPI_SOURCE)
                        MPI_Send(&payload_interval, 1, datatype_interval, worker, TAG_SOLUTION, MPI_COMM_WORLD);
                break;
                
            default:
                break;
        }
    }
    
    /** Sending stop singal to all the workers. **/
    for (int w = 1; w <= n_workers; w++)
        MPI_Send(&payload_interval, 1, datatype_interval, w, TAG_NO_MORE_WORK, MPI_COMM_WORLD);
    
    printf("[Master] Number of works received: %03d\n", number_of_works_received);
    printf("[Master] No more work.\n");
}

/**
 * The worker receives the seeding then initalize all the necessary structures and creates in each thread one B&B.
 *
 */
void MasterWorkerPBB::runWorkerProcess() {
    printf("[WorkerPBB-%03d] Worker ready...\n", rank);
    int source = MASTER_RANK;
    sleeping_bb = 0;
    there_is_more_work = 1;
    
    int worker_at_right = rank + 1; /** Worker at the right. **/
    if (worker_at_right > n_workers) /** Ring formation: The last worker has at it rigth the first worker. **/
        worker_at_right = 1;
    
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
    printf("[WorkerPBB-%03d] Pool size: %3lu %3d [%3d %3d]\n", rank, globalPool.unsafe_size(), branches_created, problem.getLowerBoundInObj(0), problem.getLowerBoundInObj(1));
    
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
    int send_sol = 1;
    vector<Solution> paretoFront;
    vector<Solution> subFront;
    vector<Solution> solutionsToSend;
    
    Solution received_solution(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    Solution sFront(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    Solution sSub(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    
    Interval interval_to_share(problem.getNumberOfOperations());
    
    paretoFront.clear();
    while (sleeping_bb < threads_per_node){ /** Waits for all B&Bs to end. **/
        if (there_is_more_work == 1 && globalPool.isEmptying()){
            MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, MASTER_RANK, TAG_REQUEST_MORE_WORK); /** Request more work from Master. **/
            
            printf("[WorkerPBB-%03d] Waiting for message.\n", rank);
            MPI_Recv(&payload_interval, 1, datatype_interval, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printMessageStatus(status.MPI_SOURCE, status.MPI_TAG);
            
            switch (status.MPI_TAG) {
                    
                case TAG_INTERVAL:
                    printf("[WorkerPBB-%03d] Receiving more work [B: %3d; D: (%4.4f, %4.4f)].\n", rank, payload_interval.build_up_to, payload_interval.distance[0], payload_interval.distance[1]);
                    printPayloadInterval();
                    branch_init(payload_interval);
                    
                    branches_in_loop = splitInterval(branch_init); /** Splits and push to GlobalPool. **/
                    branches_created += branches_in_loop;
                    printf("[WorkerPBB-%03d] Interval divided in %3d sub-intervals.\n", rank, branches_in_loop);
                    
                    /** This avoid to send repeated solutions. **/
                    subFront = BB_container.getParetoFront();
                    solutionsToSend.clear();
                    if (paretoFront.empty()){ /** If the pareto front is empty then send all the front. **/
                        paretoFront = BB_container.getParetoFront();
                        solutionsToSend = BB_container.getParetoFront();
                    }
                    else{
                        for (int sub_sol = 0; sub_sol < subFront.size(); ++sub_sol) {  /** Choosing the new non-dominated solutions to send. **/
                            sSub = subFront.at(sub_sol);
                            send_sol = 1;
                            for (int fro_sol = 0; fro_sol < paretoFront.size(); ++fro_sol) {
                                sFront = paretoFront.at(fro_sol);
                                if(sSub.dominates(sFront) == Equals){
                                    send_sol = 0;
                                    fro_sol = (int) paretoFront.size();
                                }
                            }
                            if (send_sol == 1)
                                solutionsToSend.push_back(sSub);
                        }
                    }
                    paretoFront = BB_container.getParetoFront(); /** Updates the Pareto front. **/
                    payload_interval.build_up_to = (int) solutionsToSend.size();
                    if (solutionsToSend.size() > 0) {
                        printf("[WorkerPBB-%03d] Sending %3d solutions to worker at right.\n", rank, (int) solutionsToSend.size());
                        /** TODO: Improve this function. Send all the pareto set in one message instead of multiple messages. **/
                        for (int n_sol = 0; n_sol < solutionsToSend.size(); ++n_sol) {
                            received_solution = solutionsToSend.at(n_sol);
                            storesSolutionInInterval(payload_interval, received_solution);
                            
                            /** Sending the solutions to the worker at the right. With this reducing the number of messages sended and also guaranting to send the best solution to all the nodes. If the sended solution is good the worker at the right will be sended to their node at the right. **/
                            MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, worker_at_right, TAG_SOLUTION);
                            printf("[WorkerPBB-%03d] Sending solution to WorkerPBB-%03d with [%4.0f, %4.0f].\n", rank, worker_at_right, received_solution.getObjective(0), received_solution.getObjective(1));
                        }
                    }
                    break;
                    
                case TAG_SHARE_WORK:
                    printf("[WorkerPBB-%03d] Pool size %03lu.\n", rank, globalPool.unsafe_size());
                    
                    if (globalPool.try_pop(interval_to_share)) {
                        storesPayloadInterval(payload_interval, interval_to_share);
                        printf("[WorkerPBB-%03d] Sending interval to Master with D:(%4.4f, %4.4f).\n", rank, payload_interval.distance[0], payload_interval.distance[1]);
                        MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, MASTER_RANK, TAG_SHARE_WORK);
                    }else{
                        printf("[WorkerPBB-%03d] Sending message to Master with TAG_SHARE_WORK with empty interval.\n", rank);
                        payload_interval.build_up_to = -1; /** Send with -1 to indicate no more work. **/
                        MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, MASTER_RANK, TAG_SHARE_WORK);
                    }
                    break;
                    
                case TAG_SOLUTION:
                    
                    recoverSolutionFromPayload(payload_interval, received_solution);
                    
                    /** Update the bounds with the received solution. **/
                    problem.updateBestMakespanSolutionWith(received_solution);
                    problem.updateBestMaxWorkloadSolutionWith(received_solution);
                    
                    if (status.MPI_SOURCE == MASTER_RANK)
                        printf("[WorkerPBB-%03d] Receiving solution from Master with [%4.0f, %4.0f].\n", rank, received_solution.getObjective(0), received_solution.getObjective(1));
                    else
                        printf("[WorkerPBB-%03d] Receiving solution from WorkerPBB-%03d with [%4.0f, %4.0f].\n", rank, status.MPI_SOURCE, received_solution.getObjective(0), received_solution.getObjective(1));
                    
                    paretoContainer.add(received_solution);
                    break;
                    
                case TAG_NO_MORE_WORK:  /** There is no more work.**/
                    there_is_more_work = 0;
                    printf("[WorkerPBB-%03d] Receiving tag with no more work, now waiting for %03d B&B to end.\n", rank, threads_per_node - (int) sleeping_bb);
                    
                    while (sleeping_bb < threads_per_node) { /** TODO: this can be moved outside this switch-case. **/
                    }
                    
                    MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, MASTER_RANK, TAG_FINISH_WORK);
                    
                    break;
                    
                case TAG_WORKER_READY:
                    printf("[WorkerPBB-%03d] Worker Ready!.\n", rank);
                    break;
                    
                default:
                    break;
            }
        }
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
    
    BB_container.getParetoFront();
    BB_container.printParetoFront(0);
    //    BB_container.saveParetoFront();
    //    BB_container.saveSummarize();
    bb_threads.clear();
    //printf("[WorkerPBB-%03d] Data swarm recollected and saved.\n", rank);
    printf("[WorkerPBB-%03d] Parallel Branch And Bound ended.\n", rank);
}

void MasterWorkerPBB::loadInstance(Payload_problem_fjssp& problem, const char *filePath) {
    char extension[4];
    int instance_with_release_time = 1; /** This is used because the Kacem's instances have releases times and the other sets dont. **/
    
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
    printf("[Master] Name: %s extension: %s\n", name_file_ext[0].c_str(), name_file_ext[1].c_str());
    std::strcpy(extension, name_file_ext[1].c_str());
    
    /** If the instance is Kacem then it has the release time in the first value of each job. **/
    const int kacem_legnth = 5;
    char kacem[kacem_legnth] {'K', 'a', 'c', 'e', 'm'};
    for (int character = 0; character < kacem_legnth && instance_with_release_time == 1; ++character)
        instance_with_release_time = (kacem[character] != name_file_ext[0][character]) ? 0 : 1;
    
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
                problem.n_operations_in_job[n_job] = std::stoi(elemens.at(1));
                problem.n_operations += problem.n_operations_in_job[n_job];
                problem.release_times[n_job] = ((instance_with_release_time == 1) ? std::stoi(elemens.at(0)) : 0);
            }
            
            problem.processing_times = new int[problem.n_operations * problem.n_machines];
            int op_counter = 0;
            for (int n_job = 0; n_job < problem.n_jobs; ++n_job) {
                int token = (instance_with_release_time == 0)?1:2;
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

void MasterWorkerPBB::unpack_payload_part2(Payload_problem_fjssp& payload_problem) {
    if(isMaster())
        printf("[Master] Jobs: %d Operations: %d Machines: %d\n", payload_problem.n_jobs, payload_problem.n_operations, payload_problem.n_machines);
    else
        printf("[WorkerPBB-%03d] Jobs: %d Operations: %d Machines: %d\n", rank, payload_problem.n_jobs, payload_problem.n_operations, payload_problem.n_machines);
    
    problem.loadInstancePayload(payload_problem);
    
    if(isMaster())
        problem.printProblemInfo();
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
                    
                    branch_to_split.setDistance(0, BranchAndBound::distanceToObjective(fjssp_data.getMakespan(), problem.getLowerBoundInObj(0)));
                    branch_to_split.setDistance(1, BranchAndBound::distanceToObjective(fjssp_data.getMaxWorkload(), problem.getLowerBoundInObj(1)));
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

void MasterWorkerPBB::storesPayloadInterval(Payload_interval& payload, const Interval& interval){
    payload.build_up_to = interval.getBuildUpTo();
    payload.priority = interval.getPriority();
    payload.deep = interval.getDeep();
    payload.max_size = interval.getSize();
    payload.distance[0] = interval.getDistance(0);
    payload.distance[1] = interval.getDistance(1);
    
    for (int var = 0; var < interval.getSize(); ++var)
        payload.interval[var] = interval.getValueAt(var);
}

void MasterWorkerPBB::recoverSolutionFromPayload(const Payload_interval &payload, Solution &solution){
    solution.setObjective(0, payload.distance[0]);
    solution.setObjective(1, payload.distance[1]);
    
    for (int n_var = 0; n_var < solution.getNumberOfVariables(); ++n_var)
        solution.setVariable(n_var, payload.interval[n_var]);
}

void MasterWorkerPBB::storesSolutionInInterval(Payload_interval &payload, const Solution& solution){
    payload.distance[0] = solution.getObjective(0); /** This stores the first objective. **/
    payload.distance[1] = solution.getObjective(1); /** This stores the second objective. **/
    for (int var = 0; var < solution.getNumberOfVariables(); ++var)
        payload.interval[var] = solution.getVariable(var);
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

void MasterWorkerPBB::printMessageStatus(int source, int tag){
    if (rank == MASTER_RANK)
        printf("[Master] Received message from WorkerPBB-%03d with %s.\n", source, TAGS[tag - 190]);
    else if (source == MASTER_RANK)
        printf("[WorkerPBB-%03d] Received message from Master with %s.\n", rank, TAGS[tag - 190]);
    else
        printf("[WorkerPBB-%03d] Received message from WorkerPBB-%03d with %s.\n", rank, source, TAGS[tag - 190]);
}
