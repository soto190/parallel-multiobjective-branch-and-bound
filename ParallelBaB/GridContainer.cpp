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
    m_Data.reserve(cols * rows);
    for (int indey = 0; indey < rows; ++indey)
        for (int index = 0; index < cols; ++index)
            m_Data.push_back(ParetoBucket(index, indey));
}

GridContainer::GridContainer(const GridContainer& toCopy):
cols(toCopy.getCols()),
rows(toCopy.getRows()),
numberOfElements((unsigned long) toCopy.getSizeAtomic()),
m_Data(toCopy.m_Data){
}

GridContainer::~GridContainer(){
    m_Data.clear();
}

GridContainer& GridContainer::operator()(unsigned int width, unsigned int height) {
    cols = width;
    rows = height;
    numberOfElements.store(0);
    m_Data.clear(); /** Release previous used memory. **/
    m_Data.reserve(cols * rows);
    for (int indey = 0; indey < rows; ++indey)
        for (int index = 0; index < cols; ++index)
            m_Data.push_back(ParetoBucket(index, indey));
    return *this;
}

int GridContainer::set(const Solution& obj, size_t x, size_t y) {
    unsigned long size_before = m_Data[y * cols + x].getSize();
    int updated = m_Data[y * cols + x].push_back(obj);
    unsigned long size_after = m_Data[y * cols + x].getSize();
    
    numberOfElements.fetch_and_add(size_after - size_before);
    return updated;
}

std::vector<Solution>& GridContainer::get(size_t x, size_t y) {
    return m_Data[y * cols + x].getVector();
}

unsigned int GridContainer::getCols() const {
    return cols;
}

unsigned int GridContainer::getRows() const {
    return rows;
}

unsigned long GridContainer::getSize() const{
    return numberOfElements;
}

tbb::atomic<unsigned long> GridContainer::getSizeAtomic() const{
    return numberOfElements;
}

BucketState GridContainer::getStateOf(size_t x, size_t y) const{
    return m_Data[y * cols + x].getState();
}

unsigned long GridContainer::getSizeOf(size_t x, size_t y) const{
    return m_Data[y * cols + x].getSize();
}

int GridContainer::improvesBucket(const Solution& obj, size_t x, size_t y){
    return m_Data[y * cols + x].produceImprovement(obj);
}

void GridContainer::setNonDominatedState(size_t x, size_t y){
    m_Data[y * cols + x].setNonDominated();
}

void GridContainer::setDominatedState(size_t x, size_t y){
    m_Data[y * cols + x].setDominated();
}

void GridContainer::setUnexploredState(size_t x, size_t y){
    m_Data[y * cols + x].setUnexplored();
}

unsigned long GridContainer::clear(size_t x, size_t y) {
    unsigned long size_before = m_Data[y * cols + x].getSize();
    m_Data[y * cols + x].setDominated();
    m_Data[y * cols + x].clear();
    numberOfElements.fetch_and_add(-size_before);
    return size_before;
}

void GridContainer::print() const{
    for (int index = 0; index < rows * cols; ++index)
        if (m_Data[index].getState() == BucketState::nondominated)
            m_Data[index].print();
}
