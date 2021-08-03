#include <bits/stdc++.h>
using namespace std;
class edge
{
    public:
    int x;
    int y;
    edge()
    {
        x=0;
        y=0;
    }
    edge(int a,int b)
    {
        x=a;
        y=b;
    }
    bool operator < (const edge& e) const
    {
        if(this->x!=e.x)
        {
            return this->x<e.x;
        }
        if(this->y!=e.y)
        {
            return this->y<e.y;
        }
        return true;
    }
};

bool comp_edges(edge a,edge b);

class graph
{
    public:
    int N;
    int M;
    vector<edge>edgeList;
    vector<vector<edge>>bccs;
    vector<set<int>>bcc_verts;
    map<pair<int,int>,int> edge_map;
    int* dis;
    int nbcc;
    int* offset_arr; // N+1
    int* edge_arr; // M
    int* bcc_arr;  // M
    int* cut_ver;  // N
    int* subg_size; // M
    int* subg_size_vert; // N
    double* btw; // N
    double* cc; // N
    int* edge_rev_arr; // M
    graph()
    {
        N=0;
        M=0;
        nbcc=0;
    }
    graph(int n,int m,vector <edge> list) // sorted list
    {
        N=n;
        M=m;
        edgeList=list;
        nbcc=0;
    }
    ~graph()
    {
        free(offset_arr);
        free(edge_arr);
        free(bcc_arr);
        free(cut_ver);
        free(subg_size);
    }
    void conv_to_al();
    void find_bcc();
    void find_bcc(int u ,int* par,int* vis,int* low,int &time,stack<edge> &s,stack<int> &s2,stack<int> &s3);
    void colour();
    void create_rev();
    void print();
    void find_btw();
    void closeness_cen();
    void do_basic();
    void update_cc(vector<edge> l);
    
};
void update_btw(graph ** g,vector<edge> added,vector<edge> deleted);
vector<vector<int>> affected_comps (graph*g1,graph*g2);
vector<edge> up(vector<edge> edgeList , vector<edge> added ,vector<edge> deleted);

struct cuda_copies
{
    int* edge_arr;
    int* offset_arr;
    int* cut_ver;
    int* subg_size;
    int* bcc_arr;
    int* edge_rev_arr;
};

struct cuda_copies copy_to_gpu(graph* g);

int* _copy( int * cpu_arr ,int n);

void _free (struct cuda_copies c);

void check_vec();
