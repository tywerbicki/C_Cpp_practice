#include "mm256_extensions.h"
#include <immintrin.h>
#include <stdio.h>

//Print 256-bit register packed with 8 fp32.
void _mm256_print_ps(__m256* vec)
{
    float* elem = (float*)aligned_alloc(sizeof(float)*8, sizeof(float)*8);
    _mm256_store_ps(elem, *vec);
    for (size_t i = 0; i < 8; i++)
    {
        printf("%f ", *(elem + i));
    }
    printf("\n");
    free(elem);
}

//Print 256-bit register packed with 8 sint32.
void _mm256_print_si256(__m256i* vec)
{
    int* elem = (int*)aligned_alloc(sizeof(int)*8, sizeof(int)*8);
    _mm256_store_si256((__m256i*)elem, *vec);
    for (size_t i = 0; i < 8; i++)
    {
        printf("%d ", *(elem + i));
    }
    printf("\n");
    free(elem);
}