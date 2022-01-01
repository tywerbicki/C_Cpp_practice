#include "mm256_extensions.h"
#include <stdio.h>

int main(void)
{
    __m256 ones = _mm256_set1_ps(1.0f);

    _mm256_print_ps(&ones);

    __m256i neg2s = _mm256_castps_si256(_mm256_add_ps(ones, _mm256_set1_ps(-3.0f)));

    _mm256_print_si256(&neg2s);

    float neg2 = -2.0f;
    printf("%d \n", *((int*)&neg2));

    return 0;
}