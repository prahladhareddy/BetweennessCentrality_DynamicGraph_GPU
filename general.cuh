struct vec
{
    int* arr;
    int max_size;
    int size;
};

extern __device__ void init_vec(struct vec *v);

extern __device__ void  push(struct vec * v,int a);