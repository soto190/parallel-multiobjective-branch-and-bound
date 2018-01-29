//
//  MOSA.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/26/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//
/**
 * Multi-Objective Simulated Annealing (MOSA) algorithm.
 */
#include "MOSA.hpp"

MOSA::MOSA(const ProblemFJSSP& problem):
problem(problem){
    initial_temperature = 0;
    final_temperature = 0;
    current_temperature = 0;
    alpha_value = 0;
    metropolis_iterations = 0;
    max_metropolis_iterations = 0;
}

MOSA::~MOSA(){
    
}

void MOSA::solve(){
    initProgress();
    Solution current(sampleSolution);
    Solution candidate (sampleSolution);
    float energy_diff = 0;
    while (!isStoppingCriteriaReached()) {
        while (isMetropolisCycleActive()) {
            candidate = perturbate(current);
            energy_diff = computeEnergy(current, candidate);
            if (energy_diff <= 0) {
                current = candidate;
            }else if(acceptanceCriterion(energy_diff, getCurrentTemperature())){
                current = candidate;
            }
            increaseMetropolisIterations();
        }
        coolingScheme();
        update();
    }
}

float MOSA::getInitialTemperature() const{
    return initial_temperature;
}

float MOSA::getFinalTemperature() const{
    return final_temperature;
}

float MOSA::getCurrentTemperature() const{
    return current_temperature;
}

float MOSA::getCoolingRate() const{
    return alpha_value;
}

unsigned long MOSA::getCurrentMetropolisIterations() const{
    return metropolis_iterations;
}

unsigned long MOSA::getMaxMetropolisIterations() const{
    return max_metropolis_iterations;
}

void MOSA::setInitialTemperature(float temperature){
    initial_temperature = temperature;
}

void MOSA::setFinalTemperature(float temperature){
    final_temperature = temperature;
}

void MOSA::setMaxMetropolisIterations(unsigned long iterations){
    max_metropolis_iterations = iterations;
}

void MOSA::setEnergyFunction(){
    
}

void MOSA::setCoolingScheme(){
    
}

void MOSA::setAcceptanceCriterion(){
    
}

void MOSA::setSampleSolution(const Solution& sample){
    sampleSolution = sample;
}

void MOSA::initProgress(){
    
}

int MOSA::isStoppingCriteriaReached() const{
    if (getCurrentTemperature() < getFinalTemperature())
        return 0;
    return 1;
}

int MOSA::isMetropolisCycleActive() const{
    if (getCurrentMetropolisIterations() < getMaxMetropolisIterations())
        return  1;
    return 0;
}

void MOSA::increaseMetropolisIterations(){
    metropolis_iterations++;
}

int MOSA::acceptanceCriterion(float energy, float temperature){
    std::uniform_real_distribution<double> unif_dis(0.0, 1.0);

    /** Boltzmann acceptance criterion. **/
    if (exp( (-energy) / temperature) < unif_dis(generator))
        return 1;
    return 0;
}

void MOSA::coolingScheme(){
    current_temperature = getCoolingRate() * getCurrentTemperature();
}

float MOSA::computeEnergy(const Solution &candidate, const Solution &current){
    float energy = 0.0;
    for (int obj = 0; obj < candidate.getNumberOfObjectives(); ++obj)
        energy += candidate.getObjective(obj) - current.getObjective(obj);
    return energy;
}

void MOSA::update(){
    metropolis_iterations = 0;
}

const Solution MOSA::perturbate(const Solution& solution_input){
    Solution perturbated_output(solution_input);
    
    
    problem.evaluate(perturbated_output);
    return perturbated_output;
}

void MOSA::generateRandomSolution(){
    
}
