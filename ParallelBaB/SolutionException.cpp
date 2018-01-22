//
//  SolutionException.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/22/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "SolutionException.hpp"

SolutionException::SolutionException(SolutionErrorCode error_code, std::string error_msg):error_code(error_code), error_message(error_msg){
    
}

SolutionException::~SolutionException() throw(){

}

const char* SolutionException::what() const throw(){
    std::string error_text = "SolutionException: ";
    
    switch (error_code) {
        case OK_SOLUTION:
            return error_message.c_str();
            break;
            
        case OBJECTIVES_OUT_OF_RANGE:
            error_text += "Objectives out of range: ";
            return (error_text + error_message.c_str()).c_str();
            break;
            
        case VARIABLES_OUT_OF_RANGE:
            error_text += "Variables out of range: ";
            return (error_text + error_message.c_str()).c_str();
            break;
            
        default:
            return (error_text + error_message.c_str()).c_str();
            break;
    }
}

const char* SolutionException::getError() const {
    return error_message.c_str();
}
