//
//  NSGA_II.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "NSGA_II.hpp"

NSGA_II::NSGA_II(const ProblemFJSSP& problem):problem(problem){
    generations_is_stop_criteria = 0;
    evaluations_is_stop_criteria = 0;
    max_population = 10;
    population.reserve(10);
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
    generations_is_stop_criteria = 1;
}

void NSGA_II::setMaxNumberOfEvaluations(unsigned long max_evaluations){
    max_number_of_evaluations = max_evaluations;
    evaluations_is_stop_criteria = 1;
}

unsigned long NSGA_II::getMaxPopulationSize() const{
    return max_population;
}

void NSGA_II::createInitialPopulation(){
    
    population.push_back(sample_solution);
    
    for (int n_solutions = 1; n_solutions < getMaxPopulationSize(); ++n_solutions) {
        population.push_back(sample_solution);
    }
}

void NSGA_II::selection(){
    
}

void NSGA_II::crossover(){
    
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
    
}

vector<Solution> NSGA_II::crossoverOperator(const Solution &parent1, const Solution &parent2){
    vector<Solution> offspring;
    offspring.reserve(2);
    
    Solution offspring1(parent1);
    Solution offspring2(parent2);
    
    
    
    offspring.push_back(offspring1);
    offspring.push_back(offspring2);
    
    return offspring;
}

/**
 *  Each gene has a probability to be mutated. The probability is given by the mutation rate.
 **/
void NSGA_II::mutationOperator(Solution &solution){
    
    double mutation_probability;
    int chromosome_size = solution.getNumberOfVariables();
    int new_allel = 0;
    std::uniform_real_distribution<double> unif_dis(0.0, 1.0);
    std::uniform_int_distribution<> unif_int_dis(0, problem.getUpperBound(0));

    FJSSPdata fjssp_data(problem.getNumberOfJobs(), problem.getNumberOfOperations(), problem.getNumberOfMachines());
    
    for (int gene = 0; gene < chromosome_size; ++gene){
        mutation_probability = unif_dis(generator);
        if (mutation_probability < getMutationRate()) {
            new_allel = unif_int_dis(generator);
            
            solution.setVariable(gene, new_allel);
            problem.evaluateDynamic(solution, fjssp_data, gene);
            
            /**
             * NOTE: If the new allel produces an infactible schedule; consider to undo the gene mutation or
             * search for anothe posible allel. This can be time consuming when the schedule is almost complete.
             **/
        }else{
            problem.evaluateDynamic(solution, fjssp_data, gene);
        }
    }
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
    vector<Solution> paretoFront;

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

int NSGA_II::isStoppingCriteriaReached() const{
    
    if (stoppingCriteriaIsGenerations() && isMaxNumberOfGenerationsReached())
        return 1;
    
    if (stoppingCriteriaIsEvaluations() && isMaxNumberOfEvaluationsReached())
        return 1;
    
    return 0;
}

int NSGA_II::stoppingCriteriaIsGenerations() const{
    return generations_is_stop_criteria;
}

int NSGA_II::stoppingCriteriaIsEvaluations() const{
    return evaluations_is_stop_criteria;
}

int NSGA_II::isMaxNumberOfGenerationsReached() const{
    if(number_of_generations_performed < max_number_of_generations)
        return 0;
    return 1;
}

int NSGA_II::isMaxNumberOfEvaluationsReached() const{
    if (number_of_evaluations_performed < max_number_of_evaluations)
        return 0;
    return 1;
}

void NSGA_II::updateProgress(){
    increaseNumberOfGenerations();
}

void NSGA_II::solve(){
    while (!isStoppingCriteriaReached()) {
        selection();
        crossover();
        mutation();
        evaluatePopulation();
        replacement();
        updateProgress();
    }
}
