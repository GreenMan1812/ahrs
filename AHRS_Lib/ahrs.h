#ifndef AHRS_H
#define AHRS_H
#include "main.h"
typedef struct {
    int8_t components[5];
    int8_t v0_c0;
    int8_t v1_c1;
    int8_t v2_c1;
    int8_t v4_c4;
    int8_t v0_c4;
} neighbor_umbrella_t;

static inline uint8_t is_zero(float* vec, uint8_t blocksize)
{

    while(blocksize--){
        if(fabsf(vec[blocksize]) <= 1e-5){
            return 0;
        }
    }
    return 1;
}
#endif
