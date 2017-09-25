//
//  main.cpp
//  PhDProject
//
//  Created by Carlos Soto on 08/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include <iostream>
#include <cstring>
#include <exception>
#include "Solution.hpp"
#include "Problem.hpp"
#include "ProblemHCSP.hpp"
#include "ProblemTSP.hpp"
#include "ProblemVRP.hpp"
#include "ProblemFJSSP.hpp"
#include "BranchAndBound.hpp"
#include "ParallelBranchAndBound.hpp"
#include "myutils.hpp"
#include "GridContainer.hpp"
#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/task_group.h>
#include <mpi.h>

#define MASTER_NODE 0

#define TAG_INTERVAL 190
#define TAG_SOLUTION 191
#define TAG_FINISH_WORK 192
#define TAG_SLAVE_READY 193

using namespace std;

int my_rank; /* number of process */
int n_processes; /* number of processes */

void printProblem(const Payload_problem_fjssp& problem) {
    
    printf("J:%3d M:%3d O:%3d\n", problem.n_jobs, problem.n_machines, problem.n_operations);
    
    for (int n_job = 0; n_job < problem.n_jobs; ++n_job)
        printf("%3d", problem.n_operations_in_job[n_job]);
    printf("\n");
    
    for (int n_job = 0; n_job < problem.n_jobs; ++n_job)
        printf("%3d", problem.release_times[n_job]);
    printf("\n");
    
}

/**
 * @param Struct_problem_fjssp
 * This is only called by MASTER_NODE to load the instance and then send it to each node. Each node compute all the data needed.**/
void load_instance(Payload_problem_fjssp& problem, char filePath[]) {
    
    char extension[4];
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

void unpack_payload_part1(Payload_problem_fjssp& problem, Payload_interval& interval) {
    if (my_rank > MASTER_NODE) {
        problem.release_times = new int[problem.n_jobs];
        problem.n_operations_in_job = new int[problem.n_jobs];
        problem.processing_times = new int[problem.n_operations * problem.n_machines];
    }
    
    interval.max_size = problem.n_operations;
    interval.interval = new int[problem.n_operations];
}

void slave_unpack_payload_part2(Payload_problem_fjssp& problem) {
    
    printf("[%d] Jobs: %d Operations: %d Machines: %d\n", my_rank, problem.n_jobs, problem.n_operations,
           problem.n_machines);
    
    printf("[%d] ", my_rank);
    for (int n_job = 0; n_job < problem.n_jobs; ++n_job)
        printf("%d ", problem.release_times[n_job]);
    printf("\n");
    
    printf("[%d] ", my_rank);
    for (int n_job = 0; n_job < problem.n_jobs; ++n_job)
        printf("%d ", problem.n_operations_in_job[n_job]);
    printf("\n");
    
    printf("[%d] ", my_rank);
    for (int proc_t = 0; proc_t < problem.n_operations * problem.n_machines; ++proc_t)
        printf("%d ", problem.processing_times[proc_t]);
    printf("\n");
    
}

void prepare_payload_problem_part1(const Payload_problem_fjssp& payload_problem, MPI_Datatype* datatype_problem) {
    const int n_blocks = 3;
    int blocks[n_blocks] = { 1, 1, 1 };
    MPI_Aint displacements_interval[n_blocks];
    MPI_Datatype types[n_blocks] = { MPI_INT, MPI_INT, MPI_INT };
    MPI_Aint addr_base, addr_n_jobs, addr_n_operations, addr_n_machines;
    
    MPI_Get_address(&payload_problem, &addr_base);
    MPI_Get_address(&(payload_problem.n_jobs), &addr_n_jobs);
    MPI_Get_address(&(payload_problem.n_operations), &addr_n_operations);
    MPI_Get_address(&(payload_problem.n_machines), &addr_n_machines);
    
    displacements_interval[0] = static_cast<MPI_Aint>(0);
    displacements_interval[1] = addr_n_operations - addr_base;
    displacements_interval[2] = addr_n_machines - addr_base;
    
    MPI_Type_create_struct(n_blocks, blocks, displacements_interval, types, datatype_problem);
    MPI_Type_commit(datatype_problem);
}

void prepare_payload_problem_part2(const Payload_problem_fjssp& payload_problem, MPI_Datatype* datatype_problem) {
    const int n_blocks = 6;
    int blocks[n_blocks] = { 1, 1, 1, payload_problem.n_jobs, payload_problem.n_jobs, payload_problem.n_operations
        * payload_problem.n_machines };
    MPI_Aint displacements_interval[n_blocks];
    MPI_Datatype types[n_blocks] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT };
    MPI_Aint addr_base, addr_n_jobs, addr_n_operations, addr_n_machines, addr_release_times, addr_n_oper_in_job,
    addr_processing_times;
    
    MPI_Get_address(&payload_problem, &addr_base);
    MPI_Get_address(&(payload_problem.n_jobs), &addr_n_jobs);
    MPI_Get_address(&(payload_problem.n_operations), &addr_n_operations);
    MPI_Get_address(&(payload_problem.n_machines), &addr_n_machines);
    MPI_Get_address(payload_problem.release_times, &addr_release_times);
    MPI_Get_address(payload_problem.n_operations_in_job, &addr_n_oper_in_job);
    MPI_Get_address(payload_problem.processing_times, &addr_processing_times);
    
    displacements_interval[0] = static_cast<MPI_Aint>(0);
    displacements_interval[1] = addr_n_operations - addr_base;
    displacements_interval[2] = addr_n_machines - addr_base;
    displacements_interval[3] = addr_release_times - addr_base;
    displacements_interval[4] = addr_n_oper_in_job - addr_base;
    displacements_interval[5] = addr_processing_times - addr_base;
    
    MPI_Type_create_struct(n_blocks, blocks, displacements_interval, types, datatype_problem);
    MPI_Type_commit(datatype_problem);
}

void prepare_payload_solution(const Payload_solution& payload_solution, MPI_Datatype* datatype_solution) {
    const int n_blocks = 6;
    int blocks[n_blocks] = { 1, 1, 1, payload_solution.n_objectives, payload_solution.n_variables };
    MPI_Aint displacements_interval[n_blocks];
    MPI_Datatype types[n_blocks] = { MPI_INT, MPI_INT, MPI_INT, MPI_DOUBLE, MPI_INT };
    MPI_Aint addr_base, addr_n_obj, addr_n_var, addr_build, addr_obj, addr_var;
    
    MPI_Get_address(&payload_solution, &addr_base);
    MPI_Get_address(&(payload_solution.n_objectives), &addr_n_obj);
    MPI_Get_address(&(payload_solution.n_variables), &addr_n_var);
    MPI_Get_address(&(payload_solution.build_up_to), &addr_build);
    MPI_Get_address(payload_solution.objective, &addr_obj);
    MPI_Get_address(payload_solution.variable, &addr_var);
    
    displacements_interval[0] = static_cast<MPI_Aint>(0);
    displacements_interval[1] = addr_n_var - addr_base;
    displacements_interval[2] = addr_build - addr_base;
    displacements_interval[3] = addr_obj - addr_base;
    displacements_interval[4] = addr_var - addr_base;
    
    MPI_Type_create_struct(n_blocks, blocks, displacements_interval, types, datatype_solution);
    MPI_Type_commit(datatype_solution);
}

void prepare_payload_interval(Payload_interval& payload_interval, MPI_Datatype* datatype_interval) {
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
    
    MPI_Type_create_struct(n_blocks, blocks, displacements_interval, types, datatype_interval);
    MPI_Type_commit(datatype_interval);
}

void run_master_node(Payload_interval& messge_interval, MPI_Datatype* datatype_interval) {
    /**
     * This is the Master node.
     * Repeat send one interval to each node.
     * - Generates an interval for each node.
     **/
    int node_dest = 0;
    
    for (node_dest = 1; node_dest < n_processes; node_dest++) {
        
        // Generates an interval.
        messge_interval.priority = node_dest;
        messge_interval.deep = node_dest;
        messge_interval.build_up_to = node_dest;
        messge_interval.distance[0] = 0.3;
        messge_interval.distance[1] = 0.7;
        
        for (int element = 0; element < messge_interval.max_size; ++element)
            messge_interval.interval[element] = element * node_dest;
        
        printf("[%d] Sending: %d %d %d %d %f %f\n",
               MASTER_NODE,
               messge_interval.priority,
               messge_interval.deep,
               messge_interval.build_up_to,
               messge_interval.max_size,
               messge_interval.distance[0],
               messge_interval.distance[1]);
        
        printf("[%d] ", MASTER_NODE);
        for (int element = 0; element < messge_interval.max_size; ++element)
            printf("%d ", messge_interval.interval[element]);
        printf("\n");
        
        MPI_Send(&messge_interval, 1, *datatype_interval, node_dest, TAG_INTERVAL, MPI_COMM_WORLD);
        
        /** Here starts the part to monitor each node. **/
    }
}

/** These are the Slaves nodes. Each slave launch a parallel B&B with the received interval. **/
void run_slave_node(int num_threads, Payload_problem_fjssp problem, Payload_interval& interval, MPI_Datatype* datatype_interval) {
    
    int source = MASTER_NODE;
    MPI_Status status; /* return status for receive */
    
    MPI_Recv(&interval, 1, *datatype_interval, source, TAG_INTERVAL, MPI_COMM_WORLD, &status);
    printf("[%d] Receiving: %d %d %d %d %f %f\n",
           my_rank,
           interval.priority,
           interval.deep,
           interval.build_up_to,
           interval.max_size,
           interval.distance[0],
           interval.distance[1]);
    
    printf("[%d] ", my_rank);
    for (int element = 0; element < interval.max_size; ++element)
        printf("%d ", interval.interval[element]);
    printf("\n");
    
    /**
     * Starts a parallel B&B with the received interval.
     * - Parallel B&B Broadcast new solutions to all nodes.
     **/
}
/**
 * argv[1] = number of threads.
 * argv[2] = problem: TSP, HCSP, VRP, and FJSSP.
 * argv[3] = First input file of instance.
 * argv[4] = Second input file of instance.
 * argv[5] = output folder for Pareto front, the file name is given by the instance name.
 */

int main(int argc, char * argv[]) {

    const int arg_num_threads = 1;
    //const int arg_problem = 2;
    const int arg_input_file = 3;
//    const int arg_input_file2 = 4;
//    const int arg_output = 5;
    

    /** Starts MPI version. **/
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
    
    Payload_problem_fjssp payload_problem; /** Storage to send and receive the problem data. **/
    Payload_interval payload_interval; /** Storage for sending-receiving intervals. **/
    MPI_Datatype datatype_problem; /** For committing. **/
    MPI_Datatype datatype_interval; /** For committing. **/
    
    if (my_rank == MASTER_NODE)
        load_instance(payload_problem, argv[arg_input_file]); /** Reads the instance and prepares the pay load to be broadcasted to all nodes. **/
    
    prepare_payload_problem_part1(payload_problem, &datatype_problem);
    MPI_Bcast(&payload_problem, 1, datatype_problem, MASTER_NODE, MPI_COMM_WORLD); /* The MASTER_NODE broadcast the part 1 of payload and the slaves nodes receives it. */
    
    unpack_payload_part1(payload_problem, payload_interval); /** Each node initialize their structures. The MASTER_NODE has initialized them from the load_instance. **/
    
    prepare_payload_problem_part2(payload_problem, &datatype_problem); /** Prepares the second part.**/
    MPI_Bcast(&payload_problem, 1, datatype_problem, MASTER_NODE, MPI_COMM_WORLD); /** The MASTER_NODE Broadcast the pending arrays. **/
    
    prepare_payload_interval(payload_interval, &datatype_interval); /** Each node prepares the payload. **/
    
    if (my_rank == MASTER_NODE) {
        run_master_node(payload_interval, &datatype_interval);
    } else {
        run_slave_node(stoi(argv[arg_num_threads]), payload_problem, payload_interval, &datatype_interval);
    }
    
    /* Shut down MPI */
    MPI_Type_free(&datatype_interval);
    MPI_Type_free(&datatype_problem);
    MPI_Finalize();
    return 0;
/** Ends MPI version.**/
    
	return 0;
}
