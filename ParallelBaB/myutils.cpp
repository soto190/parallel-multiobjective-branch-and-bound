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
    istringstream buffer(s);
    string item;
    elems.clear();
    if (delim == ' ' || delim == '\t')
        std::copy(std::istream_iterator<std::string>(buffer),
                  std::istream_iterator<std::string>(),
                  std::back_inserter(elems));
    else
        while (getline(buffer, item, delim))
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

double round_double(double value) {
    return round(value * 1000000) / 1000000;
}

/** Returns a value between -1 and 1. **/
float distance_to_objective(const float value, const float best) {
    return (value - best) / value;
}

int binarySearch(double value, const double * array, int size) {
    int low = 0, high = size - 1, mid;
    
    if (value >= array[high])
        return high;
    if(value <= array[low])
        return low;
    
    low++;
    high--;
    
    while (low <= high) {
        mid = (low + high) * 0.5f;
        if (array[mid] <= value && value < array[mid + 1])
            return mid;
        else if (value < array[mid])
            high = mid - 1;
        else if (value > array[mid])
            low = mid + 1;
    }
    return high;
}

/**
 * To avoid to compute the factorial for each number.
 * http://www.tsm-resources.com/alists/fact.html
 **/
unsigned long getFactorial(int n) {
    unsigned long factorial[21];
    if (n > 0 && n < 20) {
        factorial[0] = 1;
        factorial[1] = 1;
        factorial[2] = 2;
        factorial[3] = 6;
        factorial[4] = 24;
        factorial[5] = 120;
        factorial[6] = 720;
        factorial[7] = 5040;
        factorial[8] = 40320;
        factorial[9] = 362880;
        factorial[10] = 3628800;
        factorial[11] = 39916800;
        factorial[12] = 479001600;
        factorial[13] = 6227020800;
        factorial[14] = 87178291200;
        factorial[15] = 1307674368000;
        factorial[16] = 20922789888000;
        factorial[17] = 355687428096000;
        factorial[18] = 6402373705728000;
        factorial[19] = 121645100408832000;
        factorial[20] = 2432902008176640000;
    } else {
        printf("Not factorial for: %d\n", n);
        return -1;
    }
    return factorial[n];
}
