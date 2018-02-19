//
//  NSGA_II.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef NSGA_II_hpp
#define NSGA_II_hpp

#include <stdio.h>
#include <random>
#include "ProblemFJSSP.hpp"
#include "ParetoFront.hpp"

/**
 *
 * This is a micro-genetic algorithm based on NSGA-II. It receives an initial solution to create the population.
 *
 **/
class NSGA_II{
public:
    NSGA_II(const ProblemFJSSP& problem);
    ~NSGA_II();

    void solve();

    void setSampleSolution(const Solution& solution);
    void setCrossoverRate(double rate);
    void setMutationRate(double rate);
    void setMaxPopulationSize(unsigned long population_size);
    void setMaxNumberOfGenerations(unsigned long max_generations);
    void setMaxNumberOfEvaluations(unsigned long max_evaluations);
    unsigned long getMaxPopulationSize() const;

private:
    double crossover_rate;
    double mutation_rate;
    
    unsigned long max_number_of_generations;
    unsigned long max_number_of_evaluations;
    unsigned long number_of_generations_performed;
    unsigned long number_of_evaluations_performed;
    
    bool generations_is_stop_criteria;
    bool evaluations_is_stop_criteria;
    unsigned long max_population;
    
    std::default_random_engine generator;
    
    ProblemFJSSP problem;
    Solution sample_solution;
    vector<Solution> population;
    
    void initialize();
    void createInitialPopulation();
    void selection();
    void crossover();
    void mutation();
    void evaluatePopulation();
    void replacement();
    
    vector<Solution> crossoverOperator(const Solution& parent1, const Solution& parent2);
    void mutationOperator(Solution& solution);
    double getCrossoverRate() const;
    double getMutationRate() const;
    void fastNonDominatedSort();
    void crowdingDistance();
    unsigned long increaseNumberOfGenerations();
    unsigned long increaseNumberOfEvaluations();
    bool isStoppingCriteriaReached() const;
    bool stoppingCriteriaIsGenerations() const;
    bool stoppingCriteriaIsEvaluations() const;
    bool isMaxNumberOfGenerationsReached() const;
    bool isMaxNumberOfEvaluationsReached() const;
    void updateProgress();
    void printPopulation() const;
};
#endif /* NSGA_II_hpp */
