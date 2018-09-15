//
//  VRPTWdata.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 9/10/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "VRPTWdata.hpp"

VRPTWdata::VRPTWdata(unsigned int number_of_customers, unsigned int max_number_of_vehicles, unsigned int max_capacity):
number_of_customers(number_of_customers),
max_number_of_vehicles(max_number_of_vehicles),
max_capacity(max_capacity),
n_vehicles(0),
n_dispatched_customers(0),
current_node(0),
current_position(0),
vehicle_cost(new double[max_number_of_vehicles]),
travel_time(new double[max_number_of_vehicles]),
capacity(new unsigned int[max_number_of_vehicles]),
ended_service(new double[number_of_customers]),
total_cost(0),
max_cost(0),
total_travel_time(0),
max_travel_time(0),
is_feasible(true),
is_complete(false) {
    for (unsigned int vehicle = 0; vehicle < getMaxNumberOfVehicles(); ++vehicle) {
        capacity[vehicle] = getMaxVehicleCapacity();
        vehicle_cost[vehicle] = 0;
        travel_time[vehicle] = 0;
    }

    for (unsigned int customer = 0; customer < getNumberOfCustomers(); ++customer)
        ended_service[customer] = 0;
}

VRPTWdata::VRPTWdata(const VRPTWdata& toCopy):
number_of_customers(toCopy.getNumberOfCustomers()),
max_number_of_vehicles(toCopy.getMaxNumberOfVehicles()),
max_capacity(toCopy.getMaxVehicleCapacity()),
n_vehicles(toCopy.getNumberOfRoutes()),
n_dispatched_customers(toCopy.getNumberOfDispatchedCustomers()),
current_node(toCopy.getCurrentNode()),
current_position(toCopy.getPosition()),
vehicle_cost(new double[max_number_of_vehicles]),
travel_time(new double[max_number_of_vehicles]),
capacity(new unsigned int[max_number_of_vehicles]),
ended_service(new double[number_of_customers]),
total_cost(toCopy.getTotalCost()),
max_cost(toCopy.getMaxCost()),
total_travel_time(toCopy.getToltaTravelTime()),
max_travel_time(toCopy.getMaxTravelTime()),
is_feasible(toCopy.isFeasible()),
is_complete(toCopy.isComplete()) {

    for (unsigned int route = 0; route < toCopy.getNumberOfRoutes(); ++route) {
        vehicle_cost[route] = toCopy.getVehicleCost(route);
        travel_time[route] = toCopy.getVehicleTravelTime(route);
        capacity[route] = toCopy.getVehicleCapacity(route);
    }

    for (unsigned int customer = 0; customer < toCopy.getNumberOfCustomers(); ++customer) 
        ended_service[customer] = toCopy.getCustomerServiceEndedAt(customer);

}

VRPTWdata::~VRPTWdata() {
    delete [] vehicle_cost;
    delete [] travel_time;
    delete [] capacity;
    delete [] ended_service;
}

void VRPTWdata::increaseNumberOfVehicles(unsigned int by_n) {
    n_vehicles += by_n;
}

void VRPTWdata::increaseNumberOfDispatchedCustomers(unsigned int by_n) {
    n_dispatched_customers += by_n;
    if (getNumberOfDispatchedCustomers() == getNumberOfCustomers())
        setComplete();
}

void VRPTWdata::increaseCurrentPosition(unsigned int by_n) {
    current_position += by_n;
}

void VRPTWdata::decreaseNumberOfVehicles(unsigned int by_n) {
    n_vehicles -= by_n;
}

void VRPTWdata::decreaseNumberOfDispatchedCustomers(unsigned int by_n) {
    n_dispatched_customers -= by_n;
    if (getNumberOfDispatchedCustomers() < getNumberOfCustomers())
        setIncomplete();
}

void VRPTWdata::decreaseCurrentPosition(unsigned int by_n) {
    current_position -= by_n;
}

void VRPTWdata::increaseCurrentVehicleCost(double by_n) {
    vehicle_cost[n_vehicles] += by_n;
    total_cost += by_n;
    if (vehicle_cost[n_vehicles] > max_cost)
        max_cost = vehicle_cost[n_vehicles];
}

void VRPTWdata::increaseCurrentVehicleTravelTime(double by_n) {
    travel_time[n_vehicles] += by_n;
    if (travel_time[n_vehicles] > max_travel_time)
        max_travel_time = travel_time[n_vehicles];
}
void VRPTWdata::increaseCurrentVehicleCapacity(unsigned int by_n) {
    capacity[n_vehicles] += by_n;
}

void VRPTWdata::decreaseCurrentVehicleCost(double by_n) {
    vehicle_cost[n_vehicles] -= by_n;
    total_cost -= by_n;

    for (unsigned int vehicle = 0; vehicle < n_vehicles; ++vehicle)
        if (vehicle_cost[vehicle] > max_cost)
            max_cost = vehicle_cost[vehicle];
}

void VRPTWdata::decreaseCurrentVehicleTravelTime(double by_n) {
    travel_time[n_vehicles] -= by_n;

    for (unsigned int vehicle = 0; vehicle < n_vehicles; ++vehicle)
        if (travel_time[vehicle] > max_travel_time)
            max_travel_time = travel_time[vehicle];
}

void VRPTWdata::decreaseCurrentVehicleCapacity(unsigned int by_n) {
    capacity[n_vehicles] -= by_n;
}

void VRPTWdata::setNumberOfVehicles(unsigned int new_n_routes) {
    n_vehicles = new_n_routes;
}

void VRPTWdata::setNumberOfDispatchedCustomers(unsigned int dispatched_customers) {
    n_dispatched_customers = dispatched_customers;
    if (n_dispatched_customers < getNumberOfCustomers())
        is_complete = false;
}

void VRPTWdata::setCurrentPosition(unsigned int position) {
    current_position = position;
}

void VRPTWdata::setCurrentCustomer(unsigned int customer) {
    current_node = customer;
}

void VRPTWdata::setCurrentNode(unsigned int customer) {
    current_node = customer;
}

void VRPTWdata::setCurrentVehicleCost(double to_n) {
    vehicle_cost[n_vehicles] = to_n;
}

void VRPTWdata::setCurrentVehicleTravelTime(double to_n) {
    travel_time[n_vehicles] = to_n;
    if (travel_time[n_vehicles] > max_travel_time)
        max_travel_time = travel_time[n_vehicles];
}

void VRPTWdata::setCurrentVehicleCapacity(unsigned int to_n) {
    capacity[n_vehicles] = to_n;
}

void VRPTWdata::setCustomerServiceEndedAt(unsigned int customer, unsigned int time) {
    ended_service[customer] = time;
}

void VRPTWdata::setTotalTravelTime(double new_travel_time) {
    total_travel_time = new_travel_time;
}

void VRPTWdata::setMaxCost(double new_max_cost) {
    max_cost = new_max_cost;
}

void VRPTWdata::setTotalCost(double new_total_cost) {
    total_cost = new_total_cost;
}

unsigned int VRPTWdata::getNumberOfCustomers() const {
    return number_of_customers;
}

unsigned int VRPTWdata::getMaxNumberOfVehicles() const {
    return max_number_of_vehicles;
}

unsigned int VRPTWdata::getMaxVehicleCapacity() const {
    return max_capacity;
}

/** Returns the current node value. **/
unsigned int VRPTWdata::getCurrentNode() const {
    return current_node;
}

unsigned int VRPTWdata::getPosition() const {
    return current_position;
}

unsigned int VRPTWdata::getNumberOfRoutes() const {
    return n_vehicles;
}

unsigned int VRPTWdata::getNumberOfDispatchedCustomers() const {
    return n_dispatched_customers;
}

double VRPTWdata::getCurrentVehicleCost() const {
    return vehicle_cost[n_vehicles];
}

double VRPTWdata::getCurrentVehicleTravelTime() const {
    return travel_time[n_vehicles];
}

unsigned int VRPTWdata::getCurrentVehicleCapacity() const {
    return capacity[n_vehicles];
}

double VRPTWdata::getVehicleCost(unsigned int n_route) const {
    return vehicle_cost[n_route];
}

double VRPTWdata::getVehicleTravelTime(unsigned int n_route) const {
    return travel_time[n_route];
}

unsigned int VRPTWdata::getVehicleCapacity(unsigned int n_route) const {
    return capacity[n_route];
}

/** The index of customers starts at 1.**/
double VRPTWdata::getCustomerServiceEndedAt(unsigned int customer) const {
    return ended_service[customer - 1];
}

void VRPTWdata::createNewVehicle(unsigned int max_capacity) {
    increaseNumberOfVehicles(1);
    setCurrentNode(0); /** Node 0 is the depot. **/
    setCurrentVehicleCapacity(max_capacity);
    setCurrentVehicleCost(0);
    setCurrentVehicleTravelTime(0);
}

void VRPTWdata::removeLastVehicle(unsigned int customer) {
    decreaseNumberOfVehicles(1);
    setCurrentNode(customer);
}

unsigned int VRPTWdata::getNumberOfVehiclesUsed() const {
    return n_vehicles + 1; /** + 1 to count the vehicle with id 0. **/
}

double VRPTWdata::getMaxCost() const {
    return max_cost;
}

double VRPTWdata::getTotalCost() const {
    return total_cost;
}

double VRPTWdata::getToltaTravelTime() const {
    return total_travel_time;
}

double VRPTWdata::getMaxTravelTime() const {
    return max_travel_time;
}

double VRPTWdata::getObjective(int n_obj) const {
    switch (n_obj) {
        case 0:
            return total_cost;
            break;

        case 1:
            return n_vehicles;
            break;

        case 2:
            return max_cost;
            break;

        case 3:
            return total_travel_time;
            break;

        default:
            break;
    }
    return -1;
}

bool VRPTWdata::isFeasible() const {
    return is_feasible;
}

void VRPTWdata::setFeasible() {
    is_feasible = true;
}

void VRPTWdata::setInfeasible() {
    is_feasible = false;
}

bool VRPTWdata::isComplete() const {
    return is_complete;
}

void VRPTWdata::setComplete() {
    is_complete = true;
}

void VRPTWdata::setIncomplete() {
    is_complete = false;
}

void VRPTWdata::print() const {
    printf("***********\n");
    printf("Position: %d", getPosition());
    for (unsigned int route = 0; route < getMaxNumberOfVehicles(); ++route) {
        printf("[%d] Cost: %6.6f\n", route, vehicle_cost[route]);
        printf("[%d] Travel time: %6.6f\n", route, travel_time[route]);
        printf("[%d] Capacity: %d\n", route, capacity[route]);
    }

    /* for (unsigned int customer = 0; customer < getNumberOfCustomers(); ++customer)
        printf("%d -> %6.3f\n", customer, ended_service[customer]);
     */
    printf("***********\n");

}
