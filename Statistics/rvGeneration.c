#include<stdlib.h>
#include<stdio.h>
#include<math.h>

static inline float f32_uniform(const float a, const float b)
{   return ((float)rand() / (float)RAND_MAX * (b-a)) + a;  }

float f32_exponential(const float alpha)
{   return -1.0F * alpha * logf(f32_uniform(0, 1)); }

float f32_normal(const float mu, const float sigma)
{
    static float n2;
    static int generate = 1;

    if (!generate)
    {
        generate = 1;
        return n2;
    }

    float u1, u2, flag, n1, tmp;

    do
    {
        u1 = f32_uniform(-1, 1); u2 = f32_uniform(-1, 1);
        flag = (u1*u1) + (u2*u2);
    } while (flag > 1);
    tmp = sqrtf(-2 * logf(flag) / flag);
    n1 = u1 * tmp * sigma + mu;
    n2 = u2 * tmp * sigma + mu;
    generate = 0;

    return n1;
}

int main(void)
{
    srand(9999);

    for (size_t i = 0; i < 10; i++)
    {   printf("%f ", f32_uniform(-1, 1));  }
    printf("\n");

    for (size_t i = 0; i < 10; i++)
    {   printf("%f ", f32_exponential(1));  }
    printf("\n");

    for (size_t i = 0; i < 10; i++)
    {   printf("%f ", f32_normal(5, 2));  }
    printf("\n");

    return 0;
}
