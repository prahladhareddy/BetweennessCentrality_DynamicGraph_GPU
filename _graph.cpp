#include "_graph.h"

bool comp_edges(edge a,edge b)
{
    if (a.x != b.x)
        return a.x < b.x;

    if (a.y != b.y)
        return a.y < b.y;
 
    return true;
}

void graph::conv_to_al()
{
    offset_arr = (int *)malloc((N+1)*sizeof(int));
    edge_arr = (int *)malloc(M*sizeof(int));
    for(int i=0;i<N+1;i++)
    {
        offset_arr[i]=0;
    }
    for(int i=0;i<M;i++)
    {
        offset_arr[edgeList[i].x+1]++;
        edge_arr[i]=edgeList[i].y;
        edge_map[pair<int,int>(edgeList[i].x,edgeList[i].y)]=i;
    }
    for(int i=0;i<N;i++)
    {
        offset_arr[i+1]+=offset_arr[i];
    }
}

void graph::find_bcc(int u ,int* par,int* vis,int* low,int &time,stack<edge> &s,stack<int> &s2,stack<int> &s3)
{
    vis[u]=time+1;
    low[u]=time++;
    int c=0;
    s2.push(u);
    s3.push(u);
    for(int i=offset_arr[u];i<offset_arr[u+1];i++)
    {
        int v = edge_arr[i];

        if(vis[v]==-1)
        {
            c++;
            par[v]=u;

            s.push(edge(u,v));
            find_bcc(v,par,vis,low,time,s,s2,s3);

            subg_size[i]=subg_size_vert[v]+1;
            subg_size_vert[u]+= subg_size[i];
            low[u]=min(low[u],low[v]);

            if((vis[u]==1 && c>1)||(vis[u]>1 && low[v] >= vis[u]))
            {
                cut_ver[u]=1;
                edge e = s.top();
                vector<edge> l;
                set<int>S;
                nbcc++;
                while(e.x!=u || e.y !=v)
                {
                    l.push_back(e);
                    S.insert(e.x);
                    S.insert(e.y);
                    s.pop();
                    e = s.top();
                }
                l.push_back(e);
                S.insert(e.x);
                S.insert(e.y);
                bcc_verts.push_back(S);
                s.pop();
                bccs.push_back(l);
            }
        }
        else if(v!=par[u])
        {
            low[u] = min(low[u],vis[v]);
            if(vis[v]<vis[u])
            {
                s.push(edge(u,v));
            }
        }
        else 
        {
            subg_size[i]=-1;
        }

    }

    if(vis[u]==1)
    {
        while(!s2.empty())
        {
            int t= s2.top();
            s2.pop();
            int ind = -1;
            int sum = 0;
            for(int j = offset_arr[t];j<offset_arr[t+1];j++)
            {
                if(subg_size[j]==-1)
                {
                    ind = j;
                }
                else 
                {
                    sum+=subg_size[j];
                }
            }
            if(ind!=-1)
            {
                subg_size[ind]=subg_size_vert[u]-sum;
            }
        }
    }
}

void graph::find_bcc()
{
    bcc_arr = (int*) malloc(M*sizeof(int));
    cut_ver = (int*) malloc(N*sizeof(int));
    subg_size = (int*) malloc(M*sizeof(int));
    subg_size_vert = (int*) malloc(N*sizeof(int));

    int* vis = (int*) malloc(N*sizeof(int));
    int* par = (int*) malloc(N*sizeof(int));
    int* low = (int*) malloc(N*sizeof(int));
    int x=0;

    stack<edge> s;
    stack<int> s2;
    stack<int> s3;
    for(int i=0;i<N;i++)
    {
        cut_ver[i]=0;
        subg_size_vert[i]=0;
        vis[i]=-1;
        low[i]=0;
    }

    for(int i=0;i<M;i++)
    {
        bcc_arr[i]=0;
        subg_size[i]=0;
    }

    int c=0;
    for(int i=0;i<N;i++)
    {
        if(vis[i]!=-1)
        {
            continue;
        }
        x=0;
        find_bcc(i,par,vis,low,x,s,s2,s3);
        c++;
        if(!s.empty())
        {
            nbcc++;
            set<int> S;
            vector<edge> l;
            while(!s.empty())
            {
                l.push_back(s.top());
                edge e = s.top();
                S.insert(e.x);
                S.insert(e.y);
                s.pop();
            }
            bccs.push_back(l);
            bcc_verts.push_back(S);
        }
    }
    
}

void graph::colour()
{
    for(int i=0;i<bccs.size();i++)
    {
        for(int j=0;j<bccs[i].size();j++)
        {
            bcc_arr[edge_map[pair<int,int>(bccs[i][j].x,bccs[i][j].y)]]=i;
            bcc_arr[edge_map[pair<int,int>(bccs[i][j].y,bccs[i][j].x)]]=i;
        }
    }
}

void graph::create_rev()
{
    edge_rev_arr = (int*) malloc (M*sizeof(int));
    for(int i=0;i<M;i++)
    {
        edge_rev_arr[i]=edge_map[pair<int,int>(edgeList[i].y,edgeList[i].x)];
    }
}

void graph::do_basic()
{
    conv_to_al();
    find_bcc();
    colour();
    create_rev();
}

void graph::print()
{   
    cout<<"EDGE LIST\n";
    for(int i=0;i<edgeList.size();i++)
    {
        cout<<edgeList[i].x<<" "<<edgeList[i].y<<"\n";
    }
    cout<<"AL\n";
    for(int i=0;i<N+1;i++)
    {
        cout<<offset_arr[i]<<" ";
    }
    cout<<"\n";
    for(int i=0;i<M;i++)
    {
        cout<<edge_arr[i]<<" ";
    }
    cout<<"\n";
    cout<<"BCC"<<"\n";
    for(int i=0;i<bccs.size();i++)
    {
        for(int j=0;j<bccs[i].size();j++)
        {
            cout<<bccs[i][j].x<<"--"<<bccs[i][j].y<<" ";
        }
        cout<<"\n";
    }
    cout<<"BCC VER"<<"\n";
    for(int i=0;i<bcc_verts.size();i++)
    {
        for(auto iter=bcc_verts[i].begin();iter != bcc_verts[i].end();iter++)
        {
            cout<<*iter<<" ";
        }
        cout<<"\n";
    }
    cout<<"ATT"<<"\n";
    for(int i=0;i<N;i++)
    {
        cout<<cut_ver[i]<<" ";
    }
    cout<<"\n";
    cout<<"sung"<<"\n";
    for(int i=0;i<M;i++)
    {
        cout<<subg_size[i]<<" ";
    }
    cout<<"\n";
    cout<<"sung"<<"\n";
    for(int i=0;i<N;i++)
    {
        cout<<subg_size_vert[i]<<" ";
    }
    cout<<"\n";
    cout<<"COLOURS\n";
    for(int i=0;i<M;i++)
    {
        cout<<bcc_arr[i]<<" ";
    }
    cout<<"\n";
    cout<<"rev\n";
    for(int i=0;i<M;i++)
    {
        cout<<edge_rev_arr[i]<<" ";
    }
}

vector<vector<int>> affected_comps (graph*g1,graph*g2)
{
    vector<vector<int>>V;
    vector<int>v1;
    vector<int>v2;
    int p1=0;
    int p2=0;

    int c1[g1->nbcc];
    int c2[g2->nbcc];

    for(int i=0;i<g1->nbcc;i++)
    {
        c1[i]=-1;
    }
    for(int i=0;i<g2->nbcc;i++)
    {
        c2[i]=-1;
    }

    int n1=g1->M;
    int n2=g2->M;

    while(p1<n1||p2<n2)
    {
        int x1=-1;
        int x2=-1;
        edge e1,e2;
        
        if(p1<n1)
        {
            x1=g1->bcc_arr[p1];
            e1=g1->edgeList[p1];
        }
        if(p2<n2)
        {
            x2=g2->bcc_arr[p2];
            e2=g2->edgeList[p2];
        }
        
        if(p2==n2)
        {
            if(c1[x1]>=0)
            {
                c2[c1[x1]]=-2;
                c1[x1]=-2;
            }
            else 
            {
                c1[x1]=-2;
            }
            p1++;
            continue;
        }

        if(p1==n1)
        {
            if(c2[x2]>=0)
            {
                c1[c2[x2]]=-2;
                c2[x2]=-2;
            }
            else 
            {
                c2[x2]=-2;
            }
            p2++;
            continue;
        }

        if(comp_edges(e1,e2) && comp_edges(e2,e1))
        { 
            if(c1[x1]==-2)
            {
                if(c2[x2]==-1)
                {
                    c2[x2]==-2;
                }
                else if(c2[x2]>=0)
                {
                    c1[c2[x2]]=-2;
                    c2[x2]=-2;
                }
            }
            else if(c1[x1]==-1)
            {
                if(c2[x2]==-2)
                {
                    c1[x1]=-2;
                }
                else if(c2[x2]==-1)
                {
                    c1[x1]=x2;
                    c2[x2]=x1;
                }
                else if(c2[x2]>=0)
                {
                    c1[c2[x2]]=-2;
                    c2[x2]=-2;
                    c1[x1]=-2;
                }
            }
            else if(c1[x1]>=0)
            {
                if(c2[x2]==-2)
                {
                    c2[c1[x1]]=-2;
                    c1[x1]=-2;
                }
                else if(c2[x2]==-1)
                {
                    c2[x2]=-2;
                    c2[c1[x1]]=-2;
                    c1[x1]=-2;
                }
                else if(c2[x2]!=c1[x1])
                {
                    c2[c1[x1]]=-2;
                    c1[x1]=-2;
                    c1[c2[x2]]=-2;
                    c2[x2]=-2;
                }
            }
            p1++;
            p2++;
        }
        else if(comp_edges(e1,e2))
        {
            if(c1[x1]==-1)
            {
                c1[x1]=-2;
            }
            else if(c1[x1]>=0)
            {
                c2[c1[x1]]=-2;
                c1[x1]=-2;
            }
            p1++;
        }
        else if(comp_edges(e2,e1))
        {
            if(c2[x2]==-1)
            {
                c2[x2]=-2;
            }
            else if(c2[x2]>=0)
            {
                c1[c2[x2]]=-2;
                c2[x2]=-2;
            }
            p2++;
        }
    }

    for(int i=0;i<g1->nbcc;i++)
    {
        if(c1[i]==-2)
        {
            v1.push_back(i);
        }
    }
    for(int i=0;i<g2->nbcc;i++)
    {
        if(c2[i]==-2)
        {
            v2.push_back(i);
        }
    }
    V.push_back(v1);
    V.push_back(v2);
    return V;
}

vector<edge> up(vector<edge> edgeList , vector<edge> added,vector<edge> deleted)
{
    sort(added.begin(),added.end(),comp_edges);
    sort(deleted.begin(),deleted.end(),comp_edges);
    vector<edge> new_list;
    int i1=0;
    int i2=0;
    vector<edge>v;
    while(i1<edgeList.size() && i2<added.size())
    {
        edge e1,e2;
        e1 = edgeList[i1];
        e2 = added[i2];
        if(comp_edges(e1,e2)&&comp_edges(e2,e1))
        {
            v.push_back(added[i2]);
            i1++;
            i2++;
        }
        else if(comp_edges(e1,e2))
        {
            v.push_back(edgeList[i1]);
            i1++;
        }
        else 
        {
            v.push_back(added[i2]);
            i2++;
        }
    }
    while(i1<edgeList.size())
    {
        v.push_back(edgeList[i1]);
        i1++;
    }
    while(i2<added.size())
    {
        v.push_back(added[i2]);
        i2++;
    }
    i1=0;
    i2=0;
    while(i1<v.size() && i2<deleted.size())
    {
        edge e1,e2;
        e1 = v[i1];
        e2 = deleted[i2];
        if(comp_edges(e1,e2)&&comp_edges(e2,e1))
        {
            i1++;
            i2++;
        }
        else if(comp_edges(e1,e2))
        {
            new_list.push_back(e1);
            i1++;
        }
        else 
        {
            i2++;
        }
    }
    while(i1<v.size())
    {
        new_list.push_back(v[i1]);
        i1++;
    }
    return new_list;
}
