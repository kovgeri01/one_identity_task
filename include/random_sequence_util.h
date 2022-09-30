/// @brief Seeds stdlib.h's random number generator.
/// @param __seed the seed of the random number generator
void init_random_generator(unsigned int __seed);

/// @brief Generates a random byte sequence using stdlib.h's rand
/// @param slenght the lenght of the random byte sequente to generate.
/// @param fdToWrite the filedescriptor to write the random byte sequence into.
void generate_byte_sequence(unsigned long slenght, int fdToWrite);
