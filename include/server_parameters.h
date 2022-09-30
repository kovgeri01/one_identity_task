#ifndef SERVER_PARAMETERS_H
#define SERVER_PARAMETERS_H

typedef struct
{
    int port;
    int isDaemon;
} ServerParameters;

/// @brief Parses the server's parameters from the process's parameters.
/// @param argc The argumment count
/// @param argv The argument vector
/// @return The parsed server parameters
ServerParameters ServerParameters_parse(int argc, char** argv);

#endif