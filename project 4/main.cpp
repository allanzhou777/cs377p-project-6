#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>


struct Edge {
    int src;
    int dest;
    int weight;
};


const double DAMPING_FACTOR = 0.85; 
const double CONVERGENCE_THRESHOLD = 1e-4; 
const int MAX_ITERATIONS = 100;  


void write_outdegree_to_file (const std::vector<int>& outdegree, const std::string& output_file, int max_outdegree, std::vector<int>& smaller_outdegree) {
    std::ofstream file(output_file);
    std::vector<int> outdegree_freq;

    
    outdegree_freq.resize(max_outdegree + 1, 0);
    smaller_outdegree.resize(102, 0);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file " << output_file << std::endl;
        return;
    }



    // iterate thru all nodes
    for (int i = 1; i < outdegree.size(); i++) {
        outdegree_freq[outdegree[i]]++;
    }

    // iterate through all nodes
    // int binsize = max_outdegree / 100;
    // for (int i = 1; i < outdegree.size(); i++) {
    //     int smaller_outdegree_index = outdegree[i] / binsize;
    //     smaller_outdegree[smaller_outdegree_index]++;
    // }

    // for (int i = 0; i < 100; i++) {
    //     file << "# nodes with outdegree in bin #" << i << ": " << smaller_outdegree[i] << std::endl;
    // }

    file << "\n\n" << std::endl;

    for (int i = 0; i <= max_outdegree; i++) {
        file << "# nodes with outdegree " << i << ": " << outdegree_freq[i] << std::endl;
    }

    file.close();
    std::cout << "Outdegrees have been written to " << output_file << std::endl;
}

void write_ranks_to_file(const std::vector<double>& ranks, const std::string& output_file) {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file " << output_file << std::endl;
        return;
    }

    for (size_t i = 1; i < ranks.size(); ++i) {  // Start from 1 since nodes are 1-indexed
        file << "Node " << i << ": PageRank = " << ranks[i] << "\n";
    }

    file.close();
    std::cout << "Ranks have been written to " << output_file << std::endl;
}



std::vector<double> page_rank(const std::vector<int>& row_ptr, 
                              const std::vector<int>& col_idx, std::vector<int>& values,
                              int num_nodes, int num_edges, std::vector<int>&out_degree) {
    std::vector<double> pr (num_nodes + 1, 1.0 / num_nodes); 
    pr[0] = 0;
    std::vector<double> new_pr(num_nodes + 1, 0.0);  



    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        std::fill(new_pr.begin(), new_pr.end(), 0.0);
        double dangling_sum = 0.0;


        // push neighbors
        for (int node = 1; node <= num_nodes; node++) {
            if (out_degree[node] > 0) {
                double contribution = pr[node] / out_degree[node]; 
                int edge_idx = row_ptr[node];
                int end_edge_idx = (node == num_nodes) ? num_edges + 1 : row_ptr[node + 1];
                while (edge_idx < end_edge_idx) {
                    int neighbor = col_idx[edge_idx];
                    new_pr[neighbor] += contribution;
                    edge_idx++;
                }
            }
            // dangling node (PR stays same)
            else {
                dangling_sum += pr[node];  
            }
        }



        // damping
        for (int node = 1; node <= num_nodes; node++) {
            new_pr[node] = DAMPING_FACTOR * (new_pr[node] + dangling_sum / num_nodes) + (1.0 - DAMPING_FACTOR) / num_nodes;
        }

        // normalize
        double sum = std::accumulate(new_pr.begin(), new_pr.end(), 0.0);
        for (int node = 1; node <= num_nodes; node++) {
            new_pr[node] /= sum;
        }

        // check for convergence
        bool converged = true;
        for (int node = 1; node <= num_nodes; node++) {
            if (std::abs(new_pr[node] - pr[node]) > CONVERGENCE_THRESHOLD) {
                converged = false;
            }
            pr[node] = new_pr[node];
        }

        if (converged) {
            break;
        }
    }

    return pr;
}


// reads dimacs file - modifies list of edges, numEdges and numNodes
void read_dimacs_file(const std::string& filename, std::vector<Edge>& edges, int& numNodes, int& numEdges) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // comment
        if (line[0] == 'c') {
            continue;
        }
        // header
        else if (line[0] == 'p') {
            std::istringstream iss(line);
            std::string problemType;
            iss >> problemType >> problemType >> numNodes >> numEdges;
        }
        // data
        else if (line[0] == 'a') {
            int src, dest, weight;
            std::istringstream iss(line);
            char a;
            iss >> a >> src >> dest >> weight;
            edges.push_back({ src, dest, weight });
        }
    }

    file.close();
}


void convert_dimacs_file_to_csr_memory(int numNodes, const std::vector<Edge>& edges, std::vector<int>& row_ptr, std::vector<int>& col_ind,
                                       std::vector<int>& values, std::vector<int>& outDegree, int& max_out_degree) {
    outDegree.resize(numNodes + 1, 0);

    max_out_degree = 0;

    // get outDegree of each node
    for (const auto& edge : edges) {
        outDegree[edge.src]++;
    }

    // get the maximum outdegree
    for (int i = 1; i <= numNodes; i++) {
        max_out_degree = std::max(max_out_degree, outDegree[i]);
    }

    std::cout << "max outdegree: " << max_out_degree << std::endl;

    // create the row_ptr
    row_ptr.resize(numNodes + 1, 0);
    for (int i = 1; i <= numNodes; ++i) {
        row_ptr[i] = row_ptr[i - 1] + outDegree[i - 1];
    }

    // create column and edge weight array
    col_ind.resize(edges.size());
    values.resize(edges.size());

    // modify row, col, and edge weight
    std::vector<int> insertionIndex = row_ptr;
    for (const auto& edge : edges) {
        int index = insertionIndex[edge.src]++;
        col_ind[index] = edge.dest;
        values[index] = edge.weight;
    }
}

// print the csr representation to double check
void print_csr_from_memory(const std::vector<int>& row_ptr, const std::vector<int>& col_ind, const std::vector<int>& values) {
    
    std::cout << "\nprinting_csr_from_memory" << std::endl;
    std::cout << "row_ptr: ";
    for (const auto& r : row_ptr) std::cout << r << " ";
    std::cout << std::endl;

    std::cout << "col_ind: ";
    for (const auto& c : col_ind) std::cout << c << " ";
    std::cout << std::endl;

    std::cout << "values: ";
    for (const auto& v : values) std::cout << v << " ";
    std::cout << std::endl;

    std::cout << "\n" << std::endl;
}

void convert_csr_memory_to_dimacs_file(const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<int>& values,
    int num_nodes, int num_edges,
    const std::string& output_file) {

    // open output file
    std::ofstream file(output_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file " << output_file << std::endl;
        return;
    }

    file << "p sp " << num_nodes << " " << num_edges << "\n";

    // two for loops, we iterate through all nodes, and for every edge in a node
    for (int node = 0; node <= num_nodes; ++node) {
        int start = row_ptr[node];   
        int end = row_ptr[node + 1]; 

        // in this case, there is no ending index, so we use numEdges as final index, which considers all edges for the last node
        if (node == num_nodes) {
            end = num_edges;
        }

        for (int edge_idx = start; edge_idx < end; ++edge_idx) {
            int destination = col_idx[edge_idx];
            int weight = values[edge_idx];
            file << "a " << (node) << " " << (destination) << " " << weight << " " <<"\n";
        }
    }

    file.close();
    std::cout << "Graph has been written to " << output_file << " in DIMACS format." << std::endl;
}


int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1; 
    }

    std::string input_file = argv[1];
    std::string csr_to_dimacs_output_file = "csr_to_dimacs_output.dimacs"; 
    std::string ranks_output_file = "ranks_output.txt";
    std::string outdegree_file = "outdegree.txt";

    std::vector<double> ranks;
    std::vector<Edge> edges;
    std::vector<int> outDegree;
    std::vector<int> smaller_outdegree;
    

    int numNodes = 0, numEdges = 0, max_outdegree = 0;

    read_dimacs_file(input_file, edges, numNodes, numEdges);

    // CSR arrays
    std::vector<int> row_ptr, col_ind, values;

    convert_dimacs_file_to_csr_memory(numNodes, edges, row_ptr, col_ind, values, outDegree, max_outdegree);

    // print_csr_from_memory(row_ptr, col_ind, values);

    convert_csr_memory_to_dimacs_file (row_ptr, col_ind, values, numNodes, numEdges, ranks_output_file);

    ranks = page_rank(row_ptr, col_ind, values, numNodes, numEdges, outDegree);

    write_ranks_to_file(ranks, ranks_output_file);


    // Note: the currently set binsize only works for certain files and
    //must be changed to get appropriately sized bins for the three graphs
    write_outdegree_to_file(outDegree, outdegree_file, max_outdegree, smaller_outdegree);



    return 0;
}
