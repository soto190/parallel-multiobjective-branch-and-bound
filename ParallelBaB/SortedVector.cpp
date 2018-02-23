//
//  SortedVector.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "SortedVector.hpp"

SortedVector::SortedVector() {
}

SortedVector::~SortedVector() {
    m_data.clear();
}

/**
 * The elements are sorted by dominance. It also stores the dominated solutions at the end.
 * returns 1.
 **/
int SortedVector::push(const ObjectiveValues & objValues, const SORTING_TYPES sort_type) {
    
    switch (sort_type) {
        case SORTING_TYPES::DOMINANCE:
            push_dominance(objValues);
            break;
            
        case SORTING_TYPES::DIST_1:
            push_dist1(objValues);
            break;
            
        case SORTING_TYPES::DIST_2:
            push_dist2(objValues);
            break;
            
        case SORTING_TYPES::DIST_COMB:
            push_dist_comb(objValues);
            break;
            
        default:
            m_data.push_back(objValues);
            break;
    }
    return 1;
}

int SortedVector::push_dist1(const ObjectiveValues& objValues) {
    
    switch (m_data.size()) {
        case 0:
            m_data.push_back(objValues);
            break;
        case 1:
            if(objValues.getDistance(0) > m_data.front().getDistance(0))
                m_data.push_front(objValues);
            else if(objValues.getDistance(0) < m_data.front().getDistance(0))
                m_data.push_back(objValues);
            else
                m_data.push_back(objValues);
            break;
            
        case 2:
            if(objValues.getDistance(0) >= m_data.front().getDistance(0))
                m_data.push_front(objValues);
            else if(objValues.getDistance(0) <= m_data.back().getDistance(0))
                m_data.push_back(objValues);
            else
                m_data.insert(m_data.begin() + 1, objValues);
            break;
        default:
            if(objValues.getDistance(0) >= m_data.front().getDistance(0))
                m_data.push_front(objValues);
            else if(objValues.getDistance(0) <= m_data.back().getDistance(0))
                m_data.push_back(objValues);
            else
                m_data.insert(m_data.begin() + binarySearchDecrement(objValues, SORTING_TYPES::DIST_1), objValues);
            break;
    }
    return 1;
}

int SortedVector::push_dist2(const ObjectiveValues& objValues) {
    switch (m_data.size()) {
        case 0:
            m_data.push_back(objValues);
            break;
        case 1:
            if(objValues.getDistance(1) > m_data.front().getDistance(1))
                m_data.push_front(objValues);
            else if(objValues.getDistance(1) < m_data.front().getDistance(1))
                m_data.push_back(objValues);
            else
                m_data.push_back(objValues);
            break;
            
        case 2:
            if(objValues.getDistance(1) >= m_data.front().getDistance(1))
                m_data.push_front(objValues);
            else if(objValues.getDistance(1) <= m_data.back().getDistance(1))
                m_data.push_back(objValues);
            else
                m_data.insert(m_data.begin() + 1, objValues);
            break;
            
        default:
            if(objValues.getDistance(1) >= m_data.front().getDistance(1))
                m_data.push_front(objValues);
            else if(objValues.getDistance(1) <= m_data.back().getDistance(1))
                m_data.push_back(objValues);
            else
                m_data.insert(m_data.begin() + binarySearchDecrement(objValues, SORTING_TYPES::DIST_2), objValues);
            break;
    }
    
    return 1;
}

int SortedVector::push_dist_comb(const ObjectiveValues& objValues) {
    return 1;
}

int SortedVector::push_dominance(const ObjectiveValues& objValues) {
    Dom domF, domB;
    bool inserted = 0;
    
    switch (m_data.size()) {
        case 0:
            m_data.push_back(objValues);
            break;
            
        case 1:
            switch (objValues.dominance(m_data.front())) {
                case Dom::Eq:
                    m_data.push_front(objValues);
                    break;
                case Dom::Nondom:
                    m_data.push_front(objValues);
                    break;
                case Dom::Domtes:
                    m_data.push_front(objValues);
                    break;
                case Dom::Domted:
                    m_data.push_back(objValues);
                    break;
                default:
                    break;
            }
            break;
            
        case 2:
            domF = objValues.dominance(m_data.front());
            domB = objValues.dominance(m_data.back());
            if (domF == Dom::Domtes)
                m_data.push_front(objValues);
            else if(domB == Dom::Domted)
                m_data.push_back(objValues);
            else /** That means that can be non-dominated by the two solutions, or dominated by front and dominates back. In either case it goes to mid. **/
                m_data.insert(m_data.begin() + 1, objValues);
            break;
            
        default:
            domF = objValues.dominance(m_data.front());
            domB = objValues.dominance(m_data.back());
            if (domF == Dom::Domtes || domF == Dom::Nondom || domF == Dom::Eq)
                m_data.push_front(objValues);
            else if(domB == Dom::Domted || domB == Dom::Nondom || domB == Dom::Eq)
                m_data.push_back(objValues);
            else {
                for (size_t count = 1; count < m_data.size(); ++count)
                    switch (objValues.dominance(m_data[count])) {
                        case Dom::Eq:
                            m_data.insert(m_data.begin() + count, objValues);
                            count = m_data.size();
                            inserted = 1;
                            break;
                        case Dom::Nondom:
                            m_data.insert(m_data.begin() + count, objValues);
                            count = m_data.size();
                            inserted = 1;
                            break;
                        case Dom::Domtes:
                            m_data.insert(m_data.begin() + count, objValues);
                            count = m_data.size();
                            inserted = 1;
                            break;
                        case Dom::Domted:
                            break;
                        default:
                            break;
                    }
                if(inserted == 0)
                    m_data.push_back(objValues);
            }
            break;
    }
    return 1;
}

std::deque<ObjectiveValues>::iterator SortedVector::begin() {
    return m_data.begin();
}

std::deque<ObjectiveValues>::iterator SortedVector::end() {
    return m_data.end();
}

unsigned long SortedVector::binarySearchIncrement(const ObjectiveValues & objValues, const SORTING_TYPES sort) {
    unsigned long low, high, mid;
    
    float value = objValues.getSomethingToSort(sort);
    low = 0;
    high = m_data.size() - 1;
    
    if (value <= m_data[low].getSomethingToSort(sort))
        return low;
    if (value >= m_data[high].getSomethingToSort(sort))
        return high;
    low++;
    high--;
    
    while (low <= high) {
        mid = (low + high) * 0.5f;
        if (m_data[mid].getSomethingToSort(sort) <= value && value <= m_data[mid + 1].getSomethingToSort(sort))
            return mid;
        else if (value < m_data[mid].getSomethingToSort(sort))
            high = mid - 1;
        else if (value > m_data[mid].getSomethingToSort(sort))
            low = mid + 1;
    }
    return high;
}

unsigned long SortedVector::binarySearchDecrement(const ObjectiveValues & objValues, const SORTING_TYPES sort) {
    unsigned long low, high, mid;
    float value = objValues.getSomethingToSort(sort);
    low = 0;
    high = m_data.size() - 1;
    
    if (value >= m_data[low].getSomethingToSort(sort))
        return low;
    if (value <= m_data[high].getSomethingToSort(sort))
        return high;
    
    low++;
    high--;
    
    while (low <= high) {
        mid = (low + high) * 0.5f;
        if (m_data[mid - 1].getSomethingToSort(sort) >= value && value >= m_data[mid].getSomethingToSort(sort))
            return mid;
        else if (value > m_data[mid].getSomethingToSort(sort))
            high = mid - 1;
        else if (value < m_data[mid].getSomethingToSort(sort))
            low = mid + 1;
    }
    return low;
}

void SortedVector::print() {
    std::deque<ObjectiveValues>::iterator it = m_data.begin();
    int counter = 0;
    for (it = m_data.begin(); it != m_data.end(); ++it) {
        printf("[%3d] ", counter++);
        (*it).print();
    }
}
