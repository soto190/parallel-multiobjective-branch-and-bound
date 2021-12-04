//
//  ParetoFront.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 2/2/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "ParetoFront.hpp"

ParetoFront::ParetoFront():
state(FrontState::Unexplored),
m_vec() {

}

ParetoFront::ParetoFront(const ParetoFront& toCopy):
state(toCopy.getState()),
m_vec(toCopy.getVectorToCopy()) {

}

ParetoFront::ParetoFront(const std::vector<Solution>& set_of_solutions):
state(FrontState::Unexplored) {
    for (size_t index = 0; index < set_of_solutions.size(); ++index)
        push_back(set_of_solutions.at(index));
}

ParetoFront::~ParetoFront() {
    m_vec.clear();
}

ParetoFront& ParetoFront::operator()(const ParetoFront &rhs) {

    if (this == &rhs)
        return *this;

    for (size_t n_sol = 0; n_sol < rhs.size(); ++n_sol)
        push_back(rhs.at(n_sol));

    return *this;
}

ParetoFront& ParetoFront::operator=(const ParetoFront &rhs) {

    if (this == &rhs)
        return *this;

    state = rhs.getState();
    m_vec.clear();
    for (size_t n_sol = 0; n_sol < rhs.size(); ++n_sol)
        push_back(rhs.at(n_sol));

    return *this;
}

ParetoFront& ParetoFront::operator+=(const ParetoFront &rhs) {
    state = rhs.getState();
    for (size_t n_sol = 0; n_sol < rhs.size(); ++n_sol)
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
    for (size_t index = 0; index < m_vec.size(); ++index)
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
    for (size_t nSol = 0; nSol < to_join.size(); ++nSol)
        push_back(to_join.at(nSol));
}

bool ParetoFront::push_back(const Solution& new_solution) {
    unsigned int it_dominates = 0;
    unsigned int is_nondominated_by = 0;
    unsigned int is_dominated_by = 0;
    bool is_equals = false;
    bool is_added = false;

    std::vector<Solution>::iterator begin = m_vec.begin();
    
    for (size_t nSol = 0; nSol < m_vec.size(); ++nSol)
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

std::vector<Solution>::iterator ParetoFront::begin() {
    return m_vec.begin();
}

std::vector<Solution>::iterator ParetoFront::end() {
    return m_vec.end();
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

std::ostream &operator<<(std::ostream& stream, const ParetoFront& pareto_f) {
    for (size_t nSol = 0; nSol < pareto_f.size(); ++nSol) {
        //stream << '[' << n_sol++ << ']';
        //stream << pareto_f.at(nSol);
        stream << pareto_f.at(nSol).getObjective(0) << ',' << pareto_f.at(nSol).getObjective(1) << ',' << pareto_f.at(nSol).getObjective(2) << std::endl;
    }
    return stream;
}

void ParetoFront::print() const {
    unsigned int n_sol = 0;
    for (const auto& nSol : m_vec) {
        printf("[%3d] ", n_sol++);
        nSol.print();
    }
}
