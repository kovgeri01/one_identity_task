#ifndef CLIENT_PARAMETERS_H
#define CLIENT_PARAMETERS_H

typedef struct
{
    char* ipAddress;
    int port;
    int lenghtOfRandomByteSequence;
    int numberOfRandomBytesToRequest;
    int doInParallel;
} ClientParameters;

/// @brief Parses the client's parameters from the process's parameters.
/// @param argc The argumment count
/// @param argv The argument vector
/// @return The parsed client parameters
ClientParameters ClientParameters_parse(int argc, char** argv);

/// @brief Frees up any member fields held by the client parameter, but not the object itself.
/// @param clientParamters
void ClientParameters_freeMembers(ClientParameters* clientParamters);

#endif