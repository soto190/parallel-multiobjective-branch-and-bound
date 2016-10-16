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
int binarySearch(double value, double * vector, int size);




#endif /* myutils_hpp */
