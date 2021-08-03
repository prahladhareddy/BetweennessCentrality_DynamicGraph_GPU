#include "_graph.h"

int main(int argc,char **argv)
{
    freopen(argv[1], "r", stdin);
	freopen(argv[2], "w", stdout);
    vector<edge>l;
    int n,m;
    int p,q;
    cin>>n;
    cin>>m;
    int m1=0;
    for(int i=0;i<m;i++)
    {
        int x;
        int y;
        cin>>x;
        cin>>y;
        l.push_back(edge(x,y));
        l.push_back(edge(y,x));
        m1++;
    }
    sort(l.begin(),l.end(),comp_edges);
    graph *g = new graph(n,2*m1,l);
    g->do_basic();
    g->find_btw();
    int t;
    cin>>t;
    for(int j=0;j<t;j++)
    {
        vector<edge> added;
        vector<edge> del;
        int k;
        cin>>k;
        for(int i=0;i<k;i++)
        {
            edge e;
            cin>>e.x;
            cin>>e.y;
            added.push_back(e);
            added.push_back(edge(e.y,e.x));
            m1+=1;
        }
        update_btw(&g,added,del);
    }
    return 0;
}
