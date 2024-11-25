# CS377P Assignment 6: Bellman-Ford Algorithm

This project runs Bellman-Ford's algorithm on the **road-NY** and **rmat15** graphs, with both sequential and parallel versions.  

There are 5 versions of the code you can try out, ranging from the basic sequential implementation to parallel ones that use locks and atomic operations.

### Versions of Bellman-Ford

1. **bf-0**: Sequential Bellman-Ford  
2. **bf-1**: Parallel Bellman-Ford with one lock for the whole graph  
3. **bf-2**: Parallel Bellman-Ford with a lock for each node  
4. **bf-3**: Parallel Bellman-Ford with a spin-lock for each node  
5. **bf-4**: Parallel Bellman-Ford using compare-and-swap (CAS)

### Graphs

You can run the algorithms on these graphs:  
- **road-NY**  
- **rmat15**

### How to Run

1. **Compile the code**  
   Run `make` to build all 5 versions of Bellman-Ford.

2. **Run a specific version on a graph**  
   Use `make run-specific VERSION=<version_number> GRAPH=<graph_name>`.  
   Example:  
   ```bash
   make run-specific VERSION=4 GRAPH=road-NY
   ```  
   This runs `bf-4` on the `road-NY` graph with 1, 2, 4, 8, and 16 threads. The output will be saved to `temp.txt`.

### Output Files

For convenience, I've included previous outputs for all 5 versions in the `outputs/` directory.  
