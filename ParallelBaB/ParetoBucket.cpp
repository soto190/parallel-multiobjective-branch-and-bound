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
    state.store(FrontState::Unexplored);
    m_vec.reserve(50);
};

ParetoBucket::ParetoBucket(unsigned long posx, unsigned long posy):
posx(posx),
posy(posy) {
    size.store(0);
    state.store(FrontState::Unexplored);
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
    state.fetch_and_store(FrontState::Unexplored);
}

void ParetoBucket::setNonDominated() {
    state.fetch_and_store(FrontState::NonDominated);
}

void ParetoBucket::setDominated() {
    state.fetch_and_store(FrontState::dominated);
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

FrontState ParetoBucket::getState() const {
    return state;
}

tbb::atomic<unsigned long> ParetoBucket::getSizeAtomic() const {
    return size;
}

tbb::atomic<FrontState> ParetoBucket::getStateAtomic() const {
    return state;
}

std::vector<Solution>& ParetoBucket::getVector() {
    return m_vec;
}

const std::vector<Solution>& ParetoBucket::getVectorToCopy() const {
    return m_vec;
}

bool ParetoBucket::produceImprovement(const Solution& obj) {
    tbb::queuing_rw_mutex::scoped_lock m_lock(improving_lock, false);
    unsigned long index = 0, size_vec = m_vec.size();
    bool improves = true;
    for (index = 0; index < size_vec; ++index)
        switch (obj.dominanceTest(m_vec[index])) {
                
            case DominanceRelation::Dominated:
                improves = false;
                index = size_vec;
                break;
                
            case DominanceRelation::Equals:
                improves = false;
                index = size_vec;
                break;
                
            case DominanceRelation::Dominates:
                improves = true;
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
 * Returns true if the solution was added.
 * The solution is added if the ParetoBucket is improved, and deletes all dominated solutions.
 *
 ***/
bool ParetoBucket::push_back(const Solution& new_sol) {

    for (unsigned int n_obj = 0; n_obj < new_sol.getNumberOfObjectives(); ++n_obj)
        if (new_sol.getObjective(n_obj) == 0)
            return false;

    tbb::queuing_rw_mutex::scoped_lock m_lock(improving_lock, true);
    unsigned int dominates = 0;
    unsigned int nondominated = 0;
    unsigned int dominated = 0;
    unsigned int equals = 0;
    
    std::vector<Solution>::iterator begin = m_vec.begin();
    bool was_added = false;
    for (unsigned long nSol = 0; nSol < m_vec.size(); ++nSol)
        switch (new_sol.dominanceTest(m_vec.at(nSol))) {
                
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
                m_vec.push_back(new_sol); /** Creates a new copy. **/
                size.fetch_and_increment();
                was_added = true;
        }
    
    return was_added;
}

void ParetoBucket::clear() {
    size.fetch_and_store(0);
    tbb::queuing_rw_mutex::scoped_lock m_lock(improving_lock, true);
    m_vec.clear();
    m_vec.resize(0);
}

void ParetoBucket::print() const {
    printf("[%3lu %3lu] [%lu %d]:\n", posx, posy, (unsigned long) size, (FrontState) state);
    for (int nSol = 0; nSol < m_vec.size(); ++nSol)
        m_vec[nSol].print();
}
