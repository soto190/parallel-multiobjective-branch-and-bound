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
#include "ProblemFJSSP.hpp"

/**
 *
 * This is a micro-genetic algorithm based on NSGA-II. It receives an initial solution to create the population.
 *
 **/
class NSGA_II{
public:
    NSGA_II(const ProblemFJSSP& problem);
    ~NSGA_II();

    void execute();

    void setSampleSolution(const Solution& solution);
    void setCrossoverRate(double rate);
    void setMutationRate(double rate);
    void setPopulationSize(unsigned long population_size);
    void setMaxNumberOfGenerations(unsigned long max_generations);
    void setMaxNumberOfEvaluations(unsigned long max_evaluations);

private:
    double crossover_rate;
    double mutation_rate;
    
    unsigned long max_number_of_generations;
    unsigned long max_number_of_evaluations;
    unsigned long number_of_generations_performed;
    unsigned long number_of_evaluations_performed;
    
    int generations_is_stop_criteria;
    int evaluations_is_stop_criteria;
    int max_population;
    
    ProblemFJSSP problem;
    
    vector<Solution> population;
    Solution sample_solution;
    
    void crossover();
    void mutation();
    void selection();
    void evaluatePopulation();
    void replacement();
    
    void fastNonDominatedSort();
    void crowdingDistance();
    unsigned long increaseNumberOfGenerations();
    unsigned long increaseNumberOfEvaluations();
    int isStoppingCriteriaReached();
    int stoppingCriteriaIsGenerations();
    int stoppingCriteriaIsEvaluations();
    int isMaxNumberOfGenerationsReached();
    int isMaxNumberOfEvaluationsReached();
    void updateProgress();
};

#endif /* NSGA_II_hpp */
