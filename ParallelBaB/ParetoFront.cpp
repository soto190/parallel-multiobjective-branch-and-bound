//
//  ParetoFront.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 2/2/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "ParetoFront.hpp"

ParetoFront::ParetoFront(){
    
}

ParetoFront::ParetoFront(const ParetoFront& toCopy){
    
}

ParetoFront::ParetoFront(const std::vector<Solution>& set_of_solutions) {
    for (unsigned long n_sol = 0; n_sol < set_of_solutions.size(); ++n_sol)
        push_back(set_of_solutions.at(n_sol));
}

ParetoFront::~ParetoFront(){
    
}

const Solution ParetoFront::at(unsigned long position) const{
   return m_vec.at(position);
}

const Solution ParetoFront::back() const{
    return m_vec.back();
}

const Solution ParetoFront::front() const{
    return m_vec.front();
}

std::vector<Solution>& ParetoFront::getVector(){
    return m_vec;
}

const std::vector<Solution>& ParetoFront::getVectorToCopy() const {
    return m_vec;
}

int ParetoFront::produceImprovement(const Solution& obj){
    unsigned long index = 0;
    unsigned long size_vec = m_vec.size();
    int improves_the_front = 1;
    for (index = 0; index < size_vec; ++index)
        switch (obj.dominanceTest(m_vec[index])) {
                
            case DominanceRelation::Dominated:
                improves_the_front = 0;
                index = size_vec;
                break;
                
            case DominanceRelation::Equals:
                improves_the_front = 0;
                index = size_vec;
                break;
                
            case DominanceRelation::Dominates:
                improves_the_front = 1;
                index = size_vec;
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

bool ParetoFront::push_back(const Solution& obj){
    unsigned int dominates = 0;
    unsigned int nondominated = 0;
    unsigned int dominated = 0;
    unsigned int equals = 0;
    bool is_added = false;

    std::vector<Solution>::iterator begin = m_vec.begin();
    
    for (unsigned long nSol = 0; nSol < m_vec.size(); ++nSol)
       
        switch (obj.dominanceTest(m_vec.at(nSol))) {
            case DominanceRelation::Dominates:
                m_vec.erase(begin + nSol);
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
            || nondominated == m_vec.size()
            || dominated == 0)) {
            m_vec.push_back(obj);
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

void ParetoFront::clear(){
    m_vec.clear();
}

void ParetoFront::print() const {
    for (int nSol = 0; nSol < m_vec.size(); ++nSol)
        m_vec.at(nSol).print();
}
