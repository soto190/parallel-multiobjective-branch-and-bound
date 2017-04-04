//
//  ProblemHCSP.cpp
//  PhDProject
//
//  Created by Carlos Soto on 21/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "ProblemHCSP.hpp"


ProblemHCSP::ProblemHCSP(const ProblemHCSP& toCopy):Problem(toCopy){
    
    this->totalTasks = toCopy.totalTasks;
    this->totalMachines = toCopy.totalMachines;
    this->totalConfig = toCopy.totalConfig;
    this->totalMappings = toCopy.totalMappings;
    
    this->mappingConfig = new int * [this->totalMappings];
    this->maxConfigIn = new int[this->totalConfig];
    this->processingTime = new double * [this->totalTasks];
    this->voltage = new double * [this->totalConfig];
    this->speed = new double * [this->totalConfig];
    
    this->totalTasks = 0;
    this->totalMachines = 0;
    this->totalConfig = 0;
    this->totalMappings = 0;
    
    int config = 0, machine = 0, task;
    
    for (task = 0 ; task < this->totalTasks; task++) {
        this->processingTime[task] = new double[this->totalMachines];
        
        for (machine = 0; machine < this->totalMachines; machine++)
            this->processingTime[task][machine] = toCopy.processingTime[task][machine];
    }
    
    for (config = 0; config < this->totalConfig; config++){
        
        this->maxConfigIn[config] = toCopy.maxConfigIn[config];
        
        this->voltage[config] = new double[this->totalMachines];
        this->speed[config] = new double[this->totalMachines];
        
        for (machine = 0; machine < this->totalMachines; machine++) {
            this->speed[config][machine] = toCopy.speed[config][machine];
        }
    }
    
    for (config = 0; config < this->totalConfig; config++) {
        this->mappingConfig[config] = new int[2];
        this->mappingConfig[config][0] = toCopy.mappingConfig[config][0];
        this->mappingConfig[config][1] = toCopy.mappingConfig[config][1];
    }
    
}

ProblemHCSP::~ProblemHCSP(){
    int task = 0;
    int config = 0;
    int mappings = 0;
    
    for(task = 0; task < this->totalTasks; task++)
        delete[]  processingTime[task];
    
    delete [] processingTime;
    
    for(config = 0; config < this->totalConfig; config++){
        delete[] voltage[config];
        delete[] speed[config];
    }
    
    delete[] voltage;
    delete[] speed;
    
    for(mappings = 0; mappings < this->totalMappings; mappings++)
        delete[] mappingConfig[mappings];
    
    
    delete[] mappingConfig;
    delete[] maxConfigIn;
}

ProblemHCSP& ProblemHCSP::operator=(const ProblemHCSP &toCopy){
    this->totalTasks = toCopy.totalTasks;
    this->totalMachines = toCopy.totalMachines;
    this->totalConfig = toCopy.totalConfig;
    this->totalMappings = toCopy.totalMappings;
    
    this->mappingConfig = new int * [this->totalMappings];
    this->maxConfigIn = new int[this->totalConfig];
    this->processingTime = new double * [this->totalTasks];
    this->voltage = new double * [this->totalConfig];
    this->speed = new double * [this->totalConfig];
    
    this->totalTasks = 0;
    this->totalMachines = 0;
    this->totalConfig = 0;
    this->totalMappings = 0;
    
    int config = 0, machine = 0, task;
    
    for (task = 0 ; task < this->totalTasks; task++) {
        this->processingTime[task] = new double[this->totalMachines];
        
        for (machine = 0; machine < this->totalMachines; machine++)
            this->processingTime[task][machine] = toCopy.processingTime[task][machine];
        
    }
    
    for (config = 0; config < this->totalConfig; config++){
        
        this->maxConfigIn[config] = toCopy.maxConfigIn[config];
        
        this->voltage[config] = new double[this->totalMachines];
        this->speed[config] = new double[this->totalMachines];
        
        for (machine = 0; machine < this->totalMachines; machine++) {
            this->speed[config][machine] = toCopy.speed[config][machine];
        }
    }
    
    for (config = 0; config < this->totalConfig; config++) {
        this->mappingConfig[config] = new int[2];
        this->mappingConfig[config][0] = toCopy.mappingConfig[config][0];
        this->mappingConfig[config][1] = toCopy.mappingConfig[config][1];
    }
    
    return *this;
}

ProblemType ProblemHCSP::getType() const{ return ProblemType::combination;}
int ProblemHCSP::getStartingLevel(){ return 0; }
int ProblemHCSP::getFinalLevel(){ return this->totalVariables - 1;}
int ProblemHCSP::getLowerBound(int indexVar) const{ return 0;}
int ProblemHCSP::getUpperBound(int indexVar) const{ return this->totalMappings - 1; }
int ProblemHCSP::getLowerBoundInObj(int nObj) const{ return INT_MAX; }
int ProblemHCSP::getTotalElements(){return 0;}
int * ProblemHCSP::getElemensToRepeat(){ return nullptr;}
int ProblemHCSP::getDecodeMap(int map, int position){return 0;}
int ProblemHCSP::getCodeMap(int value1, int value2){return 0;}
int ProblemHCSP::getTimesValueIsRepeated(int value){return 0;}

double ProblemHCSP::computeProcessingTime(int task, int machine, int config){
   return this->processingTime[task][machine] / this->speed[config][machine];
}

double ProblemHCSP::computeEnergy(int task, int machine, int config, double proc_time){
    return proc_time * pow(this->voltage[config][machine], 2);
}

double ProblemHCSP::evaluate(Solution & solution){
    
    int mapping = 0;
    int machine = 0;
    int config = 0;
    double energy = 0;
    double proc_prime = 0;
    double makespan = 0;
    
    int i_machine = 0;
    int i_task = 0;
    
    for (i_machine = 0; i_machine < this->totalMachines; i_machine++)
        solution.execTime[i_machine] = 0;
    
    for (i_task = 0; i_task < this->totalTasks; i_task++) {
        mapping = solution.getVariable(0);
        machine = this->mappingConfig[mapping][0];
        config = this->mappingConfig[mapping][1];
        
        proc_prime = this->processingTime[i_task][machine] / this->speed[config][machine];
        solution.execTime[machine] += proc_prime;
        energy += proc_prime * this->voltage[config][machine] * this->voltage[config][machine];
        
        if(solution.execTime[machine] >= solution.execTime[solution.machineWithMakespan]){
            solution.machineWithMakespan = machine;
            makespan = solution.execTime[machine];
        }
    }
    
    solution.setObjective(0, makespan);
    solution.setObjective(1, energy);
    return 0;
}

/**
 * Receives a partial Solution and the level to evaluate the solution.
 * This functio uses the value of the last evaluation.
 */
/*
double ProblemHCSP::evaluatePartial(Solution * solution, int levelEvaluation){
    
    int mapping = solution->getVariable(levelEvaluation);
    int machine = this->mappingConfig[mapping][0];
    int config = this->mappingConfig[mapping][1];
    
    double proc_prime = this->computeProcessingTime(levelEvaluation, machine, config);
    solution->execTime[machine] += proc_prime;
    
    double energy = solution->getObjective(1) + this->computeEnergy(levelEvaluation, machine, config, proc_prime);
    
    for (machine = 0; machine < this->totalMachines; machine++)
        if(solution->execTime[machine] >= solution->execTime[solution->machineWithMakespan]){
            solution->machineWithMakespan = machine;
            solution->setObjective(0, solution->execTime[machine]);
        }
    
    solution->setObjective(1, energy);
    return 0;
}
*/


double ProblemHCSP::evaluatePartial(Solution & solution, int levelEvaluation){
    
    int mapping = 0;
    int machine = 0;
    int config = 0;
    double energy = 0;
    double proc_prime = 0;
    double makespan = 0;
    
    int i_machine = 0;
    int i_task = 0;
    
    for (i_machine = 0; i_machine < this->totalMachines; i_machine++)
        solution.execTime[i_machine] = 0;
    
    for (i_task = 0; i_task <= levelEvaluation; i_task++) {
        mapping = solution.getVariable(i_task);
        machine = this->mappingConfig[mapping][0];
        config = this->mappingConfig[mapping][1];
        
        proc_prime = this->computeProcessingTime(i_task, machine, config);// this->processingTime[i_task][machine] / this->speed[config][machine];
        solution.execTime[machine] += proc_prime;
        energy += this->computeEnergy(i_task, machine, config, proc_prime);
        if(solution.execTime[machine] >= solution.execTime[solution.machineWithMakespan]){
            solution.machineWithMakespan = machine;
            makespan = solution.execTime[machine];
        }
    }
    
    solution.setObjective(0, makespan);
    solution.setObjective(1, energy);
    return 0;
}

double ProblemHCSP::evaluateLastLevel(Solution * solution){
    
    /**
     * For this problem there is no special evaluation on the last level.
     **/
/*
    int machine = 0;
    for (machine = 0; machine < this->totalMachines; machine++)
        if(solution->execTime[machine] >= solution->execTime[solution->machineWithMakespan]){
            solution->machineWithMakespan = machine;
            solution->objective[0] = solution->execTime[machine];
        }
*/
    return 0.0;
}


/**
 * Removes the $n$ level. Decreases the objective values removing the values from lastLevel to nextLevel.
 **/
double ProblemHCSP::removeLastEvaluation(Solution * solution, int lastLevel, int nextLevel){
    
    int mapping = 0;
    int machine = 0;
    int config = 0;
    int level = 0;
    double proc_prime = 0;
    
    for (level = lastLevel; level >= nextLevel; level--) {
        
        mapping = solution->getVariable(level);
        machine = this->mappingConfig[mapping][0];
        config = this->mappingConfig[mapping][1];
        
        proc_prime = this->computeProcessingTime(level, machine, config);
        solution->execTime[machine] -= proc_prime;
        solution->objective[1] -= this->computeEnergy(level, machine, config, proc_prime);
        
        solution->setVariable(level, -1);
    }
    
    for (machine = 0; machine < this->totalMachines; machine++)
        if(solution->execTime[machine] >= solution->execTime[solution->machineWithMakespan]){
            solution->machineWithMakespan = machine;
            solution->objective[0] = solution->execTime[machine];
        }
    
    return 0.0;
}

double ProblemHCSP::removeLastLevelEvaluation(Solution * solution, int newLevel){
    
    if(newLevel > 0){
        int mapping = 0;
        int machine = 0;
        int config = 0;
        int level = 0;
        double proc_prime = 0;
        
        for (level = this->getNumberOfVariables() - 1; level >= newLevel; level--) {
            
            mapping = solution->getVariable(level);
            machine = this->mappingConfig[mapping][0];
            config = this->mappingConfig[mapping][1];
            
            proc_prime = this->computeProcessingTime(level, machine, config);
            solution->execTime[machine] -= proc_prime;
            solution->objective[1] -= this->computeEnergy(level, machine, config, proc_prime);
            solution->setVariable(level, -1);
        }
        
        for (machine = 0; machine < this->totalMachines; machine++)
            if(solution->execTime[machine] >= solution->execTime[solution->machineWithMakespan]){
                solution->machineWithMakespan = machine;
                solution->objective[0] = solution->execTime[machine];
            }
    }
     
    return 0.0;
}

void ProblemHCSP::createDefaultSolution(Solution & solution){}
void ProblemHCSP::getSolutionWithLowerBoundInObj(int nObj, Solution & solution){}

void ProblemHCSP::printInstance(){
    
    printf("Hello from Problem Scheduling!\n");
}

void ProblemHCSP::printProblemInfo() const{
    printf("Total tasks: %d\n", this->totalTasks);
    printf("Total machines: %d\n", this->totalMachines);
    printf("Total mappings: %d\n", this->totalMappings);
    
    printf("Proccesig times:\n");
    int task = 0, machine = 0, config = 0, mapping = 0;
    for (task = 0; task < this->totalTasks; task++) {
        printf("[%4d] ", task);
        for (machine = 0; machine < this->totalMachines; machine++)
            printf("%6.3f ", this->processingTime[task][machine]);
        printf("\n");
    }
    
    printf("Voltage and speed configurations\n");
    for (config = 0; config < this->totalConfig; config++) {
        printf("[%4d] ", config);
        for (machine = 0; machine < this->totalMachines; machine++)
            if(voltage[config][machine] != 0.0)
                printf("%4.2f %4.2f ", this->voltage[config][machine], this->speed[config][machine]);
            else
                printf("---- ---- ");
        
        printf("\n");
    }
    
    printf("Configurations mapping\n");
    for (mapping = 0; mapping < this->totalMappings; mapping++)
        printf("[%4d] %3d %3d\n", mapping, this->mappingConfig[mapping][0], this->mappingConfig[mapping][1]);
    
}

void ProblemHCSP::loadInstance(char *filePath[]){
    
    std::ifstream infile(filePath[0]);
    std::string line;
    std::vector<std::string> elemens;
    std::vector<std::string> splited;
    
    elemens = split(filePath[0], '_');
    
    unsigned long sizeElemens = elemens.size();
    
    this->totalTasks = std::stoi(elemens.at(sizeElemens - 2));
    this->totalMachines = std::stoi(elemens.at(sizeElemens - 3));
    
    this->totalVariables = this->totalTasks;
    
    std::getline(infile, line);
    std::getline(infile, line);
    std::getline(infile, line);
    
    this->processingTime = new double * [this->totalTasks];
    int task = 0;
    int machine = 0;
    
    for (task = 0; task < this->totalTasks; task++) {
        this->processingTime[task] = new double[this->totalMachines];
        std::getline(infile, line);
        elemens = split(line, ':');
        splited = split(elemens.at(1), ',');
        for(machine = 0; machine < this->totalMachines; machine++)
            //this->processingTime[task][machine] = std::stof(splited.at(machine).c_str());
            this->processingTime[task][machine] = std::stof(splited.at(machine));
    }
    
    elemens.clear();
    splited.clear();
    infile.close();
    
    this->readMachinesConfigurations(filePath[1]);
}

void ProblemHCSP::readMachinesConfigurations(char *filePath){
    
    std::ifstream infile(filePath);
    std::string line;
    std::vector<std::string> elemens;
    
    std::getline(infile, line); /** reads comment in first line. **/
    std::getline(infile, line); /** reads total config and machines. **/

    elemens = split(line, ' ');
    this->totalConfig = std::stoi(elemens.at(0));
    int totalMachinesInConfig = std::stoi(elemens.at(1));
    
    if (this->totalMachines < totalMachinesInConfig)
        totalMachinesInConfig = this->totalMachines;

    
    this->maxConfigIn = new int [this->totalMachines];
    this->voltage = new double * [this->totalConfig];
    this->speed = new double * [this->totalConfig];
    
    int machine = 0;
    for (machine = 0; machine < this->totalMachines; machine++)
        this->maxConfigIn[machine] = 0;
    
    this->totalMappings = 0;
    int config = 0;
    
    
    for (config = 0; config < this->totalConfig; config++)
        this->voltage[config] = new double[this->totalMachines];

    for (config = 0; config < this->totalConfig; config++)
        this->speed[config] = new double[this->totalMachines];

    std::getline(infile, line); /** reads comment of voltage and speed. **/

    std::string voltageS;
    std::string speedS;
    int pos = 0;
    for (config = 0; config < this->totalConfig; config++) {
        std::getline(infile, line);
        elemens = split(line, ' ');
        pos = 0;
        for(machine = 0; machine < totalMachinesInConfig; machine++){
            voltageS = elemens.at(++pos);
            speedS = elemens.at(++pos);

            this->voltage[config][machine] = 0;
            this->speed[config][machine] = 0;
            
            if(voltageS.compare("----") != 0){
                this->maxConfigIn[machine]++;
                this->totalMappings++;
        
                this->voltage[config][machine] = std::stod(voltageS);
                this->speed[config][machine] = std::stod(speedS);
            }
        }
    }
    
    for(config = 0; config < this->totalConfig; config++)
        for (machine = totalMachinesInConfig; machine < this->totalMachines; machine++) {
            this->maxConfigIn[machine] = this->maxConfigIn[machine - totalMachinesInConfig];
            this->voltage[config][machine] = this->voltage[config][machine - totalMachinesInConfig];
            this->speed[config][machine] = this->speed[config][machine - totalMachinesInConfig];
            if ( this->speed[config][machine] != 0.0)
                this->totalMappings++;
        }
    
    int m_config = 0;
    int m_machine = 0;
    int k_config = 0;
    this->mappingConfig = new int * [this->totalMappings];
    
    for(k_config = 0; k_config < this->totalMappings; k_config++)
        this->mappingConfig[k_config] = new int[2];
    
    k_config = 0;
    for(m_machine = 0; m_machine < this->totalMachines; m_machine++)
        for(m_config = 0; m_config < this->maxConfigIn[m_machine]; m_config++){
            this->mappingConfig[k_config][0] = m_machine;
            this->mappingConfig[k_config][1] = m_config;
            k_config++;
        }
    
    elemens.clear();
    infile.close();
}

void ProblemHCSP::printSolution(const Solution & solution) const{
    printPartialSolution(solution, this->totalVariables);
}

void ProblemHCSP::printSolutionInfo(const Solution & solution) const{
    printf("TODO: Implement this function.\n");
}

void ProblemHCSP::printPartialSolution(const Solution & solution, int level) const{

}
