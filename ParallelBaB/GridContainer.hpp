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
#include "tbb/mutex.h"


enum BucketState {
	unexplored = 0, nondominated = 1, dominated = 2
};

/** Wrapping a vector. **/
//template<class T>
 class ParetoBucket{
 
 private:
     BucketState state;
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
        m_vec(toCopy.m_vec){
            size.fetch_and_store(toCopy.size);
     };
     
     void setState(BucketState new_state){ state = new_state; }
     void setUnexplored(){ state = BucketState::unexplored; }
     void setNonDominated(){ state = BucketState::nondominated; }
     void setDominated(){ state = BucketState::dominated; }
     
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
      * The solution is added if the ParetoBucket is improved.
      *
      ***/
     int push_back(const Solution& t){

         mutex_update.lock();
         
         unsigned int status[4];
         status[0] = 0;
         status[1] = 0;
         status[2] = 0;
         status[3] = 0;
         
         std::vector<Solution>::iterator begin = m_vec.begin();
         int wasAdded = 0;
         int toAdd = 0;
         unsigned long nSol = 0;
         int domination;
         
         for (nSol = 0; nSol < size; nSol++) {
             
             domination = t.dominates(m_vec.at(nSol));
             
             switch (domination) {
                     
                 case DominanceRelation::Dominates:
                     
                     m_vec.erase(begin + nSol);
                     size.fetch_and_store(size - 1);
                     status[0]++;
                     nSol--;
                     toAdd = 1;
                     break;
                     
                 case DominanceRelation::Nondominated:
                     status[1]++;
                     break;
                     
                 case DominanceRelation::Dominated:
                     status[2]++;
                     nSol = m_vec.size();
                     break;
                     
                 case DominanceRelation::Equals:
                     status[3] = 1;
                     nSol = m_vec.size();
                     break;
             }
         }
         
         /**
          * status[3] is to avoid to add solutions with the same objective values in the front, remove it if repeated objective values are requiered.
          */
         if ((status[3] == 0)
             && (m_vec.size() == 0
                 || status[0] > 0
                 || status[1] == size
                 || status[2] == 0)) {
                 m_vec.push_back(t); /** Creates a new copy. **/
                 wasAdded = 1;
                 size.fetch_and_store(size + 1);
             }
         
         mutex_update.unlock();
         
         return wasAdded;
     };
     
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
 

//template<class T>
class GridContainer {

    //std::vector<std::vector<T>> m_Data;
    tbb::concurrent_vector<ParetoBucket> m_Data;
	unsigned int cols;
	unsigned int rows;

public:
	GridContainer();

	GridContainer(int width, int height) {
		cols = width;
		rows = height;
		m_Data.reserve(cols * rows);
        int index_c = 0, index_r = 0;

        for (index_r = 0; index_r < rows; index_r++)
            for (index_c = 0; index_c < cols; index_c++)
                m_Data.push_back(*new ParetoBucket(index_r, index_c));
        
	}
    
    ~GridContainer(){
    }

	std::vector<Solution>& operator()(size_t x, size_t y) {
		return m_Data[y * cols + x].m_vec;
	}

	int set(Solution obj, size_t x, size_t y) {
		return m_Data[y * cols + x].push_back(obj);
	}

	std::vector<Solution>& get(size_t x, size_t y) {
		return m_Data[y * cols + x].m_vec;
	}

	int getCols() const {
		return this->cols;
	}

	int getRows() const {
		return this->rows;
	}

	unsigned long getSizeOf(size_t x, size_t y) const {
		return m_Data[y * cols + x].m_vec.size();
	}
    
    int improvesBucket(Solution obj, size_t x, size_t y){
        return m_Data[y * cols * x].produceImprovement(obj);
    }

	void clear(size_t x, size_t y) {
		m_Data[y * cols + x].m_vec.clear();
		m_Data[y * cols + x].m_vec.reserve(0);
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

	unsigned long totalElements;

	GridContainer grid;
	std::vector<Solution> paretoFront;
	BucketState * gridState;
	tbb::mutex Mutex_Up;

	unsigned long debug_counter = 0;

public:
	unsigned long activeBuckets;
	unsigned long unexploredBuckets;
	unsigned long disabledBuckets;

	HandlerContainer();
	~HandlerContainer();
	HandlerContainer(int width, int height, double maxValX, double maxValY);
	int add(Solution & solution);
	void checkCoordinate(const Solution & solution, int * coordinate) const; /** TODO: Choose an appropiate name method.**/
	int set(Solution & solution, int x, int y);
	int improvesTheGrid(const Solution & solution);
	int improvesTheBucket(const Solution & solution, int x, int y);

	std::vector<Solution>& get(int x, int y);
	void clearContainer(int x, int y);
	unsigned int getRows() const;
	unsigned int getCols() const;
	unsigned long getSize() const;
	unsigned long getSizeOf(int x, int y) const;

	BucketState getStateOf(int x, int y) const;
	void setStateOf(BucketState state, int x, int y);
	void printGridSize();
	void printStates();

	int updateBucket(Solution & solution, int x, int y);
	int updateFront(Solution & solution, std::vector<Solution>& front);

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
