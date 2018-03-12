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
    branchsandbound_per_worker = 0;
    datatype_interval = 0;
    datatype_problem = 0;
    datatype_solution = 0;
    
    branches_explored = 0;
    branches_created = 0;
    branches_pruned = 0;
}

MasterWorkerPBB::MasterWorkerPBB(int num_nodes, int num_threads, const char file_instance[], const char output_path_results[]) :
n_workers(num_nodes - 1),
branchsandbound_per_worker(num_threads) {
    rank = 0;
    std::strcpy(instance_file, file_instance);
    std::strcpy(output_path, output_path_results);
    datatype_interval = 0;
    datatype_problem = 0;
    datatype_solution = 0;
    
    branches_explored = 0;
    branches_created = 0;
    branches_pruned = 0;
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
    strcpy(TAGS[5], "TAG_NOT_ENOUGH_WORK");
    strcpy(TAGS[6], "TAG_REQUEST_MORE_WORK");
    strcpy(TAGS[7], "TAG_SHARE_WORK");
    
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    if(isMaster())
        printf("[Master] Running on %s.\n", processor_name);
    else
        printf("[WorkerPBB-%03d] Running on %s.\n", getRank(), processor_name);
    
    buildOutputFiles();
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
        printf("[WorkerPBB-%03d] Barrier reached at time: %6.6f\n", getRank(), time_span.count());
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
        loadInstance(payload_problem, instance_file);
    
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
    int number_of_workers_with_work;
    int number_of_works_received = 0;
    
    vector<Interval> pending_work;
    Interval work_to_send(problem.getNumberOfOperations());
    vector<Solution> received_solutions;
    Solution received_solution(problem.getNumberOfObjectives(), payload_interval.max_size);
    
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
                    
                    MPI_Send(&payload_interval, 1, datatype_interval, status.MPI_SOURCE, TAG_INTERVAL, MPI_COMM_WORLD);
                    workers_with_work[status.MPI_SOURCE] = 1;
                    n_intervals++;
                } else if(pending_work.size() > 0) {
                    /** TODO: Starts a process to request work from other nodes.
                     *  Strategy 1: Request work to the node who has received less works. Which means the worker has received a larger job or it is a slow worker. The workers whoes request more jobs are fastest or their job is easiest.
                     *  Strategy 2: Request work to all nodes except the source worker.
                     *  Strategy 3: The source worker request to the worker at their rigth to share work with him.
                     **/
                    work_to_send = pending_work.back();
                    pending_work.pop_back();
                    storesPayloadInterval(payload_interval, work_to_send);
                    
                    MPI_Send(&payload_interval, 1, datatype_interval, status.MPI_SOURCE, TAG_INTERVAL, MPI_COMM_WORLD);
                    
                    workers_with_work[status.MPI_SOURCE] = 1;
                    
                } else {
                    requesting_worker = status.MPI_SOURCE; /** Saving the requesting worker. **/
                    workers_with_work[requesting_worker] = 0;  /** Requesting node doesn't has work. **/
                    for (int worker = 1; worker <= n_workers; ++worker)  /** Requesting all nodes to share one work except the source worker.**/
                        if(worker != requesting_worker && workers_with_work[worker] == 1) {
                            MPI_Send(&payload_interval, 1, datatype_interval, worker, TAG_SHARE_WORK, MPI_COMM_WORLD);
                        }
                    
                    for (int worker = 1; worker <= n_workers; ++worker) /** Receives the response of each worker. **/
                        if(worker != requesting_worker && workers_with_work[worker] == 1) {
                            MPI_Recv(&payload_interval, 1, datatype_interval, worker, TAG_SHARE_WORK, MPI_COMM_WORLD, &status);
                            if (payload_interval.build_up_to > -1) {
                                number_of_works_received++;
                                work_to_send(payload_interval);
                                pending_work.push_back(work_to_send);
                                
                            }else
                                workers_with_work[worker] = 0;
                        }
                    
                    number_of_workers_with_work = 0; /** Verify if the workers have work.**/
                    for (int worker = 1; worker <= n_workers; ++worker)
                        if (workers_with_work[worker] == 1)
                            number_of_workers_with_work++;
                    
                    if (number_of_workers_with_work == 0) /** If no worker has work to share then send a stop signal to all workers. **/
                        for (int worker = 1; worker <= n_workers; ++worker) {
                            MPI_Send(&payload_interval, 1, datatype_interval, worker, TAG_NOT_ENOUGH_WORK, MPI_COMM_WORLD);
                            sleeping_workers++;
                        }
                    else {
                        /** Re-send the received work to the requesting worker. **/
                        work_to_send = pending_work.back();
                        pending_work.pop_back();
                        storesPayloadInterval(payload_interval, work_to_send);
                        MPI_Send(&payload_interval, 1, datatype_interval, requesting_worker, TAG_INTERVAL, MPI_COMM_WORLD);
                        workers_with_work[requesting_worker] = 1; /** The requesting worker now has work. **/
                    }
                }
                break;
                
            case TAG_SHARE_WORK:
                number_of_works_received++;
                work_to_send(payload_interval);
                pending_work.push_back(work_to_send);
                break;
                
            case TAG_FINISH_WORK:
                sleeping_workers++;
                break;
                
            case TAG_NOT_ENOUGH_WORK:
                number_of_workers_with_work = 0;
                workers_with_work[status.MPI_SOURCE] = 0;
                
                for (int worker = 1; worker <= n_workers; ++worker)
                    if (workers_with_work[worker] == 1)
                        number_of_workers_with_work++;
                
                if (number_of_workers_with_work == 0) /** If any worker has work to share then send a stop signal to all workers. **/
                    for (int worker = 1; worker <= n_workers; ++worker) {
                        MPI_Send(&payload_interval, 1, datatype_interval, worker, TAG_NOT_ENOUGH_WORK, MPI_COMM_WORLD);
                        sleeping_workers++;
                    }
                break;
                
            case TAG_SOLUTION:
                /** This tag is not used in this implementation because each worker shares their solutions found to the worker at right. **/
                
                recoverSolutionFromPayload(payload_interval, received_solution);
                received_solutions.push_back(received_solution);
                
                /** Re-sending the solution to other workers. **/
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
        MPI_Send(&payload_interval, 1, datatype_interval, w, TAG_NOT_ENOUGH_WORK, MPI_COMM_WORLD);
    
    /**
     * Receive data from workers.
     **/
    
    printf("[Master] Number of works received: %03d\n", number_of_works_received);
    printf("[Master] No more work.\n");
}

/**
 * The worker receives the seeding then initalize all the necessary structures and creates in each thread one B&B.
 */
void MasterWorkerPBB::runWorkerProcess() {
    printf("[WorkerPBB-%03d] Worker ready...\n", getRank());
    int source = MASTER_RANK;
    sleeping_bb = 0;
    there_is_more_work = true;
    
    int worker_at_right = rank + 1; /** Worker at the right. **/
    if (worker_at_right > n_workers) /** Ring formation: The last worker has at it rigth the first worker. **/
        worker_at_right = 1;
    
    printf("[WorkerPBB-%03d] Waiting for interval.\n", rank);
    MPI_Recv(&payload_interval, 1, datatype_interval, source, TAG_INTERVAL, MPI_COMM_WORLD, &status);
    branch_init(payload_interval);
    
    sharedPool.setSizeEmptying((unsigned long) (branchsandbound_per_worker * 2)); /** If the global pool reach this size then the B&B tasks starts sending part of their work to the global pool. **/
    
    Solution solution (problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    problem.createDefaultSolution(solution);
    
    paretoContainer(25, 25, solution.getObjective(0), solution.getObjective(1), problem.getLowerBoundInObj(0), problem.getLowerBoundInObj(1));
    
    BranchAndBound BB_container(rank, 0, problem, branch_init);
    branches_created += BB_container.initGlobalPoolWithInterval(branch_init);
    BB_container.setSummarizeFile(summarize_file);
    BB_container.setParetoFrontFile(pareto_front_file);
    BB_container.setPoolFile(pool_file);
    

    printf("[WorkerPBB-%03d] Pool size: %3lu %3lu [%3d %3d]\n", getRank(), sharedPool.unsafe_size(), branches_created, problem.getLowerBoundInObj(0), problem.getLowerBoundInObj(1));
    
    set_ref_count(branchsandbound_per_worker + 1);
    
    tbb::task_list tl; /** When task_list is destroyed it doesn't calls the destructor. **/
    vector<BranchAndBound *> bb_threads;
    int n_bb = 0;
    while (n_bb++ < branchsandbound_per_worker) {
        BranchAndBound * BaB_task = new (tbb::task::allocate_child()) BranchAndBound(getRank(), n_bb, problem, branch_init);
        bb_threads.push_back(BaB_task);
        tl.push_back(*BaB_task);
    }
    
    printf("[WorkerPBB-%03d] Spawning the swarm...\n", getRank());
    tbb::task::spawn(tl);
    /**TODO: Spawn the thread with the metaheuristic. **/
    int send_sol = 1;
    ParetoFront paretoFront;
    ParetoFront subFront;
    ParetoFront solutionsToSend;
    
    Solution received_solution(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    Solution sSub(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    
    Interval interval_to_share(problem.getNumberOfOperations());
    
    paretoFront.clear();
    while (sleeping_bb < branchsandbound_per_worker) { /** Waits for all B&Bs to end. **/
        if (thereIsMoreWork() && sharedPool.isEmptying()) {
            MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, MASTER_RANK, TAG_REQUEST_MORE_WORK);
            
            MPI_Recv(&payload_interval, 1, datatype_interval, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printMessageStatus(status.MPI_SOURCE, status.MPI_TAG);
            
            switch (status.MPI_TAG) {
                    
                case TAG_INTERVAL:
                    branch_init(payload_interval);
                    
                    splitInterval(branch_init); /** Splits and push to GlobalPool. **/
                    
                    /** This avoid to send repeated solutions. **/
                    subFront = BB_container.getParetoFront();
                    solutionsToSend.clear();
                    if (paretoFront.empty()) { /** If the pareto front is empty then send all the front. **/
                        paretoFront = BB_container.getParetoFront();
                        solutionsToSend = BB_container.getParetoFront();
                    }
                    else {
                        for (int sub_sol = 0; sub_sol < subFront.size(); ++sub_sol) {  /** Choosing the new non-dominated solutions to send. **/
                            sSub = subFront.at(sub_sol);
                            send_sol = 1;
                            for (int fro_sol = 0; fro_sol < paretoFront.size(); ++fro_sol) {
                                DominanceRelation result_dom = sSub.dominanceTest(paretoFront.at(fro_sol));
                                if(result_dom == Equals || result_dom == Dominated) {
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
                        /** TODO: Improve this function. Send all the pareto set in one message instead of multiple messages. **/
                        for (int n_sol = 0; n_sol < solutionsToSend.size(); ++n_sol) {
                            received_solution = solutionsToSend.at(n_sol);
                            storesSolutionInInterval(payload_interval, received_solution);
                            
                            /** Sending the solutions to the worker at the right. With this reducing the number of messages sended and also guaranting to send the best solution to all the nodes. If the sended solution is good the worker at the right will be sended to their node at the right. **/
                            MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, worker_at_right, TAG_SOLUTION);
                        }
                    }
                    break;
                    
                case TAG_SHARE_WORK:
                    
                    if (sharedPool.try_pop(interval_to_share)) {
                        storesPayloadInterval(payload_interval, interval_to_share);
                        MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, MASTER_RANK, TAG_SHARE_WORK);
                    } else {
                        payload_interval.build_up_to = -1; /** Send with -1 to indicate no more work. **/
                        MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, MASTER_RANK, TAG_SHARE_WORK);
                    }
                    break;
                    
                case TAG_SOLUTION:
                    
                    recoverSolutionFromPayload(payload_interval, received_solution);
                    
                    problem.updateBestMakespanSolutionWith(received_solution);
                    problem.updateBestMaxWorkloadSolutionWith(received_solution);
                    
                    paretoContainer.add(received_solution);
                    break;
                    
                case TAG_NOT_ENOUGH_WORK:  /** There is no more work.**/
                    there_is_more_work = false;
                    printf("[WorkerPBB-%03d] Not engouh work received at %f.\n", getRank(), BB_container.getElapsedTime() );
                    while (sleeping_bb < branchsandbound_per_worker) { /** TODO: this can be moved outside this switch-case. **/
                    }
                    
                    MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, MASTER_RANK, TAG_FINISH_WORK);
                    
                    break;
                    
                case TAG_WORKER_READY:
                    printf("[WorkerPBB-%03d] Worker Ready!.\n", getRank());
                    break;
                    
                default:
                    break;
            }
        }
        //BB_container.saveEvery(300);
    }
    
    /** Recollects the data. **/
    BB_container.getElapsedTime();
    BranchAndBound* bb_in;
    while (!bb_threads.empty()) {
        
        bb_in = bb_threads.back();
        bb_threads.pop_back();
        
        BB_container.increaseNumberOfNodesExplored(bb_in->getNumberOfNodesExplored());
        BB_container.increaseNumberOfCallsToBranch(bb_in->getNumberOfCallsToBranch());
        BB_container.increaseNumberOfNodesCreated(bb_in->getNumberOfNodesCreated());
        BB_container.increaseNumberOfCallsToPrune(bb_in->getNumberOfCallsToPrune());
        BB_container.increaseNumberOfNodesPruned(bb_in->getNumberOfNodesPruned());
        BB_container.increaseNumberOfReachedLeaves(bb_in->getNumberOfReachedLeaves());
        BB_container.increaseNumberOfUpdatesInLowerBound(bb_in->getNumberOfUpdatesInLowerBound());
        BB_container.increaseSharedWork(bb_in->getSharedWork());
    }
    BB_container.increaseNumberOfNodesExplored(branches_explored);
    BB_container.increaseNumberOfNodesCreated(branches_created);
    BB_container.increaseNumberOfNodesPruned(branches_pruned);
    
    /**
     * Send the data to Master.
     **/

    BB_container.getParetoFront();
    BB_container.printParetoFront(0);
    BB_container.saveParetoFront();
    BB_container.saveSummarize();
    BB_container.saveGlobalPool();
    bb_threads.clear();
    
    printf("[WorkerPBB-%03d] Data swarm recollected and saved.\n", getRank());
    printf("[WorkerPBB-%03d] Parallel Branch And Bound ended.\n", getRank());

}

bool MasterWorkerPBB::thereIsMoreWork() const {
    return there_is_more_work;
}

void MasterWorkerPBB::loadInstance(Payload_problem_fjssp& problem, const char *filePath) {
    char extension[4];
    int instance_with_release_time = 1; /** This is used because the Kacem's instances have release times and the other sets dont. **/
    
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
    
    /** If the instance is Kacem then it has the release time in the first value of each job. TODO: Re-think if its a good idea to update all the instances to include release time at 0. **/
    const int kacem_legnth = 5;
    char kacem[kacem_legnth] {'K', 'a', 'c', 'e', 'm'};
    for (int character = 0; character < kacem_legnth && instance_with_release_time == 1; ++character)
        instance_with_release_time = (kacem[character] == name_file_ext[0][character]) ? 1 : 0;
 
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
                problem.n_operations_in_job[n_job] = (instance_with_release_time == 1) ? std::stoi(elemens.at(1)): std::stoi(elemens.at(0));
                problem.n_operations += problem.n_operations_in_job[n_job];
                problem.release_times[n_job] = ((instance_with_release_time == 1) ? std::stoi(elemens.at(0)) : 0);
            }
            
            problem.processing_times = new int[problem.n_operations * problem.n_machines];
            int op_counter = 0;
            for (int n_job = 0; n_job < problem.n_jobs; ++n_job) {
                int token = (instance_with_release_time == 1)?2:1;
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
    }else
        printf("[Master] Unable to open instance file.\n");
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
    
    problem.loadInstancePayload(payload_problem);
    
    if(isMaster())
        problem.printProblemInfo();
}

int MasterWorkerPBB::getRank() const {
    return rank;
}

bool MasterWorkerPBB::isMaster() const {
    return rank == MASTER_RANK;
}

bool MasterWorkerPBB::isWorker() const {
    return rank > MASTER_RANK;
}

int MasterWorkerPBB::splitInterval(Interval& branch_to_split) {
    Solution solution(problem.getNumberOfObjectives(), problem.getNumberOfVariables());
    int split_level = branch_to_split.getBuildUpTo() + 1;
    int branches_created_in = 0;
    int num_elements = problem.getTotalElements();
    int map = 0;
    int value_to_add = 0;

    FJSSPdata fjssp_data(problem.getNumberOfJobs(), problem.getNumberOfOperations(), problem.getNumberOfMachines());
    fjssp_data.reset(); /** This function call is not necesary because the structurs are empty.**/
    
    fjssp_data.setMinTotalWorkload(problem.getSumOfMinPij());
    for (int machine = 0; machine < problem.getNumberOfMachines(); ++machine) {
        fjssp_data.setBestWorkloadInMachine(machine, problem.getBestWorkload(machine));
        fjssp_data.setTempBestWorkloadInMachine(machine, problem.getBestWorkload(machine));
    }
    
    for (int row = 0; row <= branch_to_split.getBuildUpTo(); ++row) {
        map = branch_to_split.getValueAt(row);
        solution.setVariable(row, map);
        problem.evaluateDynamic(solution, fjssp_data, row);
    }
    
    for (int job = 0; job < num_elements; ++job)
        if (fjssp_data.getNumberOfOperationsAllocatedFromJob(job) < problem.getTimesThatValueCanBeRepeated(job)) {
            int op = problem.getOperationInJobIsNumber(job, fjssp_data.getNumberOfOperationsAllocatedFromJob(job));
            unsigned long machines_aviable = problem.getNumberOfMachinesAvaibleForOperation(op);
            for (int machine = 0; machine < machines_aviable; ++machine) {
                int new_machine = problem.getMachinesAvaibleForOperation(op, machine);
                value_to_add = problem.getEncodeMap(job, new_machine);
                solution.setVariable(split_level, value_to_add);
                problem.evaluateDynamic(solution, fjssp_data, split_level);
                branches_explored++;

                if (paretoContainer.improvesTheGrid(solution)) {
                    branch_to_split.setValueAt(split_level, value_to_add);
                    branch_to_split.setDistance(0, BranchAndBound::distanceToObjective(fjssp_data.getMakespan(), problem.getLowerBoundInObj(0)));
                    branch_to_split.setDistance(1, BranchAndBound::distanceToObjective(fjssp_data.getMaxWorkload(), problem.getLowerBoundInObj(1)));
                    
                    branch_to_split.setHighPriority();

                    sharedPool.push(branch_to_split); /** The vector adds a copy of interval. **/
                    branch_to_split.removeLastValue();
                    branches_created_in++;
                    branches_created++;
                } else
                    branches_pruned++;
                problem.evaluateRemoveDynamic(solution, fjssp_data, split_level);
            }
        }
    return branches_created_in;
}

void MasterWorkerPBB::storesPayloadInterval(Payload_interval& payload, const Interval& interval) {
    payload.build_up_to = interval.getBuildUpTo();
    payload.priority = interval.getPriority();
    payload.deep = interval.getDeep();
    payload.max_size = interval.getSize();
    payload.distance[0] = interval.getDistance(0);
    payload.distance[1] = interval.getDistance(1);
    
    for (int var = 0; var < interval.getSize(); ++var)
        payload.interval[var] = interval.getValueAt(var);
}

void MasterWorkerPBB::recoverSolutionFromPayload(const Payload_interval &payload, Solution &solution) {
    solution.setObjective(0, payload.distance[0]);
    solution.setObjective(1, payload.distance[1]);
    
    for (int n_var = 0; n_var < solution.getNumberOfVariables(); ++n_var)
        solution.setVariable(n_var, payload.interval[n_var]);
}

void MasterWorkerPBB::storesSolutionInInterval(Payload_interval &payload, const Solution& solution) {
    payload.distance[0] = solution.getObjective(0); /** This stores the first objective. **/
    payload.distance[1] = solution.getObjective(1); /** This stores the second objective. **/
    for (int var = 0; var < solution.getNumberOfVariables(); ++var)
        payload.interval[var] = solution.getVariable(var);
}

void MasterWorkerPBB::setParetoFrontFile(const char outputFile[255]) {
    std::strcpy(pareto_front_file, outputFile);
}

void MasterWorkerPBB::setSummarizeFile(const char outputFile[255]) {
    std::strcpy(summarize_file, outputFile);
}

void MasterWorkerPBB::buildOutputFiles() {
    
    std::vector<std::string> paths;
    std::vector<std::string> name_file;
    paths = split(instance_file, '/');
    
    unsigned long int sizeOfElems = paths.size();
    name_file = split(paths[sizeOfElems - 1], '.');
    
    std::string output_file_pool = output_path + name_file[0];
    std::string output_file_ivm = "";
    std::string output_file_summarize = "";
    std::string output_file_pareto = "";

    output_file_ivm = output_file_pool;
    output_file_summarize = output_file_ivm;
    output_file_pareto = output_file_ivm;
    
    long long rank_long_long = static_cast<long long>(getRank());
    output_file_ivm += "-node" + std::to_string(rank_long_long) + "-ivm" + std::to_string(static_cast<long long>(getNumberOfBranchsAndBound())) + ".txt";
    std::strcpy(ivm_file, output_file_ivm.c_str());
    output_file_pool += "-node" + std::to_string(rank_long_long) + "-pool.txt";
    std::strcpy(pool_file, output_file_pool.c_str());
    output_file_summarize += "-node" + std::to_string(rank_long_long) + "-summarize.txt";
    std::strcpy(summarize_file, output_file_summarize.c_str());
    output_file_pareto += "-node" + std::to_string(rank_long_long) + "-pf.txt";
    std::strcpy(pareto_front_file, output_file_pareto.c_str());
}

void MasterWorkerPBB::printPayloadInterval() const {
    if (isMaster())
        printf("[Master] [ ");
    else
        printf("[WorkerPBB-%03d] [ ", getRank());
    
    for (int element = 0; element < payload_interval.max_size; ++element)
        if (payload_interval.interval[element] == -1)
            printf("- ");
        else
            printf("%d ", payload_interval.interval[element]);
    printf("]\n");
}

int MasterWorkerPBB::getNumberOfWorkers() const {
    return n_workers;
}

int MasterWorkerPBB::getNumberOfBranchsAndBound() const {
    return branchsandbound_per_worker;
}

void MasterWorkerPBB::printMessageStatus(int source, int tag) {
    if (rank == MASTER_RANK)
        printf("[Master] Received message from WorkerPBB-%03d with %s.\n", source, getTagText(tag));
    else if (source == MASTER_RANK)
        printf("[WorkerPBB-%03d] Received message from Master with %s.\n", getRank(), getTagText(tag));
    else
        printf("[WorkerPBB-%03d] Received message from WorkerPBB-%03d with %s.\n", getRank(), source, getTagText(tag));
}

const char* MasterWorkerPBB::getTagText(int tag) const {
    return TAGS[tag - 190];
}
