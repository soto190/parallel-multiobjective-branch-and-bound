//
//  IVMTreeException.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 12/6/17.
//  Copyright © 2017 Carlos Soto. All rights reserved.
//

#include "IVMTreeException.hpp"

IVMTreeException::IVMTreeException(IVMTreeErrorCode error_code, std::string error_msg):error_code(error_code), error_message(error_msg){
}

IVMTreeException::~IVMTreeException() throw(){
}

const char* IVMTreeException::what() const throw(){
    std::string error_text = "";
    switch (error_code) {
        case OK:
            return error_message.c_str();
            break;
        
        case MATRIX_OUT_OF_RANGE:
            error_text = "Matrix out of range: ";
            return (error_text + error_message.c_str()).c_str();
            break;
            
        case VECTOR_OUT_OF_RANGE:
            error_text = "Vector out of range: ";
            return (error_text + error_message.c_str()).c_str();
            break;
        
        case INTEGER_OUT_OF_RANGE:
            error_text = "Integer with invalid index: ";
            return (error_text + error_message.c_str()).c_str();;
            break;
            
        default:
            return (error_text + error_message.c_str()).c_str();
            break;
    }
}

const char* IVMTreeException::getError() const {
    return error_message.c_str();
}

