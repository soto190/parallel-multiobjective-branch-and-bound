//
//  Grid.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 22/09/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "GridContainer.hpp"

GridContainer::GridContainer(unsigned int width, unsigned int height):
cols(width),
rows(height),
deep(1){
    numberOfElements.store(0);
    m_Data.reserve(cols * rows * deep);
    for (int indey = 0; indey < rows; ++indey)
        for (int index = 0; index < cols; ++index)
            for (int indez = 0; indez < deep; ++indez)
                m_Data.push_back(ParetoBucket(index, indey, indez));
}

GridContainer::GridContainer(unsigned int width, unsigned int height, unsigned int deep_size):
cols(width),
rows(height),
deep(deep_size){
    numberOfElements.store(0);
    m_Data.reserve(cols * rows * deep);
    for (int indez = 0; indez < deep; ++indez)
        for (int indey = 0; indey < rows; ++indey)
            for (int index = 0; index < cols; ++index)
                m_Data.push_back(ParetoBucket(index, indey, indez));
}

GridContainer::GridContainer(const GridContainer& toCopy):
cols(toCopy.getNumberOfCols()),
rows(toCopy.getNumberOfRows()),
deep(toCopy.getNumberOfDeep()),
numberOfElements((unsigned long) toCopy.getSizeAtomic()),
m_Data(toCopy.m_Data){
    
}

GridContainer::~GridContainer(){
    m_Data.clear();
}

GridContainer& GridContainer::operator()(unsigned int width, unsigned int height, unsigned int deep_size) {
    cols = width;
    rows = height;
    deep = deep_size;
    numberOfElements.store(0);
    m_Data.clear();
    m_Data.reserve(cols * rows * deep);
    for (int indez = 0; indez < deep; ++indez)
        for (int indey = 0; indey < rows; ++indey)
            for (int index = 0; index < cols; ++index)
                m_Data.push_back(ParetoBucket(index, indey, indez));
    
    return *this;
}

int GridContainer::addTo(const Solution& obj, size_t x, size_t y, size_t z) {
    size_t index = getIndexPosition(x, y, z);
    unsigned long size_before = m_Data[index].getSize();
    int updated = m_Data[index].push_back(obj);
    unsigned long size_after = m_Data[index].getSize();
    numberOfElements.fetch_and_add(size_after - size_before);
    return updated;
}

/**
 * The generalization of 'row-major' can be found in:
 * - https://en.wikipedia.org/wiki/Row-_and_column-major_order
 *
 **/
size_t GridContainer::getIndexPosition(size_t x, size_t y, size_t z) const{
    return z * cols * rows + y * cols + x;
}

std::vector<Solution>& GridContainer::get(size_t x, size_t y, size_t z) {
    return m_Data[getIndexPosition(x, y, z)].getVector();
}

unsigned int GridContainer::getNumberOfCols() const {
    return cols;
}

unsigned int GridContainer::getNumberOfRows() const {
    return rows;
}

unsigned int GridContainer::getNumberOfDeep() const{
    return deep;
}

unsigned long GridContainer::getSize() const{
    return numberOfElements;
}

tbb::atomic<unsigned long> GridContainer::getSizeAtomic() const{
    return numberOfElements;
}

BucketState GridContainer::getStateOf(size_t x, size_t y, size_t z) const{
    return m_Data[getIndexPosition(x, y, z)].getState();
}

unsigned long GridContainer::getSizeOf(size_t x, size_t y, size_t z) const{
    return m_Data[getIndexPosition(x, y, z)].getSize();
}

int GridContainer::isImprovingTheBucket(const Solution& obj, size_t x, size_t y, size_t z){
    return m_Data[getIndexPosition(x, y, z)].isImproving(obj);
}

void GridContainer::setNonDominatedState(size_t x, size_t y, size_t z){
    m_Data[getIndexPosition(x, y, z)].setNonDominated();
}

void GridContainer::setDominatedState(size_t x, size_t y, size_t z){
    m_Data[getIndexPosition(x, y, z)].setDominated();
}

void GridContainer::setUnexploredState(size_t x, size_t y, size_t z){
    m_Data[getIndexPosition(x, y, z)].setUnexplored();
}

unsigned long GridContainer::clear(size_t x, size_t y, size_t z) {
    size_t index = getIndexPosition(x, y, z);
    unsigned long size_before = m_Data[index].getSize();
    m_Data[index].setDominated();
    m_Data[index].clear();
    numberOfElements.fetch_and_add(-size_before);
    return size_before;
}

void GridContainer::print() const{
    for (int index = 0; index < rows * cols * deep; ++index)
        if (m_Data[index].getState() == BucketState::NonDominated)
            m_Data[index].print();
}
