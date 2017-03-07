//
//  Dominance.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/10/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "Dominance.hpp"

DominanceRelation dominanceOperator(Solution & leftSolution,
		Solution & rightSolution) {
	int objective = 0;
	int solLIsBetterIn = 0;
	int solRIsBetterIn = 0;
	int equals = 1;

	/**
	 * For more objectives consider
	 * if (solAIsBetterIn > 0 and solBIsBetterIn > 0) break the FOR because the solutions are non-dominated.
	 **/
	for (objective = 0; objective < leftSolution.totalObjectives; objective++) {
		double objL = leftSolution.getObjective(objective);
		double objR = rightSolution.getObjective(objective);

		if (objL < objR) {
			solLIsBetterIn++;
			equals = 0;
		} else if (objR < objL) {
			solRIsBetterIn++;
			equals = 0;
		}
	}

	if (equals == 1)
		return DominanceRelation::Equals;
	else if (solLIsBetterIn > 0 && solRIsBetterIn == 0)
		return DominanceRelation::Dominates;
	else if (solRIsBetterIn > 0 && solLIsBetterIn == 0)
		return DominanceRelation::Dominated;
	else
		return DominanceRelation::Nondominated;
}

/**
 * Stores a copy of the received solution.
 */
int updateFront(Solution & solution, std::vector<Solution>& paretoFront) {
	unsigned int status[4];
	status[0] = 0;
	status[1] = 0;
	status[2] = 0;
	status[3] = 0;

	std::vector<Solution>::iterator begin = paretoFront.begin();
	int wasAdded = 0;

	unsigned long nSol = 0;
	int domination;

	for (nSol = 0; nSol < paretoFront.size(); nSol++) {

		domination = solution.dominates(paretoFront.at(nSol)); //dominanceOperator(solution, paretoFront.at(nSol));

		switch (domination) {

		case DominanceRelation::Dominates:

			paretoFront.erase(begin + nSol);
			status[0]++;
			nSol--;
			break;

		case DominanceRelation::Nondominated:
			status[1]++;
			break;

		case DominanceRelation::Dominated:
			status[2]++;
			nSol = paretoFront.size();
			break;

		case DominanceRelation::Equals:
			status[3] = 1;
			nSol = paretoFront.size();
			break;
		}
	}

	/**
	 * status[3] is to avoid to add solutions with the same objective values in the front, remove it if repeated objective values are requiered.
	 */
	//if(status[0] > 0 || status[1] == this->paretoFront.size() || status[2] == 0){
	if ((status[3] == 0)
			&& (paretoFront.size() == 0 || status[0] > 0
					|| status[1] == paretoFront.size() || status[2] == 0)) {
		paretoFront.push_back(solution); /** Creates a new copy. **/
		wasAdded = 1;
	}

	return wasAdded;
}

/**
 *
 * Receives a vector of solutions and removes all the dominated solutions keeping only the non-dominated solutions. Also removes solutions with the same pair of objectives.
 *
 **/
void extractParetoFront(std::vector<Solution>& front) {

	std::vector<Solution>::iterator begin = front.begin();

	unsigned long nextSol = 0;
	unsigned long currentSol = 0;
	DominanceRelation domination;
	if (front.size() > 0) {
		for (currentSol = 0; currentSol < front.size() - 1; currentSol++) {
			Solution solution = front.at(currentSol);
			for (nextSol = currentSol + 1; nextSol < front.size(); nextSol++) {

				domination = dominanceOperator(solution, front.at(nextSol));

				if (domination == DominanceRelation::Dominates) {
					front.erase(begin + nextSol); /** Removes the nextSolution. **/
					nextSol--; /** Moves the pointer one position back. **/
				} else if (domination == DominanceRelation::Dominated
						|| domination == DominanceRelation::Equals) {
					front.erase(begin + currentSol); /** Removes the currentSolution. **/
					currentSol--; /** Moves the pointer one position back. **/
					nextSol = front.size(); /** Ends the loop. **/
				}
			}
		}
	}
}
