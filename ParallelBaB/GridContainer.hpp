//
//  Grid.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 22/09/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#ifndef Grid_hpp
#define Grid_hpp

#include <stdio.h>
#include <vector>
#include <array>
#include "Solution.hpp"
#include "Dominance.hpp"
#include "myutils.hpp"
#include "ParetoBucket.hpp"
#include "tbb/concurrent_vector.h"

/**
 * An example of the Grid vectorization:
 *  https://stackoverflow.com/questions/14015556/how-to-map-the-indexes-of-a-matrix-to-a-1-dimensional-array-c
 **/
#define BIG_VALUE 999999999

class GridContainer {
private:
    unsigned int cols;
    unsigned int rows;
    tbb::atomic<unsigned long> numberOfElements;
    tbb::concurrent_vector<ParetoBucket> m_Data;
    
public:
    GridContainer(unsigned int width, unsigned int height):
    cols(width),
    rows(height) {
        numberOfElements.store(0);
        m_Data.reserve(cols * rows);
        for (int indey = 0; indey < rows; ++indey)
            for (int index = 0; index < cols; ++index)
                m_Data.push_back(ParetoBucket(index, indey));
    }
    
    GridContainer(const GridContainer& toCopy):
    cols(toCopy.getCols()),
    rows(toCopy.getRows()),
    numberOfElements((unsigned long) toCopy.getSizeAtomic()),
    m_Data(toCopy.m_Data){
    }
    
    ~GridContainer(){
        m_Data.clear();
    }
    
    GridContainer& operator()(unsigned int width, unsigned int height) {
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
    
    int set(const Solution& obj, size_t x, size_t y) {
        unsigned long size_before = m_Data[y * cols + x].getSize();
        int updated = m_Data[y * cols + x].push_back(obj);
        unsigned long size_after = m_Data[y * cols + x].getSize();
        
        numberOfElements.fetch_and_add(size_after - size_before);
        return updated;
    }
    
    std::vector<Solution>& get(size_t x, size_t y) {
        return m_Data[y * cols + x].getVector();
    }
    
    unsigned int getCols() const {
        return cols;
    }
    
    unsigned int getRows() const {
        return rows;
    }
    
    unsigned long getSize() const{
        return numberOfElements;
    }
    
    tbb::atomic<unsigned long> getSizeAtomic() const{
        return numberOfElements;
    }
    
    BucketState getStateOf(size_t x, size_t y) const{
        return m_Data[y * cols + x].getState();
    }
    
    unsigned long getSizeOf(size_t x, size_t y) const{
        return m_Data[y * cols + x].getSize();
    }
    
    int improvesBucket(const Solution& obj, size_t x, size_t y){
        return m_Data[y * cols + x].produceImprovement(obj);
    }
    
    void setStateOf(BucketState new_state, size_t x, size_t y){
        m_Data[y * cols + x].setState(new_state);
    }
    
    unsigned long clear(size_t x, size_t y) {
        unsigned long size_before = m_Data[y * cols + x].getSize();
        m_Data[y * cols + x].setDominated();
        m_Data[y * cols + x].clear();
        numberOfElements.fetch_and_add(-size_before);
        return size_before;
    }
    
    void print() const{
        for (int index = 0; index < rows * cols; ++index)
            if (m_Data[index].getState() == BucketState::nondominated)
                m_Data[index].print();
    }
};

#endif /* Grid_hpp */
