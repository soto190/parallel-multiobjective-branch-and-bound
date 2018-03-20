//
//  ParetoFront.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 2/2/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "ParetoFront.hpp"

ParetoFront::ParetoFront() {
    state = FrontState::Unexplored;
}

ParetoFront::ParetoFront(const ParetoFront& toCopy) {
    m_vec.reserve(toCopy.size());
    state = toCopy.getState();
    for (unsigned long solution = 0; solution < toCopy.size(); ++solution)
        m_vec.push_back(toCopy.at(solution));
}

ParetoFront::ParetoFront(const std::vector<Solution>& set_of_solutions) {
    state = FrontState::Unexplored;
    for (unsigned long n_sol = 0; n_sol < set_of_solutions.size(); ++n_sol)
        push_back(set_of_solutions.at(n_sol));
}

ParetoFront::~ParetoFront() {
    m_vec.clear();
}

ParetoFront& ParetoFront::operator()(const ParetoFront &rhs) {

    if (this == &rhs)
        return *this;

    for (unsigned long n_sol = 0; n_sol < rhs.size(); ++n_sol)
        push_back(rhs.at(n_sol));

    return *this;
}

ParetoFront& ParetoFront::operator=(const ParetoFront &rhs) {

    if (this == &rhs)
        return *this;

    state = rhs.getState();
    m_vec.clear();
    for (unsigned long n_sol = 0; n_sol < rhs.size(); ++n_sol)
        push_back(rhs.at(n_sol));

    return *this;
}

ParetoFront& ParetoFront::operator+=(const ParetoFront &rhs) {
    state = rhs.getState();
    for (unsigned long n_sol = 0; n_sol < rhs.size(); ++n_sol)
        push_back(rhs.at(n_sol));
    
    return *this;
}

ParetoFront& ParetoFront::operator+(const ParetoFront& rhs) {
    *this += rhs;
    return *this;
}

FrontState ParetoFront::getState() const {
    return state;
}

void ParetoFront::setUnexplored() {
    state = FrontState::Unexplored;
}

void ParetoFront::setNonDominated() {
    state = FrontState::NonDominated;
}

void ParetoFront::setDominated() {
    state = FrontState::dominated;
}

const Solution ParetoFront::at(unsigned long position) const {
   return m_vec.at(position);
}

const Solution ParetoFront::back() const {
    return m_vec.back();
}

const Solution ParetoFront::front() const {
    return m_vec.front();
}

std::vector<Solution>& ParetoFront::getVector() {
    return m_vec;
}

const std::vector<Solution>& ParetoFront::getVectorToCopy() const {
    return m_vec;
}

bool ParetoFront::produceImprovement(const Solution& obj) const {
    bool improves_the_front = true;
    for (unsigned long index = 0; index < m_vec.size(); ++index)
        switch (obj.dominanceTest(m_vec.at(index))) {
                
            case DominanceRelation::Dominated:
                improves_the_front = false;
                index = m_vec.size();
                break;
                
            case DominanceRelation::Equals:
                improves_the_front = false;
                index = m_vec.size();
                break;
                
            case DominanceRelation::Dominates:
                improves_the_front = true;
                index = m_vec.size();
                break;
                
            case DominanceRelation::Nondominated:
                break;
                
            default:
                break;
        }
    
    return improves_the_front;
}

void ParetoFront::join(const ParetoFront &to_join) {
    for (unsigned long nSol = 0; nSol < to_join.size(); ++nSol)
        push_back(to_join.at(nSol));
}

bool ParetoFront::push_back(const Solution& new_solution) {
    unsigned int it_dominates = 0;
    unsigned int is_nondominated_by = 0;
    unsigned int is_dominated_by = 0;
    bool is_equals = false;
    bool is_added = false;

    std::vector<Solution>::iterator begin = m_vec.begin();
    
    for (unsigned long nSol = 0; nSol < m_vec.size(); ++nSol)
        switch (new_solution.dominanceTest(m_vec.at(nSol))) {
            case DominanceRelation::Dominates:
                m_vec.erase(begin + nSol);
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
        m_vec.push_back(new_solution);
        is_added = true;
    }
    return is_added;
}

unsigned long ParetoFront::size() const {
    return m_vec.size();
}

bool ParetoFront::empty() const {
    return m_vec.empty();
}

void ParetoFront::clear() {
    m_vec.clear();
}

void ParetoFront::print() const {
    for (int nSol = 0; nSol < m_vec.size(); ++nSol)
        m_vec.at(nSol).print();
}
