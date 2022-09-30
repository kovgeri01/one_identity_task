#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "logging.h"

#include "server.h"
#include "process_return_codes.h"
#include "random_sequence_util.h"

#define MAXIMUM_NUMBER_OF_CLIENTS 30

typedef struct
{
    int masterTerminatorPipeInFd;
    int masterTerminatorPipeOutFd;
    int *activeClients;
    int maximumClients;
} InternalServerState;

/// @brief Gets the internal server state's struct from the Server
/// @param servr The server where to get the internal server state from
/// @return The internal server state's pointer
static InternalServerState *server_getInternalServerState(Server *servr)
{
    return ((InternalServerState *)servr->internalState);
}

/// @brief Creates aa new Server from the given parameters
/// @param serverParameters The parameters to create the server from
/// @return The server instance that was created
Server *Server_new(ServerParameters serverParameters)
{
    Server *servr = (Server *)(malloc(sizeof(Server)));
    servr->serverParameters = serverParameters;
    servr->internalState = malloc(sizeof(InternalServerState));
    InternalServerState *internalState = server_getInternalServerState(servr);
    internalState->maximumClients = MAXIMUM_NUMBER_OF_CLIENTS;
    internalState->activeClients = (int *)(calloc(MAXIMUM_NUMBER_OF_CLIENTS, sizeof(int)));
    int pipefds[2];
    if (pipe(pipefds) == -1)
    {
        logging_log_error("Error while creating pipe for terminating the main loop!");
        exit(FAILED_TO_CREATE_TERMINATION_PIPE);
    }
    internalState->masterTerminatorPipeInFd = pipefds[0];
    internalState->masterTerminatorPipeOutFd = pipefds[1];
    if (pipe(pipefds) == -1)
    {
        logging_log_error("Error while creating pipe for terminating the worker thread!");
        exit(FAILED_TO_CREATE_TERMINATION_PIPE);
    }
    return servr;
}

/// @brief Starts up the server according to the configuration, and serves any incoming requests.
/// @param servr The server to run.
static void Server_run(Server *servr)
{
    logging_log_info("Server starting up...\n");
    int serverSocket, addressLength;
    struct sockaddr_in address;
    int socketIndex;
    char readBuffer[1024];
    logging_log_info("Creating server socket.\n");
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        logging_log_error("Failed to create server socket!");
        exit(FAILED_TO_CREATE_SERVER_SOCKET);
    }

    int optVal = 1;
    logging_log_info("Setting socket up to allow multiple connections.\n");
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&optVal,
                   sizeof(optVal)) < 0)
    {
        logging_log_error("Failed to set server socket to allow multiple connections!");
        exit(FAILED_TO_SET_SERVER_SOCKET_TO_ALLOW_MULTIPLE_CONNECTIONS);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(servr->serverParameters.port);
    logging_log_info("Binding server socket to localhost:%d.\n", servr->serverParameters.port);
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        logging_log_error("Failed to bind server!");
        exit(FAILED_TO_BIND_SERVER);
    }

    InternalServerState *internalServerState = server_getInternalServerState(servr);

    if (listen(serverSocket, 30) < 0)
    {
        logging_log_error("Failed to listen on socket.");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        fd_set socketSet;
        FD_ZERO(&socketSet);
        FD_SET(serverSocket, &socketSet);
        FD_SET(internalServerState->masterTerminatorPipeOutFd, &socketSet);

        int biggestFdNum = serverSocket;
        if (biggestFdNum < internalServerState->masterTerminatorPipeOutFd)
        {
            biggestFdNum = internalServerState->masterTerminatorPipeOutFd;
        }

        int activeClientId;
        for (activeClientId = 0; activeClientId < internalServerState->maximumClients; activeClientId++)
        {
            int currentSocket = internalServerState->activeClients[activeClientId];

            if (currentSocket > 0)
            {
                FD_SET(currentSocket, &socketSet);
                if (currentSocket > biggestFdNum)
                    biggestFdNum = currentSocket;
            }
        }

        logging_log_info("Server waiting for incoming connections.\n");
        if (select(biggestFdNum + 1, &socketSet, NULL, NULL, NULL) == -1)
        {
            logging_log_errno("Error when waiting for connections during select()");
        }
        if (FD_ISSET(serverSocket, &socketSet))
        {
            int new_socket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t *)&addressLength);
            if (new_socket == -1)
            {
                logging_log_errno("Error when accepting socket with accept()");
            }
            else if (new_socket > 0)
            {
                logging_log_info("New client wants to connect.\n");
                int activeClientIndex;
                int addedToActiveClients = 0;
                for (activeClientIndex = 0; activeClientIndex < internalServerState->maximumClients && !addedToActiveClients; ++activeClientIndex)
                {
                    if (internalServerState->activeClients[activeClientIndex] == 0)
                    {
                        internalServerState->activeClients[activeClientIndex] = new_socket;
                        logging_log_info("Accepted client, added to active connection pool, to get served by the worker thread.\n");
                        addedToActiveClients = 1;
                    }
                }
                if (!addedToActiveClients)
                {
                    logging_log_info("Failed to accept client, no more space left for active connections!\n");
                    close(new_socket);
                }
            }
            int messageToWorker = 1;
            logging_log_info("Waking up the worker thread.\n");
        }
        else
        {
            int activeClientId;
            for (activeClientId = 0; activeClientId < internalServerState->maximumClients; activeClientId++)
            {
                int currentActiveClient = internalServerState->activeClients[activeClientId];

                if (FD_ISSET(currentActiveClient, &socketSet))
                {
                    if (read(currentActiveClient, readBuffer, 1024) == 0)
                    {
                        close(currentActiveClient);
                        internalServerState->activeClients[activeClientId] = 0;
                        logging_log_info("Connection fd #%d disconnected.\n", currentActiveClient);
                    }
                    else
                    {
                        int bytesToGenerate = atoi(readBuffer);
                        generate_byte_sequence(bytesToGenerate, currentActiveClient);
                        close(currentActiveClient);
                        internalServerState->activeClients[activeClientId] = 0;
                        logging_log_info("Generated number for connection fd #%d.\n", currentActiveClient);
                    }
                }
            }
        }
    }
}

/// @brief Starts the server in daemon mode
/// @param serverToStart the server to start.
static void Server_daemonRun(Server *serverToStart)
{
    int pid;

    pid = fork();
    if (pid < 0)
    {
        logging_log_error("Failed to create daemon process!");
        exit(FAILED_TO_CREATE_DAEMON_PROCESS);
    }
    if (pid > 0)
    {
        exit(NO_ERROR);
    }

    umask(0);

    int sid = setsid();
    if (sid < 0)
    {
        logging_log_error("Failed to create daemon process' session!");
        exit(FAILED_TO_CREATE_DAEMON_SESSION);
    }

    if ((chdir("/")) < 0)
    {
        logging_log_error("Failed to change daemon process' working directory!");
        exit(FAILED_TO_SWITCH_DAEMON_WORKING_DIRECTORY);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    freopen("/opt/one_identity/server_stdout.txt", "a+", stdout);
    freopen("/opt/one_identity/server_stderr.txt", "a+", stderr);
    Server_run(serverToStart);
}

/// @brief Start the server according to it's config
/// @param serverToStart The server to start
void Server_start(Server *serverToStart)
{
    if (serverToStart->serverParameters.isDaemon)
    {
        logging_log_info(" Starting random sequence generator server as daemon on port %d\n", serverToStart->serverParameters.port);
        Server_daemonRun(serverToStart);
    }
    else
    {
        logging_log_info(" Starting random sequence generator server on port %d\n", serverToStart->serverParameters.port);
        Server_run(serverToStart);
    }
}

/// @brief Checks whether the server is a daemon process or not.
/// @param servr The server to check
/// @return 0 if the server is not a daemon process, otherwise 1.
int Server_isDaemon(Server *servr)
{
    return servr->serverParameters.isDaemon;
}

/// @brief Gracefullt terminates the server.
/// @param serverToTermmintate The server to terminate
void Server_terminate(Server *serverToTermmintate)
{
    int terminationFlag = 1;
    write(server_getInternalServerState(serverToTermmintate)->masterTerminatorPipeInFd, &terminationFlag, sizeof(int));
}

/// @brief Free the resources held by the server after termination.
/// @param serverToFree The server to free.
void Server_free(Server *serverToFree)
{
    {
        InternalServerState *internalState = server_getInternalServerState(serverToFree);
        free((void *)internalState->activeClients);
        internalState->activeClients = 0;
    }
    free(serverToFree->internalState);
    serverToFree->internalState = 0;
    free(serverToFree);
    serverToFree = 0;
}