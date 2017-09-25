//
//  MasterSlave.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 21/09/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "MasterWorker.hpp"

MasterWorker::MasterWorker(){}

MasterWorker::MasterWorker(int num_workers, int num_threads, const char file_instance[]):
n_workers(num_workers),
threads_per_node(num_threads){
    strcpy(file, file_instance);
}

MasterWorker::~MasterWorker(){
}

void MasterWorker::run(){
    rank = MPI::COMM_WORLD.Get_rank();
    n_workers = MPI::COMM_WORLD.Get_size();
    
    if(isMaster())
        loadInstance(payload_problem, file);
    
    preparePayloadProblemPart1(payload_problem, datatype_problem);
    MPI::COMM_WORLD.Bcast(&payload_problem, 1, datatype_problem, MASTER_RANK); /* The MASTER_NODE broadcast the part 1 of payload and the slaves nodes receives it. */

    unpack_payload_part1(payload_problem, payload_interval); /** Each node initialize their structures. The MASTER_NODE has initialized them from the load_instance. **/
    preparePayloadProblemPart2(payload_problem, datatype_problem); /** Prepares the second part (processing times).**/
    MPI::COMM_WORLD.Bcast(&payload_problem, 1, datatype_problem, MASTER_RANK);
    
    preparePayloadInterval(payload_interval, datatype_interval); /** Each node prepares the payload for receiving intervals. **/
    
    if (isMaster())
        runMasterProcess();
    else
        runWorkerProcess();
    
    MPI_Type_free(&datatype_interval);
    MPI_Type_free(&datatype_problem);
}

void MasterWorker::runMasterProcess(){
    /**
     * This is the Master node.
     * Repeat send one interval to each node.
     * - Generates an interval for each node.
     **/
    int node_dest = 0;
    
    for (node_dest = 1; node_dest < n_workers; node_dest++) {
        
        // Generates an interval.
        payload_interval.priority = node_dest;
        payload_interval.deep = node_dest;
        payload_interval.build_up_to = node_dest;
        payload_interval.distance[0] = 0.3;
        payload_interval.distance[1] = 0.7;
        
        for (int element = 0; element < payload_interval.max_size; ++element)
            payload_interval.interval[element] = element * node_dest;
        
        printf("[%d] Sending: %d %d %d %d %f %f\n",
               MASTER_RANK,
               payload_interval.priority,
               payload_interval.deep,
               payload_interval.build_up_to,
               payload_interval.max_size,
               payload_interval.distance[0],
               payload_interval.distance[1]);
        
        printf("[%d] ", MASTER_RANK);
        for (int element = 0; element < payload_interval.max_size; ++element)
            printf("%d ", payload_interval.interval[element]);
        printf("\n");
        MPI::COMM_WORLD.Send(&payload_interval, 1, datatype_interval, node_dest, TAG_INTERVAL);
        
        /** Here starts the part to monitor each node. **/
    }
}

void MasterWorker::runWorkerProcess(){
    
    int source = MASTER_RANK;

    ProblemFJSSP problem(payload_problem);
    problem.printProblemInfo();
    
    MPI::COMM_WORLD.Recv(&payload_interval, 1, datatype_interval, source, TAG_INTERVAL);
    
    printf("[%d] Receiving: %d %d %d %d %f %f\n",
           rank,
           payload_interval.priority,
           payload_interval.deep,
           payload_interval.build_up_to,
           payload_interval.max_size,
           payload_interval.distance[0],
           payload_interval.distance[1]);
    
    printf("[%d] ", rank);
    for (int element = 0; element < payload_interval.max_size; ++element)
        printf("%d ", payload_interval.interval[element]);
    printf("\n");
}

void MasterWorker::loadInstance(Payload_problem_fjssp& problem, const char *filePath){
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
    }
}

void MasterWorker::preparePayloadProblemPart1(const Payload_problem_fjssp& problem, MPI_Datatype& datatype_problem){
    const int n_blocks = 4;
    int blocks[n_blocks] = { 1, 1, 1, 1};
    MPI_Aint displacements_interval[n_blocks];
    MPI_Datatype types[n_blocks] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint addr_base, addr_objectives, addr_n_jobs, addr_n_operations, addr_n_machines;
    
    MPI_Get_address(&payload_problem, &addr_base);
    MPI_Get_address(&(payload_problem.n_objectives), &addr_objectives);
    MPI_Get_address(&(payload_problem.n_jobs), &addr_n_jobs);
    MPI_Get_address(&(payload_problem.n_operations), &addr_n_operations);
    MPI_Get_address(&(payload_problem.n_machines), &addr_n_machines);
    
    displacements_interval[0] = static_cast<MPI_Aint>(0);
    displacements_interval[1] = addr_n_jobs - addr_base;
    displacements_interval[2] = addr_n_operations - addr_base;
    displacements_interval[3] = addr_n_machines - addr_base;
    
    MPI_Type_create_struct(n_blocks, blocks, displacements_interval, types, &datatype_problem);
    MPI_Type_commit(&datatype_problem);
}

void MasterWorker::preparePayloadProblemPart2(const Payload_problem_fjssp& problem, MPI_Datatype& datatype_problem){
    const int n_blocks = 7;
    int blocks[n_blocks] = {1, 1, 1, 1, payload_problem.n_jobs, payload_problem.n_jobs, payload_problem.n_operations
        * payload_problem.n_machines };
    MPI_Aint displacements_interval[n_blocks];
    MPI_Datatype types[n_blocks] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT };
    MPI_Aint addr_base, addr_objectives, addr_n_jobs, addr_n_operations, addr_n_machines, addr_release_times, addr_n_oper_in_job,
    addr_processing_times;
    
    MPI_Get_address(&payload_problem, &addr_base);
    MPI_Get_address(&(payload_problem.n_objectives), &addr_objectives);
    MPI_Get_address(&(payload_problem.n_jobs), &addr_n_jobs);
    MPI_Get_address(&(payload_problem.n_operations), &addr_n_operations);
    MPI_Get_address(&(payload_problem.n_machines), &addr_n_machines);
    MPI_Get_address(payload_problem.release_times, &addr_release_times);
    MPI_Get_address(payload_problem.n_operations_in_job, &addr_n_oper_in_job);
    MPI_Get_address(payload_problem.processing_times, &addr_processing_times);
    
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

void MasterWorker::preparePayloadInterval(const Payload_interval& interval, MPI_Datatype& datatype_interval){
    const int n_blocks = 6;
    int blocks[n_blocks] = { 1, 1, 1, 1, 2, payload_interval.max_size };
    MPI_Aint displacements_interval[n_blocks];
    MPI_Datatype types[n_blocks] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_FLOAT, MPI_INT };
    MPI_Aint addr_base, addr_priority, addr_deep, addr_build, addr_max_size, addr_distance, addr_interval;
    
    MPI_Get_address(&payload_interval, &addr_base);
    MPI_Get_address(&(payload_interval.priority), &addr_priority);
    MPI_Get_address(&(payload_interval.deep), &addr_deep);
    MPI_Get_address(&(payload_interval.build_up_to), &addr_build);
    MPI_Get_address(&(payload_interval.max_size), &addr_max_size);
    MPI_Get_address(payload_interval.distance, &addr_distance);
    MPI_Get_address(payload_interval.interval, &addr_interval);
    
    displacements_interval[0] = static_cast<MPI_Aint>(0);
    displacements_interval[1] = addr_deep - addr_base;
    displacements_interval[2] = addr_build - addr_base;
    displacements_interval[3] = addr_max_size - addr_base;
    displacements_interval[4] = addr_distance - addr_base;
    displacements_interval[5] = addr_interval - addr_base;
    
    MPI_Type_create_struct(n_blocks, blocks, displacements_interval, types, &datatype_interval);
    MPI_Type_commit(&datatype_interval);
}

void MasterWorker::preparePayloadSolution(const Payload_solution& solution, MPI_Datatype& datatype_solution){
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

void MasterWorker::unpack_payload_part1(Payload_problem_fjssp& problem, Payload_interval& interval) {
    if (isWorker()) {
        problem.release_times = new int[problem.n_jobs];
        problem.n_operations_in_job = new int[problem.n_jobs];
        problem.processing_times = new int[problem.n_operations * problem.n_machines];
    }
    
    interval.max_size = problem.n_operations;
    interval.interval = new int[problem.n_operations];
}

int MasterWorker::getRank(){
    return rank;
}

int MasterWorker::getSizeWorkers(){
    return n_workers;
}

int MasterWorker::isMaster(){
    if (rank == MASTER_RANK)
        return 1;
    return 0;
}

int MasterWorker::isWorker(){
    if (rank > MASTER_RANK)
        return 1;
    return 0;
}
