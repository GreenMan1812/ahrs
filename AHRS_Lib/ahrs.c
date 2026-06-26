#include "ahrs.h"

#define NUM_INV_MATS 10
#define MAT_DIM 3
static int8_t _grid_index(float* vec);
static int8_t _sub_grid_index(float* vec, int8_t i);
/* inverses: 10 матриц 3x3 -> 10 * 9 = 90 элементов */
static float _inverses[90] = {
    /* #0 */
    -0.309017f,  0.500000f,  0.190983f,
     0.000000f,  0.000000f, -0.618034f,
    -0.309017f, -0.500000f,  0.190983f,

    /* #1 */
    -0.190983f,  0.309017f, -0.500000f,
    -0.500000f, -0.190983f,  0.309017f,
     0.309017f, -0.500000f, -0.190983f,

    /* #2 */
    -0.618034f,  0.000000f,  0.000000f,
     0.190983f, -0.309017f, -0.500000f,
     0.190983f, -0.309017f,  0.500000f,

    /* #3 */
    -0.500000f,  0.190983f, -0.309017f,
     0.000000f, -0.618034f,  0.000000f,
     0.500000f,  0.190983f, -0.309017f,

    /* #4 */
    -0.190983f, -0.309017f, -0.500000f,
    -0.190983f, -0.309017f,  0.500000f,
     0.618034f,  0.000000f,  0.000000f,

    /* #5 */
    -0.309017f, -0.500000f, -0.190983f,
     0.190983f,  0.309017f, -0.500000f,
     0.500000f, -0.190983f,  0.309017f,

    /* #6 */
     0.309017f, -0.500000f,  0.190983f,
     0.000000f,  0.000000f, -0.618034f,
     0.309017f,  0.500000f,  0.190983f,

    /* #7 */
     0.190983f, -0.309017f, -0.500000f,
     0.500000f,  0.190983f,  0.309017f,
    -0.309017f,  0.500000f, -0.190983f,

    /* #8 */
     0.500000f, -0.190983f, -0.309017f,
     0.000000f,  0.618034f,  0.000000f,
    -0.500000f, -0.190983f, -0.309017f,

    /* #9 */
     0.309017f,  0.500000f, -0.190983f,
    -0.500000f,  0.190983f,  0.309017f,
    -0.190983f, -0.309017f, -0.500000f
};

/* _mid_inverses: 10 матриц 3x3 -> 10 * 9 = 90 элементов */
static float _mid_inverses[90] = {
    /* #0 */
     0.000000f,  1.000000f, -0.618034f,
     0.000000f, -1.000000f, -0.618034f,
    -0.618034f,  0.000000f,  1.000000f,

    /* #1 */
    -1.000000f,  0.618034f,  0.000000f,
     0.000000f, -1.000000f,  0.618034f,
     0.618034f,  0.000000f, -1.000000f,

    /* #2 */
    -0.618034f,  0.000000f, -1.000000f,
     1.000000f, -0.618034f,  0.000000f,
    -0.618034f,  0.000000f,  1.000000f,

    /* #3 */
    -1.000000f, -0.618034f,  0.000000f,
     1.000000f, -0.618034f,  0.000000f,
     0.000000f,  1.000000f, -0.618034f,

    /* #4 */
    -1.000000f, -0.618034f,  0.000000f,
     0.618034f,  0.000000f,  1.000000f,
     0.618034f,  0.000000f, -1.000000f,

    /* #5 */
    -0.618034f,  0.000000f, -1.000000f,
     1.000000f,  0.618034f,  0.000000f,
     0.000000f, -1.000000f,  0.618034f,

    /* #6 */
     0.000000f, -1.000000f, -0.618034f,
     0.000000f,  1.000000f, -0.618034f,
     0.618034f,  0.000000f,  1.000000f,

    /* #7 */
     1.000000f, -0.618034f,  0.000000f,
     0.000000f,  1.000000f,  0.618034f,
    -0.618034f,  0.000000f, -1.000000f,

    /* #8 */
     1.000000f,  0.618034f,  0.000000f,
    -1.000000f,  0.618034f,  0.000000f,
     0.000000f, -1.000000f, -0.618034f,

    /* #9 */
     0.000000f,  1.000000f,  0.618034f,
    -1.000000f, -0.618034f,  0.000000f,
     0.618034f,  0.000000f, -1.000000f
};

arm_matrix_instance_f32 _arm_matrix_inverses[10];
arm_matrix_instance_f32 _arm_matrix_mid_inverses[10];
static const neighbor_umbrella_t _neighbor_umbrellas[3] = {
    {{9, 8, 7, 12, 14}, 1, 2, 0, 0, 2},
    {{1,2,4,5,3}, 0,0,2,2,0},
    {{16,15,13,18,17}, 2,2,0,2,1},
};

static const int8_t _key_table[10] = {
    10, // 0b00
    4,  // 0b01
    5,  // 0b10
    0,
    3,
    2,
    1,
    0
};
uint8_t get_component(int8_t idx, int8_t comp_idx)
{
    if(idx < 3){
        return _neighbor_umbrellas[idx].components[comp_idx];
    }
    return (_neighbor_umbrellas[idx % 3].components[comp_idx] + 10) % 20;
}
void AHRS_GridInit()
{
    for (uint8_t i = 0; i < NUM_INV_MATS; i++) {
        arm_mat_init_f32(&_arm_matrix_inverses[i], MAT_DIM, MAT_DIM, *_inverses + i * MAT_DIM * MAT_DIM);
        arm_mat_init_f32(&_arm_matrix_mid_inverses[i], MAT_DIM, MAT_DIM, *_mid_inverses + i * MAT_DIM * MAT_DIM);
    }
}

int8_t AHRS_SectionGrid(float* vec)
{
    int8_t i = _grid_index(vec);
    if(i == -1) return -1;
    int8_t j = _sub_grid_index(vec, i);
    if(j == -1) return -1;
    return i * 4 + j;
}

int8_t _sub_grid_index(float* vec, int8_t i)
{
    float w[3] = {0.0f, 0.0f, 0.0f};
    arm_mat_vec_mult_f32(&_arm_matrix_mid_inverses[i % 10], vec, w);
    if(i > 9){
        w[0] = -w[0];
        w[1] = -w[1];
        w[2] = -w[2];
    }
    if(fabsf(w[0]) <= 1e-5 || fabsf(w[1]) <= 1e-5 || fabsf(w[2]) <= 1e-5) return -1;

    if(w[0] < 0) return 3;
    if(w[1] < 0) return 1;
    if(w[2] < 0) return 2;
    return 0;
}

int8_t _grid_index(float* vec)
{
    int8_t key = 0;
    int8_t balance = 0;
    int8_t iter = 3;
    float w[3] = {0.0f, 0.0f, 0.0f};
    arm_mat_vec_mult_f32(&_arm_matrix_inverses[0], vec, w);

    for(iter = 0; iter < 3; iter++)
    {
        if(fabsf(w[iter]) <= 1e-5){
            return -1;
        }else if(w[iter] > 0){
            key |= (1 << iter);
            balance++;
        }else{
            key &= ~(1 << iter);
            balance--;
        }
    }
    if(balance == 3)
    {
        return 0;
    }else if(balance == -3)
    {
        return 10;
    }
    int8_t umbrella = _key_table[key];
    if(balance == -1){
        w[0] = -w[0];
        w[1] = -w[1];
        w[2] = -w[2];
    }
    float u[3]={0.0f, 0.0f, 0.0f};
    if(umbrella % 3 == 0)
    {
        u[0] = w[0];
        u[1] = w[1];
        u[2] = -w[2];
    }else if(umbrella % 3 == 1)
    {
        u[0] = w[1];
        u[1] = w[2];
        u[2] = -w[0];
    }else if(umbrella % 3 == 2)
    {
        u[0] = w[2];
        u[1] = w[0];
        u[2] = -w[1];
    }
    if(u[1] > u[0])
    {
        int8_t comp = get_component(umbrella, 1);
        arm_mat_vec_mult_f32(&_arm_matrix_inverses[comp % 10], vec, w);
        if(comp > 9)
        {
            w[0] = -w[0];
            w[1] = -w[1];
            w[2] = -w[2];
        }
        float x1 = w[_neighbor_umbrellas[umbrella % 3].v1_c1];
        float x2 = w[_neighbor_umbrellas[umbrella % 3].v2_c1];
        if(x1 <0){
            return get_component(umbrella, 2);
        }
        if(x2 <0){
            return get_component(umbrella, 0);
        }
        return comp;
    }
    else if(u[1] < u[0])
    {
        int8_t comp = get_component(umbrella, 4);
        arm_mat_vec_mult_f32(&_arm_matrix_inverses[comp % 10], vec, w);
        if(comp > 9)
        {
            w[0] = -w[0];
            w[1] = -w[1];
            w[2] = -w[2];
        }
        float x4 = w[_neighbor_umbrellas[umbrella % 3].v4_c4];
        float x0 = w[_neighbor_umbrellas[umbrella % 3].v0_c4];
        if(x4 <0){
            return get_component(umbrella, 0);
        }
        if(x0 <0){
            return get_component(umbrella, 3);
        }
        return comp;
    }else if(fabsf(u[0] - u[1]) <= 1e-5)
    {
        int8_t comp = get_component(umbrella, 0);
        arm_mat_vec_mult_f32(&_arm_matrix_inverses[comp % 10], vec, w);
        if(comp > 9)
        {
            w[0] = -w[0];
            w[1] = -w[1];
            w[2] = -w[2];
        }
        float x0 = w[_neighbor_umbrellas[umbrella % 3].v0_c0];
        if(fabsf(x0) < 1e-5) return -1;
        else if(x0 < 0) return get_component(umbrella, u[0] < u[1] ? 3:2);

        return comp;
    }
    return -1;
}
