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

void NSGA_II::setMaxNumberOfGenerations(unsigned long max_generations){
    max_number_of_generations = max_generations;
    generations_is_stop_criteria = 1;
}

void NSGA_II::setMaxNumberOfEvaluations(unsigned long max_evaluations){
    max_number_of_evaluations = max_evaluations;
    evaluations_is_stop_criteria = 1;
}

void NSGA_II::crossover(){
    
}

void NSGA_II::mutation(){
    
}

void NSGA_II::selection(){
    
}

void NSGA_II::evaluatePopulation(){
    std::vector<Solution>::iterator it_solution;
    for (it_solution = population.begin(); it_solution != population.end(); ++it_solution)
        problem.evaluate(*it_solution);
}

void NSGA_II::replacement(){
    
}

void NSGA_II::fastNonDominatedSort(){
    
}

void NSGA_II::crowdingDistance(){
    
}

unsigned long NSGA_II::increaseNumberOfGenerations(){
    return number_of_generations_performed++;
}

unsigned long NSGA_II::increaseNumberOfEvaluations(){
    return number_of_evaluations_performed++;
}

int NSGA_II::isStoppingCriteriaReached(){
    
    if (stoppingCriteriaIsGenerations() && isMaxNumberOfGenerationsReached())
        return 1;
    
    if (stoppingCriteriaIsEvaluations() && isMaxNumberOfEvaluationsReached())
        return 1;
    
    return 0;
}

int NSGA_II::stoppingCriteriaIsGenerations(){
    return generations_is_stop_criteria;
}

int NSGA_II::stoppingCriteriaIsEvaluations(){
    return evaluations_is_stop_criteria;
}

int NSGA_II::isMaxNumberOfGenerationsReached(){
    if(number_of_generations_performed < max_number_of_generations)
        return 0;
    return 1;
}

int NSGA_II::isMaxNumberOfEvaluationsReached(){
    if (number_of_evaluations_performed < max_number_of_evaluations)
        return 0;
    return 1;
}

void NSGA_II::updateProgress(){
    increaseNumberOfGenerations();
}

void NSGA_II::execute(){
    while (!isStoppingCriteriaReached()) {
        selection();
        crossover();
        mutation();
        evaluatePopulation();
        replacement();
        updateProgress();
    }
}
