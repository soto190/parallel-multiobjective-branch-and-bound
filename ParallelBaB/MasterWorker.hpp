/*
 * MasterWorker.h
 *
 *  Created on: 22/09/2017
 *      Author: carlossoto
 */

#ifndef MASTERWORKER_HPP_
#define MASTERWORKER_HPP_

#include <stdio.h>
#include <mpi.h>
#include "ProblemFJSSP.hpp"
#include "Solution.hpp"
#include "Interval.hpp"
/*
 *
 */
class MasterWorker {
public:
    MasterWorker();
    MasterWorker(int num_workers, int num_threads, const char file[]);
    virtual ~MasterWorker();
    
    void run();
    int getRank();
    int getSizeWorkers();
    int isMaster();
    int isWorker();
    
private:
    Payload_problem_fjssp payload_problem;
    Payload_interval payload_interval;
    
    MPI_Datatype datatype_problem; /** For committing. **/
    MPI_Datatype datatype_interval; /** For committing. **/
    
    int rank;
    int n_workers;
    int threads_per_node;
    MPI_Status status;
    ProblemFJSSP problem;
    
    static const int MASTER_RANK = 0;
    static const int TAG_INTERVAL = 190;
    static const int TAG_SOLUTION = 191;
    static const int TAG_FINISH_WORK = 192;
    static const int TAG_SLAVE_READY = 193;
    
    char file[255];
    
    void loadInstance(Payload_problem_fjssp& problem, const char filePath[]);
    
    void preparePayloadProblemPart1(const Payload_problem_fjssp& problem, MPI_Datatype& datatype_problem);
    void preparePayloadProblemPart2(const Payload_problem_fjssp& problem, MPI_Datatype& datatype_problem);
    void preparePayloadSolution(const Payload_solution& solution, MPI_Datatype& datatype_solution);
    void preparePayloadInterval(const Payload_interval& interval, MPI_Datatype& datatype_interval);
    
    void unpack_payload_part1(Payload_problem_fjssp& problem, Payload_interval& interval);
    void unpack_payload_part2(Payload_problem_fjssp& problem);
    void runMasterProcess();
    void runWorkerProcess();
};

#endif /* MASTERWORKER_HPP_ */

