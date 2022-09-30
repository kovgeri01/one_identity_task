#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include "client_parameters.h"

typedef struct
{
    int randomNumberIndex;
    int lenghtOfRandomByteSequence;
    int clientSocketFd;
} RequestData;

/// @brief Request a random byte sequence from the server
/// @param randomNumberIndex The index of the random byte sequence (for logging purposes.)
/// @param lenghtOfRandomByteSequence The lenght of the random byte sequence
/// @param clientSocketFd The socket file descriptor to request the random number from.
void requestAndPrintRandomNumber(int randomNumberIndex, int lenghtOfRandomByteSequence, int clientSocketFd)
{
    printf("\t Requesting #%d.\n", randomNumberIndex + 1);
    char *messageToServer;
    int numberOfPrintedBytes = asprintf(&messageToServer, "%d", lenghtOfRandomByteSequence);
    ssize_t readBytes = 0;
    if (numberOfPrintedBytes > 0)
    {
        write(clientSocketFd, messageToServer, numberOfPrintedBytes);
        free(messageToServer);
        messageToServer = 0;
        int *readBuffer = malloc(lenghtOfRandomByteSequence + 1);
        while (readBytes < lenghtOfRandomByteSequence)
        {
            int readStatus = read(clientSocketFd, readBuffer + readBytes, lenghtOfRandomByteSequence - readBytes);
            if (readStatus == -1 || readStatus == 0)
            {
                break;
            }
            else
            {
                readBytes += readStatus;
            }
        }
        if (readBytes != numberOfPrintedBytes)
        {
            perror("Server returned less bytes than requested!");
        }
        readBuffer[lenghtOfRandomByteSequence] = '\0';
        printf("Returned random byte sequence for #%d: \t", randomNumberIndex + 1);
        int byteIndex;
        for (byteIndex = 0; byteIndex < lenghtOfRandomByteSequence; byteIndex++)
        {
            if (byteIndex > 0)
            {
                printf(":");
            }
            printf("%02X", readBuffer[randomNumberIndex]);
        }
        printf("\n");
        free(readBuffer);
        readBuffer = 0;
    }
}

void requestRandomNumbers(ClientParameters *clientParams)
{
    printf("Requesting #%d random byte sequences of %d in length from %s:%d.\n", clientParams->numberOfRandomBytesToRequest, clientParams->lenghtOfRandomByteSequence, clientParams->ipAddress, clientParams->port);
    int numberIndex;
    for (numberIndex = 0; numberIndex < clientParams->numberOfRandomBytesToRequest; numberIndex++)
    {

        int clientSocket;
        struct sockaddr_in address;

        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1)
        {
            perror("Failed socket creation.\n");
            exit(0);
        }
        memset(&address, 0, sizeof(address));

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(clientParams->ipAddress);
        address.sin_port = htons(clientParams->port);

        if (connect(clientSocket, (struct sockaddr *)&address, sizeof(address)) != 0)
        {
            perror("Failed to connect to the server\n");
            exit(0);
        }
        else
        {
            perror("Connected to the server, requesting numbers..\n");
            requestAndPrintRandomNumber(numberIndex, clientParams->lenghtOfRandomByteSequence, clientSocket);
        }
        close(clientSocket);
    }
}

int main(int argc, char **argv)
{
    ClientParameters clientParams = ClientParameters_parse(argc, argv);
    requestRandomNumbers(&clientParams);
    ClientParameters_freeMembers(&clientParams);
    return 0;
}