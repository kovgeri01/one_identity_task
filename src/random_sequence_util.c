#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "logging.h"

/// @brief Seeds stdlib.h's random number generator.
/// @param __seed the seed of the random number generator
void init_random_generator(unsigned int seed)
{
    srand(seed);
}

/// @brief Generates a random byte sequence using stdlib.h's rand
/// @param slenght the lenght of the random byte sequente to generate.
/// @param fdToWrite the filedescriptor to write the random byte sequence into.
void generate_byte_sequence(unsigned long slenght, int fdToWrite)
{
    unsigned long writtenBytes = 0uL;
    while (writtenBytes < slenght)
    {
        int randomNumber = rand();
        unsigned long sizeOfRandNum = sizeof(int);
        size_t bytesToWrite;
        if (slenght < (writtenBytes + sizeOfRandNum))
        {
            bytesToWrite = slenght - writtenBytes;
        }
        else
        {
            bytesToWrite = sizeOfRandNum;
        }
        if (write(fdToWrite, &randomNumber, bytesToWrite) == -1)
        {
            logging_log_errno(errno, "Error when writing the randomly generated sequence!");
        }
        writtenBytes += bytesToWrite;
    }
}
