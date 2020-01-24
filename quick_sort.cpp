#include <stdio.h>
#include <string.h>
#include<bits/stdc++.h>
#include "mpi.h"
#define MAXNUM 10007

using namespace std;
int quick_sort(int arr[], int start, int end){
    int piv = start;
    for(int i=1;i<end-start+1;i++){
        if(arr[i]<=arr[piv]){
            int t = arr[piv];
            arr[piv] = arr[piv+1];
            arr[piv+1] = t;
            int temp = arr[i];
            arr[i] = arr[piv];
            arr[piv] = temp;
            piv++;
        }
    }
    return piv;
}

int main( int argc, char **argv ) {
    int rank, numprocs, arr[100007], ct;
    MPI_Status status;
    MPI_Request request;

    /* start up MPI */
    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    
    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    if(rank == 0){
        ifstream file("input.txt");
        int d;
        while(file >> d){
            arr[ct++] = d; 
        }cout<<endl;   
    }
    double tbeg = MPI_Wtime();
    if(rank == 0){
        int pivot = quick_sort(arr, 0, ct-1);
        int rem  = ct - pivot-1;

        //Send first half to one process
        MPI_Isend(&pivot, 1, MPI_INT, 1, 10, MPI_COMM_WORLD, &request);
        MPI_Isend(arr, pivot, MPI_INT, 1, 11, MPI_COMM_WORLD, &request);

        //Send other half to another process
        MPI_Isend(&rem, 1, MPI_INT, 2, 10, MPI_COMM_WORLD, &request);
        MPI_Isend(&arr[pivot+1], ct - pivot-1, MPI_INT, 2, 11, MPI_COMM_WORLD, &request);
        
        cout<<"Rank "<<pivot<<endl;

        //Recieve sorted array from first process
        MPI_Recv(arr, ct, MPI_INT, 1, 201, MPI_COMM_WORLD, &status);
        
        //Recieve sorted array from second proccess
        MPI_Recv(&arr[pivot+1], ct - pivot-1, MPI_INT, 2, 201, MPI_COMM_WORLD, &status);

        ofstream fout;
        fout.open("output_qsort.txt");
        for(int i=0;i<ct;i++){
            fout << arr[i];
            fout << " ";
        }
        fout.close();
        // for(int i=0;i<ct;i++){
        //     cout<<arr[i]<<" ";
        // }cout<<endl;

    }else{        
        //Recieve the array to perform partition
        MPI_Recv(&ct, 1, MPI_INT, (rank-1)/2, 10, MPI_COMM_WORLD, &status);
        MPI_Recv(arr, ct, MPI_INT, (rank-1)/2, 11, MPI_COMM_WORLD, &status);

        if(rank*2 + 1 < numprocs){
            int piv = quick_sort(arr, 0, ct-1);
            int rem = ct - piv - 1;
            //send half of it to its child proc 1
            MPI_Isend(&piv, 1, MPI_INT, rank*2 + 1, 10, MPI_COMM_WORLD, &request);
            MPI_Isend(arr, piv, MPI_INT, rank*2 + 1, 11, MPI_COMM_WORLD, &request);

            //Receive elements from first process

            if(rank*2 + 2 < numprocs){
                //send half of it to its child proc 2
                MPI_Isend(&rem, 1, MPI_INT, rank*2 + 2, 10, MPI_COMM_WORLD, &request);
                MPI_Isend(&arr[piv+1], ct - piv-1, MPI_INT, rank*2 + 2, 11, MPI_COMM_WORLD, &request);

            }else{
                sort(arr+ piv+1,arr+ ct);
            }
            //recieve sorted 1st half elems 
            MPI_Recv(arr, piv, MPI_INT, rank*2+1, 201, MPI_COMM_WORLD, &status);
                        
            //receive sorted second half elems
            if(rank*2 + 2 < numprocs){
                MPI_Recv(&arr[piv+1], ct - piv-1, MPI_INT, rank*2 + 2, 201, MPI_COMM_WORLD, &status);
            }
        }else{
            sort(arr, arr+ct-1);
        }
        MPI_Isend(arr, ct, MPI_INT, (rank-1)/2, 201, MPI_COMM_WORLD, &request);
        
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