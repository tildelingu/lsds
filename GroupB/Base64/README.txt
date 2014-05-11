Last Updated: 2014-05-02

Authors:
  Tianyang HU
  Wenbin LIN
  Zhichao XU


Description:
  This program implements base64 decoding. The input is base64 encoded data, with the following format. Output is the decoded data (plaintext).
--------------------------------
[NUMBER_OF_BYTES_OF_ENCODED_DATA]
ENCODED_DATA
--------------------------------


Files:
  base64_decoder.c // Implements 2-way paralleled base64 decoding algorithm
  utilities.h // Utility functions
  libi0 // The libi0 library
  input1.txt // Sample input file
  input2.txt // Another sample input file


Instructions:
  Step 1. Compile.
  cc0 base64_decoder.c -o base64_decoder.bin
  
  Step 2. Run.
  am run base64_decoder.bin [INPUT_FILENAME]
