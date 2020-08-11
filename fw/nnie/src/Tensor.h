#ifndef INS_STRUCT_H
#define INS_STRUCT_H
/*shape of Tensor is n c h w*/    
typedef struct Tensor_S{
    int n;
    int channel;
    int height;
    int width;
    float *data;
} Tensor;


#endif