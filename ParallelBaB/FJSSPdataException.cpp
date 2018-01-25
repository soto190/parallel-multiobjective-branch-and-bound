//
//  FJSSPdataException.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/23/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "FJSSPdataException.hpp"

FJSSPdataException::FJSSPdataException(FJSSPdataErrorCode error_code, std::string error_msg):error_code(error_code), error_message(error_msg){
    
}

FJSSPdataException::~FJSSPdataException() throw(){
    
}

const char* FJSSPdataException::what() const throw(){
    std::string error_text = "FJSSPdataException: ";
    
    switch (error_code) {
        case OK_FJSSPdata:
            error_text += error_message;
            break;
            
        case JOB_OUT_OF_RANGE:
            error_text += "Job out of range " + error_message;
            break;
            
        case OPERATION_OUT_OF_RANGE:
            error_text += "Operation out of range " + error_message ;
            break;
            
        case MACHINE_OUT_OF_RANGE:
            error_text += "Machine out of range " + error_message ;
            break;
            
        default:
            error_text += "UNIDENTIFIED error: " +  error_message;
            break;
    }
    
    return error_text.c_str();
}

const char* FJSSPdataException::getError() const {
    return error_message.c_str();
}
