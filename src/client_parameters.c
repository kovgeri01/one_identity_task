#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "process_return_codes.h"
#include "client_parameters.h"
#include <string.h>

ClientParameters ClientParameters_parse(int argc, char **argv)
{
    ClientParameters client_params = {
        .ipAddress = 0,
        .port = 8787,
        .doInParallel = 0,
        .lenghtOfRandomByteSequence = 8,
        .numberOfRandomBytesToRequest = 1};
    char operator;
    while (1)
    {
        operator= getopt(argc, argv, "ip:l:n:h");
        if (operator== - 1)
        {
            break;
        }
        switch (operator)
        {
        case 'i':
            client_params.ipAddress = strdup(optarg);
            break;
        case 'p':
            client_params.port = atoi(optarg);
            break;
        case 'm':
            client_params.doInParallel = 1;
            break;
        case 'l':
            client_params.lenghtOfRandomByteSequence = atoi(optarg);
            break;
        case 'n':
            client_params.numberOfRandomBytesToRequest = atoi(optarg);
            break;
        case 'h':
        default:
            printf("Usage: %s [-i IP_ADDRESS] [-p PORT] [-l RANDOM_SEQUENCE_LENGHT] [-n NUMBER_OF_SEQUENCES]\n\t i IP_ADDRESS\t : \t Specifies the  ip address of the server to connect into.\n\t p PORT_NUM\t : \t Specifies the port number to be PORT_NUM.\n\t l RANDOM_SEQUENCE_LENGHT\t : \t Specifies lenght of the random byte sequences to request.\n\t n NUMBER_OF_SEQUENCES\t : \t Specifies the numbber of byte sequences to request from the server.\n", argv[0]);
            exit(FAILED_TO_PARSE_PARAMETERS);
            break;
        }
    }
    if (client_params.ipAddress == 0)
    {
        client_params.ipAddress = (char *)strdup("127.0.0.1");
    }
    return client_params;
}

void ClientParameters_freeMembers(ClientParameters *clientParamters)
{
    free(clientParamters->ipAddress);
    clientParamters->ipAddress = 0;
}