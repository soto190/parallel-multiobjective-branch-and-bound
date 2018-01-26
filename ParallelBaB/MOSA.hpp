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
#include "ProblemFJSSP.hpp"

class MOSA{
public:
    MOSA(const ProblemFJSSP& problem);
    ~MOSA();

    void solve();
    float getInitialTemperature() const;
    float getFinalTemperature() const;
    float getCurrentTemperature() const;
    float getCoolingRate() const;
    unsigned long getCurrentMetropolisIterations() const;
    unsigned long getMaxMetropolisIterations() const;
    void setInitialTemperature(float temperature);
    void setFinalTemperature(float temperature);
    void setMaxMetropolisIterations(unsigned long iterations);
    void setEnergyFunction();
    void setCoolingScheme();
    void setAcceptanceCriterion();
    void setSampleSolution(const Solution& sample);
    
private:
    float initial_temperature;
    float final_temperature;
    float current_temperature;
    float alpha_value;
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
    void coolingScheme();
    float computeEnergy(const Solution& current, const Solution& candidate);
    void update();
    const Solution perturbate(const Solution& solution_input);
    void generateRandomSolution();
};
#endif /* MOSA_hpp */
