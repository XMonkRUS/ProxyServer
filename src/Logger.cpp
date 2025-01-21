#include "Logger.h"

#include <iostream>

Logger::Logger(const std::string &logFile){
    
    _logStream.open(logFile, std::ios::app);
    if (!_logStream){
        std::cout<<"Can`t open log file \n";
        exit(EXIT_FAILURE);
    }

}

void Logger::log(const std::string &message){
    _logStream << message << '\n';

}
