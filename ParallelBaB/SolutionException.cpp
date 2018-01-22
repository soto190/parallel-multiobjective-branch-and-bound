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
            error_text += error_message;
            break;
            
        case OBJECTIVES_OUT_OF_RANGE:
            error_text += "Objectives out of range " + error_message;
            break;
            
        case VARIABLES_OUT_OF_RANGE:
            error_text += "Variables out of range " + error_message ;
            break;
            
        default:
            error_text += "UNIDENTIFIED error: " +  error_message;
            break;
    }
    
    return error_text.c_str();
}

const char* SolutionException::getError() const {
    return error_message.c_str();
}
