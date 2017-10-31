/*
 * MasterWorker.h
 *
 *  Created on: 22/09/2017
 *      Author: carlossoto
 */

#ifndef MASTERWORKER_HPP_
#define MASTERWORKER_HPP_

#include <mpi.h>
#include <stdio.h>
#include <chrono>
#include <tbb/task.h>
#include "BranchAndBound.hpp"
#include "Interval.hpp"
#include "ProblemFJSSP.hpp"
#include "Solution.hpp"
#include "myutils.hpp"

class MasterWorkerPBB : public tbb::task {
public:
    MasterWorkerPBB();
    MasterWorkerPBB(int num_nodes, int num_threads, const char file[]);
    virtual ~MasterWorkerPBB();
    
    void run();
    int getRank() const;
    int getSizeWorkers() const;
    int isMaster() const;
    int isWorker() const;
    tbb::task* execute();
    
private:
    Payload_problem_fjssp payload_problem;
    Payload_interval payload_interval;
    Payload_solution payload_solution;
    //Payload_interval payload_solutions[10]; /** Sending 10 solutions at the time. Recycling the interval struct because it has the same values. **/
    
    MPI_Datatype datatype_problem; /** For committing. **/
    MPI_Datatype datatype_interval; /** For committing. **/
    MPI_Datatype datatype_solution; /** For committing. **/
    //MPI_Datatype datatyple_solutions; /** For committing. **/
    
    int rank;
    int n_workers;
    int threads_per_node;
    //    int sleeping_workers;
    MPI_Status status;
    ProblemFJSSP problem;
    Interval branch_init;
    
    char file[255];
    
    static const int MASTER_RANK = 0;
    static const int TAG_INTERVAL = 191;
    static const int TAG_SOLUTION = 192;
    static const int TAG_FINISH_WORK = 193;
    static const int TAG_WORKER_READY = 194;
    static const int TAG_NO_MORE_WORK = 195;
    static const int TAG_REQUEST_MORE_WORK = 196;
    static const int TAG_SHARE_WORK = 197;
    
    char TAGS[8][50];
    
    void loadInstance(Payload_problem_fjssp& problem, const char filePath[]);
    void preparePayloadProblemPart1(const Payload_problem_fjssp& problem, MPI_Datatype& datatype_problem);
    void preparePayloadProblemPart2(const Payload_problem_fjssp& problem, MPI_Datatype& datatype_problem);
    void preparePayloadSolution(const Payload_solution& solution, MPI_Datatype& datatype_solution);
    //void preparePayloadSolutions(const Payload_interval* solutions, MPI_Datatype& datatype_solutions);
    void preparePayloadInterval(const Payload_interval& interval, MPI_Datatype& datatype_interval);
    void storesPayloadInterval(Payload_interval& payload, const Interval& interval);
    void storesSolutionInInterval(Payload_interval& payload, const Solution& solution);
    void recoverSolutionFromPayload(const Payload_interval& payload, Solution& solution);
    void unpack_payload_part1(Payload_problem_fjssp& problem, Payload_interval& interval, Payload_solution& solution);
    void unpack_payload_part2(Payload_problem_fjssp& problem);
    void runMasterProcess();
    void runWorkerProcess();
    void printPayloadInterval() const;
    int splitInterval(Interval& branch_to_split);
    void printMessageStatus(int source, int tag);
    float distanceToObjective(int value, int objective) const; /** This function is also in BranchAndBound class. TODO: Fix later.**/

};

#endif /* MASTERWORKER_HPP_ */
