//
//  Grid.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 22/09/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "GridContainer.hpp"
/*
 GridContainer::GridContainer() {
 cols = 100;
 rows = 100;
 m_Data.resize(cols * rows);
 }
 *//*
    template<class T> GridContainer3D<T>::GridContainer3D() {
    cols = 1;
    rows = 1;
    deep = 1;
    dimensions = 1;
    m_Data.resize(cols * rows * deep);
    }
    */
GridContainer::GridContainer(unsigned int width, unsigned int height):
cols(width),
rows(height) {
    numberOfElements.store(0);
    pareto_buckets.reserve(cols * rows);
    for (int indey = 0; indey < rows; ++indey)
        for (int index = 0; index < cols; ++index)
            pareto_buckets.push_back(ParetoBucket(index, indey));
}

GridContainer::GridContainer(const GridContainer& toCopy):
cols(toCopy.getNumberOfCols()),
rows(toCopy.getNumberOfRows()),
numberOfElements((unsigned long) toCopy.getSizeAtomic()),
pareto_buckets(toCopy.pareto_buckets) {

}

GridContainer::~GridContainer() {
    pareto_buckets.clear();
}

GridContainer& GridContainer::operator()(unsigned int width, unsigned int height) {
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

bool GridContainer::addTo(const Solution& obj, size_t x, size_t y) {
    size_t index = getIndexPosition(x, y);
    unsigned long size_before = pareto_buckets[index].getSize();
    bool updated = pareto_buckets[index].push_back(obj);
    unsigned long size_after = pareto_buckets[index].getSize();
    numberOfElements.fetch_and_add(size_after - size_before);
    return updated;
}

size_t GridContainer::getIndexPosition(size_t x, size_t y) const {
    return y * cols + x;
}

std::vector<Solution>& GridContainer::get(size_t x, size_t y) {
    return pareto_buckets[getIndexPosition(x, y)].getVector();
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

tbb::atomic<unsigned long> GridContainer::getSizeAtomic() const {
    return numberOfElements;
}

BucketState GridContainer::getStateOf(size_t x, size_t y) const {
    return pareto_buckets[getIndexPosition(x, y)].getState();
}

unsigned long GridContainer::getSizeOf(size_t x, size_t y) const {
    return pareto_buckets[getIndexPosition(x, y)].getSize();
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
    unsigned long size_before = pareto_buckets[index].getSize();
    pareto_buckets[index].setDominated();
    pareto_buckets[index].clear();
    numberOfElements.fetch_and_add(-size_before);
    return size_before;
}

void GridContainer::print() const {
    for (int index = 0; index < rows * cols; ++index)
        if (pareto_buckets[index].getState() == BucketState::NonDominated)
            pareto_buckets[index].print();
}
