//
//  NSGA_II.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "NSGA_II.hpp"

NSGA_II::NSGA_II(const ProblemFJSSP& problem):problem(problem){
    generations_is_stop_criteria = false;
    evaluations_is_stop_criteria = false;
    max_population = 10;
    population.reserve(max_population);
}

NSGA_II::~NSGA_II(){
    population.clear();
}

void NSGA_II::setSampleSolution(const Solution &solution){
    sample_solution = solution;
}

void NSGA_II::setCrossoverRate(double rate){
    crossover_rate = rate;
}

void NSGA_II::setMutationRate(double rate){
    mutation_rate = rate;
}

void NSGA_II::setMaxPopulationSize(unsigned long population_size){
    max_population = population_size;
}

void NSGA_II::setMaxNumberOfGenerations(unsigned long max_generations){
    max_number_of_generations = max_generations;
    generations_is_stop_criteria = true;
}

void NSGA_II::setMaxNumberOfEvaluations(unsigned long max_evaluations){
    max_number_of_evaluations = max_evaluations;
    evaluations_is_stop_criteria = true;
}

unsigned long NSGA_II::getMaxPopulationSize() const{
    return max_population;
}

void NSGA_II::createInitialPopulation(){
    population.reserve(getMaxPopulationSize());
    population.push_back(sample_solution);

    double stored_mutation = getMutationRate();
    setMutationRate(0.99);
    for (unsigned long p = population.size(); p < getMaxPopulationSize(); ++p) {
        mutationOperator(sample_solution);
        population.push_back(sample_solution);
    }
    setMutationRate(stored_mutation);

    printf("Initial population.\n");
    printPopulation();
    printf("\n");
}

void NSGA_II::selection(){
    
}

void NSGA_II::crossover(){
    vector<Solution> offsprings;
    offsprings.reserve(population.size());
    for (unsigned long pop = 0; pop < population.size() - 1; pop += 2) {
        vector<Solution> offs = crossoverOperator(population.at(pop), population.at(pop + 1));
        offsprings.insert(offsprings.end(), offs.begin(), offs.end());
    }

    population.insert(population.end(), offsprings.begin(), offsprings.end());
}

void NSGA_II::mutation(){
    std::vector<Solution>::iterator it_solution;
    for (it_solution = population.begin(); it_solution != population.end(); ++it_solution)
        mutationOperator(*it_solution);
}

void NSGA_II::evaluatePopulation(){
    std::vector<Solution>::iterator it_solution;
    for (it_solution = population.begin(); it_solution != population.end(); ++it_solution)
        problem.evaluate(*it_solution);
}

void NSGA_II::replacement(){
    //fastNonDominatedSort();
    population.erase(population.begin(), population.begin() + getMaxPopulationSize());
}

vector<Solution> NSGA_II::crossoverOperator(const Solution &parent1, const Solution &parent2){
    vector<Solution> offspring;
    offspring.reserve(2);

    parent1.print();
    parent2.print();
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

        if (jobs_op_allocated_off1[allel_p2_is_job] < problem.getNumberOfOperationsInJob(allel_p2_is_job)) {
            offspring1.setVariable(gene, allel_p2);
        } else
            for (int job = 0; job < problem.getNumberOfJobs(); ++job)
                if (jobs_op_allocated_off1[job] < problem.getNumberOfOperationsInJob(job)) {
                    int operation = problem.getOperationInJobIsNumber(job, jobs_op_allocated_off1[job]);
                    unsigned long machines_aviable = problem.getNumberOfMachinesAvaibleForOperation(operation);
                    std::uniform_int_distribution<> unif_int_mach_dis(0, static_cast<int>(machines_aviable) - 1);
                    
                    int new_machine = problem.getMachinesAvaibleForOperation(operation, unif_int_mach_dis(generator));
                    allel_p2 = problem.getEncodeMap(job, new_machine);
                    offspring1.setVariable(gene, allel_p2);
                    job = problem.getNumberOfJobs();
                }

        if (jobs_op_allocated_off2[allel_p1_is_job] < problem.getNumberOfOperationsInJob(allel_p1_is_job)) {
            offspring2.setVariable(gene, allel_p1);
        }else
            for (int job = 0; job < problem.getNumberOfJobs(); ++job)
                if (jobs_op_allocated_off2[job] < problem.getNumberOfOperationsInJob(job)) {
                    int operation = problem.getOperationInJobIsNumber(job, jobs_op_allocated_off2[job]);
                    unsigned long machines_aviable = problem.getNumberOfMachinesAvaibleForOperation(operation);
                    std::uniform_int_distribution<> unif_int_mach_dis(0, static_cast<int>(machines_aviable) - 1);
                    
                    int new_machine = problem.getMachinesAvaibleForOperation(operation, unif_int_mach_dis(generator));
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
void NSGA_II::mutationOperator(Solution &solution){

    int chromosome_size = solution.getNumberOfVariables();

    std::uniform_real_distribution<double> unif_dis(0.0, 1.0);
    std::uniform_int_distribution<int> unif_position(0, chromosome_size - 1);

    int jobs_op_allocated[problem.getNumberOfJobs()];
    
    for (int job = 0; job < problem.getNumberOfJobs(); ++job)
        jobs_op_allocated[job] = 0;

    /**
     * The genes are shaked to move them to other position.
     **/
    for (int gene = 0; gene < chromosome_size; ++gene)
        if (unif_dis(generator) < getMutationRate()) {
            int allel_value = solution.getVariable(gene);
            int new_position = unif_position(generator);
            solution.setVariable(gene, solution.getVariable(new_position));
            solution.setVariable(new_position, allel_value);
        }

    /**
     * Then they are mutated to new values.
     **/
    for (int gene = 0; gene < chromosome_size; ++gene){
        int allel_value = solution.getVariable(gene);
        int allel_is_from_job = problem.getDecodeMap(allel_value, 0);
        int machine = problem.getDecodeMap(allel_value, 1);
        int operation = problem.getOperationInJobIsNumber(allel_is_from_job, jobs_op_allocated[allel_is_from_job]);

        if (unif_dis(generator) < getMutationRate() || !problem.operationCanBeAllocatedInMachine(operation, machine)) {
            unsigned long machines_aviable = problem.getNumberOfMachinesAvaibleForOperation(operation);
            std::uniform_int_distribution<> unif_int_mach_dis(0, static_cast<int>(machines_aviable) - 1);

            int new_machine = problem.getMachinesAvaibleForOperation(operation, unif_int_mach_dis(generator));
            int new_allel = problem.getEncodeMap(allel_is_from_job, new_machine);
            solution.setVariable(gene, new_allel);
        }

        jobs_op_allocated[allel_is_from_job]++;
    }

    problem.evaluate(solution);
}

double NSGA_II::getCrossoverRate() const{
    return crossover_rate;
}

double NSGA_II::getMutationRate() const{
    return mutation_rate;
}

void NSGA_II::fastNonDominatedSort(){
    
    vector<vector<Solution>> dominates_to;
    vector<vector<Solution>> pareto_fronts;
    ParetoFront paretoFront;

    dominates_to.reserve(getMaxPopulationSize());
    pareto_fronts.reserve(getMaxPopulationSize());
    
    for (int n_sol = 0; n_sol < population.size(); ++n_sol){
        population[n_sol].setRank(-1);
        population[n_sol].setDominatedBy(0);
    }
    
    for (int n_sol_p = 0; n_sol_p < population.size(); ++n_sol_p) {
        Solution sol_p = population[n_sol_p];
        sol_p.index = n_sol_p;
        for (int n_sol_q = 0; n_sol_q < population.size(); ++n_sol_q){
            if (n_sol_p != n_sol_q){
                Solution sol_q = population[n_sol_q];
                DominanceRelation dom = sol_p.dominanceTest(sol_q);
                if (dom == DominanceRelation::Dominates) {
                    dominates_to[n_sol_p].push_back(sol_q);
                }else if (dom == DominanceRelation::Dominated){
                    sol_p.incrementDominatedBy();
                }
            }
        }
        if (sol_p.getDominatedBy() == 0) {
            sol_p.setRank(0);
            pareto_fronts[0].push_back(sol_p);
        }
    }
    
    int idx_pf = 0;
    while (!pareto_fronts[idx_pf].empty()) {
        for (int n_sol_p = 0; n_sol_p < pareto_fronts[idx_pf].size(); ++n_sol_p) {
            Solution solution_p = pareto_fronts[idx_pf].at(n_sol_p);
            for (int n_sol_q = 0; n_sol_q < dominates_to[0].size(); ++n_sol_q) {
                Solution solution_q =  dominates_to[solution_p.index].at(n_sol_q);
                solution_q.decrementDominatedBy();
                if (dominates_to[solution_p.index].at(n_sol_q).getDominatedBy() == 0) {
                    pareto_fronts[idx_pf + 1].push_back(solution_q);
                }
            }
        }
        idx_pf++;
    }
}

void NSGA_II::crowdingDistance(){
    
}

unsigned long NSGA_II::increaseNumberOfGenerations(){
    return number_of_generations_performed++;
}

unsigned long NSGA_II::increaseNumberOfEvaluations(){
    return number_of_evaluations_performed++;
}

bool NSGA_II::isStoppingCriteriaReached() const{
    if (stoppingCriteriaIsGenerations())
        return isMaxNumberOfGenerationsReached();
    
    else if (stoppingCriteriaIsEvaluations())
        return isMaxNumberOfEvaluationsReached();
    
    return false;
}

bool NSGA_II::stoppingCriteriaIsGenerations() const{
    return generations_is_stop_criteria;
}

bool NSGA_II::stoppingCriteriaIsEvaluations() const{
    return evaluations_is_stop_criteria;
}

bool NSGA_II::isMaxNumberOfGenerationsReached() const{
    if(number_of_generations_performed >= max_number_of_generations)
        return true;
    return false;
}

bool NSGA_II::isMaxNumberOfEvaluationsReached() const{
    if (number_of_evaluations_performed >= max_number_of_evaluations)
        return true;
    return false;
}

void NSGA_II::updateProgress(){
    increaseNumberOfGenerations();
}

void NSGA_II::printPopulation() const{
    for (int nSol = 0; nSol < population.size(); ++nSol)
        population[nSol].print();
}

void NSGA_II::initialize(){
    number_of_generations_performed = 0;
    number_of_evaluations_performed = 0;
    createInitialPopulation();
}

void NSGA_II::solve(){
    initialize();
    while (!isStoppingCriteriaReached()) {
        printf("%lu \n", number_of_generations_performed);
        selection();
        crossover();
        mutation();
        evaluatePopulation();
        replacement();
        updateProgress();
    }
    printPopulation();
}
