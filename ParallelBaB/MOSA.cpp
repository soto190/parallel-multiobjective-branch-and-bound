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
    max_energy_found = 0;
    min_energy_found = MAXFLOAT;
}

MOSA::~MOSA(){
    
}

void MOSA::solve(){
    initProgress();
    Solution best_found(sampleSolution);
    Solution current(sampleSolution);
    Solution candidate(sampleSolution);
    
    ParetoFront front;
    front.push_back(sampleSolution);
    
    float energy_diff = 0;
    while (!isStoppingCriteriaReached()) {
        while (isMetropolisCycleActive()) {
            candidate = perturbate(current);
            energy_diff = computeEnergy(current, candidate);
            DominanceRelation dominance_test = candidate.dominanceTest(best_found);
            if (dominance_test == DominanceRelation::Dominates || dominance_test == DominanceRelation::Nondominated){
                front.push_back(candidate);
                current = candidate;
                best_found = candidate;
            }
            else if (energy_diff <= 0)
                current = candidate;
            else if(acceptanceCriterion(energy_diff, getCurrentTemperature()))
                current = candidate;
            
            increaseMetropolisIterations();
        }
        coolingScheme();
        update();
        current = best_found;
    }
    best_found.print();
    printf("\n");
    front.print();
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

void MOSA::setCoolingRate(float new_alpha_value){
    alpha_value = new_alpha_value;
}

void MOSA::setAcceptanceCriterion(){
    
}

void MOSA::setSampleSolution(const Solution& sample){
    sampleSolution = sample;
}

void MOSA::initProgress(){
    current_temperature = getInitialTemperature();
}

int MOSA::isStoppingCriteriaReached() const{
    if (getCurrentTemperature() > getFinalTemperature())
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
    
    updateEnergies(energy);
    return energy;
}

void MOSA::update(){
    metropolis_iterations = 0;
}

const Solution MOSA::perturbate(const Solution& solution_input){
    Solution perturbated_output(solution_input);
    int new_position = 0;
    double perturbation_probability = 0.0;
    
    std::uniform_real_distribution<double> unif_dis(0.0, 1.0);
    std::uniform_int_distribution<> unif_int_dis(0, problem.getNumberOfVariables() - 1);

    for (int position = 0; position < solution_input.getNumberOfVariables(); ++position){
        perturbation_probability = unif_dis(generator);
        if (perturbation_probability < getPerturbationRate()) {
            new_position = unif_int_dis(generator);
            int code_to_move = perturbated_output.getVariable(position);
            perturbated_output.setVariable(position, perturbated_output.getVariable(new_position));
            perturbated_output.setVariable(new_position, code_to_move);
        }
    }
    
    problem.evaluate(perturbated_output);
    return perturbated_output;
}

void MOSA::generateRandomSolution(){
    
}

float MOSA::getMaxEnergyFound() const{
    return max_energy_found;
}

float MOSA::getMinEnergyFound() const{
    return min_energy_found;
}

double MOSA::getPerturbationRate() const{
    return perturbation_rate;
}

void MOSA::setPerturbationRate(double new_perturbation_rate){
    perturbation_rate = new_perturbation_rate;
}

void MOSA::setMaxEnergyFound(float energy){
    max_energy_found = energy;
}

void MOSA::setMinEnergyFound(float energy){
    min_energy_found = energy;
}

void MOSA::updateEnergies(float energy){
    if (energy > getMaxEnergyFound())
        setMaxEnergyFound(energy);
    
    if (energy < getMinEnergyFound())
        setMinEnergyFound(energy);
}

void MOSA::printDebug() const{
    printf("Temp: %f Metropolis: %lu\n", getCurrentTemperature(), getCurrentMetropolisIterations());
}
