//
//  myutils.hpp
//  PhDProject
//
//  Created by Carlos Soto on 28/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#ifndef myutils_hpp
#define myutils_hpp

#include <stdio.h>
#include <sstream>
#include <vector>
#include <deque>
#include <cmath>

using namespace std;
/*
 long split(const std::string &txt, std::vector<std::string> &vector, char ch){
 unsigned long pos = txt.find(ch);
 unsigned long initialPos = 0;
 vector.clear();
 
 // Decompose statement
 while(pos != std::string::npos ) {
 vector.push_back(txt.substr( initialPos, pos - initialPos + 1 ) );
 initialPos = pos + 1;
 
 pos = txt.find( ch, initialPos );
 }
 
 // Add the last one
 vector.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );
 
 return vector.size();
 }
 */

vector<string> &split(const string &s, char delim, vector<string> &elems);
vector<string> split(const string &s, char delim);

double roundToNearest(double value);
double round_double(double value);
int binarySearch(double value, const double * vector, int size);


enum Dom {Domtes = 1, Domted = -1, Nondom = 0, Eq = 11};

class Data3{
    int value;
    int obj[2];

public:
    Data3(){}
    Data3(int var_value, int obj1, int obj2): value(var_value){
        obj[0] = obj1;
        obj[1] = obj2;
    }
    
    Data3(const Data3 & toCopy){
        value = toCopy.getValue();
        for (int n_obj = 0; n_obj < 2; ++n_obj)
            obj[n_obj] = toCopy.getObjective(n_obj);
    }
    
    void setValue(int n_value){ value = n_value;}
    void setObjective(int n_objective, int value){obj[n_objective] = value;}
    
    int getValue() const{ return value;}
    int getObjective(int n_obj) const{return obj[n_obj];}
    
    bool operator==(const Data3& rhs) const{
        for (int n_obj = 0; n_obj < 2; ++n_obj)
            if (obj[n_obj] != rhs.getObjective(n_obj))
                return false;
        return true;
    }
    
    bool operator<(const Data3& rhs) const{
        for (int n_obj = 0; n_obj < 2; ++n_obj)
            if (obj[n_obj] > rhs.getObjective(n_obj))
                return false;
        return true;
    }
    
    bool operator<=(const Data3& rhs) const{
        for (int n_obj = 0; n_obj < 2; ++n_obj)
            if (obj[n_obj] > rhs.getObjective(n_obj))
                return false;
        return true;
    }
    
    bool operator>(const Data3& rhs) const{
        for (int n_obj = 0; n_obj < 2; ++n_obj)
            if (obj[n_obj] < rhs.getObjective(n_obj))
                return false;
        return true;
    }
    
    bool operator>=(const Data3& rhs) const{
        for (int n_obj = 0; n_obj < 2; ++n_obj)
            if (obj[n_obj] <= rhs.getObjective(n_obj))
                return false;
        return true;
    }
    
    Dom dominance(const Data3& rhs) const{
        int nObj = 0, objA = 0, objB = 0, localSolIsBetterIn = 0, exterSolIsBetterIn = 0, equals = 1;
        
        for (nObj = 0; nObj < 2; ++nObj) {
            objA = obj[nObj];
            objB = rhs.getObjective(nObj);
            
            if (objA < objB) {
                localSolIsBetterIn++;
                equals = 0;
            } else if (objB < objA) {
                exterSolIsBetterIn++;
                equals = 0;
            }
        }
        
        if (equals == 1)
            return Dom::Eq;
        else if (localSolIsBetterIn > 0 && exterSolIsBetterIn == 0)
            return Dom::Domtes;
        else if (exterSolIsBetterIn > 0 && localSolIsBetterIn == 0)
            return Dom::Domted;
        else
            return Dom::Nondom;
    }
    
    void print(){
        printf("%3d %3d %3d\n", value, obj[0], obj[1]);
    }
};

class SortedVector{
    deque<Data3> m_data;
    
public:
    int push(const Data3 & data){

        if (m_data.empty())
            m_data.push_back(data);
        else if(m_data.size() == 1)
            switch (data.dominance(m_data.front())) {
                case Dom::Eq:
                    m_data.push_front(data);
                    break;
                case Dom::Nondom:
                    m_data.push_front(data);
                    break;
                case Dom::Domted:
                    m_data.push_back(data);
                    break;
                case Dom::Domtes:
                    m_data.push_front(data);
                default:
                    break;
            }
        else if(m_data.size() == 2){
            Dom domF = data.dominance(m_data.front());
            Dom domB = data.dominance(m_data.back());
            
            if (domF == Dom::Domtes)
                m_data.push_front(data);
            else if(domB == Dom::Domted)
                m_data.push_back(data);
            else /** That means that can be non-dominated by the two solutions, or dominated by front and dominates back. In either case it goes to mid. **/
                m_data.insert(m_data.begin() + 1, data);
        }else{
            Dom domF = data.dominance(m_data.front());
            Dom domB = data.dominance(m_data.back());
            if (domF == Dom::Domtes || domF == Dom::Nondom || domF == Dom::Eq)
                m_data.push_front(data);
            else if(domB == Dom::Domted || domB == Dom::Nondom || domB == Dom::Eq)
                m_data.push_back(data);
            else{
                bool inserted = 0;
                for (size_t count = 1; count < m_data.size(); ++count)
                    switch (data.dominance(m_data.at(count))) {
                        case Dom::Eq:
                            m_data.insert(m_data.begin() + count, data);
                            count = m_data.size();
                            inserted = 1;
                            break;
                        case Dom::Nondom:
                            m_data.insert(m_data.begin() + count, data);
                            count = m_data.size();
                            inserted = 1;
                            break;
                        case Dom::Domtes:
                            m_data.insert(m_data.begin() + count, data);
                            count = m_data.size();
                            inserted = 1;
                        case Dom::Domted:
                            break;
                        default:
                            break;
                    }
                if(inserted == 0)
                    m_data.push_back(data);
            }
        }
        return 1;
    }
    
    std::deque<Data3>::iterator begin(){
        return m_data.begin();
    }
    
    std::deque<Data3>::iterator end(){
        return m_data.end();
    }
    
    void print() {
        std::deque<Data3>::iterator it = m_data.begin();
        int counter = 0;
        for (it = m_data.begin(); it != m_data.end(); ++it)
            printf("[%3d] %3d %3d %3d\n", counter++, it->getValue(), it->getObjective(0), it->getObjective(1));
    }

};

#endif /* myutils_hpp */
