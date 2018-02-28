//
//  Grid.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 22/09/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "ConcurrentGridContainer.hpp"

ConcurrentGridContainer::ConcurrentGridContainer(unsigned int width, unsigned int height):
cols(width),
rows(height) {
    numberOfElements.store(0);
    pareto_buckets.reserve(cols * rows);
    for (int indey = 0; indey < rows; ++indey)
        for (int index = 0; index < cols; ++index)
            pareto_buckets.push_back(ParetoBucket(index, indey));
}

ConcurrentGridContainer::ConcurrentGridContainer(const ConcurrentGridContainer& toCopy):
cols(toCopy.getNumberOfCols()),
rows(toCopy.getNumberOfRows()),
numberOfElements((unsigned long) toCopy.getSizeAtomic()),
pareto_buckets(toCopy.pareto_buckets) {

}

ConcurrentGridContainer::~ConcurrentGridContainer() {
    pareto_buckets.clear();
}

ConcurrentGridContainer& ConcurrentGridContainer::operator()(unsigned int width, unsigned int height) {
    cols = width;
    rows = height;
    numberOfElements.store(0);
    pareto_buckets.clear();
    pareto_buckets.reserve(cols * rows);
    for (int indey = 0; indey < rows; ++indey)
        for (int index = 0; index < cols; ++index)
            pareto_buckets.push_back(ParetoBucket(index, indey));
    return *this;
}

bool ConcurrentGridContainer::addTo(const Solution& obj, size_t x, size_t y) {
    size_t index = getIndexPosition(x, y);
    unsigned long size_before = pareto_buckets[index].getSize();
    bool updated = pareto_buckets[index].push_back(obj);
    unsigned long size_after = pareto_buckets[index].getSize();
    numberOfElements.fetch_and_add(size_after - size_before);
    return updated;
}

size_t ConcurrentGridContainer::getIndexPosition(size_t x, size_t y) const {
    return y * cols + x;
}

std::vector<Solution>& ConcurrentGridContainer::get(size_t x, size_t y) {
    return pareto_buckets[getIndexPosition(x, y)].getVector();
}

unsigned int ConcurrentGridContainer::getNumberOfCols() const {
    return cols;
}

unsigned int ConcurrentGridContainer::getNumberOfRows() const {
    return rows;
}

unsigned long ConcurrentGridContainer::getSize() const {
    return numberOfElements;
}

tbb::atomic<unsigned long> ConcurrentGridContainer::getSizeAtomic() const {
    return numberOfElements;
}

BucketState ConcurrentGridContainer::getStateOf(size_t x, size_t y) const {
    return pareto_buckets[getIndexPosition(x, y)].getState();
}

unsigned long ConcurrentGridContainer::getSizeOf(size_t x, size_t y) const {
    return pareto_buckets[getIndexPosition(x, y)].getSize();
}

bool ConcurrentGridContainer::produceImprovementInBucket(const Solution& obj, size_t x, size_t y) {
    return pareto_buckets[getIndexPosition(x, y)].produceImprovement(obj);
}

void ConcurrentGridContainer::setNonDominatedState(size_t x, size_t y) {
    pareto_buckets[getIndexPosition(x, y)].setNonDominated();
}

void ConcurrentGridContainer::setDominatedState(size_t x, size_t y) {
    pareto_buckets[getIndexPosition(x, y)].setDominated();
}

void ConcurrentGridContainer::setUnexploredState(size_t x, size_t y) {
    pareto_buckets[getIndexPosition(x, y)].setUnexplored();
}

unsigned long ConcurrentGridContainer::clear(size_t x, size_t y) {
    size_t index = getIndexPosition(x, y);
    unsigned long size_before = pareto_buckets[index].getSize();
    pareto_buckets[index].setDominated();
    pareto_buckets[index].clear();
    numberOfElements.fetch_and_add(-size_before);
    return size_before;
}

void ConcurrentGridContainer::print() const {
    for (int index = 0; index < rows * cols; ++index)
        if (pareto_buckets[index].getState() == BucketState::NonDominated)
            pareto_buckets[index].print();
}
