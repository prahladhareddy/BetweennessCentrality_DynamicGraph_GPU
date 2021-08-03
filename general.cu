#include "_graph.h"
#include "cuda.h"
#include "general.cuh"

int* _copy( int * cpu_arr ,int n)
{
    int * arr;
    cudaMalloc(&arr,(n)*sizeof(int));
    cudaMemcpy(arr, cpu_arr, sizeof(int) * (n), cudaMemcpyHostToDevice);
    return arr;
}

struct cuda_copies copy_to_gpu(graph* g)
{
    struct cuda_copies cpy;
    int n = g->N;
    int m = g->M;
    cpy.offset_arr = _copy(g->offset_arr,n+1);
    cpy.edge_arr = _copy(g->edge_arr , m);
    cpy.cut_ver = _copy(g->cut_ver,n);
    cpy.bcc_arr = _copy(g->bcc_arr,m);
    cpy.subg_size = _copy(g->subg_size,m);
    return cpy;
}

void _free (struct cuda_copies c)
{
    cudaFree(c.offset_arr);
    cudaFree(c.edge_arr);
    cudaFree(c.cut_ver);
    cudaFree(c.subg_size);
    cudaFree(c.bcc_arr);
}

// __device__ void  init_vec(struct vec* v)
// {
//     v->size=0;
//     v->max_size=4;
//     v->arr = (int*) malloc (4*sizeof(int));
// }

// __device__ void  push(struct vec *v1,int a)
// {
//     int s_max = v1->max_size;
//     int* arr = v1->arr;
//     int s = v1->size;
//     if(s<s_max)
//     {
//         arr[s]=a;
//         v1->size++;
//     }
//     else 
//     {
//         int* arr2 = (int*) malloc (2*s_max*sizeof(int));
//         for(int i=0;i<s;i++)
//         {
//             arr2[i]=arr[i];
//         }
//         free(arr);
//         arr2[s]=a;
//         v1->arr = arr2;
//         v1->size++;
//         v1->max_size = 2*s_max;
//     }
// }