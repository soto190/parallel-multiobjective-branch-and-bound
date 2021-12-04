//
//  ProblemVRPTWTW.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 8/31/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "ProblemVRPTW.hpp"

ProblemVRPTW::ProblemVRPTW():
Problem(0, 0),
number_of_customers(0),
max_vehicle_capacity(0),
max_number_of_vehicles(0),
min_cost_found(0),
max_cost_found(0),
min_number_vehicles_found(0),
max_number_vehicles_found(0),
min_makespan_found(0),
max_makespan_found(0),
total_demand(0),
coordinates(nullptr),
costs(nullptr),
time_window(nullptr),
service_time(nullptr),
demand(nullptr) {

}

ProblemVRPTW::ProblemVRPTW(int number_of_objectives, int number_of_variables):
Problem(number_of_objectives, number_of_variables),
number_of_customers(0),
max_vehicle_capacity(0),
max_number_of_vehicles(0),
total_demand(0),
min_cost_found(0),
max_cost_found(0),
min_number_vehicles_found(0),
max_number_vehicles_found(0),
min_makespan_found(0),
max_makespan_found(0),
coordinates(nullptr),
costs(nullptr),
time_window(nullptr),
service_time(nullptr),
demand(nullptr) {

}

ProblemVRPTW::ProblemVRPTW(const ProblemVRPTW& toCopy):
Problem(toCopy),
number_of_customers(toCopy.getNumberOfCustomers()),
max_vehicle_capacity(toCopy.getMaxVehicleCapacity()),
max_number_of_vehicles(toCopy.getMaxNumberOfVehicles()),
min_number_vehicles_found(toCopy.getFmin(0)),
max_number_vehicles_found(toCopy.getFmax(0)),
min_critical_cost_found(toCopy.getFmin(1)),
max_critical_cost_found(toCopy.getFmax(1)),
min_cost_found(toCopy.getFmin(3)),
max_cost_found(toCopy.getFmax(3)) {

    coordinates = new unsigned int*[getNumberOfNodes()];
    costs = new double * [getNumberOfNodes()];
    time_window = new unsigned int*[getNumberOfNodes()];
    service_time = new unsigned int[getNumberOfNodes()];
    demand = new unsigned int[getNumberOfNodes()];

    for (unsigned int customer = 0; customer < getNumberOfNodes(); ++customer) {
        coordinates[customer] = new unsigned int[2];
        coordinates[customer][0] = toCopy.getCustomerCoordinate(customer, 0);
        coordinates[customer][1] = toCopy.getCustomerCoordinate(customer, 1);

        time_window[customer] = new unsigned int[2];
        time_window[customer][0] = toCopy.getCustomerTimeWindowStart(customer);
        time_window[customer][1] = toCopy.getCustomerTimeWindowEnd(customer);

        service_time[customer] = toCopy.getCustomerServiceTime(customer);
        demand[customer] = toCopy.getCustomerDemand(customer);
        
        costs[customer] = new double[getNumberOfNodes()];
        for (unsigned int customer_destination = 0; customer_destination < getNumberOfNodes(); ++customer_destination) {
            costs[customer][customer_destination] = toCopy.getCustomerCostTo(customer, customer_destination);
        }
    }
}

ProblemVRPTW::~ProblemVRPTW() {
    if(demand != nullptr)
        delete [] demand;

    if (service_time != nullptr)
        delete [] service_time;

    for (int customer = 0; customer < getNumberOfNodes(); ++customer) {
        if (costs[customer] != nullptr )
            delete [] costs[customer];

        if (coordinates[customer] != nullptr )
            delete [] coordinates[customer];

        if (time_window[customer] != nullptr )
            delete [] time_window[customer];
    }

    if (costs != nullptr )
        delete [] costs;

    if (coordinates != nullptr)
        delete [] coordinates;

    if (time_window != nullptr)
        delete [] time_window;
}

/**
 *Copies a solution*
 - parameters toCopy: the problem to copy.
 - return: A copy of the problem received.
 */
ProblemVRPTW& ProblemVRPTW::operator=(const ProblemVRPTW &toCopy) {

    if (this == &toCopy)
        return *this;

    if (demand != nullptr) {
        delete [] demand;
        delete [] service_time;

        for (int customer = 0; customer < getNumberOfNodes(); ++customer) {
            delete [] costs[customer];
            delete [] coordinates[customer];
            delete [] time_window[customer];
        }
        delete [] costs;
        delete [] coordinates;
        delete [] time_window;
    }

    n_objectives = toCopy.getNumberOfObjectives();
    n_variables = toCopy.getNumberOfVariables();

    number_of_customers = toCopy.getNumberOfCustomers();
    max_vehicle_capacity = toCopy.getMaxVehicleCapacity();
    max_number_of_vehicles = toCopy.getMaxNumberOfVehicles();

    min_number_vehicles_found = toCopy.getLowerBoundInObj(0);
    max_number_vehicles_found = toCopy.getUpperBoundInObj(0);
    min_critical_cost_found = toCopy.getLowerBoundInObj(1);
    max_critical_cost_found = toCopy.getUpperBoundInObj(1);
    min_cost_found = toCopy.getLowerBoundInObj(2);
    max_cost_found = toCopy.getUpperBoundInObj(2);

    coordinates = new unsigned int*[getNumberOfNodes()];
    costs = new double * [getNumberOfNodes()];
    time_window = new unsigned int*[getNumberOfNodes()];
    service_time = new unsigned int[getNumberOfNodes()];
    demand = new unsigned int[getNumberOfNodes()];

    for (unsigned int customer = 0; customer < getNumberOfNodes(); ++customer) {
        coordinates[customer] = new unsigned int[2];
        coordinates[customer][0] = toCopy.getCustomerCoordinate(customer, 0);
        coordinates[customer][1] = toCopy.getCustomerCoordinate(customer, 1);

        time_window[customer] = new unsigned int[2];
        time_window[customer][0] = toCopy.getCustomerTimeWindowStart(customer);
        time_window[customer][1] = toCopy.getCustomerTimeWindowEnd(customer);

        service_time[customer] = toCopy.getCustomerServiceTime(customer);
        demand[customer] = toCopy.getCustomerDemand(customer);

        costs[customer] = new double[number_of_customers];
        for (unsigned int customer_destination = 0; customer_destination < getNumberOfNodes(); ++customer_destination) {
            costs[customer][customer_destination] = toCopy.getCustomerCostTo(customer, customer_destination);
        }
    }
    
    return *this;
}

/**
 Remember, when evaluating a branch if there are no more customers then the solution is complete. Thus, the branch can be evaluated. If one or multiple of the next conditions occurs then the branch is infeasible. 1) The time window is not reached; 2) The demand can not be supplied. Taking as an example an instance with 25 customers and 25 vehicles. In this representation, we can have a solution with [26, 27, 28, 0, 1, 2, 3]. Notice that 26, 27, 28, and 0 are the depot. Also the 0 should not appear in the permutation because the lower bound for all the variables (in this problem) is from [1, 50].

 - Parameter solution: The solution to be evaluated.

 - Returns: The value of the first objective.

 */
double ProblemVRPTW::evaluate(Solution & solution) {

    VRPTWdata data (getNumberOfCustomers(), getMaxNumberOfVehicles(), getMaxVehicleCapacity());
    for (unsigned int var = 0; var <= solution.getBuildUpTo(); ++var)
        evaluateDynamic(solution, data, var);
    return solution.getObjective(0);

//    unsigned int n_vehicles = 0;
//    unsigned int n_dispatched_customers = 0;
//    unsigned int origin = 0;
//
//    double vehicle_cost [getMaxNumberOfVehicles()];
//    double travel_time [getMaxNumberOfVehicles()];
//    unsigned int capacity [getMaxNumberOfVehicles()];
//
//    double max_travel_time = 0;
//    double max_cost = 0;
//    double total_cost = 0;
//
//    capacity[0] = getMaxVehicleCapacity();
//    vehicle_cost[0] = 0;
//    travel_time[0] = 0;
//
//    bool is_feasible = true;
//    bool is_complete = false;
//
//    for (unsigned int idx_var = 0; idx_var < getNumberOfVariables(); ++idx_var) {
//
//        /** There are more customers to dispatch. **/
//        if (n_dispatched_customers < getNumberOfCustomers()) {
//            int destination = solution.getVariable(idx_var) <= getNumberOfCustomers() ? solution.getVariable(idx_var) : 0;
//
//            /** Validating the time window and customer demand. **/
//            if (capacity[n_vehicles] >= getCustomerDemand(destination)) {
//
//                if (travel_time[n_vehicles] <= getCustomerTimeWindowStart(destination))
//                    travel_time[n_vehicles] = getCustomerTimeWindowStart(destination) + getCustomerServiceTime(destination);
//
//                else if(travel_time[n_vehicles] >= getCustomerTimeWindowStart(destination) &&
//                        travel_time[n_vehicles] <= getCustomerTimeWindowEnd(destination))
//                    travel_time[n_vehicles] += getCustomerServiceTime(destination);
//
//                else {
//                    is_feasible = false;
//                    break; /** Not a feasible time window. **/
//                }
//
//                capacity[n_vehicles] -= getCustomerDemand(destination);
//                vehicle_cost[n_vehicles] += getCustomerCostTo(origin, destination);
//
//                if (isCustomer(destination)) {
//                    origin = destination;
//                    n_dispatched_customers++;
//                } else if (origin != destination){ /** It is a customer going to depot. **/
//                    origin = 0;
//                    n_vehicles++;
//                    capacity[n_vehicles] = getMaxVehicleCapacity();
//                    vehicle_cost[n_vehicles] = 0;
//                    travel_time[n_vehicles] = 0;
//
//                } /** else is an empty route and do nothing. **/
//
//            } else {
//                is_feasible = false;
//                break; /** Not enough capacity. **/
//            }
//        } else { /** If there are no more customers to dispatch then it is a complete solution. **/
//            vehicle_cost[n_vehicles] += costs[solution.getVariable(idx_var - 1)][0]; /** Adds cost of returning to depot. **/
//            is_complete = true;
//            break;
//        }
//    }
//
//    if (is_feasible && is_complete) {
//        for (unsigned int vehicle = 0; vehicle <= n_vehicles; ++vehicle) {
//
//            if (vehicle_cost[vehicle] > max_cost)
//                max_cost = vehicle_cost[vehicle];
//
//            if (travel_time[vehicle] > max_travel_time)
//                max_travel_time = travel_time[vehicle];
//
//            total_cost += vehicle_cost[vehicle];
//        }
//
//        solution.setObjective(0, n_vehicles + 1);
//        solution.setObjective(1, total_cost);
//        solution.setObjective(2, max_cost);
//
//    } else {
//        /** Setting bad objectives values by infeasibility. **/
//        solution.setObjective(0, max_number_of_vehicles * 2);
//        solution.setObjective(1, total_distance_in_order * 2);
//        solution.setObjective(2, total_distance_in_order * 2);
//    }
//
//    return total_cost;
}

/**
 Evaluates dynamically a solution with the help of a *data* object.
 It needs to validate:
 - The first node cannot be a depot.
 - Cannot be two consecutive depots.
 - If it is the last customer to be visited.

 The input parameters are:
 - parameters solution: solution to be evaluated.
 - parameters data: The data which contains the information of the last evaluation.
 - parameters level: Level of the vector to be evaluated.

 In this function we have to consider to remove the evaluation if the solution is infeasible.
*/
void ProblemVRPTW::evaluateDynamic(Solution &solution, VRPTWdata &data, int level) {
    //cout << "Adding: " << solution.getVariable(level) << std::endl;
    unsigned int destination = solution.getVariable(level) <= getNumberOfCustomers() ? solution.getVariable(level) : 0;

    data.setCurrentPosition(level);
    if (solution.getVariable(level) > 0 && solution.isFeasible()) {
        data.increaseTimesElementIsInUse(destination);

            /** Inserting an empty node. **/
        if (level == 0 && isDepot(destination)) {
            data.setInfeasibilityType(VRPTW_INFEASIBILITY::FIRST_NODE_IS_DEPOT);
            /** cout << "The first node can not be a depot/vehicle." << std::endl; **/

        } else if(level > 0 &&
                isDepot(destination) &&
                isDepot(solution.getVariable(level - 1))) {
            data.setInfeasibilityType(VRPTW_INFEASIBILITY::CONSECUTIVE_DEPOTS);

            /** cout << "Infeasible by consecutive depots." << std::endl; **/

        } else if(data.getTimesThatElementAppears(destination) > getTimesThatValueCanBeRepeated(destination)) {
            data.setInfeasibilityType(VRPTW_INFEASIBILITY::CUSTOMER_VISITED);
            /** This is avoided by counting the times that an element appears.**/
            /** cout << "Customer is already visited." << std::endl; **/

        } else if (data.isComplete()) {
            /* data.setInfeasibilityType(VRPTW_INFEASIBILITY::COMPLETE_SOLUTION); */
            cout << "Solution is already complete." << std::endl;

        } else {
            unsigned int previous_node = 0;
            if (level > 0)
                previous_node = solution.getVariable(level - 1) <= getNumberOfCustomers() ? solution.getVariable(level - 1) : 0;

            if(data.getCurrentVehicleCapacity() >= getCustomerDemand(destination)) {
                data.setFeasible();
                if(data.getCurrentVehicleTravelTime() <= getCustomerTimeWindowStart(destination))
                    data.setCurrentVehicleTravelTime(getCustomerTimeWindowStart(destination) + getCustomerServiceTime(destination));

                else if (data.getCurrentVehicleTravelTime() >= getCustomerTimeWindowStart(destination) &&
                         data.getCurrentVehicleTravelTime() <= getCustomerTimeWindowEnd(destination))
                    data.increaseCurrentVehicleTravelTime(getCustomerServiceTime(destination));

                else
                    data.setInfeasibilityType(VRPTW_INFEASIBILITY::TIME_WINDOW);

                if (data.isFeasible()) {

                    data.decreaseCurrentVehicleCapacity(getCustomerDemand(destination));
                    data.increaseCurrentVehicleCost(getCustomerCostTo(previous_node, destination));

                    if (isCustomer(destination)) {
                        data.setCustomerServiceEndedAt(destination, data.getCurrentVehicleTravelTime());
                        data.setCurrentNode(destination);
                        data.increaseNumberOfDispatchedCustomers(1);

                        if (data.isComplete())
                            data.increaseCurrentVehicleCost(getCustomerCostTo(destination, 0));

                    } else /** It is a customer going to depot. The cost of going to depot has been already computed. **/
                        data.createNewVehicle(getMaxVehicleCapacity());

                } else { /** It is an infeasible by time window solution and do nothing. **/}

            } else {
                data.setInfeasibilityType(VRPTW_INFEASIBILITY::CAPACITY);
            }
        }
    }
    if (data.isComplete())
        solution.setComplete();
    else
        solution.setIncomplete();

    if (data.isFeasible())
        solution.setFeasible();
    else
        solution.setInfeasible();

    for (unsigned int n_obj = 0; n_obj < getNumberOfObjectives(); ++n_obj)
        solution.setObjective(n_obj, data.getObjective(n_obj));
}

/**
  *Removes the evaluation at the level indicated.*
 - Parameters:
    - solution: Object solution to remove evaluation.
    - data: The data which contains the information of the last evaluation.
    - level: Level to be evaluated.
 */
void ProblemVRPTW::evaluateRemoveDynamic(Solution & solution, VRPTWdata& data, int level) {

    data.setCurrentPosition(level);
    if (solution.getVariable(level) != 0) {
        data.decreaseTimesElementIsInUse(solution.getVariable(level));

        //cout << "Substracting: " << solution.getVariable(level) << std::endl;

        if (level == 0) /** If we are removing the first position of the vector just do a reset. **/
            data.reset();

        else if (!data.isFeasible()) {
            switch (data.getInfeasibilityType()) {
                case FIRST_NODE_IS_DEPOT:
                    data.setFeasible();
                    break;

                case CONSECUTIVE_DEPOTS:
                    data.setFeasible();
                    break;

                case CUSTOMER_VISITED:
                    data.setFeasible();
                    break;

                /*case COMPLETE_SOLUTION:
                    data.setIncomplete(); This case is handled later.
                    break;
                    */
                case TIME_WINDOW:
                    data.setFeasible();
                    break;

                case CAPACITY:
                    data.setFeasible();
                    break;

                default:
                    break;
            }
        } else {

            unsigned int node = isCustomer(solution.getVariable(level))? solution.getVariable(level) : 0;
            unsigned int previous_node = isCustomer(solution.getVariable(level - 1))? solution.getVariable(level - 1) : 0;

            if (data.isComplete()) {
                data.decreaseCurrentVehicleCost(getCustomerCostTo(node, 0));
                data.setIncomplete();
            }

            data.increaseCurrentVehicleCapacity(getCustomerDemand(node));

            if (isCustomer(node)) {
                data.setCurrentVehicleTravelTime(isCustomer(previous_node) ? data.getCustomerServiceEndedAt(previous_node) : 0);
                data.updateTravelTime();
                data.decreaseNumberOfDispatchedCustomers(1);
                data.setCurrentNode(previous_node);
                data.setCustomerServiceEndedAt(node, 0);

            } else {
                /** If the node is a vehicle then we can reduce by 1 the number of vehicles. **/
                    data.decreaseNumberOfVehicles(1);
            }
            data.decreaseCurrentVehicleCost(getCustomerCostTo(previous_node, node));
            data.setCurrentNode(previous_node);
        }
    }

    solution.pull_back();
    if (level > 0)
        data.setCurrentPosition(solution.getBuildUpTo());

    if (data.isComplete())
        solution.setComplete();
    else
        solution.setIncomplete();

    if (data.isFeasible())
        solution.setFeasible();
    else
        solution.setInfeasible();

    for (unsigned int n_obj = 0; n_obj < getNumberOfObjectives(); ++n_obj)
        solution.setObjective(n_obj, data.getObjective(n_obj));
}

double ProblemVRPTW::getFmin(int n_objective) const {
    return getLowerBoundInObj(n_objective);
}

double ProblemVRPTW::getFmax(int n_objective) const {
    return getUpperBoundInObj(n_objective);
}

double ProblemVRPTW::getBestObjectiveFoundIn(int n_objective) const {
    switch (n_objective) {
        case 0:
            return min_number_vehicles_found;
            break;

        case 1:
            return min_critical_cost_found;
            break;

        case 2:
            return min_cost_found;
            break;

        case 3:
            return min_makespan_found;

        default:
            break;
    }
    return 0.0;
}

void ProblemVRPTW::updateBestSolutionInObjectiveWith(unsigned int n_obj, const Solution& solution) {
    for (unsigned int i_obj = 0; i_obj < solution.getNumberOfObjectives(); ++i_obj) {

        if (i_obj == 0 && solution.getObjective(i_obj) < min_number_vehicles_found)
            min_number_vehicles_found = solution.getObjective(i_obj);

        if (i_obj == 1 && solution.getObjective(i_obj) < min_critical_cost_found)
            min_critical_cost_found = solution.getObjective(i_obj);

        if (i_obj == 2 && solution.getObjective(i_obj) < min_cost_found)
            min_cost_found = solution.getObjective(i_obj);
    }
}

void ProblemVRPTW::updateBestBoundsWith(const Solution& solution) {

}

void ProblemVRPTW::updateBestBoundsWithSolution(const Solution& solution) {

}

double ProblemVRPTW::evaluatePartial(Solution & solution, int levelEvaluation) {
    return 0.0;
}

double ProblemVRPTW::evaluateLastLevel(Solution * solution) {
    return 0.0;
}

double ProblemVRPTW::removeLastEvaluation(Solution * solution, int levelEvaluation, int lastLevel) {
    return 0.0;
}

double ProblemVRPTW::removeLastLevelEvaluation(Solution * solution, int newLevel) {
    return 0.0;
}

int ProblemVRPTW::getLowerBound(int indexVar) const {
    return 1;
}

int ProblemVRPTW::getUpperBound(int indexVar) const {
    return number_of_customers + 1;
}

double ProblemVRPTW::getLowerBoundInObj(int nObj) const {
    switch (nObj) {
        case 0:
            return min_number_vehicles_found;
            break;

        case 1:
            return min_critical_cost_found;
            break;
            
        case 2:
            return min_cost_found;
            break;

        case 3:
            return min_makespan_found;
            break;
    }
    return 0.0;
}

double ProblemVRPTW::getUpperBoundInObj(int nObj) const {
    switch (nObj) {
        case 0:
            return max_number_vehicles_found;
            break;

        case 1:
            return max_critical_cost_found;

        case 2:
            return max_cost_found;
            break;

        case 3:
            return max_makespan_found;
            break;
    }
    return 0.0;
}

ProblemType ProblemVRPTW::getType() const {
    return ProblemType::permutation;
}

int ProblemVRPTW::getStartingRow() {
    return 0;
}

int ProblemVRPTW::getFinalLevel() {
    return 0;
}

/**
 * In this problem this can be number_of_customers + depot. Assign to the depot the number (customers + 1) and the depot can appear multiple times indicating a new route/vehicle. Reducing the number of columns in the IVM.  The number of columns corresponds to the number of customers. The number of rows is number of customers + max_number_of_vehicles.
 *
 */
int ProblemVRPTW::getTotalElements() {
    return number_of_customers + 1;
}

int * ProblemVRPTW::getElemensToRepeat() {
    return nullptr;
}

int ProblemVRPTW::getDecodeMap(int map, int position) const {
    return 0;
}

int ProblemVRPTW::getEncodeMap(int value1, int value2) const {
    return 0;
}

/** Each customer can appears only one time. **/
int ProblemVRPTW::getTimesThatValueCanBeRepeated(int value) {
    if (value == 0 || value > getNumberOfCustomers())
        return max_number_of_vehicles;
    else
        return 1;
}

void ProblemVRPTW::createDefaultSolution(Solution& solution) {
    unsigned int n_vehicles = 1;
    solution.setBuildUpTo(-1);
    for (unsigned int customer = 1; customer <= getNumberOfCustomers(); ++customer) {
        solution.push_back(customer);
        if (solution.getBuildUpTo() < getNumberOfVariables() - 1 && n_vehicles < getMaxNumberOfVehicles()) {
            solution.push_back(getNumberOfCustomers() + 1);
            n_vehicles++;
        }
    }
    evaluate(solution);
}

void ProblemVRPTW::getSolutionWithLowerBoundInObj(int nObj, Solution& solution) {

}

unsigned int ProblemVRPTW::getNumberOfCustomers() const {
    return number_of_customers;
}

unsigned int ProblemVRPTW::getMaxVehicleCapacity() const {
    return max_vehicle_capacity;
}

unsigned int ProblemVRPTW::getMaxNumberOfVehicles() const {
    return max_number_of_vehicles;
}

unsigned int ProblemVRPTW::getCustomerCoordinate(unsigned int customer, unsigned int coordinate) const {
    return coordinates[customer][coordinate];
}

double ProblemVRPTW::getCustomerCostTo(unsigned int customer_origin, unsigned int customer_destination) const {
    return costs[customer_origin][customer_destination];
}

unsigned int ProblemVRPTW::getCustomerServiceTime(unsigned int customer) const {
    return service_time[customer];
}

unsigned int ProblemVRPTW::getCustomerDemand(unsigned int customer) const {
    return demand[customer];
}

unsigned int ProblemVRPTW::getCustomerTimeWindowStart(unsigned int customer) const {
    return time_window[customer][0];
}

unsigned int ProblemVRPTW::getCustomerTimeWindowEnd(unsigned int customer) const {
    return time_window[customer][1];
}

unsigned int ProblemVRPTW::getNumberOfNodes() const {
    return getNumberOfCustomers() + 1; /** Plus number of depots. **/
}

void ProblemVRPTW::loadInstance(char filePath[2][255], char file_extension[4]) {

    std::ifstream infile(filePath[0]);
    if (infile.is_open()) {

        std::string line;
        std::getline(infile, line); /**The first line contains the name. **/
        std::strcpy(name, line.c_str());

        std::getline(infile, line); /** Reads an empty line. **/
        std::getline(infile, line); /** Reads line with text "Vehicle". **/
        std::getline(infile, line); /** Reads lines with text "Number" and "Capacity". **/
        std::getline(infile, line); /** Reads the max number of vehicles and the capacity. **/

        std::vector<std::string> splitted_text;
        splitted_text = split(line, ' ');

        max_number_of_vehicles = std::stoi(splitted_text.at(0));
        max_vehicle_capacity = std::stoi(splitted_text.at(1));

        std::getline(infile, line); /** Reads an empty line. **/
        std::getline(infile, line); /** Reads the text "Customer". **/
        std::getline(infile, line); /** Reads the headers text. **/
        std::getline(infile, line); /** Reads another empty line. **/

        /** Reads the customers infomartion and stores it in a vector.. **/
        std::vector<std::vector<int>> customers_data;
        number_of_customers = 0;
        while (true) {
            std::getline(infile, line);

            if(infile.eof())
                break;

            splitted_text = split(line, ' '); /** Reads customer number, xcoord, ycoord, demand, ready_time, due_date, service_time**/

            std::vector<int> data;
            for (unsigned int text = 0; text < splitted_text.size(); ++text)
                data.push_back(std::stoi(splitted_text.at(text)));
            customers_data.push_back(data);

            if (data.size() == 7)
                number_of_customers++;
        }

        number_of_customers--;  /** Discounts the depot node. **/

        coordinates = new unsigned int * [getNumberOfNodes()];
        costs = new double * [getNumberOfNodes()];
        time_window = new unsigned int * [getNumberOfNodes()];
        service_time = new unsigned int[getNumberOfNodes()];
        demand = new unsigned int[getNumberOfNodes()];

        total_demand = 0;
        total_service_time = 0;
        total_distance_in_order = 0;

        max_makespan_found = 0;
        min_makespan_found = 0;
        for (unsigned int customer = 0; customer < getNumberOfNodes(); ++customer) {

            if (customers_data.at(customer).size() < 7) /** There are seven data for each customer. **/
                break;

            coordinates[customer] = new unsigned int[2];
            coordinates[customer][0] = customers_data.at(customer).at(1);
            coordinates[customer][1] = customers_data.at(customer).at(2);

            demand[customer] = customers_data.at(customer).at(3);
            total_demand += demand[customer];

            time_window[customer] = new unsigned int[2];
            time_window[customer][0] = customers_data.at(customer).at(4);
            time_window[customer][1] = customers_data.at(customer).at(5);

            service_time[customer] = customers_data.at(customer).at(6);
            total_service_time += service_time[customer];

            if (customer != 0 && time_window[customer][1] + service_time[customer] > max_makespan_found)
                max_makespan_found = time_window[customer][1] + service_time[customer];

            else if(customer != 0 && time_window[customer][0] + service_time[customer] > min_makespan_found)
                min_makespan_found = time_window[customer][0] + service_time[customer];

            costs[customer] = new double[getNumberOfNodes()];
        }

        for (unsigned int customer = 0; customer < getNumberOfNodes(); ++customer) {
            for (unsigned int customer_destination = customer + 1; customer_destination < getNumberOfNodes(); ++customer_destination)
                if (customer != customer_destination) {

                    costs[customer][customer_destination] = euclideanDistance(coordinates[customer][0], coordinates[customer][1], coordinates[customer_destination][0], coordinates[customer_destination][1]);

                    costs[customer_destination][customer] = costs[customer][customer_destination];

                } else
                    costs[customer][customer_destination] = 0; /** Probably is better to use a big number. **/

            total_distance_in_order += costs[0][customer] + costs[customer][0];
        }

        n_variables = number_of_customers + max_number_of_vehicles; /** The permutaiton size is customers + max vehicles. **/
        infile.close();

        min_number_vehicles_found = max_number_of_vehicles;
        min_critical_cost_found = total_distance_in_order;
        min_cost_found = total_distance_in_order;

        max_number_vehicles_found = max_number_of_vehicles;
        max_critical_cost_found = total_distance_in_order;
        max_cost_found = total_distance_in_order;

        Solution solution (n_objectives, n_variables);
        heuristic(solution);

        Solution solution_min_distance (n_objectives, n_variables);
        heuristic_min_dist(solution_min_distance);

        std::cout << solution;
        std::cout << solution_min_distance;

        updateBestSolutionInObjectiveWith(-1, solution);
        updateBestSolutionInObjectiveWith(-1, solution_min_distance);

    } else {
        printf("File not found\n");
        exit(EXIT_FAILURE);
    }
}

void ProblemVRPTW::loadInstancePayload(const Payload_problem_vrptw& payload) {

    n_objectives = payload.n_objectives;
    n_variables = payload.number_of_customers + payload.max_number_of_vehicles;
    
    max_number_of_vehicles = payload.max_number_of_vehicles;
    max_vehicle_capacity = payload.max_vehicle_capacity;
    number_of_customers = payload.number_of_customers;

    coordinates = new unsigned int * [getNumberOfNodes()];
    costs = new double * [getNumberOfNodes()];
    time_window = new unsigned int * [getNumberOfNodes()];
    service_time = new unsigned int[getNumberOfNodes()];
    demand = new unsigned int[getNumberOfNodes()];

    total_demand = 0;
    total_service_time = 0;
    total_distance_in_order = 0;

    max_makespan_found = 0;
    min_makespan_found = 0;

    for (unsigned int customer = 0; customer < getNumberOfNodes(); ++customer) {
        unsigned int current_position = customer * 2;
        unsigned int next_value = current_position + 1;
        coordinates[customer] = new unsigned int[2];
        coordinates[customer][0] = payload.coordinates[current_position];
        coordinates[customer][1] = payload.coordinates[next_value];

        demand[customer] = payload.demand[customer];
        total_demand += demand[customer];

        time_window[customer] = new unsigned int[2];
        time_window[customer][0] = payload.time_window[current_position];
        time_window[customer][1] = payload.time_window[next_value];

        service_time[customer] = payload.service_time[customer];
        total_service_time += service_time[customer];

        if (customer != 0 && time_window[customer][1] + service_time[customer] > max_makespan_found)
            max_makespan_found = time_window[customer][1] + service_time[customer];

        else if(customer != 0 && time_window[customer][0] + service_time[customer] > min_makespan_found)
            min_makespan_found = time_window[customer][0] + service_time[customer];

        costs[customer] = new double[getNumberOfNodes()];
    }

    for (unsigned int customer = 0; customer < getNumberOfNodes(); ++customer) {
        for (unsigned int customer_destination = customer + 1; customer_destination < getNumberOfNodes(); ++customer_destination)
            if (customer != customer_destination) {

                costs[customer][customer_destination] = euclideanDistance(coordinates[customer][0], coordinates[customer][1], coordinates[customer_destination][0], coordinates[customer_destination][1]);

                costs[customer_destination][customer] = costs[customer][customer_destination];

            } else
                costs[customer][customer_destination] = 0; /** Probably is better to use a big number. **/

        total_distance_in_order += costs[0][customer] + costs[customer][0];
    }

    n_variables = number_of_customers + max_number_of_vehicles; /** The permutaiton size is customers + max vehicles. **/

    min_number_vehicles_found = max_number_of_vehicles;
    min_critical_cost_found = total_distance_in_order;
    min_cost_found = total_distance_in_order;

    max_number_vehicles_found = max_number_of_vehicles;
    max_critical_cost_found = total_distance_in_order;
    max_cost_found = total_distance_in_order;

    Solution solution (n_objectives, n_variables);
    heuristic(solution);

    Solution solution_min_distance (n_objectives, n_variables);
    heuristic_min_dist(solution_min_distance);

    updateBestSolutionInObjectiveWith(-1, solution);
    updateBestSolutionInObjectiveWith(-1, solution_min_distance);

}

double ProblemVRPTW::euclideanDistance(unsigned int xcoord_1, unsigned int ycoord_1, unsigned int xcoord_2, unsigned int ycoord_2) const {

    int dist_x = xcoord_2 - xcoord_1;
    int dist_y = ycoord_2 - ycoord_1;
    return sqrt(dist_x * dist_x + dist_y * dist_y);
}

bool ProblemVRPTW::isCustomer(unsigned int node) const {
    return (node <= getNumberOfCustomers() && node != 0)? true : false;
}

bool ProblemVRPTW::isDepot(unsigned int node) const {
    return (node > getNumberOfCustomers()? true : false) || node == 0;
}

void ProblemVRPTW::printSolution(const Solution & solution) const{
    printPartialSolution(solution, this->n_variables);
}

void ProblemVRPTW::printPartialSolution(const Solution & solution, int level) const {

}

void ProblemVRPTW::printSolutionInfo(const Solution & solution) const {
    printf("TODO: Implement this function.\n");
}

void ProblemVRPTW::printInstance() const {
    printf("Problem VRPTW\n");
    printf("Instance name: %s\n", name);
    printf("Number of customers: %d\n", getNumberOfCustomers());
    printf("Number of maximum vehicles: %d\n", getMaxNumberOfVehicles());
    printf("Maximum capacity: %d\n", getMaxVehicleCapacity());

    printf("CUST NO.  XCOORD.    YCOORD.    DEMAND   READY TIME  DUE DATE   SERVICE TIME\n");
    for (unsigned int customer = 0; customer < getNumberOfNodes(); ++customer) {
        printf("%d %d %d %d %d %d %d\n", customer,
               getCustomerCoordinate(customer, 0),
               getCustomerCoordinate(customer, 1),
               getCustomerDemand(customer),
               getCustomerTimeWindowStart(customer),
               getCustomerTimeWindowEnd(customer),
               getCustomerServiceTime(customer));
    }

    printf("Costs as Euclidean distances\n");
    for (unsigned int customer = 0; customer < getNumberOfNodes(); ++customer) {
        printf("[%4d] ", customer);
        for (unsigned int customer_destiny = 0; customer_destiny < getNumberOfNodes(); ++customer_destiny)
            printf("%6.3f ", getCustomerCostTo(customer, customer_destiny));
        printf("\n");
    }
}

void ProblemVRPTW::printProblemInfo() const {
    printf("Problem VRPTW\n");
    printf("Instance name: %s\n", name);
    printf("Number of customers: %d\n", getNumberOfCustomers());
    printf("Number of maximum vehicles: %d\n", getMaxNumberOfVehicles());
    printf("Maximum capacity per vehicle: %d\n", getMaxVehicleCapacity());
    printf("Total demand: %d\n", total_demand);
    printf("Total service time: %d\n", total_service_time);
    printf("Min number of vehicles (total demand/max_vehicles): %d\n", min_number_vehicles_found);
    printf("Max number of vehicles: %d\n", max_number_vehicles_found);
    printf("Min cost (fixed): %f\n", min_cost_found);
    printf("Max cost (one vehicle for each customer): %f\n", max_cost_found);
    printf("Min makespan (max{Ti} + service_time): %d\n", min_makespan_found);
    printf("Max makespan (max{Tf} + service_time): %d\n", max_makespan_found);
}

double ProblemVRPTW::trunc(double lf) const {

    int iSigned = lf > 0? 1: -1;
    unsigned int uiTemp = (lf * pow(10, 6)) * iSigned; //Note I'm using unsigned int so that I can increase the precision of the truncate
    lf = (((double) uiTemp) / pow(10, 6) * iSigned);
    return lf;
}

bool ProblemVRPTW::validateVariables(Solution &solution) {
    unsigned int n_vehicles = 0;
    unsigned int n_dispatched_customers = 0;
    unsigned int origin = 0;

    double vehicle_cost [getMaxNumberOfVehicles()];
    double travel_time [getMaxNumberOfVehicles()];
    unsigned int capacity [getMaxNumberOfVehicles()];
    bool visited[getNumberOfNodes()];

    double max_travel_time = 0;
    double max_cost = 0;
    double total_cost = 0;

    capacity[0] = getMaxVehicleCapacity();
    vehicle_cost[0] = 0;
    travel_time[0] = 0;

    bool is_feasible = true;
    bool is_complete = false;

    for (unsigned int idx_n = 0; idx_n < getNumberOfNodes(); ++idx_n)
        visited[idx_n] = false;

    for (unsigned int idx_var = 0; idx_var <= solution.getBuildUpTo(); ++idx_var) {
        int destination = solution.getVariable(idx_var) <= getNumberOfCustomers() ? solution.getVariable(idx_var) : 0;

        if (idx_var == 0) {
            if (isDepot(destination)) /** First node can not be a depot. **/
                is_feasible = false;
        } else {
            int last_node = solution.getVariable(idx_var - 1);
            if (isDepot(last_node) && isDepot(destination)) /** Two consecutive depots. Empty route. **/
                is_feasible = false;

            if (isCustomer(destination) && visited[destination]) /** Already visited. **/
                is_feasible = false;

            if (n_dispatched_customers >= getNumberOfCustomers())
                is_feasible = false; /** Solution is already complete. **/

        }
        /** There are more customers to dispatch. **/
        if (is_feasible && n_dispatched_customers < getNumberOfCustomers()) {
            origin = (idx_var == 0)? 0: (solution.getVariable(idx_var - 1) >= getNumberOfCustomers()? 0: solution.getVariable(idx_var - 1));

            /** Validating the time window and customer demand. **/
            if (capacity[n_vehicles] >= getCustomerDemand(destination)) {

                if (travel_time[n_vehicles] <= getCustomerTimeWindowStart(destination))
                    travel_time[n_vehicles] = getCustomerTimeWindowStart(destination) + getCustomerServiceTime(destination);

                else if(travel_time[n_vehicles] >= getCustomerTimeWindowStart(destination) &&
                        travel_time[n_vehicles] <= getCustomerTimeWindowEnd(destination))
                    travel_time[n_vehicles] += getCustomerServiceTime(destination);

                else {
                    is_feasible = false;
                    break; /** Not a feasible time window. **/
                }

                capacity[n_vehicles] -= getCustomerDemand(destination);
                vehicle_cost[n_vehicles] += getCustomerCostTo(origin, destination);

                if (isCustomer(destination)) {
                    n_dispatched_customers++;
                } else { /** It is a customer going to depot. **/
                    n_vehicles++;
                    capacity[n_vehicles] = getMaxVehicleCapacity();
                    vehicle_cost[n_vehicles] = 0;
                    travel_time[n_vehicles] = 0;

                } /** else is an empty route and do nothing. **/

            } else {
                is_feasible = false;
                break; /** Not enough capacity. **/
            }
        } else { /** If there are no more customers to dispatch then it is a complete solution. **/
            vehicle_cost[n_vehicles] += costs[solution.getVariable(idx_var - 1)][0]; /** Adds cost of returning to depot. **/
            is_complete = true;
            break;
        }
    }

    if (is_feasible && is_complete) {
        for (unsigned int vehicle = 0; vehicle <= n_vehicles; ++vehicle) {

            if (vehicle_cost[vehicle] > max_cost)
                max_cost = vehicle_cost[vehicle];

            if (travel_time[vehicle] > max_travel_time)
                max_travel_time = travel_time[vehicle];

            total_cost += vehicle_cost[vehicle];
        }
        
        solution.setObjective(0, n_vehicles + 1);
        solution.setObjective(1, total_cost);
        solution.setObjective(2, max_cost);

    } else {
        /** Setting bad objectives values by infeasibility. **/
//        solution.setObjective(0, max_number_of_vehicles * 2);
//        solution.setObjective(1, total_distance_in_order * 2);
//        solution.setObjective(2, total_distance_in_order * 2);
    }

    return is_feasible;
}

void ProblemVRPTW::heuristic(Solution &solution) {
    VRPTWdata data(number_of_customers, max_number_of_vehicles, max_vehicle_capacity);
    int vars_fixed = 0;
    int depot = number_of_customers + 1;

    std::vector<int> customers;
    customers.reserve(number_of_customers);
    for (unsigned int cust = 1; cust <= number_of_customers; ++cust)
        customers.push_back(cust);

    while (!customers.empty()) {
        long min_due_date = time_window[0][1];
        int min_cust_due_date = 0;
        int idx_vec = 0;
        for (unsigned int idx_cust = 0; idx_cust < customers.size(); ++idx_cust) {
            int cust = customers.at(idx_cust);
            if (time_window[cust][1] < min_due_date) {
                min_due_date = time_window[cust][1];
                min_cust_due_date = cust;
                idx_vec = idx_cust;
            }
        }
        customers.erase(customers.begin() + idx_vec);

        solution.push_back(min_cust_due_date);
        evaluateDynamic(solution, data, vars_fixed);
        if (!solution.isFeasible()) {
            evaluateRemoveDynamic(solution, data, vars_fixed);

            solution.push_back(depot);
            evaluateDynamic(solution, data, vars_fixed);
            vars_fixed++;

            solution.push_back(min_cust_due_date);
            evaluateDynamic(solution, data, vars_fixed);
        }
        vars_fixed++;
    }
}

void ProblemVRPTW::heuristic_min_dist(Solution &solution) {
    VRPTWdata data(number_of_customers, max_number_of_vehicles, max_vehicle_capacity);
    int vars_fixed = 0;
    int depot = number_of_customers + 1;

    std::vector<int> customers;
    customers.reserve(number_of_customers);
    for (unsigned int cust = 1; cust <= number_of_customers; ++cust)
        customers.push_back(cust);

    int origin = 0;
    while (!customers.empty()) {
        double min_distance = getCustomerCostTo(origin, customers.at(0));
        int min_cust = customers.at(0);
        int idx_vec = 0;
        for (unsigned int idx_cust = 0; idx_cust < customers.size(); ++idx_cust) {
            int cust = customers.at(idx_cust);
            if (getCustomerCostTo(origin, cust) < min_distance) {
                min_distance = getCustomerCostTo(origin, cust);
                min_cust = cust;
                idx_vec = idx_cust;
            }
        }
        origin = min_cust;

        solution.push_back(min_cust);
        evaluateDynamic(solution, data, vars_fixed);
        if (!solution.isFeasible()) {
            evaluateRemoveDynamic(solution, data, vars_fixed);

            solution.push_back(depot);
            evaluateDynamic(solution, data, vars_fixed);
            origin = 0;
        } else
            customers.erase(customers.begin() + idx_vec);

        vars_fixed++;
    }
}
