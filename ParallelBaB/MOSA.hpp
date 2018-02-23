//
//  MOSA.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/26/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef MOSA_hpp
#define MOSA_hpp

#include <stdio.h>
#include <random>
#include "ParetoFront.hpp"
#include "ProblemFJSSP.hpp"

class MOSA {
    
public:
    MOSA(const ProblemFJSSP& problem);
    ~MOSA();

    ParetoFront solve();
    float getInitialTemperature() const;
    float getFinalTemperature() const;
    float getCurrentTemperature() const;
    float getCoolingRate() const;
    unsigned long getCurrentMetropolisIterations() const;
    unsigned long getMaxMetropolisIterations() const;
    double getPerturbationRate() const;
    unsigned long getNumberOfEvaluations() const;
    void setInitialTemperature(float temperature);
    void setFinalTemperature(float temperature);
    void setMaxMetropolisIterations(unsigned long iterations);
    void setEnergyFunction();
    void setCoolingScheme();
    void setCoolingRate(float new_alpha_value);
    void setAcceptanceCriterion();
    void setPerturbationRate(double new_perturbation_rate);
    void setSampleSolution(const Solution& sample);
    
private:
    float initial_temperature;
    float final_temperature;
    float current_temperature;
    float alpha_value;
    float max_energy_found;
    float min_energy_found;
    double perturbation_rate;
    unsigned int number_of_evaluations;
    
    unsigned long metropolis_iterations;
    unsigned long max_metropolis_iterations;
    ProblemFJSSP problem;
    Solution sampleSolution;
    vector<Solution> population;
    std::default_random_engine generator;

    void initProgress();
    int isStoppingCriteriaReached() const;
    int isMetropolisCycleActive() const;
    void increaseMetropolisIterations();
    int acceptanceCriterion(float energy, float temperature);
    float coolingScheme();
    float computeEnergy(const Solution& current, const Solution& candidate);
    void update();
    const Solution perturbate(const Solution& solution_input);
    void generateRandomSolution();
    float getMaxEnergyFound() const;
    float getMinEnergyFound() const;
    void setMaxEnergyFound(float energy);
    void setMinEnergyFound(float energy);
    void updateEnergies(float energy);
    void increaseNumberOfEvaluations();
    void printDebug() const;
};
#endif /* MOSA_hpp */
