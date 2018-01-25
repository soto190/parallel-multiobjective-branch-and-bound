//
//  ProblemFJSSPException.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 1/25/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "ProblemFJSSPException.hpp"

ProblemFJSSPException::ProblemFJSSPException(ProblemFJSSPErrorCode error_code, std::string error_msg):error_code(error_code), error_message(error_msg){
    
}

ProblemFJSSPException::~ProblemFJSSPException() throw(){
    
}

const char* ProblemFJSSPException::what() const throw(){
    std::string error_text = "ProblemFJSSPException: ";
    
    switch (error_code) {
        case OK_FJSSPproblem:
            error_text += error_message;
            break;
            
        case P_JOB_OUT_OF_RANGE:
            error_text += "Job out of range " + error_message;
            break;
            
        case P_OPERATION_OUT_OF_RANGE:
            error_text += "Operation out of range " + error_message ;
            break;
            
        case P_MACHINE_OUT_OF_RANGE:
            error_text += "Machine out of range " + error_message ;
            break;
            
        case P_CODE_OUT_OF_RANGE:
            error_text += "Code out of range " + error_message ;
            break;
            
        default:
            error_text += "UNIDENTIFIED error: " +  error_message;
            break;
    }
    
    return error_text.c_str();
}

const char* ProblemFJSSPException::getError() const {
    return error_message.c_str();
}
