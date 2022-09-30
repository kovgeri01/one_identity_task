#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "process_return_codes.h"
#include "server_parameters.h"

/// @brief Parses the server's parameters from the process's parameters.
/// @param argc The argumment count
/// @param argv The argument vector
/// @return The parsed server parameters
ServerParameters ServerParameters_parse(int argc, char **argv)
{
    ServerParameters server_params = {
        .isDaemon = 0,
        .port = 8787};
    int operator;
    while (1)
    {
        operator= getopt(argc, argv, "p:dh");
        if (operator== - 1)
        {
            break;
        }
        switch (operator)
        {
        case 'p':
            server_params.port = atoi(optarg);
            break;
        case 'd':
            server_params.isDaemon = 1;
            break;
        case 'h':
        default:
            printf("Usage: %s [-p PORT_NUM] [-d]\n \t p PORT_NUM\t : \t specifies the port number to be PORT_NUM.\n \t d \t : \t starts the process in daemon mode.\n", argv[0]);
            exit(FAILED_TO_PARSE_PARAMETERS);
            break;
        }
    }
    return server_params;
}