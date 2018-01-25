//
//  FJSSPdataException.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/23/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef FJSSPdataException_hpp
#define FJSSPdataException_hpp

#include <stdio.h>
#include <iostream>
#include <exception>

enum FJSSPdataErrorCode{OK_FJSSPdata, JOB_OUT_OF_RANGE, OPERATION_OUT_OF_RANGE, MACHINE_OUT_OF_RANGE};

class FJSSPdataException : public std::exception{
    
public:
    FJSSPdataException(FJSSPdataErrorCode error_code, std::string error_msg);
    ~FJSSPdataException() throw();
    virtual const char* what() const throw();
    
private:
    std::string error_message;
    FJSSPdataErrorCode error_code;
    const char* getError() const;
};

#endif /* FJSSPdataException_hpp */
