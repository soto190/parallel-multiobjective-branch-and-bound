//
//  Dominance.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/10/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "Dominance.hpp"


DominanceRelation dominanceOperator(Solution * solutionA, Solution * solutionB){
    int objective = 0;
    int solAIsBetterIn = 0;
    int solBIsBetterIn = 0;
    int equals = 1;
    
    /**
     * For more objectives consider
     * if (solAIsBetterIn > 0 and solBIsBetterIn > 0) break the FOR because the solutions are non-dominated.
     **/
    for (objective = 0; objective < solutionA->totalObjectives; objective++) {
        double objA = solutionA->getObjective(objective);
        double objB = solutionB->getObjective(objective);
        
        if(objA < objB){
            solAIsBetterIn++;
            equals = 0;
        }
        else if(objB < objA){
            solBIsBetterIn++;
            equals = 0;
        }
    }
    
    if(equals == 1)
        return DominanceRelation::Equals;
    else if (solAIsBetterIn > 0 && solBIsBetterIn == 0)
        return DominanceRelation::Dominates;
    else if (solBIsBetterIn > 0 && solAIsBetterIn == 0)
        return DominanceRelation::Dominated;
    else
        return DominanceRelation::Nondominated;
}


/**
 * Return the domination Status with respect to given Pareto front. Status contains four values:
 *  [0] -> The total solutions which are dominated by the current solution.
 *  [1] -> The total solutions which are no-dominated.
 *  [2] -> The total solutions which dominated the current solution.
 *  [3] -> 1 if the front contains a solution with the same objective values.
 **/
unsigned int * dominationStatus(Solution * solution, std::vector<Solution *>& front){
    unsigned int * status = new unsigned int[4];
    status[0] = 0;
    status[1] = 0;
    status[2] = 0;
    status[3] = 0;
    
    unsigned long paretoFrontSize = front.size();
    int domination = 0;
    int index = 0;
    if (paretoFrontSize > 0) {
        
        /**
         * Note: With small size fronts B&B takes more time.
         **/
        //#pragma omp parallel for if (paretoFrontSize >= 100) shared(status, solution) private(domination, index)
        for (index = 0; index < paretoFrontSize; index++) {
            
            domination = dominanceOperator(solution, front.at(index));
            
            if(domination == 1)
                status[0]++;
            else if(domination == 0)
                status[1]++;
            else if(domination == -1)
                status[2]++;
            else if(domination == 11)
                status[3] = 1;
        }
    }
    else
        status[0] = 1;
    return status;
}

int updateFront(Solution * solution, std::vector<Solution *>& paretoFront){
    unsigned int * status = new unsigned int[4];
    status[0] = 0;
    status[1] = 0;
    status[2] = 0;
    status[3] = 0;
    
    std::vector<Solution *>::iterator begin = paretoFront.begin();
    
    unsigned long nSol = 0;
    int domination = 0;
    
    for(nSol = 0; nSol < paretoFront.size(); nSol++){
        
        domination = dominanceOperator(solution, paretoFront.at(nSol));
        
        if(domination == DominanceRelation::Dominates){
            paretoFront.erase(begin + nSol);
            status[0]++;
            nSol--;
        }
        else if(domination == DominanceRelation::Nondominated)
            status[1]++;
        else if(domination == DominanceRelation::Dominated){
            status[2]++;
            nSol = paretoFront.size();
        }
        else if(domination == DominanceRelation::Equals){
            status[3] = 1;
            nSol = paretoFront.size();
        }
    }
    
    /**
     * status[3] is to avoid to add solutions with the same objective values in the front, remove it if repeated objective values are requiered.
     */
    //if(status[0] > 0 || status[1] == this->paretoFront.size() || status[2] == 0){
    int wasAdded = 0;
    if((status[3] == 0) && (paretoFront.size() == 0 || status[0] > 0 || status[1] == paretoFront.size() || status[2] == 0)){
        
        Solution * copyOfSolution = new Solution(*solution);
        
        paretoFront.push_back(copyOfSolution);
        wasAdded = 1;
    }
    
    delete [] status;
    return  wasAdded;
}

void extractParetoFront(std::vector<Solution *>& front){
    
    std::vector<Solution *>::iterator begin = front.begin();
    
    unsigned long nextSol = 0;
    unsigned long currentSol = 0;
    int domination = 0;
    
    for (currentSol = 0; currentSol < front.size() - 1; currentSol++) {
        Solution * solution = front.at(currentSol);
        for(nextSol = currentSol + 1; nextSol < front.size(); nextSol++){
            
            domination = dominanceOperator(solution, front.at(nextSol));
            
            if(domination == DominanceRelation::Dominates){
                front.erase(begin + nextSol);
                nextSol--;
            }
            else if(domination == DominanceRelation::Dominated || domination == DominanceRelation::Equals){
                front.erase(begin + currentSol);
                currentSol--;
                nextSol = front.size();
            }
        }
    }
}
