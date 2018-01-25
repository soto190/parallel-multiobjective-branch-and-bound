//
//  ProblemFJSSPException.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/25/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#ifndef ProblemFJSSPException_hpp
#define ProblemFJSSPException_hpp

#include <stdio.h>
#include <iostream>
#include <exception>

enum ProblemFJSSPErrorCode{OK_FJSSPproblem, P_JOB_OUT_OF_RANGE, P_OPERATION_OUT_OF_RANGE, P_MACHINE_OUT_OF_RANGE, P_CODE_OUT_OF_RANGE};

class ProblemFJSSPException : public std::exception{
    
public:
    ProblemFJSSPException(ProblemFJSSPErrorCode error_code, std::string error_msg);
    ~ProblemFJSSPException() throw();
    virtual const char* what() const throw();
    
private:
    std::string error_message;
    ProblemFJSSPErrorCode error_code;
    const char* getError() const;
};

#endif /* ProblemFJSSPException_hpp */
