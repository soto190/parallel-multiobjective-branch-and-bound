//
//  ParetoBucket.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/6/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "ParetoBucket.hpp"

ParetoBucket::ParetoBucket():
version_update(0),
posx(0),
posy(0) {
    size.store(0);
    state.store(FrontState::Unexplored);
    m_vec.reserve(50);
}

ParetoBucket::ParetoBucket(unsigned long posx, unsigned long posy):
version_update(0),
posx(posx),
posy(posy) {
    size.store(0);
    state.store(FrontState::Unexplored);
    m_vec.reserve(50);
}

ParetoBucket::ParetoBucket(const ParetoBucket& toCopy):
version_update(toCopy.getVersionUpdate()),
posx(toCopy.getPosx()),
posy(toCopy.getPosy()),
size((unsigned long) toCopy.getSizeAtomic()),
state(toCopy.getStateAtomic()),
m_vec(toCopy.getVectorToCopy()) {

}

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
    tbb::queuing_rw_mutex::scoped_lock m_lock(updating_lock, false);
    return m_vec;
}

void ParetoBucket::getCopyOfInnerVector(std::vector<Solution>& output_vec) {
    tbb::queuing_rw_mutex::scoped_lock m_lock(updating_lock, false);
    for (const auto& sol : m_vec)
        output_vec.push_back(sol);
}

const std::vector<Solution>& ParetoBucket::getVectorToCopy() const {
    return m_vec;
}

unsigned long ParetoBucket::getVersionUpdate() const {
    return version_update;
}

bool ParetoBucket::produceImprovement(const Solution& obj) {
    tbb::queuing_rw_mutex::scoped_lock m_lock(updating_lock, false);
    size_t size_vec = m_vec.size();
    bool improves = true;
    for (size_t index = 0; index < size_vec; ++index)
        switch (obj.dominanceTest(m_vec.at(index))) {
                
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
        if (new_sol.getObjective(n_obj) <= 1)
            return false;

    tbb::queuing_rw_mutex::scoped_lock m_lock(updating_lock, true);
    unsigned int it_dominates = 0;
    unsigned int is_nondominated_by = 0;
    unsigned int is_dominated_by = 0;
    bool is_equals = false;
    bool was_added = false;

    std::vector<Solution>::iterator begin = m_vec.begin();
    for (size_t nSol = 0; nSol < m_vec.size(); ++nSol)
        switch (new_sol.dominanceTest(m_vec.at(nSol))) {
                
            case DominanceRelation::Dominates:
                m_vec.erase(begin + nSol);
                size.fetch_and_decrement();
                it_dominates++;
                nSol--;
                break;
                
            case DominanceRelation::Nondominated:
                is_nondominated_by++;
                break;
                
            case DominanceRelation::Dominated:
                is_dominated_by++;
                nSol = m_vec.size();
                break;
                
            case DominanceRelation::Equals:
                is_equals = true;
                nSol = m_vec.size();
                break;
        }
    
    if (!is_equals && (m_vec.empty() || it_dominates > 0 || is_dominated_by == 0 || is_nondominated_by == m_vec.size())) {
        m_vec.push_back(new_sol);
        size.fetch_and_increment();
        was_added = true;
        version_update++;
    }
    
    return was_added;
}

void ParetoBucket::clear() {
    size.fetch_and_store(0);
    tbb::queuing_rw_mutex::scoped_lock m_lock(updating_lock, true);
    m_vec.clear();
    m_vec.resize(0);
}

void ParetoBucket::print() const {
    printf("[x:%3lu y:%3lu z:0] [%lu %d] [%lu]:\n", posx, posy, (unsigned long) size, (FrontState) state, version_update);
    for (size_t nSol = 0; nSol < m_vec.size(); ++nSol) {
        printf("[%3lu] ", nSol + 1);
        m_vec.at(nSol).print();
    }
}
