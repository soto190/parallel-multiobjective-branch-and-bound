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
#include "tbb/concurrent_vector.h"

enum BucketState {
	unexplored = 0, nondominated = 1, dominated = 2
};

/** Wrapping a vector. **/
//template<class T>
 class ParetoBucket{
 
 private:
     tbb::atomic<BucketState> state;
     unsigned long posx;
     unsigned long posy;
     tbb::atomic<unsigned long> size;
     tbb::mutex mutex_update;

 public:
     std::vector<Solution> m_vec;
     
     ParetoBucket():size(0){};
     ParetoBucket(unsigned long posx, unsigned long posy):
        state(BucketState::unexplored),
        posx(posx),
        posy(posy),
        size(0){
            
            m_vec.reserve(100);
            
     };
     
     ParetoBucket(const ParetoBucket& toCopy):
        state(toCopy.getState()),
        posx(toCopy.getPosx()),
        posy(toCopy.getPosy()),
        size(toCopy.getSize()),
        m_vec(toCopy.m_vec){
            
     };
     
     void setState(BucketState new_state){ state.fetch_and_store(new_state); }
     void setUnexplored(){ state.fetch_and_store(BucketState::unexplored); }
     void setNonDominated(){ state.fetch_and_store(BucketState::nondominated); }
     void setDominated(){ state.fetch_and_store(BucketState::dominated); }
     void resetSize(){ size.fetch_and_store(0); }
     
     BucketState getState() const{ return state; }
     unsigned long getPosx() const{ return posx; }
     unsigned long getPosy() const{ return posy; }
     unsigned long getSize() const{ return size; }
     
     int produceImprovement(const Solution& obj) {
         int domination;
         int improves = 1;

         unsigned long index = 0;

         for (index = 0; index < size; index++) {
             domination = obj.dominates(m_vec[index]);
             if (domination == DominanceRelation::Dominated || domination == DominanceRelation::Equals)
                 improves = 0;
         }
         
         return improves;
     }
     
     /** 
      * Returns 1 if the solution was added.
      * The solution is added if the ParetoBucket is improved, also deletes all dominated solutions.
      *
      ***/
     int push_back(const Solution& obj){

         mutex_update.lock();
         unsigned int dominates = 0;
         unsigned int nondominated = 0;
         unsigned int dominated = 0;
         unsigned int equals = 0;
         
         std::vector<Solution>::iterator begin = m_vec.begin();
         int wasAdded = 0;
         unsigned long nSol = 0;
         int domination;
         
         for (nSol = 0; nSol < size; nSol++) {
             
             domination = obj.dominates(m_vec.at(nSol));
             
             switch (domination) {
                     
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
         }
         
         if (equals == 0
            && (m_vec.size() == 0
            || dominates > 0
            || nondominated == size
            || dominated == 0)) {
                 m_vec.push_back(obj); /** Creates a new copy. **/
                 wasAdded = 1;
                 size.fetch_and_increment();
             }
         
         mutex_update.unlock();
         
         return wasAdded;
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

	unsigned int cols;
	unsigned int rows;
    tbb::atomic<unsigned long> numberOfElements;
    tbb::concurrent_vector<ParetoBucket> m_Data;

public:

    GridContainer(int width, int height):cols(width), rows(height), numberOfElements(0) {

		m_Data.reserve(cols * rows);
        int index = 0, indey = 0;

        for (indey = 0; indey < rows; indey++)
            for (index = 0; index < cols; index++)
                m_Data.push_back(ParetoBucket(index, indey));
	}
    
    GridContainer(const GridContainer& toCopy):
        cols(toCopy.getCols()),
        rows(toCopy.getRows()),
        m_Data(toCopy.m_Data),
        numberOfElements(toCopy.getSize()){
    }
    
    ~GridContainer(){
    }

	std::vector<Solution>& operator()(size_t x, size_t y) {
		return m_Data[y * cols + x].m_vec;
	}

	int set(Solution obj, size_t x, size_t y) {
        unsigned long size_before = m_Data[y * cols + x].getSize();
        int updated = m_Data[y * cols + x].push_back(obj);
        unsigned long size_after = m_Data[y * cols + x].getSize();
        
        numberOfElements.fetch_and_add(size_after - size_before);

		return updated;
	}

	std::vector<Solution>& get(size_t x, size_t y) {
		return m_Data[y * cols + x].m_vec;
	}

	int getCols() const {
		return cols;
	}

	int getRows() const {
		return rows;
	}
    
    unsigned long getSize() const{
        return numberOfElements;
    }
    
    BucketState getStateOf(size_t x, size_t y) const{
        return m_Data[y * cols + x].getState();
    }

	unsigned long getSizeOf(size_t x, size_t y) const {
		return m_Data[y * cols + x].getSize();
	}
    
    int improvesBucket(Solution obj, size_t x, size_t y){
        return m_Data[y * cols + x].produceImprovement(obj);
    }
    
    void setStateOf(BucketState new_state, size_t x, size_t y){
        m_Data[y * cols + x].setState(new_state);
    }

	unsigned long clear(size_t x, size_t y) {
        unsigned long size_before = m_Data[y * cols + x].getSize();
        m_Data[y * cols + x].setDominated();
        m_Data[y * cols + x].resetSize();
		m_Data[y * cols + x].m_vec.clear();
		m_Data[y * cols + x].m_vec.resize(0);
        numberOfElements.fetch_and_add(-size_before);
        return size_before;
	}

};
/*
template<class T>
class GridContainer3D {

	std::vector<std::vector<T>> m_Data;
	unsigned int cols;
	unsigned int rows;
	unsigned int deep;
	unsigned int dimensions;

public:
	GridContainer3D();

	GridContainer3D(int width, int height, int depth) {
		cols = width;
		rows = height;
		deep = depth;
		m_Data.resize(cols * rows * deep);
		dimensions = 3;
	}

	std::vector<T>& operator()(size_t x, size_t y, size_t z) {
		return m_Data[(x * cols) + y + (cols * rows * z)];
	}

	void set(T obj, size_t x, size_t y, size_t z) {
		m_Data[(x * cols) + y + (cols * rows * z)].push_back(obj);
	}

	std::vector<T>& get(size_t x, size_t y, size_t z) {
		return m_Data[(x * cols) + y + (cols * rows * z)];
	}

	int getCols() const {
		return this->cols;
	}

	int getRows() const {
		return this->rows;
	}

	int getDepth() const {
		return this->deep;
	}

	unsigned long getSizeOf(size_t x, size_t y, size_t z) const {
		return m_Data[(x * cols) + y + (cols * rows * z)].size();
	}

	void clear(size_t x, size_t y, size_t z) {
		m_Data[(x * cols) + y + (cols * rows * z)].clear();
		m_Data[(x * cols) + y + (cols * rows * z)].resize(1);
	}

	unsigned int getNumberOfDimensions() {
		return this->dimensions;
	}
};
*/
class HandlerContainer {

	double * rangeinx;
	double * rangeiny;
	double maxinx;
	double maxiny;

	unsigned long numberOfElements;

	GridContainer grid;
	std::vector<Solution> paretoFront;

public:
    tbb::atomic<unsigned long> activeBuckets;
	tbb::atomic<unsigned long> unexploredBuckets;
	tbb::atomic<unsigned long> disabledBuckets;

    HandlerContainer(int width, int height, double maxValX, double maxValY);
    HandlerContainer(const HandlerContainer& toCopy);
	~HandlerContainer();
    
	int add(Solution & solution);
	void checkCoordinate(const Solution & solution, int * coordinate) const; /** TODO: Choose an appropiate name method.**/
	int set(Solution & solution, int x, int y);
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
	void printGridSize();
	void printStates();

	double getMaxIn(int dimension);

	std::vector<Solution>& getParetoFront();
};
/*
class HandlerContainer3D {

	double * rangeinx;
	double * rangeiny;
	double * rangeinz;
	double maxinx;
	double maxiny;
	double maxinz;

	int * dimensionSize;
	double * maxin;

	unsigned long totalElements;

	GridContainer3D<Solution> grid;
	BucketState * gridState;

	unsigned long debug_counter = 0;

public:
	unsigned long activeBuckets;
	unsigned long unexploredBuckets;
	unsigned long disabledBuckets;

	HandlerContainer3D();
	~HandlerContainer3D();
	HandlerContainer3D(int width, int height, int depth, double maxValX,
			double maxValY, double maxValZ);

	int * add(Solution & solution);
	int * getCandidateBucket(Solution & solution);
	int set(Solution & solution, int x, int y, int z);

	std::vector<Solution>& get(int x, int y, int z);
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
	unsigned int getSizeOfDimension(int dimension);
	unsigned int getNumberOfDimension();

	std::vector<Solution> getParetoFront();
};
*/
#endif /* Grid_hpp */
