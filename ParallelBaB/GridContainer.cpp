//
//  GridContainer.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 2/27/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "GridContainer.hpp"

GridContainer::GridContainer(unsigned int width, unsigned int height):
cols(width),
rows(height) {
    numberOfElements = 0;
    pareto_buckets.reserve(cols * rows);
    for (int indey = 0; indey < rows; ++indey)
        for (int index = 0; index < cols; ++index)
            pareto_buckets.push_back(ParetoFront());
}

GridContainer::GridContainer(const GridContainer& toCopy):
cols(toCopy.getNumberOfCols()),
rows(toCopy.getNumberOfRows()),
numberOfElements(toCopy.getSize()),
pareto_buckets(toCopy.getParetoBuckets()) {

}

GridContainer::~GridContainer() {
    pareto_buckets.clear();
}

GridContainer& GridContainer::operator()(unsigned int width, unsigned int height) {
    cols = width;
    rows = height;
    numberOfElements = 0;
    pareto_buckets.clear();
    pareto_buckets.reserve(cols * rows);
    for (int indey = 0; indey < rows; ++indey)
        for (int index = 0; index < cols; ++index)
            pareto_buckets.push_back(ParetoFront());
    return *this;
}

bool GridContainer::addTo(const Solution& obj, size_t x, size_t y) {
    size_t index = getIndexPosition(x, y);
    unsigned long size_before = pareto_buckets[index].size();
    bool updated = pareto_buckets[index].push_back(obj);
    unsigned long size_after = pareto_buckets[index].size();
    numberOfElements = size_after - size_before;
    return updated;
}

size_t GridContainer::getIndexPosition(size_t x, size_t y) const {
    return y * cols + x;
}

const ParetoFront& GridContainer::getParetoFrontAt(size_t x, size_t y) const {
    return pareto_buckets.at(getIndexPosition(x, y));
}

const std::vector<ParetoFront> GridContainer::getParetoBuckets() const {
    return pareto_buckets;
}

unsigned int GridContainer::getNumberOfCols() const {
    return cols;
}

unsigned int GridContainer::getNumberOfRows() const {
    return rows;
}

unsigned long GridContainer::getSize() const {
    return numberOfElements;
}

FrontState GridContainer::getStateOf(size_t x, size_t y) const {
    return pareto_buckets[getIndexPosition(x, y)].getState();
}

unsigned long GridContainer::getSizeOf(size_t x, size_t y) const {
    return pareto_buckets[getIndexPosition(x, y)].size();
}

bool GridContainer::produceImprovementInBucket(const Solution& obj, size_t x, size_t y) {
    return pareto_buckets[getIndexPosition(x, y)].produceImprovement(obj);
}

void GridContainer::setNonDominatedState(size_t x, size_t y) {
    pareto_buckets[getIndexPosition(x, y)].setNonDominated();
}

void GridContainer::setDominatedState(size_t x, size_t y) {
    pareto_buckets[getIndexPosition(x, y)].setDominated();
}

void GridContainer::setUnexploredState(size_t x, size_t y) {
    pareto_buckets[getIndexPosition(x, y)].setUnexplored();
}

unsigned long GridContainer::clear(size_t x, size_t y) {
    size_t index = getIndexPosition(x, y);
    unsigned long size_before = pareto_buckets[index].size();
    pareto_buckets[index].setDominated();
    pareto_buckets[index].clear();
    numberOfElements -= size_before;
    return size_before;
}

void GridContainer::print() const {
    for (int index = 0; index < rows * cols; ++index)
        if (pareto_buckets[index].getState() == FrontState::NonDominated)
            pareto_buckets[index].print();
}
