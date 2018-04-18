//
//  GridContainer3D.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 4/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "GridContainer3D.hpp"

GridContainer3D::GridContainer3D(unsigned int width, unsigned int height, unsigned int deep_n):
cols(width),
rows(height),
deep(deep_n){
    number_of_elements = 0;
    pareto_buckets.reserve(cols * rows * deep);
    for (int index = 0; index < cols * rows * deep; ++index)
            pareto_buckets.push_back(ParetoFront());
}

GridContainer3D::GridContainer3D(const GridContainer3D& toCopy):
cols(toCopy.getNumberOfCols()),
rows(toCopy.getNumberOfRows()),
deep(toCopy.getNumberOfDeep()),
number_of_elements(toCopy.getNumberOfElements()),
pareto_buckets(toCopy.getParetoBuckets()) {

}

GridContainer3D::~GridContainer3D() {
    pareto_buckets.clear();
}

GridContainer3D& GridContainer3D::operator()(unsigned int width, unsigned int height, unsigned int deep_n) {
    cols = width;
    rows = height;
    deep = deep_n;
    number_of_elements = 0;
    pareto_buckets.clear();
    pareto_buckets.reserve(cols * rows * deep);
    for (int index = 0; index < cols * rows * deep; ++index)
        pareto_buckets.push_back(ParetoFront());
    return *this;
}

bool GridContainer3D::addTo(const Solution& obj, size_t x, size_t y, size_t z) {
    size_t index = getIndexPosition(x, y, z);
    unsigned long size_before = pareto_buckets[index].size();
    bool updated = pareto_buckets[index].push_back(obj);
    unsigned long size_after = pareto_buckets[index].size();
    number_of_elements = size_after - size_before;
    return updated;
}

size_t GridContainer3D::getIndexPosition(size_t x, size_t y, size_t z) const {
    return z * cols * rows + y * cols + x;
}

const ParetoFront& GridContainer3D::getParetoFrontAt(size_t x, size_t y, size_t z) const {
    return pareto_buckets.at(getIndexPosition(x, y, z));
}

const std::vector<ParetoFront> GridContainer3D::getParetoBuckets() const {
    return pareto_buckets;
}

unsigned int GridContainer3D::getNumberOfCols() const {
    return cols;
}

unsigned int GridContainer3D::getNumberOfRows() const {
    return rows;
}

unsigned int GridContainer3D::getNumberOfDeep() const {
    return deep;
}

unsigned long GridContainer3D::getNumberOfElements() const {
    return number_of_elements;
}

unsigned long GridContainer3D::getNumberOfBuckets() const {
    return pareto_buckets.size();
}

FrontState GridContainer3D::getStateOf(size_t x, size_t y, size_t z) const {
    return pareto_buckets[getIndexPosition(x, y, z)].getState();
}

unsigned long GridContainer3D::getSizeOf(size_t x, size_t y, size_t z) const {
    return pareto_buckets[getIndexPosition(x, y, z)].size();
}

bool GridContainer3D::produceImprovementInBucket(const Solution& obj, size_t x, size_t y, size_t z) {
    return pareto_buckets[getIndexPosition(x, y, z)].produceImprovement(obj);
}

void GridContainer3D::setNonDominatedState(size_t x, size_t y, size_t z) {
    pareto_buckets[getIndexPosition(x, y, z)].setNonDominated();
}

void GridContainer3D::setDominatedState(size_t x, size_t y, size_t z) {
    pareto_buckets[getIndexPosition(x, y, z)].setDominated();
}

void GridContainer3D::setUnexploredState(size_t x, size_t y, size_t z) {
    pareto_buckets[getIndexPosition(x, y, z)].setUnexplored();
}

void GridContainer3D::clearAll(){
    pareto_buckets.clear();
}

unsigned long GridContainer3D::clear(size_t x, size_t y, size_t z) {
    size_t index = getIndexPosition(x, y, z);
    unsigned long size_before = pareto_buckets[index].size();
    pareto_buckets[index].setDominated();
    pareto_buckets[index].clear();
    number_of_elements -= size_before;
    return size_before;
}

void GridContainer3D::print() const {
    for (auto pareto_f = pareto_buckets.begin(); pareto_f != pareto_buckets.end(); ++pareto_f)
        if ((*pareto_f).getState() == FrontState::NonDominated)
            (*pareto_f).print();
}
