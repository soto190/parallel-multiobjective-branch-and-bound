//
//  IVMTreeException.hpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/6/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#ifndef IVMTreeException_hpp
#define IVMTreeException_hpp

#include <stdio.h>
#include <iostream>
#include <exception>

/**
 * How to hanlde exceptions in c++.
 * - https://isocpp.org/wiki/faq/exceptions
 */

enum IVMTreeErrorCode{OK_IVM, MATRIX_OUT_OF_RANGE, VECTOR_OUT_OF_RANGE, INTEGER_OUT_OF_RANGE};

class IVMTreeException : public std::exception {

public:
    IVMTreeException(IVMTreeErrorCode error_code, std::string error_msg);
    ~IVMTreeException() throw();
    virtual const char* what() const throw();

private:
    std::string error_message;
    IVMTreeErrorCode error_code;
    const char* getError() const;
};
#endif /* IVMTreeException_hpp */
