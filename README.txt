# cs377p assignment 6

This is cs377p assignment 6, which performs Bellman-Ford's algorithm on the road-NY and rmat15 graphs in sequential and parallelized implementations. 

There are 5 versions of the code you can run (labelled bf-0, bf-1, ..., bf-4). 

bf-0: sequential Bellman-Ford
bf-1: Parallel Bellman-Ford with a lock on the entire graphs
bf-2: Parallel Bellman-Ford with a lock on each node
bf-3: Parallel Bellman-Ford with a spin-lock on each node
bf-4: Parallel Bellman-Ford with a compare-and-swap operation

Additionally, there are two graphs, which are the road-NY and rmat15 graphs

Below are the steps to run a specific version of BF on a specific graph. 

1. Run 'make' to compile all 5 versions of BF. 

2. Choose a version and a graph to run on. run `make run-specific VERSION=4 GRAPH=road-NY` to run version 4 of BF on road-NY using 1, 2, 4, 8, and 16 threads. The output will be outputted to a temp.txt file. 

For convenience, previous outputs for all 5 versions are located in the outputs directory. 



