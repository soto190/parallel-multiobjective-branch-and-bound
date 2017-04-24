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

class Data3{
    int value;
    int obj[2];

public:
    Data3();
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
            if (obj[n_obj] >= rhs.getObjective(n_obj))
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
    
};

class SortedVector{
    deque<Data3> m_data;
    
public:
    void push(const Data3 & data){
        if (m_data.empty())
            m_data.push_back(data);
        else{
            if (data <= m_data.front())
                m_data.push_front(data);
            else if(data >= m_data.back())
                m_data.push_back(data);
            else{/** Do a quick search splitting in two parts. **/
                int mid = m_data.size() * 0.5f;
                size_t low = 0, high = m_data.size();
                
                if (data >= m_data.at(mid) && data <= m_data.at(mid + 1)) {
                    m_data.insert(m_data.begin() + mid, data);
                }
                
                m_data.at(mid);
            }
        }
    }
};

#endif /* myutils_hpp */
