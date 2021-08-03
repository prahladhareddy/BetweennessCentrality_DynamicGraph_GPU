#include "_graph.h"
#include "cuda.h"
#include "general.cuh"

__global__ void k_find_btw(int* offset_arr ,int* edge_arr , double* btw ,int* edge_rev_arr,int N,int M,int off,double *g_ans,int *g_dis,int *g_path,int *g_stack,int *g_queue,int* g_parent,int* g_vis)
{
    
    int id = blockIdx.x +  off;
    if(id>=N)
    {
        return;
    }
    if(threadIdx.x == 0 && id<N)
    {
        double *ans = &g_ans[N*id];
        int *vis = &g_vis[N*id];
        int *dis = &g_dis[N*id];
        int *path = &g_path[N*id];
        int* stack = &g_stack[N*id];
        int* queue = &g_queue[N*id];
        int* parent = &g_parent[M*id];
        for(int i=0;i<N;i++)
        {
            vis[i]=0;
        }
        for(int i=0;i<N;i++)
        {
            ans[i]=0;
        }

        for(int i=0;i<N;i++)
        {
            dis[i]=0;
        }

        for(int i=0;i<N;i++)
        {
            path[i]=0;   
        }

        for(int i=0;i<M;i++)
        {
            parent[i]=0;
        }

        queue[0]=id;
        stack[0]=id;
        int q_start=0;
        int q_end=1;

        int s_top=1;

        vis[id]=1;
        while(q_start<q_end)
        {
            int t = queue[q_start];
            q_start++;

            for(int i=offset_arr[t];i<offset_arr[t+1];i++)
            {
                int x = edge_arr[i];
                if(vis[x]==0)
                {
                    path[x]+=path[t];
                    if(t==id)
                    {
                        path[x]=1;
                    }
                    queue[q_end]=x;
                    q_end++;
                    vis[x]=1;
                    stack[s_top]=x;
                    s_top++;
                    parent[edge_rev_arr[i]]=1;
                    dis[x]=dis[t]+1;
                }
                else if((dis[t]+1)==dis[x])
                {
                    path[x]+=path[t];
                    parent[edge_rev_arr[i]]=1;
                }
            }
        }

        while(s_top!=0)
        {
            int t = stack[s_top-1];
            s_top--;
            for(int i=offset_arr[t];i<offset_arr[t+1];i++)
            {
                if(parent[i]==1)
                {
                    int x = edge_arr[i];
                    double div = (double)path[x]/(double)path[t];
                    ans[x]+=1.0*(1.0+ans[t]*1.0)*(1.0*div);
                }
            }
        }
        for(int i=0;i<N;i++)
        {
            btw[id*N+i]=ans[i];
        }
    }
}



void graph::find_btw()
{
    btw = (double*) malloc (N*sizeof(double));
    int* gpu_offset_arr;
    int* gpu_edge_arr;
    double* gpu_btw;
    int* gpu_edge_rev_arr;
    double* result = (double*) malloc (N*N*sizeof(double));
    cudaMalloc( &gpu_offset_arr, sizeof(int) * (1+N) );
    cudaMalloc( &gpu_edge_arr, sizeof(int) * (M) );
    cudaMalloc( &gpu_btw, sizeof(double) * (N*N) );
    cudaMalloc( &gpu_edge_rev_arr, sizeof(int) * (M) );
    cudaMemcpy(gpu_offset_arr, offset_arr, sizeof(int) * (1+N), cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_edge_arr, edge_arr, sizeof(int) * M, cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_edge_rev_arr, edge_rev_arr, sizeof(int) * M, cudaMemcpyHostToDevice);

    double *g_ans ;
    int *g_vis ;
    int *g_dis ;
    int *g_path ;
    int* g_stack ;
    int* g_queue ;
    int* g_parent ;

    cudaMalloc(&g_ans,(N*N*sizeof(double)));
    cudaMalloc(&g_dis,(N*N*sizeof(int)));
    cudaMalloc(&g_path,(N*N*sizeof(int)));
    cudaMalloc(&g_stack,(N*N*sizeof(int)));
    cudaMalloc(&g_queue,(N*N*sizeof(int)));
    cudaMalloc(&g_vis,(N*N*sizeof(int)));
    cudaMalloc(&g_parent,(N*M*sizeof(int)));


    k_find_btw<<<N,1>>> (gpu_offset_arr,gpu_edge_arr,gpu_btw,gpu_edge_rev_arr,N,M,0,g_ans,g_dis,g_path,g_stack,g_queue,g_parent,g_vis);

    cudaMemcpy(result,gpu_btw,N*N*sizeof(double),cudaMemcpyDeviceToHost);

    cudaFree(g_ans);
    cudaFree(g_parent);
    cudaFree(g_dis);
    cudaFree(g_path);
    cudaFree(g_stack);
    cudaFree(g_queue);
    cudaFree(g_vis);

    for(int i=0;i<N;i++)
    {
        btw[i]=0;
    }
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
        {
            btw[i]+=result[j*N+i];
        }
        btw[i]/=2;
        cout<<btw[i]<<" ";
    }
    cout<<"\n";
}


void __global__  k_btw_bcc( struct cuda_copies c,int N,int M,int *arr,int comp,int* lock,int factor,double* bc)
{
    int id = blockIdx.x;
    if(threadIdx.x==0)
    {
        double *ans1 = (double *)malloc(N*sizeof(double));
        double *ans2 = (double *)malloc(N*sizeof(double));
        int *vis = (int *)malloc(N*sizeof(int));
        int *dis = (int *)malloc(N*sizeof(int));
        int *path = (int *)malloc(N*sizeof(int));

        int* stack = (int*)malloc(N*sizeof(int));
        int* queue = (int*)malloc(N*sizeof(int));
        struct vec* parent = (struct vec*)malloc(N*sizeof(struct vec));

        for(int i=0;i<N;i++)
        {
            init_vec(&parent[i]);
        }
        
        for(int i=0;i<N;i++)
        {
            vis[i]=0;
        }

        for(int i=0;i<N;i++)
        {
            ans1[i]=0;
        }

        for(int i=0;i<N;i++)
        {
            ans2[i]=0;
        }

        for(int i=0;i<N;i++)
        {
            dis[i]=0;
        }

        for(int i=0;i<N;i++)
        {
            path[i]=0;   
        }

        queue[0]=arr[id];
        stack[0]=arr[id];
        int q_start=0;
        int q_end=1;

        int s_top=1;

        vis[arr[id]]=1;
        while(q_start<q_end)
        {
            int t = queue[q_start];
            q_start++;

            for(int i=c.offset_arr[t];i<c.offset_arr[t+1];i++)
            {
                if(c.bcc_arr[i]!=comp)
                {
                    continue;
                }

                int x = c.edge_arr[i];
                if(vis[x]==0)
                {
                    path[x]+=path[t];
                    if(t==arr[id])
                    {
                        path[x]=1;
                    }
                    queue[q_end]=x;
                    q_end++;
                    vis[x]=1;
                    stack[s_top]=x;
                    s_top++;
                    push(&parent[x],t);
                    dis[x]=dis[t]+1;
                }
                else if((dis[t]+1)==dis[x])
                {
                    path[x]+=path[t];
                    push(&parent[x],t);
                }
            }
        }

        int f1 = c.cut_ver[arr[id]];
        int f2=0;
        int s1 = 0;
        int s2 = 0;
        if(f1)
        {
            for(int i= c.offset_arr[arr[id]];i<c.offset_arr[arr[id]+1];i++)
            {
                if(c.bcc_arr[i]!=comp)
                {
                    s1+=c.subg_size[i];
                }
            }
        }
        
        while(s_top!=0)
        {
            int t = stack[s_top-1];
            s_top--;
            f2 = c.cut_ver[t];
            s2=0;
            if(f2)
            {
                for(int i= c.offset_arr[t];i<c.offset_arr[t+1];i++)
                {
                    if(c.bcc_arr[i]!=comp)
                    {
                        s2+=c.subg_size[i];
                    }
                }
            }
            if(f1&&f2 && t!=arr[id])
            {
                ans1[t]=s1*s2;
            }
            else if(f1&&f2)
            {
                ans1[t]=s1*(N-s1-1);
            }
            for(int i=0;i<parent[t].size;i++)
            {
                int p = parent[t].arr[i];
                ans2[p]+= ((double)path[p]/(double)path[t])*(1.0+ans2[t]*1.0);
                if(f1)
                {
                    ans1[p]+=ans1[t]*1.0*(double)path[p]/(double)path[t];
                }
            }

            double a=0;
            if(t!=arr[id])
            {
                a+=factor*ans2[t]/2.0;
            }
            if(f1)
            {
                a+=factor*(ans2[t]*s1)*1.0;
                a+=factor*ans1[t]/2.0;

            }
            bool leaveLoop = false;
            while (!leaveLoop) {
                if (atomicExch(lock, 1) == 0) {
                    
                    bc[t]+= a;
                    leaveLoop = true;
                    atomicExch(lock,0);
                }
            } 

        }

    }
}

void up_comp(graph*g,int comp,struct cuda_copies c,int flag,double* gpu_btw)
{   
    int num_vert = g->bcc_verts[comp].size();
    int* arr = (int *)malloc(num_vert*sizeof(int));
    int ind=0;
    for(auto iter = (g->bcc_verts[comp].begin());iter != (g->bcc_verts[comp].end());iter++)
    {
        arr[ind] = *iter;
        ind++;
    }
    
    int* gpu_arr = _copy(arr,num_vert);
    int x=0;
    int* lock = _copy(&x,1);
    k_btw_bcc<<<num_vert,1>>>(c,g->N,g->M,gpu_arr,comp,lock,flag,gpu_btw);
    cudaDeviceSynchronize();
}

void update_btw(graph** g,vector<edge> added,vector<edge> deleted)
{
    graph* g1 =*g;
    vector<edge> new_list = up(g1->edgeList,added,deleted);
    int m = g1->M+added.size()-deleted.size();
    graph* g2 = new graph(g1->N,m,new_list);
    g2->do_basic();
    vector<vector<int>>aff_comp = affected_comps (g1,g2);

    double* gpu_btw;
    cudaMalloc(&gpu_btw,g1->N*sizeof(double));
    cudaMemcpy(gpu_btw,g1->btw,g1->N*sizeof(double),cudaMemcpyHostToDevice);
    struct cuda_copies cpy1 = copy_to_gpu( g1 );
    for(int i=0;i<aff_comp[0].size();i++)
    {
        up_comp(g1,aff_comp[0][i],cpy1,-1,gpu_btw);
    }
    struct cuda_copies cpy2 = copy_to_gpu( g2 );
    for(int i=0;i<aff_comp[1].size();i++)
    {
        up_comp(g2,aff_comp[1][i],cpy2,1,gpu_btw);
    }
    g2->btw = (double *)malloc((g2->N)*sizeof(double));
    cudaMemcpy(g2->btw,gpu_btw,(g2->N)*sizeof(double),cudaMemcpyDeviceToHost);
    cout<<"updated\n";
    for(int i=0;i<g2->N;i++)
    {
        cout<<g2->btw[i]<<" ";   
    }
    cout<<"\n";
    *g=g2;
}

__device__ void  init_vec(struct vec* v)
{
    v->size=0;
    v->max_size=4;
    v->arr = (int*) malloc (4*sizeof(int));
}

__device__ void  push(struct vec *v1,int a)
{
    int s_max = v1->max_size;
    int* arr = v1->arr;
    int s = v1->size;
    if(s<s_max)
    {
        arr[s]=a;
        v1->size++;
    }
    else 
    {
        int* arr2 = (int*) malloc (2*s_max*sizeof(int));
        for(int i=0;i<s;i++)
        {
            arr2[i]=arr[i];
        }
        free(arr);
        arr2[s]=a;
        v1->arr = arr2;
        v1->size++;
        v1->max_size = 2*s_max;
    }
}

