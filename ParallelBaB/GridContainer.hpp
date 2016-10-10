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
      /*
        int container = 0;
        for (container = 0; container < cols * rows; container++) {
            m_Data[container]->resize(255);
        }
        */
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

};

class HandlerContainer{

    double * rangeinx;
    double * rangeiny;
    double maxinx;
    double maxiny;
    
    unsigned long totalElements;
    
    GridContainer<Solution *> grid;

public:
    
    HandlerContainer(int width, int height, double maxValX, double maxValY);
    int * add(Solution * solution);
    
    std::vector<Solution *>& get(int x, int y);
    void clearContainer(int x, int y);
    unsigned int getRows();
    unsigned int getCols();
    unsigned long getSize();
    unsigned long getSizeOf(int x, int y);
    
    void printGridSize();
};

#endif /* Grid_hpp */
