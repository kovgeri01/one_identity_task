#ifndef SERVER_H
#define SERVER_H

#include "server_parameters.h"


typedef struct Server
{
    ServerParameters serverParameters;
    void* internalState;
} Server;

/// @brief Creates aa new Server from the given parameters
/// @param serverParameters The parameters to create the server from
/// @return The server instance that was created
Server* Server_new(ServerParameters);

/// @brief Start the server according to it's config
/// @param serverToStart The server to start
void Server_start(Server*);

/// @brief Checks whether the server is a daemon process or not.
/// @param servr The server to check
/// @return 0 if the server is not a daemon process, otherwise 1.
int Server_isDaemon(Server*);

/// @brief Gracefullt terminates the server.
/// @param serverToTermmintate The server to terminate
void Server_terminate(Server*);

/// @brief Free the resources held by the server after termination.
/// @param serverToFree The server to free.
void Server_free(Server*);

#endif
