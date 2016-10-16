//
//  myutils.cpp
//  PhDProject
//
//  Created by Carlos Soto on 28/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

#include "myutils.hpp"

using namespace std;

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
        if (!item.empty())
            elems.push_back(item);
    
    return elems;
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    
    return elems;
}

double roundToNearest(double num) {
    return (num > 0.0) ? floor(num + 0.5) : ceil(num - 0.5);
}

double round_double(double value){
    return round(value * 1000000) / 1000000;
}

int binarySearch(double value, double * array, int size){
    int low, high, mid;
    
    low = 0;
    high = size - 1;
    
    if(value >= array[high])
        return high;
    
    while (low <= high) {
        mid = (low + high) / 2;
        
        if(array[mid] <= value && value < array[mid + 1]){
            return mid;
        }
        else if(array[mid] > value){
            high = mid - 1;
        }else if(array[mid] < value){
            low = mid + 1;
        }
    }
    
    return high;
}
