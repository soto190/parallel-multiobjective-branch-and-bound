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
//
//  myutils.cpp
//  PhDProject
//
//  Created by Carlos Soto on 28/06/16.
//  Copyright © 2016 Carlos Soto. All rights reserved.
//

//#include "myutils.hpp"
