//
//  ParetoBucket.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/6/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "ParetoBucket.hpp"

ParetoBucket::ParetoBucket():
posx(0),
posy(0) {
    size.store(0);
    state.store(BucketState::Unexplored);
    m_vec.reserve(50);
};

ParetoBucket::ParetoBucket(unsigned long posx, unsigned long posy):
posx(posx),
posy(posy) {
    size.store(0);
    state.store(BucketState::Unexplored);
    m_vec.reserve(50);
};

ParetoBucket::ParetoBucket(const ParetoBucket& toCopy):
posx(toCopy.getPosx()),
posy(toCopy.getPosy()),
size((unsigned long) toCopy.getSizeAtomic()),
state(toCopy.getStateAtomic()),
m_vec(toCopy.getVectorToCopy()) {

};

ParetoBucket::~ParetoBucket() {
    m_vec.clear();
}

void ParetoBucket::setPosX(unsigned long new_posx) {
    posx = new_posx;
}

void ParetoBucket::setPosY(unsigned long new_posy) {
    posy = new_posy;
}

void ParetoBucket::setPositionXY(unsigned long new_posx, unsigned long new_posy) {
    posx = new_posx;
    posy = new_posy;
}

void ParetoBucket::setUnexplored() {
    state.fetch_and_store(BucketState::Unexplored);
}

void ParetoBucket::setNonDominated() {
    state.fetch_and_store(BucketState::NonDominated);
}

void ParetoBucket::setDominated() {
    state.fetch_and_store(BucketState::dominated);
}

unsigned long ParetoBucket::getPosx() const {
    return posx;
}

unsigned long ParetoBucket::getPosy() const {
    return posy;
}

unsigned long ParetoBucket::getSize() const {
    return size;
}

BucketState ParetoBucket::getState() const {
    return state;
}

tbb::atomic<unsigned long> ParetoBucket::getSizeAtomic() const {
    return size;
}

tbb::atomic<BucketState> ParetoBucket::getStateAtomic() const {
    return state;
}

std::vector<Solution>& ParetoBucket::getVector() {
    return m_vec;
}

const std::vector<Solution>& ParetoBucket::getVectorToCopy() const {
    return m_vec;
}

int ParetoBucket::produceImprovement(const Solution& obj){
    tbb::queuing_rw_mutex::scoped_lock m_lock(improving_lock, false);
    unsigned long index = 0, size_vec = m_vec.size();
    int improves = 1;
    for (index = 0; index < size_vec; ++index)
        switch (obj.dominanceTest(m_vec[index])) {
                
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
 * The solution is added if the ParetoBucket is improved, and deletes all dominated solutions.
 *
 ***/
int ParetoBucket::push_back(const Solution& obj) {
    tbb::queuing_rw_mutex::scoped_lock m_lock(improving_lock, true);
    unsigned int dominates = 0;
    unsigned int nondominated = 0;
    unsigned int dominated = 0;
    unsigned int equals = 0;
    
    std::vector<Solution>::iterator begin = m_vec.begin();
    int wasAdded = 0;
    for (unsigned long nSol = 0; nSol < m_vec.size(); ++nSol)
        switch (obj.dominanceTest(m_vec.at(nSol))) {
                
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

void ParetoBucket::clear() {
    size.fetch_and_store(0);
    tbb::queuing_rw_mutex::scoped_lock m_lock(improving_lock, true);
    m_vec.clear();
    m_vec.resize(0);
}

void ParetoBucket::print() const {
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
