# C Programming Language Practice
Practicing various paradigms of the C programming language.

## CSV_Operations

An example of a potential representation of a float data set in C. The program reads in data from `numbers.txt` and stores it in a useful way. It should be noted that this implementation does not contain many necessary checks, and would therefore require some additions prior to deployment in a production setting.

## Statistics
Various computational statistics operations.

### rvGeneration.c
Generation of uniform, exponential, and normal random variables. <br>
Because this program makes use of the `math.h` header file, the `-lm` compiler flag must be included when compiling.

## Linking Practice
Practice linking function definitions in source code to program modules.

### mm256Extensions

#### Purpose

Additional, useful simd functions.

#### Implementation

1)  Compile `mm256_extentions_source.c` and `seeVectors.c` to object files. <br>
    Example: `gcc -Wall -c mm256_extentions_source.c -mavx`

2)  Compile the program by linking the object files. <br>
    Example: `gcc -o test.exe mm256_extentions_source.o seeVectors.o`
    
3)  Run the program.
