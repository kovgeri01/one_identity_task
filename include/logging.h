#ifndef CLIENT_PARAMETERS_H
#define CLIENT_PARAMETERS_H

void logging_init(int daemon);

void logging_log_info(char* format, ...);

void logging_log_error(char* format, ...);

void logging_log_errno(int errno, char* format, ...);

#endif