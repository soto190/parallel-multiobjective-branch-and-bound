//
//  NSGA_II.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "NSGA_II.hpp"

NSGA_II::NSGA_II(const ProblemFJSSP& problem):problem(problem) {
    generations_is_stop_criteria = false;
    evaluations_is_stop_criteria = false;
    max_population = 10;
    population.reserve(max_population * 2);
    //std::random_device seed;
    //generator.seed(seed());
}

NSGA_II::~NSGA_II() {
    population.clear();
}

void NSGA_II::setSampleSolution(const Solution &solution) {
    sample_solution = solution;
}

void NSGA_II::setCrossoverRate(double rate) {
    crossover_rate = rate;
}

void NSGA_II::setMutationRate(double rate) {
    mutation_rate = rate;
}

void NSGA_II::setMaxPopulationSize(unsigned long population_size) {
    max_population = population_size;
}

void NSGA_II::setMaxNumberOfGenerations(unsigned long max_generations) {
    max_number_of_generations = max_generations;
    generations_is_stop_criteria = true;
}

void NSGA_II::setMaxNumberOfEvaluations(unsigned long max_evaluations) {
    max_number_of_evaluations = max_evaluations;
    evaluations_is_stop_criteria = true;
}

unsigned long NSGA_II::getMaxPopulationSize() const {
    return max_population;
}

void NSGA_II::createInitialPopulation() {
    population.reserve(getMaxPopulationSize() * 2);
    population.push_back(sample_solution);

    double stored_mutation = getMutationRate();
    setMutationRate(0.99);
    for (unsigned long p = population.size(); p < getMaxPopulationSize(); ++p) {
        mutationOperator(sample_solution);
        population.push_back(sample_solution);
    }
    setMutationRate(stored_mutation);
}

void NSGA_II::selection() {
    
}

void NSGA_II::crossover() {
    vector<Solution> offsprings;
    offsprings.reserve(population.size());
    for (unsigned long pop = 0; pop < population.size() - 1; pop += 2) {
        vector<Solution> offs = crossoverOperator(population.at(pop), population.at(pop + 1));
        offsprings.insert(offsprings.end(), offs.begin(), offs.end());
    }

    population.insert(population.end(), offsprings.begin(), offsprings.end());
}

void NSGA_II::mutation() {
    std::vector<Solution>::iterator it_solution;
    for (it_solution = population.begin(); it_solution != population.end(); ++it_solution)
        mutationOperator(*it_solution);
}

void NSGA_II::evaluatePopulation() {
    std::vector<Solution>::iterator it_solution;
    for (it_solution = population.begin(); it_solution != population.end(); ++it_solution) {
        problem.evaluate(*it_solution);
        increaseNumberOfEvaluations();
    }
}

void NSGA_II::replacement() {
    population = fastNonDominatedSort(population);
}

vector<Solution> NSGA_II::crossoverOperator(const Solution &parent1, const Solution &parent2) {
    vector<Solution> offspring;
    offspring.reserve(2);

    Solution offspring1(parent1);
    Solution offspring2(parent2);
    
    int mid = parent1.getNumberOfVariables() / 2;
    int chromosome_size = parent1.getNumberOfVariables();
    int allel_p1;
    int allel_p2;
    int allel_p1_is_job;
    int allel_p2_is_job;

    int jobs_op_allocated_off1[problem.getNumberOfJobs()];
    int jobs_op_allocated_off2[problem.getNumberOfJobs()];

    for (int job = 0; job < problem.getNumberOfJobs(); ++job) {
        jobs_op_allocated_off1[job] = 0;
        jobs_op_allocated_off2[job] = 0;
    }

    for (int gene = 0; gene < mid; ++gene) {
        allel_p1_is_job = problem.getDecodeMap(parent1.getVariable(gene), 0);
        allel_p2_is_job = problem.getDecodeMap(parent2.getVariable(gene), 0);

        jobs_op_allocated_off1[allel_p1_is_job]++;
        jobs_op_allocated_off2[allel_p2_is_job]++;
    }

    for (int gene = mid; gene < chromosome_size; ++gene) {
        allel_p1 = parent1.getVariable(gene);
        allel_p2 = parent2.getVariable(gene);

        allel_p1_is_job = problem.getDecodeMap(allel_p1, 0);
        allel_p2_is_job = problem.getDecodeMap(allel_p2, 0);

        if (jobs_op_allocated_off1[allel_p2_is_job] < problem.getNumberOfOperationsInJob(allel_p2_is_job))
            offspring1.setVariable(gene, allel_p2);
        else
            for (int job = 0; job < problem.getNumberOfJobs(); ++job)
                if (jobs_op_allocated_off1[job] < problem.getNumberOfOperationsInJob(job)) {
                    int operation = problem.getOperationInJobIsNumber(job, jobs_op_allocated_off1[job]);
                    unsigned long machines_aviable = problem.getNumberOfMachinesAvaibleForOperation(operation);
                    //std::uniform_int_distribution<unsigned int> unif_int_mach_dis(0, static_cast<int>(machines_aviable) - 1);
                    // int r = unif_int_mach_dis(generator);
                    int r = rand() / (RAND_MAX / static_cast<int>(machines_aviable));
                    int new_machine = problem.getMachinesAvaibleForOperation(operation, r);
                    allel_p2 = problem.getEncodeMap(job, new_machine);
                    offspring1.setVariable(gene, allel_p2);
                    job = problem.getNumberOfJobs();
                }

        if (jobs_op_allocated_off2[allel_p1_is_job] < problem.getNumberOfOperationsInJob(allel_p1_is_job))
            offspring2.setVariable(gene, allel_p1);
        else
            for (unsigned int job = 0; job < problem.getNumberOfJobs(); ++job)
                if (jobs_op_allocated_off2[job] < problem.getNumberOfOperationsInJob(job)) {
                    int operation = problem.getOperationInJobIsNumber(job, jobs_op_allocated_off2[job]);
                    unsigned long machines_aviable = problem.getNumberOfMachinesAvaibleForOperation(operation);

                    //std::uniform_int_distribution<unsigned int> unif_int_mach_dis(0, static_cast<int>(machines_aviable) - 1);
                    //int r = unif_int_mach_dis(generator);
                    int r = rand() / (RAND_MAX / static_cast<int>(machines_aviable));
                    int new_machine = problem.getMachinesAvaibleForOperation(operation, r);

                    allel_p1 = problem.getEncodeMap(job, new_machine);
                    offspring2.setVariable(gene, allel_p1);
                    job = problem.getNumberOfJobs();
                }

        allel_p1_is_job = problem.getDecodeMap(offspring1.getVariable(gene), 0);
        allel_p2_is_job = problem.getDecodeMap(offspring2.getVariable(gene), 0);

        jobs_op_allocated_off1[allel_p1_is_job]++;
        jobs_op_allocated_off2[allel_p2_is_job]++;
    }

    offspring.push_back(offspring1);
    offspring.push_back(offspring2);

    return offspring;
}

/**
 *  Each gene has a probability to be mutated. The probability is given by the mutation rate.
 **/
void NSGA_II::mutationOperator(Solution &solution) {

    int chromosome_size = solution.getNumberOfVariables();

    //std::uniform_real_distribution<double> unif_dis(0.0, 1.0);
    //std::uniform_int_distribution<unsigned int> unif_position(0, chromosome_size - 1);

    int jobs_op_allocated[problem.getNumberOfJobs()];
    
    for (int job = 0; job < problem.getNumberOfJobs(); ++job)
        jobs_op_allocated[job] = 0;

    /**
     * The genes are shaked to move them to other position.
     **/
    for (int gene = 0; gene < chromosome_size; ++gene) {
//        double r = unif_dis(generator);
        double r = ((double) rand() / (RAND_MAX));
        if (r < getMutationRate()) {
            int allel_value = solution.getVariable(gene);
//            int new_position = unif_position(generator);
            int new_position = rand() / (RAND_MAX / static_cast<int>(chromosome_size));
            solution.setVariable(gene, solution.getVariable(new_position));
            solution.setVariable(new_position, allel_value);
        }
    }
    /**
     * Then they are mutated to new values.
     **/
    for (int gene = 0; gene < chromosome_size; ++gene) {
        int allel_value = solution.getVariable(gene);
        int allel_is_from_job = problem.getDecodeMap(allel_value, 0);
        int machine = problem.getDecodeMap(allel_value, 1);
        int operation = problem.getOperationInJobIsNumber(allel_is_from_job, jobs_op_allocated[allel_is_from_job]);
        //double r = unif_dis(generator);
        double r = ((double) rand() / (RAND_MAX));
        if (r < getMutationRate() || !problem.operationCanBeAllocatedInMachine(operation, machine)) {
            unsigned long machines_aviable = problem.getNumberOfMachinesAvaibleForOperation(operation);

            //std::uniform_int_distribution<unsigned long> unif_int_mach_dis(0, static_cast<int>(machines_aviable) - 1);
            //int n_machine = unif_int_mach_dis(generator);
            int n_machine = rand() / (RAND_MAX / static_cast<int>(machines_aviable));

            int new_machine = problem.getMachinesAvaibleForOperation(operation, n_machine);
            int new_allel = problem.getEncodeMap(allel_is_from_job, new_machine);
            solution.setVariable(gene, new_allel);
        }

        jobs_op_allocated[allel_is_from_job]++;
    }

    problem.evaluate(solution);
}

double NSGA_II::getCrossoverRate() const {
    return crossover_rate;
}

double NSGA_II::getMutationRate() const {
    return mutation_rate;
}

unsigned long NSGA_II::getNumberOfEvaluations() const {
    return number_of_evaluations_performed;
}

vector<Solution> NSGA_II::fastNonDominatedSort(vector<Solution>& population_to_sort) {
    
    vector<vector<Solution*>> dominates_to;
    vector<vector<Solution*>> pareto_fronts;

    dominates_to.reserve(getMaxPopulationSize() * 2);
    pareto_fronts.reserve(getMaxPopulationSize() * 2);

    vector<Solution> new_population;
    new_population.reserve(getMaxPopulationSize() * 2);
    
    for (int n_sol = 0; n_sol < population_to_sort.size(); ++n_sol) {
        population_to_sort.at(n_sol).setRank(-1);
        population_to_sort.at(n_sol).setDominatedBy(0);
        dominates_to.push_back(vector<Solution*>());
    }

    pareto_fronts.push_back(vector<Solution*>());

    int solution_counter_p = 0;
    for (std::vector<Solution>::iterator sol_p = population_to_sort.begin(); sol_p != population_to_sort.end(); ++sol_p) {
        (*sol_p).setIndex(solution_counter_p);
        for (std::vector<Solution>::iterator sol_q = population_to_sort.begin(); sol_q != population_to_sort.end(); ++sol_q) {
            DominanceRelation dom = (*sol_p).dominanceTest(*sol_q);
            if (dom == DominanceRelation::Dominates)
                dominates_to.at(solution_counter_p).push_back(&(*sol_q));
            else if (dom == DominanceRelation::Dominated) {
                (*sol_p).incrementDominatedBy();
            }
        }
        if ((*sol_p).getDominatedBy() == 0) {
            (*sol_p).setRank(0);
            pareto_fronts.at(0).push_back(&(*sol_p));
        }
        solution_counter_p++;
    }
    
    int idx_pf = 0;
    while (!pareto_fronts.at(idx_pf).empty()) {
        pareto_fronts.push_back(vector<Solution*>());
        for (std::vector<Solution*>::iterator solution_p = pareto_fronts.at(idx_pf).begin(); solution_p != pareto_fronts.at(idx_pf).end(); ++solution_p)
            for (std::vector<Solution*>::iterator solution_q = dominates_to.at((*solution_p)->getIndex()).begin(); solution_q != dominates_to.at((*solution_p)->getIndex()).end(); ++solution_q) {
                (*solution_q)->decrementDominatedBy();
                if ((*solution_q)->getDominatedBy() == 0 && (*solution_q)->getRank() == -1) {
                    (*solution_q)->setRank(idx_pf + 1);
                    pareto_fronts.at(idx_pf + 1).push_back(*solution_q);
                }
            }
        idx_pf++;
    }

    /** Crowding distance assignment. **/
    while (new_population.size() < getMaxPopulationSize())
        for (unsigned long front = 0; front < pareto_fronts.size(); ++front)
            if (getMaxPopulationSize() - new_population.size() > pareto_fronts.at(front).size())
                for (std::vector<Solution*>::iterator solution_p = pareto_fronts.at(front).begin(); solution_p != pareto_fronts.at(front).end(); ++solution_p)
                    new_population.push_back(Solution(*(*solution_p)));
            else {
                vector<Solution> crowded_front;
                crowded_front.reserve(pareto_fronts.at(front).size());

                for (std::vector<Solution*>::iterator solution_p = pareto_fronts.at(front).begin(); solution_p != pareto_fronts.at(front).end(); ++solution_p)
                    crowded_front.push_back(Solution(*(*solution_p)));

                if (pareto_fronts.at(front).size() > 2)
                    crowdingDistanceAssignment(crowded_front);

                for (std::vector<Solution>::iterator solution_p = crowded_front.begin(); solution_p != crowded_front.end() && new_population.size() < getMaxPopulationSize(); ++solution_p)
                    new_population.push_back(*solution_p);
            }

    return new_population;
}

vector<Solution> NSGA_II::crowdingDistanceAssignment(vector<Solution>& front) {
    unsigned long length = front.size() - 1;
    unsigned int n_objectives = front.at(0).getNumberOfObjectives();

    for (std::vector<Solution>::iterator solution_p = front.begin(); solution_p != front.end(); ++solution_p) {
        (*solution_p).setDistance(0);
        (*solution_p).setSortByObjective(0);
    }

    for (unsigned int n_obj = 0; n_obj < n_objectives; ++n_obj) {
        sort(front.begin(), front.end());
        front.at(0).setDistance(INFINITY);
        front.at(0).setSortByObjective(n_obj + 1);
        front.at(length).setDistance(INFINITY);
        front.at(length).setSortByObjective(n_obj + 1);
        for (unsigned long sol = 1; sol <= length - 1; ++sol) {
            double actual_distance = front.at(sol).getDistance();
            double new_distance = abs(front.at(sol + 1).getObjective(n_obj) - front.at(sol - 1).getObjective(n_obj));
            front.at(sol).setDistance(actual_distance + new_distance);
            front.at(sol).setSortByObjective(n_obj + 1);
        }
    }

    sort(front.begin(), front.end());
    return front;
}

unsigned long NSGA_II::increaseNumberOfGenerations() {
    return number_of_generations_performed++;
}

unsigned long NSGA_II::increaseNumberOfEvaluations() {
    return number_of_evaluations_performed++;
}

bool NSGA_II::isStoppingCriteriaReached() const {
    if (stoppingCriteriaIsGenerations())
        return isMaxNumberOfGenerationsReached();
    
    else if (stoppingCriteriaIsEvaluations())
        return isMaxNumberOfEvaluationsReached();
    
    return false;
}

bool NSGA_II::stoppingCriteriaIsGenerations() const {
    return generations_is_stop_criteria;
}

bool NSGA_II::stoppingCriteriaIsEvaluations() const {
    return evaluations_is_stop_criteria;
}

bool NSGA_II::isMaxNumberOfGenerationsReached() const {
    if(number_of_generations_performed >= max_number_of_generations)
        return true;
    return false;
}

bool NSGA_II::isMaxNumberOfEvaluationsReached() const {
    if (number_of_evaluations_performed >= max_number_of_evaluations)
        return true;
    return false;
}

void NSGA_II::updateProgress() {
    increaseNumberOfGenerations();
}

void NSGA_II::printPopulation() const {
    for (int nSol = 0; nSol < population.size(); ++nSol) {
        printf("[%3d] ", nSol);
        population.at(nSol).print();
    }
}

void NSGA_II::initialize() {
    number_of_generations_performed = 0;
    number_of_evaluations_performed = 0;
    createInitialPopulation();
}

ParetoFront NSGA_II::solve() {
    initialize();
    while (!isStoppingCriteriaReached()) {
        selection();
        crossover();
        mutation();
        evaluatePopulation();
        replacement();
        updateProgress();
    }

    ParetoFront pareto_front (population);
    return pareto_front;
}
