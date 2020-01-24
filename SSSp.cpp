#include<bits/stdc++.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"

using namespace std;
#define ll long long int
#define MAX 100009
#define INF 1e+8

ll vis[MAX];

int main( int argc, char **argv ) {
    int rank, numprocs;
    ll n, e;
    map<pair<ll, ll>, ll> m;
    vector<int> g[MAX];
    vector<int> dist[MAX];
    int d[MAX];
    ll source;

    MPI_Status status;
    MPI_Request request;

    /* start up MPI */
    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    
    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    if(rank == 0){
        ifstream file("input2.txt");
        ll d;
        int ct = 0;
        while(file >> d){
            if(ct==0)n=d;
            if(ct==1)e=d;
            if(ct==e+2 && ct > 2){
                cout<<"jas"<<ct<<endl;
                source = d;
                break;
            }
            if(ct>1){
                ll x, y, w;
                x = d;
                file >> y >> w;               
                g[x].push_back(y);
                g[y].push_back(x);
                dist[x].push_back(w);
                dist[y].push_back(w);
            }
            ct++;
        } 
        file.close();
    }
    
    double tbeg = MPI_Wtime();
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&e, 1, MPI_INT, 0, MPI_COMM_WORLD);
    numprocs = numprocs-1;
    if(rank == 0){
        for(int i=1;i<=n;i++){
            d[i] = INF;
        }
        d[source] = 0;
        for(int i=0;i<numprocs-1;i++){
            for(int j=1;j<=n/numprocs;j++){
                ll ct = g[i*(n/numprocs) + j].size();
                MPI_Send(&ct, 1, MPI_INT, i+1, 1e+8 + ((1e+8)/2)*i - j, MPI_COMM_WORLD);
                MPI_Send(g[i*(n/numprocs) + j].data(), g[i*(n/numprocs) + j].size(), MPI_INT, i+1, 10 + j, MPI_COMM_WORLD);
                MPI_Send(dist[i*(n/numprocs) + j].data(), dist[i*(n/numprocs) + j].size(), MPI_INT, i+1, 11 + j, MPI_COMM_WORLD);

            }
        }
        for(int j=1;j<=n/numprocs + n%numprocs;j++){
            ll ct = g[(numprocs-1)*(n/numprocs) + j].size();
            MPI_Send(&ct, 1, MPI_INT, numprocs, 1e+8 + ((1e+8)/2)*(numprocs) - j, MPI_COMM_WORLD);
            MPI_Send(g[(numprocs-1)*(n/numprocs) + j].data(), g[(numprocs-1)*(n/numprocs) + j].size(), MPI_INT, numprocs, 10 + j, MPI_COMM_WORLD);     
            MPI_Send(dist[(numprocs-1)*(n/numprocs) + j].data(), dist[(numprocs-1)*(n/numprocs) + j].size(), MPI_INT, numprocs, 11 + j, MPI_COMM_WORLD);
        }
        for(int k=0;k<n;k++){
            for(int i=0;i<numprocs;i++){
                MPI_Send(&d[1], n, MPI_INT, i+1, 420 + i+1, MPI_COMM_WORLD);
            }
            ////
            for(int i=0;i<numprocs-1;i++){
                MPI_Recv(&d[i*(n/numprocs) + 1], (n/numprocs), MPI_INT, i+1, 1024 + i+1, MPI_COMM_WORLD, &status);
            }
            MPI_Recv(&d[(numprocs-1)*(n/numprocs) + 1], (n/numprocs) + n%numprocs, MPI_INT, numprocs, 1024 + numprocs, MPI_COMM_WORLD, &status);
        }
        ofstream fout;
        fout.open("output_SSSP.txt");
        for(int i=1;i<=n;i++){
            fout << i;
            fout << " ";
            fout<< d[i];
            fout<<endl;
        }
        fout.close();
        // for(int i=1;i<=n;i++){
        //     cout<<d[i]<<" ";
        // }cout<<endl;
        
    }else{
        
        ll ct = 1;
        if(rank != numprocs){
            for(int i=1;i<=n/numprocs;i++){
                int ind = (rank-1)*(n/numprocs) + i;
                MPI_Recv(&ct, 1, MPI_INT, 0, 1e+8 + ((1e+8)/2)*(rank-1) - i,MPI_COMM_WORLD, &status);
                g[ind].resize(ct);
                dist[ind].resize(ct);
                MPI_Recv(g[ind].data(), ct, MPI_LONG_LONG_INT, 0, 10 + i, MPI_COMM_WORLD, &status);
                MPI_Recv(dist[ind].data(), ct, MPI_LONG_LONG_INT, 0, 11 + i, MPI_COMM_WORLD, &status);
            }

        }
        else{
            for(int i=1;i<=n/numprocs + n%numprocs;i++){
                int ind = (rank-1)*(n/numprocs) + i;
                MPI_Recv(&ct, 1, MPI_INT, 0, 1e+8 + ((1e+8)/2)*(rank) - i,MPI_COMM_WORLD, &status);
                g[ind].resize(ct);
                dist[ind].resize(ct);
                MPI_Recv(g[ind].data(), ct, MPI_INT, 0, 10+i, MPI_COMM_WORLD, &status);
                MPI_Recv(dist[ind].data(), ct, MPI_LONG_LONG_INT, 0, 11 + i, MPI_COMM_WORLD, &status);
            }
        }

        if(rank != numprocs){
            for(int k=0;k<n;k++){
                MPI_Recv(&d[1], n, MPI_INT, 0, 420 + rank, MPI_COMM_WORLD, &status);
                for(int i=1;i<=n/numprocs;i++){
                    int ind = (rank-1)*(n/numprocs) + i;
                    for(int j=0;j<g[ind].size();j++){
                        d[ind] = min(d[ind], d[g[ind][j]] + dist[ind][j]);
                    }
                }
                MPI_Send(&d[(rank-1)*(n/numprocs) + 1], (n/numprocs), MPI_INT, 0, 1024 + rank, MPI_COMM_WORLD);
            }
        }else{
            for(int k=0;k<n;k++){
                MPI_Recv(&d[1], n, MPI_INT, 0, 420 + rank, MPI_COMM_WORLD, &status);
                for(int i=1;i<=n/numprocs + n%numprocs;i++){
                    int ind = (rank-1)*(n/numprocs) + i;
                    for(int j=0;j<g[ind].size();j++){
                        d[ind] = min(d[ind], d[g[ind][j]] + dist[ind][j]);
                    }
                }
                MPI_Send(&d[(rank-1)*(n/numprocs) + 1], (n/numprocs) + n%numprocs, MPI_INT, 0, 1024 + rank, MPI_COMM_WORLD);

            }
        }
    }
    /* write your code here */
    MPI_Barrier( MPI_COMM_WORLD );
    double elapsedTime = MPI_Wtime() - tbeg;
    double maxTime;
    MPI_Reduce( &elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    if ( rank == 0 ) {
        printf( "Total time (s): %f\n", maxTime );
    }

    /* shut down MPI */
    MPI_Finalize();
    return 0;
}