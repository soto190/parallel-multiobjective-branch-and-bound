//
//  ProblemVRPTW.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 8/31/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef ProblemVRPTW_hpp
#define ProblemVRPTW_hpp

#include <stdio.h>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

#include "Problem.hpp"
#include "VRPTWdata.hpp"
#include "myutils.hpp"

typedef struct {
    unsigned int n_objectives;
    unsigned int max_number_of_vehicles;
    unsigned int max_vehicle_capacity;
    unsigned int number_of_customers;
    unsigned int * coordinates;  /** Length is (number_of_customers + depot) x 2. **/
    unsigned int * time_window;  /** Length is (number_of_customers + depot) x 2. **/
    unsigned int * service_time; /** Length is (number_of_customers + depot). **/
    unsigned int * demand;       /** Length is (number_of_customers + depot). **/
} Payload_problem_vrptw;

enum VRTPW_OBJECTIVES {NUMBER_OF_VEHICLES, TOTAL_COST, CRITICAL_COST, TOTAL_TRAVEL_TIME, CRITICAL_TRAVEL_TIME};

class ProblemVRPTW: public Problem {

private:
    unsigned int ** coordinates; /** A matrix of n x 2 with the ubication of each customer. The index 0 is the depot. The first column is the x coordinate and the second column is the y coord. **/
    double** costs;         /** A matrix of costs. This can be the euclidean distance between customers. **/
    unsigned int** time_window;   /** A vector of n x 2. The first column is the starting time window, the second column is the ending time window. **/
    unsigned int* service_time;   /** Service time required for each customer. **/
    unsigned int* demand;         /** Demand of each customer. **/
    unsigned int number_of_customers;
    unsigned int max_vehicle_capacity;
    unsigned int max_number_of_vehicles;

    unsigned int total_demand;
    unsigned int total_service_time;
    double total_distance_in_order;

    /** The diference between the variables found with lower_bound are that found variables corresponds to solutions.
     * Lower bound variables are the minimum achiviable values.
     */
    double min_cost_found;
    double max_cost_found;
    double min_critical_cost_found;
    double max_critical_cost_found;
    unsigned int min_number_vehicles_found;
    unsigned int max_number_vehicles_found;
    unsigned int min_makespan_found;
    unsigned int max_makespan_found;

public:

    ProblemVRPTW();
    ProblemVRPTW(int number_of_objectives, int number_of_variables);
    ProblemVRPTW(const ProblemVRPTW& toCopy);
    ProblemVRPTW(const Payload_problem_vrptw& payload_problem);
    ~ProblemVRPTW();

    ProblemVRPTW& operator=(const ProblemVRPTW& toCopy);

    double evaluate(Solution & solution);
    void evaluateDynamic(Solution & solution, VRPTWdata & data, int level);
    void evaluateRemoveDynamic(Solution & solution, VRPTWdata& data, int level);

    double evaluatePartial(Solution & solution, int levelEvaluation);
    double evaluateLastLevel(Solution * solution);
    double removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel);
    double removeLastLevelEvaluation(Solution * solution, int newLevel);

    void createDefaultSolution(Solution & solution);
    void getSolutionWithLowerBoundInObj(int nObj, Solution& solution);

    int getLowerBound(int indexVar) const;
    int getUpperBound(int indexVar) const;
    double getLowerBoundInObj(int nObj) const;
    double getUpperBoundInObj(int nObj) const;

    ProblemType getType() const;
    int getStartingRow();
    int getFinalLevel();

    int * getElemensToRepeat();
    int getTotalElements();
    int getDecodeMap(int map, int position) const;
    int getEncodeMap(int value1, int value2) const;
    int getTimesThatValueCanBeRepeated(int value);

    void printSolution(const Solution & solution) const;
    void printPartialSolution(const Solution & solution, int level) const;
    void printSolutionInfo(const Solution & solution) const;

    void printInstance() const;
    void printProblemInfo() const;
    void loadInstance(char filePath[2][255], char file_extension[4]);
    void loadInstancePayload(const Payload_problem_vrptw& payload);

    /** VRPTW functions **/
    unsigned int getNumberOfCustomers() const;
    unsigned int getMaxVehicleCapacity() const;
    unsigned int getMaxNumberOfVehicles() const;
    unsigned int getCustomerCoordinate(unsigned int customer, unsigned int coordinate) const;
    double getCustomerCostTo(unsigned int customer_origin, unsigned int customer_destination) const;
    unsigned int getCustomerServiceTime(unsigned int customer) const;
    unsigned int getCustomerDemand(unsigned int customer) const;
    unsigned int getCustomerTimeWindowStart(unsigned int customer) const;
    unsigned int getCustomerTimeWindowEnd(unsigned int customer) const;
    unsigned int getNumberOfNodes() const;

    double getFmin(int n_objective) const;
    double getFmax(int n_objective) const;
    double getBestObjectiveFoundIn(int n_objective) const;

    void updateBestSolutionInObjectiveWith(unsigned int n_obj, const Solution& solution);

    void updateBestBoundsWith(const Solution& solution);
    void updateBestBoundsWithSolution(const Solution& solution);

    //void loadInstancePayload(const Payload_problem_fjssp& payload);
    bool validateVariables(Solution& solution);
    void heuristic(Solution& solution);
    void heuristic_min_dist(Solution& solution);
private:
    double euclideanDistance(unsigned int xcoord_1, unsigned int ycoord_1, unsigned int xcoord_2, unsigned int ycoord_2) const;
    bool isCustomer(unsigned int node) const;
    bool isDepot(unsigned int node) const;
    double trunc(double value) const;
};
#endif /* ProblemVRPTW_hpp */
