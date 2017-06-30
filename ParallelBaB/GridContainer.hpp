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
#include "tbb/mutex.h"
#include "tbb/spin_rw_mutex.h"
#include "tbb/queuing_rw_mutex.h"
#include "tbb/concurrent_vector.h"

enum BucketState {
	unexplored = 0, nondominated = 1, dominated = 2
};

typedef tbb::queuing_rw_mutex Lock;

/** Wrapping a vector. **/
//template<class T>
 class ParetoBucket{
 
 private:
     unsigned long posx;
     unsigned long posy;
     Lock improving_lock;

     tbb::atomic<unsigned long> size;
     tbb::atomic<BucketState> state;
     std::vector<Solution> m_vec;

 public:
     
     ParetoBucket():
     posx(0),
     posy(0){
         size.store(0);
         state.store(BucketState::unexplored);
         m_vec.reserve(50);
     };
     
     ParetoBucket(unsigned long posx, unsigned long posy):
     posx(posx),
     posy(posy){
         size.store(0);
         state.store(BucketState::unexplored),
         m_vec.reserve(50);
     };
     
     ParetoBucket(const ParetoBucket& toCopy):
     state(toCopy.getStateAtomic()),
     posx(toCopy.getPosx()),
     posy(toCopy.getPosy()),
     size(toCopy.getSizeAtomic()),
     m_vec(toCopy.getVectorToCopy()){
     };
     
     void setPosX(unsigned long new_posx){ posx = new_posx;}
     void setPosY(unsigned long new_posy){ posy = new_posy;}
     void setPositionXY(unsigned long new_posx, unsigned long new_posy){ posx = new_posx; posy = new_posy;}
     void setState(BucketState new_state){ state.fetch_and_store(new_state); }
     void setUnexplored(){ state.fetch_and_store(BucketState::unexplored); }
     void setNonDominated(){ state.fetch_and_store(BucketState::nondominated); }
     void setDominated(){ state.fetch_and_store(BucketState::dominated); }
     void resetSize(){ size.fetch_and_store(0);}
     
     unsigned long getPosx() const{ return posx; }
     unsigned long getPosy() const{ return posy; }
     unsigned long getSize() const{ return size; }
     BucketState getState() const{ return state; }
     
     tbb::atomic<unsigned long> getSizeAtomic() const{return size;}
     tbb::atomic<BucketState> getStateAtomic() const{return state;}

     std::vector<Solution>& getVector() {return m_vec;}
     const std::vector<Solution>& getVectorToCopy() const {return m_vec;}
     
     int produceImprovement(const Solution& obj){

         tbb::queuing_rw_mutex::scoped_lock m_lock(improving_lock, false);
        
         unsigned long index = 0, size_vec = m_vec.size();
         int improves = 1;
         for (index = 0; index < size_vec; ++index)
            switch (obj.dominates(m_vec[index])) {
                
                case DominanceRelation::Dominated:
                    improves = 0;
                    index = size_vec;
                    break;
                
                case DominanceRelation::Equals:
                    improves = 0;
                    index = size_vec;
                    break;
                    
                case DominanceRelation::Dominates:
                    improves = 1;
                    index = size_vec;
                    break;
                
                case DominanceRelation::Nondominated:
                    break;
                
                default:
                    break;
             }

         return improves;
     }
     
     /** 
      * Returns 1 if the solution was added.
      * The solution is added if the ParetoBucket is improved, also deletes all dominated solutions.
      *
      ***/
     int push_back(const Solution& obj){
         
         tbb::queuing_rw_mutex::scoped_lock m_lock(improving_lock, true);
         unsigned int dominates = 0;
         unsigned int nondominated = 0;
         unsigned int dominated = 0;
         unsigned int equals = 0;
         
         std::vector<Solution>::iterator begin = m_vec.begin();
         int wasAdded = 0;
         unsigned long nSol = 0;

         for (nSol = 0; nSol < m_vec.size(); ++nSol)
             switch (obj.dominates(m_vec.at(nSol))) {
                     
                 case DominanceRelation::Dominates:
                     m_vec.erase(begin + nSol);

                     size.fetch_and_decrement();
                     dominates++;
                     nSol--;
                     break;
                     
                 case DominanceRelation::Nondominated:
                     nondominated++;
                     break;
                     
                 case DominanceRelation::Dominated:
                     dominated++;
                     nSol = m_vec.size();
                     break;
                     
                 case DominanceRelation::Equals:
                     equals = 1;
                     nSol = m_vec.size();
                     break;
             }
         
         if (equals == 0
            && (m_vec.size() == 0
            || dominates > 0
            || nondominated == size
            || dominated == 0)) {
                m_vec.push_back(obj); /** Creates a new copy. **/
                size.fetch_and_increment();
                wasAdded = 1;
             }

         return wasAdded;
     }
     
     void clear(){
         size.fetch_and_store(0);
         tbb::queuing_rw_mutex::scoped_lock m_lock(improving_lock, true);
         m_vec.clear();
         m_vec.resize(0);
     }
     
     void print() const{
         printf("[%3lu %3lu] [%luu %d]:\n", posx, posy, (unsigned long) size, (BucketState) state);
         for (int nSol = 0; nSol < m_vec.size(); ++nSol)
             m_vec[nSol].print();
     }
     
     /*
      //Join two buckets;
      Bucket<T> operator+(const Bucket<T>& bucket){
         Bucket<T> new_bucket;
         new_bucket.m_vec = bucket.m_vec.clone();
         new_bucket._vec.push_back(m_vec);
         return new_bucket;
     };
     */
 };
 
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
        int index = 0, indey = 0;
        for (indey = 0; indey < rows; ++indey)
            for (index = 0; index < cols; ++index)
                m_Data.push_back(ParetoBucket(index, indey));
    }
    
    GridContainer(const GridContainer& toCopy):
    cols(toCopy.getCols()),
    rows(toCopy.getRows()),
    m_Data(toCopy.m_Data),
    numberOfElements(toCopy.getSizeAtomic()){
    }
    
    ~GridContainer(){
    }

	GridContainer& operator()(unsigned int width, unsigned int height) {
        cols = width;
        rows = height;
        numberOfElements.store(0);
        m_Data.reserve(cols * rows);
        int index = 0, indey = 0;
        for (indey = 0; indey < rows; ++indey)
            for (index = 0; index < cols; ++index)
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

	unsigned int getCols() const { return cols; }
	unsigned int getRows() const { return rows; }
    unsigned long getSize() const{ return numberOfElements; }
    
    tbb::atomic<unsigned long> getSizeAtomic() const{ return numberOfElements;}
    BucketState getStateOf(size_t x, size_t y) const{return m_Data[y * cols + x].getState();}
	unsigned long getSizeOf(size_t x, size_t y) const{return m_Data[y * cols + x].getSize();}
    int improvesBucket(const Solution& obj, size_t x, size_t y){return m_Data[y * cols + x].produceImprovement(obj);}
    void setStateOf(BucketState new_state, size_t x, size_t y){m_Data[y * cols + x].setState(new_state);}

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

class HandlerContainer {

private:
	double * rangeinx;
	double * rangeiny;
	double maxinx;
	double maxiny;
    unsigned long numberOfElements;

    tbb::atomic<unsigned long> activeBuckets;
    tbb::atomic<unsigned long> unexploredBuckets;
    tbb::atomic<unsigned long> disabledBuckets;
    tbb::atomic<int> best_value_found_in_obj [2]; /** for two objectives **/
    
	GridContainer grid;
	std::vector<Solution> paretoFront;

public:
    
    HandlerContainer():grid(0,0){};
    HandlerContainer(unsigned int width, unsigned int height, double maxValX, double maxValY);
    HandlerContainer(const HandlerContainer& toCopy);
	~HandlerContainer();
    
    HandlerContainer& operator()(unsigned int width, unsigned int height, double maxValX, double maxValY, int minValX, int minValY);
    
	int add(const Solution & solution);
    int set(const Solution & solution, int x, int y);
	void checkCoordinate(const Solution & solution, int * coordinate) const; /** TODO: Choose an appropiate name method.**/
	int improvesTheGrid(const Solution & solution);
	int improvesTheBucket(const Solution & solution, int x, int y);

	std::vector<Solution>& get(int x, int y);
	void clearContainer(int x, int y);
    unsigned long getNumberOfActiveBuckets() const;
    unsigned long getNumberOfUnexploredBuckets() const;
    unsigned long getNumberOfDisabledBuckets() const;
	unsigned int getRows() const;
	unsigned int getCols() const;
	unsigned long getSize() const;
	unsigned long getSizeOf(int x, int y) const;
	BucketState getStateOf(int x, int y) const;
    
	void setStateOf(BucketState state, int x, int y);
	void printGridSize() const;
	void printStates() const;
    void print() const;

	double getMaxIn(int dimension);
    int getBestValueFoundIn(int obj) const;
    
	std::vector<Solution>& getParetoFront();
};

#endif /* Grid_hpp */
