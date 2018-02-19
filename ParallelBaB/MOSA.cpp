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
    
    ParetoFront p_front;
    p_front.push_back(sampleSolution);
    
    float energy_diff = 0;
    while (!isStoppingCriteriaReached()) {
        while (isMetropolisCycleActive()) {
            candidate = perturbate(current);
            energy_diff = computeEnergy(current, candidate);
            if (p_front.push_back(candidate))
                current = candidate;
            else if (energy_diff <= 0)
                current = candidate;
            else if(acceptanceCriterion(energy_diff, getCurrentTemperature()))
                current = candidate;
            increaseMetropolisIterations();
        }
        coolingScheme();
        update();
        std::uniform_int_distribution<> unif_int_dis(0, static_cast<int>(p_front.size() - 1));
        current = p_front.at(unif_int_dis(generator));
    }
    
    printf("Number of evaluatios: %lu\n", getNumberOfEvaluations());
    best_found.print();
    printf("\n");
    p_front.print();
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

unsigned long MOSA::getNumberOfEvaluations() const{
    return number_of_evaluations;
}

int MOSA::acceptanceCriterion(float energy, float temperature){
    std::uniform_real_distribution<double> unif_dis(0.0, 1.0);

    /** Boltzmann acceptance criterion. **/
    if (exp( (-energy) / temperature) < unif_dis(generator))
        return 1;
    return 0;
}

float MOSA::coolingScheme(){
    current_temperature = getCoolingRate() * getCurrentTemperature();
    return current_temperature;
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
    int n_code_values = problem.getNumberOfMachines();
    int new_machine = 0;
    int code_to_move = 0;
    int code_is_from_job = 0;
    int new_code = 0;

    std::uniform_real_distribution<double> unif_dis(0.0, 1.0);
    std::uniform_int_distribution<> unif_int_dis(0, problem.getNumberOfVariables() - 1);
    std::uniform_int_distribution<> unif_int_mach_dis(0, problem.getNumberOfMachines() - 1);

    for (int position = 0; position < solution_input.getNumberOfVariables(); ++position){
        perturbation_probability = unif_dis(generator);
        if (perturbation_probability < getPerturbationRate()) {
            new_position = unif_int_dis(generator);
            new_machine = unif_int_mach_dis(generator);
            code_to_move = perturbated_output.getVariable(position);
            code_is_from_job = floor(code_to_move / n_code_values);
            new_code = problem.getEncodeMap(code_is_from_job, new_machine);
            perturbated_output.setVariable(position, perturbated_output.getVariable(new_position));
            perturbated_output.setVariable(new_position, new_code);
        }
    }

    problem.evaluate(perturbated_output);
    increaseNumberOfEvaluations();
    return perturbated_output;
}

void MOSA::generateRandomSolution(){
    
}

void MOSA::increaseNumberOfEvaluations(){
    number_of_evaluations++;
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
