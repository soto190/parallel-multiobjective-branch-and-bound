//
//  ProblemTSP.cpp
//  PhDProject
//
//  Created by Carlos Soto on 21/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "ProblemTSP.hpp"


ProblemTSP::ProblemTSP(const ProblemTSP& toCopy):Problem(toCopy){
    this->costCoord = new int * [toCopy.totalVariables];
    this->costs = new double * [toCopy.totalVariables];
    this->cityCoord = new int * [toCopy.totalVariables];
    this->euclideanDistance = new double * [toCopy.totalVariables];
    
    int origin = 0, dest = 0;
    for (origin = 0; origin < this->totalVariables; origin++) {
        this->costCoord[origin] = new int [toCopy.totalVariables];
        this->costs[origin] = new double [toCopy.totalVariables];
        this->cityCoord[origin] = new int [toCopy.totalVariables];
        this->euclideanDistance[origin] = new double [toCopy.totalVariables];
        
        for (dest = 0; dest < this->totalVariables; ++dest) {
            this->costCoord[origin][dest] = toCopy.costCoord[origin][dest];
            this->costs[origin][dest] = toCopy.costs[origin][dest];
            this->cityCoord[origin][dest] = toCopy.cityCoord[origin][dest];
            this->euclideanDistance[origin][dest] = toCopy.euclideanDistance[origin][dest];
            
        }
    }
}

ProblemTSP::ProblemTSP(int totalObjectives, int totalVariables):Problem(totalObjectives, totalVariables){
    this->costCoord = new int * [1];
    this->costs = new double * [1];
    this->cityCoord = new int * [1];
    this->euclideanDistance = new double * [1];
    
}

ProblemTSP::~ProblemTSP(){
    int city = 0;
    for (city = 0; city < this->totalVariables; city++) {
        delete[] euclideanDistance[city];
        delete[] cityCoord[city];
        delete[] costs[city];
        delete[] costCoord[city];
    }
    
    delete[] euclideanDistance;
    delete[] cityCoord;
    delete[] costs;
    delete[] costCoord;
    
}

ProblemTSP& ProblemTSP::operator=(const ProblemTSP& toCopy){
    
    this->costCoord = new int * [toCopy.totalVariables];
    this->costs = new double * [toCopy.totalVariables];
    this->cityCoord = new int * [toCopy.totalVariables];
    this->euclideanDistance = new double * [toCopy.totalVariables];
    
    int origin = 0, dest = 0;
    for (origin = 0; origin < this->totalVariables; origin++) {
        this->costCoord[origin] = new int [toCopy.totalVariables];
        this->costs[origin] = new double [toCopy.totalVariables];
        this->cityCoord[origin] = new int [toCopy.totalVariables];
        this->euclideanDistance[origin] = new double [toCopy.totalVariables];
        
        for (dest = 0; dest < this->totalVariables; ++dest) {
            this->costCoord[origin][dest] = toCopy.costCoord[origin][dest];
            this->costs[origin][dest] = toCopy.costs[origin][dest];
            this->cityCoord[origin][dest] = toCopy.cityCoord[origin][dest];
            this->euclideanDistance[origin][dest] = toCopy.euclideanDistance[origin][dest];
            
        }
    }
    return *this;
}

ProblemType ProblemTSP::getType() const{
    return ProblemType::permutation;
}

int ProblemTSP::getStartingLevel(){
    return 1;
}

int ProblemTSP::getFinalLevel(){
    return this->getNumberOfVariables() - getStartingLevel();
}

int ProblemTSP::getLowerBound(int indexVar) const{
    return 1;
}

int ProblemTSP::getUpperBound(int indexVar) const{
    return this->getNumberOfVariables() - 1;
}

int ProblemTSP::getLowerBoundInObj(int nObj) const{
    return INT_MAX;
}

int ProblemTSP::getTotalElements(){
    return 0;
}

int * ProblemTSP::getElemensToRepeat(){
    return nullptr;
}

int ProblemTSP::getMapping(int map, int position){
    return 0;
}

int ProblemTSP::getMappingOf(int value1, int value2){
    return 0;
}

int ProblemTSP::getTimesValueIsRepeated(int value){
    return 0;
}

double ProblemTSP::evaluate(Solution & solution){
    double sumDist = 0;
    double sumCost = 0;
    int city = 0;
    
    for (city = 0; city < this->getNumberOfVariables() - 1; city++) {
        int origen = solution.getVariable(city);
        int destino = solution.getVariable(city + 1);
        
        sumDist += this->euclideanDistance[origen][destino];
        sumCost += this->costs[origen][destino];
        
    }
    
    int last = solution.getVariable(this->getNumberOfVariables() - 1);
    int first = solution.getVariable(0);
    
    sumDist += this->euclideanDistance[last][first];
    sumCost += this->costs[last][first];
    
    solution.setObjective(0, sumDist);
    solution.setObjective(1, sumCost);
    
    return sumDist;
}

/**
 * This functions adds the distance and cost from the actual city and previous city.
 *
 **/
double ProblemTSP::evaluatePartial(Solution& solution, int levelEvaluation){
    /*
    
    int origin = solution->getVariable(levelEvaluation - 1);
    int destiny = solution->getVariable(levelEvaluation);
    
    double distance = solution->getObjective(0) + this->euclideanDistance[origin][destiny];
    double cost = solution->getObjective(1) + this->costs[origin][destiny];
    solution->setObjective(0, distance);
    solution->setObjective(1, cost);
    
    */
    double sumDist = 0;
    double sumCost = 0;
    int city = 0;
    
    for (city = 1; city <= levelEvaluation; city++) {
        int origen = solution.getVariable(city - 1);
        int destino = solution.getVariable(city);
        
        sumDist += this->euclideanDistance[origen][destino];
        sumCost += this->costs[origen][destino];
        
    }
    
    solution.setObjective(0, sumDist);
    solution.setObjective(1, sumCost);
    
    return sumDist;

    
    return 0;
}

/**
 * Some problems requieres a different evaluation in the leaf or last variable.
 * For example in TSP the las level includes the sum of the cost of the last city with the first city.
 */
double ProblemTSP::evaluateLastLevel(Solution * solution){
    int last = solution->getVariable(this->getNumberOfVariables() - 1);
    int first = solution->getVariable(0);
    
    double distance = solution->getObjective(0) + this->euclideanDistance[last][first];
    double cost = solution->getObjective(1) + this->costs[last][first];
    
    solution->setObjective(0, distance);
    solution->setObjective(1, cost);
    
    return 0;
    
}

double ProblemTSP::removeLastEvaluation(Solution* solution, int lastLevel, int nextLevel){
    
    if(nextLevel <= lastLevel && nextLevel > 0){
        
        int origen = 0, destino = 0;
        double distance = 0;
        double cost = 0;
        int city = 0;
        
        for (city = lastLevel; city >= nextLevel; city--) {
            origen = solution->getVariable(city - 1);
            destino = solution->getVariable(city);
            
            distance += this->euclideanDistance[origen][destino];
            cost += this->costs[origen][destino];
            solution->setVariable(city, -1);
        }
        
        distance = solution->getObjective(0) - distance;
        cost = solution->getObjective(1) - cost;
        
        solution->setObjective(0, distance);
        solution->setObjective(1, cost);
    }
    
    return 0.0;
}

double ProblemTSP::removeLastLevelEvaluation(Solution * solution, int newLevel){
    if(newLevel > 0){
        int last = solution->getVariable(this->getNumberOfVariables() - 1);
        int first = solution->getVariable(0);
        
        int origen = 0, destino = 0;
        
        double distance = 0;
        double cost = 0;
        int city = 0;
        
        for (city = this->getNumberOfVariables() - 1; city >= newLevel; city--) {
            origen = solution->getVariable(city - 1);
            destino = solution->getVariable(city);
            
            distance += this->euclideanDistance[origen][destino];
            cost += this->costs[origen][destino];
            solution->setVariable(city, -1);
        }
        
        distance = solution->getObjective(0) - this->euclideanDistance[last][first] - distance;
        cost = solution->getObjective(1) - this->costs[last][first] - cost;
        
        solution->setObjective(0, distance);
        solution->setObjective(1, cost);
    }
    
    return 0.0;
}

void ProblemTSP::createDefaultSolution(Solution& solution){
    int city = 0;
    for (city = 0; city < this->getUpperBound(0); city++)
        solution.setVariable(city, city);
    
}

void ProblemTSP::getSolutionWithLowerBoundInObj(int nObj, Solution& solution){    
}

void ProblemTSP::printInstance(){
    
    printf("Hello from Problem TSP!\n");
}

void ProblemTSP::printProblemInfo() const{
    int row = 0, col = 0;
    
    for (row = 0; row < this->totalVariables; row++) {
        printf("[%6d] ", row);
        for (col = 0; col < this->totalVariables; col++)
            printf("%10.3f ", this->euclideanDistance[row][col]);
        printf("\n");
    }
    
    printf("\n");
    for (row = 0; row < this->totalVariables; row++) {
        printf("[%6d] ", row);
        for (col = 0; col < this->totalVariables; col++)
            printf("%10.3f ", this->costs[row][col]);
        printf("\n");
    }
}

double ProblemTSP::computeEuclideanDistance(int x1, int y1, int x2, int y2){
    
    double x = x1 - x2;
    double y = y1 - y2;
    
    return sqrt(pow(x, 2) + pow(y, 2));
}

void ProblemTSP::loadInstance(char *filePath[]){
    
    std::ifstream infile(filePath[0]);
    std::string line;
    std::vector<std::string> elemens;
    
    while (std::getline(infile, line)) {
        elemens = split(line, ' ');
        std::string label = elemens.at(0).c_str();
        
        /** The string "NODE_COORD_SECTION" is read with the carriage return "NODE_COORD_SECTION\r".
         The next line removes it.**/
        //label = std::regex_replace(label, std::regex("(\r)"), "");
        label.erase(label.size() - 1);
        
        /** Reads the total of cities. **/
        if(label.compare("NAME") == 0){
            printf("Name read...\n");
        }
        
        else if(label.compare("COMMENT") == 0){
            printf("Comment read...\n");
        }
        
        else if(label.compare("TYPE") == 0){
            printf("Type read...\n");
        }
        
        else if(label.compare("DIMENSION") == 0){
            printf("Dimension read...\n");
            this->setNumberOfVariables(std::stoi(elemens.at(2).c_str()));
        }
        
        else if(label.compare("EDGE_WEIGHT_TYPE") == 0){
            printf("Edge weight type read...\n");
        }
        else if(label.compare("NODE_COORD_SECTION") == 0){
            /** Reads the coordenates of each city and computes the distances between cities. **/
            int totalCities = this->totalVariables;
            int city = 0;
            
            this->cityCoord = new int*[totalCities];
            for (city = 0; city < totalCities; city++) {
                
                this->cityCoord[city] = new int[2];
                
                std::getline(infile, line);
                elemens = split(line, ' ');
                
                this->cityCoord[city][0] = std::stoi(elemens.at(1).c_str());
                this->cityCoord[city][1] = std::stoi(elemens.at(2).c_str());
            }
            
            this->euclideanDistance = new double * [totalCities];
            for (city = 0; city < totalCities; city++) {
                this->euclideanDistance[city] = new double[totalCities];
                this->euclideanDistance[city][city] = 0;
            }
            
            int origin = 0;
            int destiny = 0;
            for (origin = 0; origin < totalCities - 1; origin++)
                for (destiny = origin + 1; destiny < totalCities; destiny++) {
                    this->euclideanDistance[origin][destiny] = this->computeEuclideanDistance(this->cityCoord[origin][0], this->cityCoord[origin][1], this->cityCoord[destiny][0], this->cityCoord[destiny][1]);
                    
                    this->euclideanDistance[destiny][origin] = this->euclideanDistance[origin][destiny];
                }
            printf("Node coord section read...\n");
        }
    }
    
    elemens.clear();
    
    infile.close();
    
    this->readCost(filePath[1]);
    
}

void ProblemTSP::readCost(char *filePath){
    
    std::ifstream infile(filePath);
    std::string line;
    std::vector<std::string> elemens;
    int dimensionInSecondFile = 0;
    
    while (std::getline(infile, line)) {
        elemens = split(line, ' ');
        std::string label = elemens.at(0).c_str();
        
        /** The string "NODE_COORD_SECTION" is read with the carriage return "NODE_COORD_SECTION\r".
         The next line removes it.**/
        //label = std::regex_replace(label, std::regex("(\r)"), "");
        
        /** Reads the total of cities. **/
        if(label.compare("NAME") == 0){
            printf("Name read...\n");
        }
        
        else if(label.compare("COMMENT") == 0){
            printf("Comment read...\n");
        }
        
        else if(label.compare("TYPE") == 0){
            printf("Type read...\n");
        }
        
        else if(label.compare("DIMENSION") == 0){
            printf("Dimension read...\n");
            dimensionInSecondFile = std::stoi(elemens.at(2).c_str());
            
            if(dimensionInSecondFile != this->totalVariables){
                printf("##### Dimensions from input files are different!!!!...");
            }
        }
        
        else if(label.compare("EDGE_WEIGHT_TYPE") == 0){
            printf("Edge weight type read...\n");
        }
        else if(label.compare("NODE_COORD_SECTION") == 0){
            /** Reads the coordenates of each city and computes the distances between cities. **/
            
            int totalCities = this->totalVariables;
            int city = 0;
            
            this->costCoord = new int *[totalCities];
            for (city = 0; city < totalCities; city++) {
                
                this->costCoord[city] = new int[2];
                
                std::getline(infile, line);
                elemens = split(line, ' ');
                
                this->costCoord[city][0] = std::stoi(elemens.at(1).c_str());
                this->costCoord[city][1] = std::stoi(elemens.at(2).c_str());
                
            }
            
            this->costs = new double*[totalCities];
            for (city = 0; city < totalCities; city++) {
                this->costs[city] = new double[totalCities];
                this->costs[city][city] = 0;
            }
            
            int origin = 0;
            int destiny = 0;
            for (origin = 0; origin < totalCities - 1; origin++) {
                for (destiny = origin + 1; destiny < totalCities; destiny++) {
                    
                    this->costs[origin][destiny] = this->computeEuclideanDistance(this->costCoord[origin][0], this->costCoord[origin][1], this->costCoord[destiny][0], this->costCoord[destiny][1]);
                    
                    this->costs[destiny][origin] = this->costs[origin][destiny];
                    
                }
            }
            printf("Node coord section read...\n");
        }
    }
    
    elemens.clear();
    infile.close();
}

void ProblemTSP::printSolution(const Solution & solution) const{ printPartialSolution(solution, this->totalVariables);}
void ProblemTSP::printSolutionInfo(const Solution & solution) const{ printf("TODO: Implement this function.\n");}
void ProblemTSP::printPartialSolution(const Solution & solution, int level) const{}
