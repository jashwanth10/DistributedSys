# Quick Sort:

## Implementation:
    - A Pivot is selected.
    - First Partition is done in root process.
    - Elements smaller than pivot is sent to process 1 and greater to process 2.
    - Similarly partitions is done in the subsequent processes where process (i) sends its partitions to process(2*i + 1) and process (2*i + 2).
    - This leads to a formation of tree.
    - After all processes are exhausted, if elements are still remaining, normal sequential sort is done.
    - Every child node sends its sorted array to its parent node and so on... and finally root node collects sorted arrays from process 1 and process 2.

## Complexity:
    - O((n/(p+2)) * log(n/(p+2)))

## How to Run:
    - mpic++ -o quick quick_sort.cpp
    - mpirun -np #(no.of processes) --oversubscribe quick
    - output is written to the file "output_qsort.txt"

## Note:
    -Atleast three processes should be run.


# Single Source Shortest Path:

## Implementation:
    - Bellman Ford Algorithm is parallelized.
    - Initialize a distance array with [0 INF INF INF ....]
    - V(# of vertices) iteration passes are done on this array.
    - In each iteration process (i) processes(relaxes) d[i*(V/numprocs)] to d[i*(V/numprocs) + (V/numprocs)].
    - Then each process sends that portion of distance array to the root.
    - Root updates its distance array from all the values that has been received from processes and broadcasts the whole array to all processes.
    - This is done V(# of vertices) times. 

## Complexity:
    - O(E*V/p)

## How to Run:
    - mpic++ -o short SSSp.cpp
    - mpirun -np #(no.of processes) --oversubscribe short
    - output is written to the file "output_SSSP.txt"

