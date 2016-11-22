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

enum BucketState {unexplored = 0, nondominated = 1, dominated = 2};

template<class T>
class GridContainer{
    
    std::vector<std::vector<T>> m_Data;
    unsigned int cols;
    unsigned int rows;
    
public:
    GridContainer();

    GridContainer(int width, int height){
        cols = width;
        rows = height;
        m_Data.resize(cols * rows);
    }
    
    std::vector<T>& operator()(size_t x, size_t y){
        return m_Data[x * cols + y];
    }
    
    void set(T obj, size_t x, size_t y){
       m_Data[x * cols + y].push_back(obj);
    }
    
    std::vector<T>& get(size_t x, size_t y){
        return m_Data[x * cols + y];
    }
    
    int getCols(){
        return this->cols;
    }
    
    int getRows(){
        return this->rows;
    }
    
    unsigned long getSizeOf(size_t x, size_t y){
        return m_Data[x * cols + y].size();
    }
    
    void clear(size_t x, size_t y){
        
        Solution * pd;
        for(std::vector<Solution *>::iterator it = m_Data[x * cols + y].begin(); it != m_Data[x * cols + y].end(); ++it) {
            pd = * it;
            delete pd;
        }

        
        m_Data[x * cols + y].clear();
        m_Data[x * cols + y].resize(1);
    }

};

template<class T>
class GridContainer3D{
    
    std::vector<std::vector<T>> m_Data;
    unsigned int cols;
    unsigned int rows;
    unsigned int deep;
    
public:
    GridContainer3D();
    
    GridContainer3D(int width, int height, int depth){
        cols = width;
        rows = height;
        deep = depth;
        m_Data.resize(cols * rows * deep);
    }
    
    std::vector<T>& operator()(size_t x, size_t y, size_t z){
        return m_Data[(x * cols) + y + (cols * rows * z)];
    }
    
    void set(T obj, size_t x, size_t y, size_t z){
        m_Data[(x * cols) + y + (cols * rows * z)].push_back(obj);
    }
    
    std::vector<T>& get(size_t x, size_t y, size_t z){
        return m_Data[(x * cols) + y + (cols * rows * z)];
    }
    
    int getCols(){
        return this->cols;
    }
    
    int getRows(){
        return this->rows;
    }
    
    int getDepth(){
        return this->deep;
    }
    
    unsigned long getSizeOf(size_t x, size_t y, size_t z){
        return m_Data[(x * cols) + y + (cols * rows * z)].size();
    }
    
    void clear(size_t x, size_t y, size_t z){
        
        Solution * pd;
        for(std::vector<Solution *>::iterator it = m_Data[(x * cols) + y + (cols * rows * z)].begin(); it != m_Data[(x * cols) + y + (cols * rows * z)].end(); ++it) {
            pd = * it;
            delete pd;
        }
        
        m_Data[(x * cols) + y + (cols * rows * z)].clear();
        m_Data[(x * cols) + y + (cols * rows * z)].resize(1);
    }
};


class HandlerContainer{

    double * rangeinx;
    double * rangeiny;
    double maxinx;
    double maxiny;
    
    unsigned long totalElements;
    
    GridContainer<Solution *> grid;
    int * gridState;
    
    unsigned long debug_counter = 0;


public:
    unsigned long activeBuckets;
    unsigned long unexploredBuckets;
    unsigned long disabledBuckets;
    
    HandlerContainer();
    ~HandlerContainer();
    HandlerContainer(int width, int height, double maxValX, double maxValY);
    int * add(Solution * solution);
    int * checkCoordinate(Solution * solution); /**NOTE TODO: Choose an appropiate name method.**/
    int set(Solution * solution, int x, int y);
    
    
    std::vector<Solution *>& get(int x, int y);
    void clearContainer(int x, int y);
    unsigned int getRows();
    unsigned int getCols();
    unsigned long getSize();
    unsigned long getSizeOf(int x, int y);

    int getStateOf(int x, int y);
    void setStateOf(int state, int x, int y);
    void printGridSize();
    void printStates();
    
    int updateBucket(Solution * solution, int x, int y);
    
    double getMaxIn(int dimension);
    
    std::vector<Solution *> getParetoFront();
};

class HandlerContainer3D{
    
    double * rangeinx;
    double * rangeiny;
    double * rangeinz;
    double maxinx;
    double maxiny;
    double maxinz;

    int * dimensionSize;
    double * maxin;
    
    unsigned long totalElements;
    
    GridContainer3D<Solution *> grid;
    BucketState * gridState;
    
    unsigned long debug_counter = 0;
    
    
public:
    unsigned long activeBuckets; /** It is equal to non-dominated buckets. **/
    unsigned long unexploredBuckets;
    unsigned long disabledBuckets; /** It is equal to domianted buckets. **/
    
    HandlerContainer3D();
    ~HandlerContainer3D();
    HandlerContainer3D(int width, int height, int depth, double maxValX, double maxValY, double maxValZ);
    
    int * add(Solution * solution);
    int * checkCoordinate(Solution * solution); /**NOTE TODO: Choose an appropiate name method.**/
    int set(Solution * solution, int x, int y, int z);
    
    
    std::vector<Solution *>& get(int x, int y, int z);
    void clearContainer(int x, int y, int z);
    
    unsigned int getRows();
    unsigned int getCols();
    unsigned int getDepth();
    
    unsigned long getSize();
    unsigned long getSizeOf(int x, int y, int z);
    
    BucketState getStateOf(int x, int y, int z);
    void setStateOf(BucketState state, int x, int y, int z);
    void printGridSize();
    void printStates();
    
    int updateBucket(Solution * solution, int x, int y, int z);
    
    double getMaxIn(int dimension);
    unsigned int getDimensionSize(int dimension);
    
    std::vector<Solution *> getParetoFront();
};

#endif /* Grid_hpp */
