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
