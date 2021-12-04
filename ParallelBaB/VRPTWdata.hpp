//
//  VRPTWdata.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 9/10/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef VRPTWdata_hpp
#define VRPTWdata_hpp

#include <stdio.h>
#include <string>
#include <math.h>

enum VRPTW_INFEASIBILITY{FIRST_NODE_IS_DEPOT, CONSECUTIVE_DEPOTS, CUSTOMER_VISITED, COMPLETE_SOLUTION, TIME_WINDOW, CAPACITY, NONE};

class VRPTWdata {

private:

    unsigned int number_of_customers;
    unsigned int max_number_of_vehicles;
    unsigned int max_capacity;

    unsigned int n_vehicles;
    unsigned int n_dispatched_customers;
    unsigned int current_node;
    unsigned int current_position;

    double * vehicle_cost;
    double * travel_time;
    unsigned int * capacity;
    double * ended_service;

    unsigned int * variable_times_used;

    double total_cost = 0;
    double max_cost = 0;
    double total_travel_time = 0;
    double max_travel_time = 0;

    bool is_feasible;
    bool is_complete;

    VRPTW_INFEASIBILITY infeasiblity_type;

public:
    VRPTWdata(unsigned int number_of_customers, unsigned int max_number_of_vehicles, unsigned int max_capacity);
    VRPTWdata(const VRPTWdata& toCopy);
    ~VRPTWdata();

    VRPTWdata& operator()(unsigned int number_of_customers, unsigned int max_number_of_vehicles, unsigned int max_capacity);

    void increaseNumberOfVehicles(unsigned int by_n);
    void increaseNumberOfDispatchedCustomers(unsigned int by_n);
    void increaseCurrentPosition(unsigned int by_n);

    void decreaseNumberOfVehicles(unsigned int by_n);
    void decreaseNumberOfDispatchedCustomers(unsigned int by_n);
    void decreaseCurrentPosition(unsigned int by_n);

    void increaseCurrentVehicleCost(double by_n);
    void increaseCurrentVehicleTravelTime(double by_n);
    void increaseCurrentVehicleCapacity(unsigned int by_n);

    void decreaseCurrentVehicleCost(double by_n);
    void decreaseCurrentVehicleTravelTime(double by_n);
    void decreaseCurrentVehicleCapacity(unsigned int by_n);

    void setNumberOfVehicles(unsigned int new_n_routes);
    void setNumberOfDispatchedCustomers(unsigned int dispatched_customers);
    void setCurrentPosition(unsigned int position);
    void setCurrentCustomer(unsigned int customer);
    void setCurrentNode(unsigned int customer);

    void setCurrentVehicleCost(double to_n);
    void setCurrentVehicleTravelTime(double to_n);
    void setCurrentVehicleCapacity(unsigned int to_n);

    void setCustomerServiceEndedAt(unsigned int customer, unsigned int time);

    void setTotalTravelTime(double new_travel_time);
    void setMaxCost(double new_max_cost);
    void setTotalCost(double new_total_cost);

    unsigned int getNumberOfNodes() const;
    unsigned int getNumberOfCustomers() const;
    unsigned int getMaxNumberOfVehicles() const;
    unsigned int getMaxVehicleCapacity() const;
    unsigned int getCurrentNode() const; /** Returns the current customer or depot. **/
    unsigned int getPosition() const;
    unsigned int getNumberOfRoutes() const;
    unsigned int getNumberOfDispatchedCustomers() const;

    void updateTravelTime();
    
    double getCurrentVehicleCost() const;
    double getCurrentVehicleTravelTime() const;
    unsigned int getCurrentVehicleCapacity() const;

    double getVehicleCost(unsigned int n_route) const;
    double getVehicleTravelTime(unsigned int n_route) const;
    unsigned int getVehicleCapacity(unsigned int n_route) const;

    double getCustomerServiceEndedAt(unsigned int customer) const;

    void createNewVehicle(unsigned int max_capacity);
    //void removeLastVehicle(unsigned int customer);

    void increaseTimesElementIsInUse(unsigned int element);
    void decreaseTimesElementIsInUse(unsigned int element);
    unsigned int getTimesThatElementAppears(unsigned int element) const;

    unsigned int getNumberOfVehiclesUsed() const;
    double getMaxCost() const;
    double getTotalCost() const;
    double getToltaTravelTime() const;
    double getMaxTravelTime() const;
    double getObjective(int n_obj) const;

    VRPTW_INFEASIBILITY getInfeasibilityType() const;
    void setInfeasibilityType(VRPTW_INFEASIBILITY in_type);
    void setInfeasibleByFirstNodeDepot();
    void setInfeasibleByConsecutiveDepots();
    void setInfeasibleByCustomerVisited();
    void setInfeasibleByCompleteSolution();
    void setInfeasibleByTimeWindow();
    void setInfeasibleByCapacity();

    bool isFeasible() const;
    void setFeasible();
    void setInfeasible();

    bool isComplete() const;
    void setComplete();
    void setIncomplete();

    void reset();
    void print() const;

};

#endif /* VRPTWdata_hpp */