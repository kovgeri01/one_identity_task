#include <stdio.h>
#include <gtest/gtest.h>

extern "C" 
{
  #include "random_sequence_util.h"
}

class RandomSequenceGeneratorTestSection : public testing::Test
{
public:
  void SetUp() override
  {
    init_random_generator(0);
  }
};

TEST_F(RandomSequenceGeneratorTestSection, generate_byte_sequence_test)
{
  {
    FILE * byteSequenceFdOut = fopen("ut_file.bin","w");
    generate_byte_sequence(8, fileno(byteSequenceFdOut));
    fclose(byteSequenceFdOut);
    unsigned char generatedBytes[8];
    unsigned char expectedBytes[]{103,69,139,107,198,35,123,50};
    FILE * byteSequenceFdIn = fopen("ut_file.bin","r");
    unsigned char generatedNumberBuffer[2048];
    ssize_t readNumOfBytes = read(fileno(byteSequenceFdIn),generatedNumberBuffer,2048);
    EXPECT_EQ(8, readNumOfBytes);
    for(int byteIndex = 0; byteIndex < 8; byteIndex++)
    {
      EXPECT_EQ(generatedNumberBuffer[byteIndex], expectedBytes[byteIndex]);
    }
  }

  {
    FILE * byteSequenceFdOut = fopen("ut_file.bin","w");
    generate_byte_sequence(7, fileno(byteSequenceFdOut));
    fclose(byteSequenceFdOut);
    unsigned char generatedBytes[7];
    unsigned char expectedBytes[]{105,152,60,100,115,72,51};
    FILE * byteSequenceFdIn = fopen("ut_file.bin","r");
    unsigned char generatedNumberBuffer[2048];
    ssize_t readNumOfBytes = read(fileno(byteSequenceFdIn),generatedNumberBuffer,2048);
    EXPECT_EQ(7, readNumOfBytes);
    for(int byteIndex = 0; byteIndex < 7; byteIndex++)
    {
      EXPECT_EQ(generatedNumberBuffer[byteIndex], expectedBytes[byteIndex]);
    }
  }

  {
    FILE * byteSequenceFdOut = fopen("ut_file.bin","w");
    generate_byte_sequence(9, fileno(byteSequenceFdOut));
    fclose(byteSequenceFdOut);
    unsigned char generatedBytes[9];
    unsigned char expectedBytes[]{81,220,176,116,255,92,73,25, 74};
    FILE * byteSequenceFdIn = fopen("ut_file.bin","r");
    unsigned char generatedNumberBuffer[2048];
    ssize_t readNumOfBytes = read(fileno(byteSequenceFdIn),generatedNumberBuffer,2048);
    EXPECT_EQ(9, readNumOfBytes);
    for(int byteIndex = 0; byteIndex < 9; byteIndex++)
    {
      EXPECT_EQ(generatedNumberBuffer[byteIndex], expectedBytes[byteIndex]);
    }
  }
}