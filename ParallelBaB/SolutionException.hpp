//
//  SolutionException.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/22/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef SolutionException_hpp
#define SolutionException_hpp

#include <stdio.h>
#include <iostream>
#include <exception>

/**
 * How to hanlde exceptions in c++.
 * - https://isocpp.org/wiki/faq/exceptions
 */

enum SolutionErrorCode{OK_SOLUTION, OBJECTIVES_OUT_OF_RANGE, VARIABLES_OUT_OF_RANGE};

class SolutionException : public std::exception{
    
public:
    SolutionException(SolutionErrorCode error_code, std::string error_msg);
    ~SolutionException() throw();
    virtual const char* what() const throw();
    
private:
    std::string error_message;
    SolutionErrorCode error_code;
    const char* getError() const;
};
#endif /* SolutionException_hpp */
