//
//  IVMTree.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 16/01/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "IVMTree.hpp"

IVMTree::IVMTree() {
	this->whoIam = -1;
	this->rows = 1;
	this->cols = 1;
	this->hasBranches = 0;
	this->active_node = new int[rows];
	this->max_nodes_in_level = new int[rows];
	this->ivm = new int *[rows];

	int r = 0, c = 0;

	for (r = 0; r < rows; r++) {
		this->ivm[r] = new int[cols];
		this->active_node[r] = -1;
		this->max_nodes_in_level[r] = 0;
		for (c = 0; c < cols; c++)
			this->ivm[r][c] = -1;
	}

	this->starting_level = 0;
	this->active_level = 0;
	this->start_exploration = new int[rows];
	this->end_exploration = new int[rows];
}

IVMTree::IVMTree(int rows, int cols) {

	this->rows = rows;
	this->cols = cols;
	this->hasBranches = 1;
	this->active_node = new int[rows];
	this->max_nodes_in_level = new int[rows];
	this->ivm = new int *[rows];

	int r = 0, c = 0;

	for (r = 0; r < rows; r++) {
		this->ivm[r] = new int[cols];
		this->active_node[r] = -1;
		this->max_nodes_in_level[r] = 0;
		for (c = 0; c < cols; c++)
			this->ivm[r][c] = -1;
	}

	this->starting_level = 0;
	this->active_level = 0;
	this->start_exploration = new int[rows];
	this->end_exploration = new int[rows];
}

IVMTree::~IVMTree() {
	printf("IVM%3d\n", this->whoIam);

	int r = 0;
	for (r = 0; r < rows; r++)
		delete[] this->ivm[r];

	delete[] active_node;
	delete[] max_nodes_in_level;
	delete[] ivm;
	delete[] start_exploration;
	delete[] end_exploration;
}

int IVMTree::getNumberOfRows() const {
	return this->rows;
}

int IVMTree::getNumberOfCols() const {
	return this->cols;
}

int IVMTree::getTreeDeep() const {
	return this->rows;
}

void IVMTree::setOwner(int idBB) {
	this->whoIam = idBB;
}

int IVMTree::getOwner() const {
	return this->whoIam;
}

void IVMTree::setActiveLevel(int level) {
	this->active_level = level;
}

/**
 * Initialize th IVM with the given interval.
 *
 */
void IVMTree::setExplorationInterval(int starting_level, int *starts,
		int *ends) {
	int level = 0;
	this->starting_level = starting_level;
	this->active_level = starting_level - 1;

	for (level = 0; level < this->rows; level++) {
		this->active_node[level] = starts[level];
		this->start_exploration[level] = starts[level];
		this->end_exploration[level] = ends[level];
		this->max_nodes_in_level[level] = 0;
		this->ivm[level][starts[level]] = starts[level];
	}

}

int IVMTree::hasPendingBranches() const {
	return this->hasBranches;
}

int IVMTree::getCurrentLevel() const {
	return this->active_level;
}

void IVMTree::setNode(int level, int value) {
	this->ivm[level][this->max_nodes_in_level[level]] = value;
	this->max_nodes_in_level[level]++;
}

int IVMTree::moveToNextLevel() {
	return this->active_level++;
}

int IVMTree::moveToNextNode() {
	return this->ivm[active_level][active_node[active_level]];
}

int IVMTree::getActiveNode() const {
	return this->ivm[active_level][active_node[active_level]];
}

int IVMTree::getFatherNode() const {
	return this->ivm[active_level - 1][active_node[active_level - 1]];
}

/** Prune the active node and set the active_level pointing a new active node. **/
int IVMTree::pruneActiveNode() {

	this->ivm[active_level][active_node[active_level]] = -1; /** Marks the node as removed. **/
	this->max_nodes_in_level[active_level]--; /** Reduces the number of nodes in the active level. **/

	if (max_nodes_in_level[active_level] > 0) { /** If there are more nodes in the active level, then move to the next node. **/

		/** TODO: Re-think if we always move to the right or if we can search for another node. **/

		this->active_node[active_level]++; /** Moves to the node of the right. **/
		return this->ivm[active_level][active_node[active_level]];
	}

	/** If the active level doesnt have more nodes then move to the father node while there are pending nodes **/
	while (max_nodes_in_level[active_level] < 1) { /** TODO: Check, why max_nodes_in_level reach -1, the minimun should be 0. **/

		this->active_node[active_level] = -1; /** Mark the level. **/
		this->active_level--; /** Go to father node. **/
		this->ivm[active_level][active_node[active_level]] = -1; /** Prune the father node. **/
		this->max_nodes_in_level[active_level]--; /** Reduce the number of nodes. **/

		if (active_level == root_node) { /** Verify if it is the root level of the tree. */
			this->hasBranches = 0; /** Mark it as there are no more branches. **/
			return this->ivm[active_level][active_node[root_node]]; /** Return the active node. **/
		}
		if (max_nodes_in_level[active_level] > 0) {
			this->active_node[active_level]++; /** Move to the next node. **/
			return this->ivm[active_level][active_node[active_level]]; /** Return the active node. **/
		}
	}

	return this->ivm[active_level][active_node[active_level]];
}

IVMTree& IVMTree::operator()(int rows, int cols) {
	this->rows = rows;
	this->cols = cols;
	this->hasBranches = 1;
	this->active_node = new int[rows];
	this->max_nodes_in_level = new int[rows];
	this->ivm = new int *[rows];

	int r = 0, c = 0;

	for (r = 0; r < rows; r++) {
		this->ivm[r] = new int[cols];
		this->active_node[r] = -1;
		this->max_nodes_in_level[r] = 0;
		for (c = 0; c < cols; c++)
			this->ivm[r][c] = -1;
	}

	this->starting_level = 0;
	this->active_level = 0;
	this->start_exploration = new int[rows];
	this->end_exploration = new int[rows];
	return *this;
}

void IVMTree::showIVM() {

	int r = 0, c = 0;

	char sep = '-';
	for (r = 0; r < this->rows; r++) {
		/** The integer verctor. **/
		if (this->active_node[r] == -1)
			printf("[%3d] %3c ", r, sep);
		else
			printf("[%3d] %3d ", r, this->active_node[r]);

		/** The solution. **/
		if (this->active_node[r] == -1
				|| this->ivm[r][this->active_node[r]] == -1)
			printf(" %3c ", sep);
		else
			printf(" %3d ", this->ivm[r][this->active_node[r]]);

		/** Max nodes in level. **/
		//if (this->active_nodes[r] == -1 || this->ivm[r][this->active_nodes[r]] == -1)
		//  printf(" %3c | ", sep);
		//else
		printf(" %3d | ", this->max_nodes_in_level[r]);

		/** The matrix. **/
		for (c = 0; c < this->cols; c++)
			if (this->ivm[r][c] == -1)
				printf("%3c", sep);
			else
				printf("%3d", this->ivm[r][c]);

		/** The active level. **/
		if (this->active_level == r)
			printf("|*\n");
		else
			printf("|\n");
	}
}

