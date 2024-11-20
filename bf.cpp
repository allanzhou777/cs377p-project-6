#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <pthread.h>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>

using namespace std;

struct Edge
{
  int src;
  int dest;
  int weight;
};

struct ThreadData
{
  int thread_id;
  int num_threads;

};


struct Node
{
  int tbd;
};

#define EXIT_FAILURE 1
std::mutex mtx_global;
int numNodes = 0;
int numEdges = 0;
vector<vector<Edge>> all_edges; // keep in mind, 1 indexed, so all_edges[0] is null
vector<int> distances;
bool DEBUG_ON = true;
string input_file;
bool made_change = false;
std::atomic<bool> made_change_atomic;

void read_dimacs_file(const string &filename)
{
  ifstream file(filename);

  if (!file.is_open())
  {
    cerr << "Error opening file: " << filename << endl;
    return;
  }

  string line;
  while (getline(file, line))
  {
    // comment
    if (line[0] == 'c')
    {
      continue;
    }
    // header
    else if (line[0] == 'p')
    {
      istringstream iss(line);
      string problemType;
      iss >> problemType >> problemType >> numNodes >> numEdges;

      if (DEBUG_ON)
        cout << "numNodes: " << numNodes << endl;
      if (DEBUG_ON)
        cout << "numEdges: " << numEdges << endl;

      all_edges.resize(numNodes + 1);

    }
    // data
    else if (line[0] == 'a')
    {
      int src, dest, weight;
      char a;

      istringstream iss(line);
      iss >> a >> src >> dest >> weight;

      all_edges[src].push_back({src, dest, weight});

      all_edges[dest].push_back({dest, src, weight}); // if bidirectional
    }
  }

  file.close();
}

void print_some_edges()
{
  for (int i = 1; i < all_edges.size() / 1000; i++)
  {
    cout << "source " << i << ": ";
    for (int j = 0; j < all_edges[i].size(); j++)
    {
      cout << " " << all_edges[i][j].weight;
    }
    cout << endl;
  }
}

void seq_bf()
{
  

  auto clock_start = chrono::high_resolution_clock::now();
  for (int iter = 1; iter <= numNodes; iter++)
  {
    made_change = false;
    for (int u = 1; u <= numNodes; u++)
    {
      for (const Edge &edge : all_edges[u])
      {
        int v = edge.dest;
        int weight = edge.weight;
        if (distances[u] != INT32_MAX && distances[u] + weight < distances[v])
        {
          distances[v] = distances[u] + weight;
          made_change = true;
        }
      }
    }
    if (!made_change)
    {
      break;
    }
  }
  auto clock_end = std::chrono::high_resolution_clock::now();
  chrono::duration<double> duration = clock_end - clock_start;
  cout << "Elapsed time: " << duration.count() << " seconds" << endl;

  if (DEBUG_ON)
  {
    cout << "----------Final shortest paths for each node----------" << endl;
    for (int i = 1; i < numNodes; i++)
    {
      cout << "node " << i << ": " << distances[i] << endl;
    }
  }
}





void *parallel_bf_global_helper (void *arg) {

  struct ThreadData *data = (struct ThreadData *)arg;
  int thread_id = data->thread_id;
  int num_threads = data->num_threads;

  // print_some_edges();
  
  
  for (int u = thread_id; u <= numNodes; u += num_threads)
  {
    for (const Edge &edge : all_edges[u])
    {
      int v = edge.dest;
      int weight = edge.weight;
      if (distances[u] != INT32_MAX && distances[u] + weight < distances[v])
      {


        mtx_global.lock();
        if (distances[u] != INT32_MAX && distances[u] + weight < distances[v]) {
          distances[v] = distances[u] + weight;
          made_change_atomic = true;
        }
        mtx_global.unlock();


      }
    }
  }
  return NULL;
}

void parallel_bf_global(int num_threads)
{
  

  pthread_t threads[num_threads];            // array of thread identifiers
  struct ThreadData threadData[num_threads]; // array to hold thread data
  distances.resize(numNodes + 1, INT32_MAX);
    // initialize starting node to have distance 0
      if (input_file == "./dimacs/wiki.dimacs")
      {
        distances[1] = 0;
      }
      else if (input_file == "./dimacs/rmat15.dimacs")
      {
        distances[1] = 0;
      }
      else if (input_file == "./dimacs/road-NY.dimacs")
      {
        distances[140961] = 0;
      }

  
  auto clock_start = chrono::high_resolution_clock::now();
  for (int iter = 1; iter <= numNodes; iter++)
  {

    int rc;
    made_change_atomic = false;

    for (int i = 1; i <= num_threads; i++)
    {
      threadData[i].thread_id = i;
      threadData[i].num_threads = num_threads;

      if ((rc = pthread_create(&threads[i], NULL, parallel_bf_global_helper, &threadData[i])))
      {
        fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
        return;
      }
    }




    for (int i = 1; i <= num_threads; i++)
    {
      pthread_join(threads[i], NULL);
    }

    if (!made_change_atomic) {
      break;
    }




  }
  auto clock_end = std::chrono::high_resolution_clock::now();
  chrono::duration<double> duration = clock_end - clock_start;
  cout << "Elapsed time: " << duration.count() << " seconds" << endl;





  if (DEBUG_ON)
  {
    cout << "----------Final shortest paths for each node----------" << endl;
    for (int i = 1; i < numNodes; i++)
    {
      cout << "node " << i << ": " << distances[i] << endl;
    }
  }
}


int main(int argc, char *argv[])
{

  if (argc < 2)
  {
    cerr << "Usage: " << argv[0] << " <input_file>" << endl;
    return 1;
  }

  input_file = argv[1];

  read_dimacs_file(input_file);

  // print_some_edges();

  // seq_bf();

  vector<int> thread_counts = {1, 2, 4, 8, 16};
  for (int num_threads : thread_counts)
  {
    cout << "\nRunning with " << num_threads << " threads..." << endl;

    distances.assign(numNodes + 1, INT32_MAX); // Reset distances
    if (input_file == "./dimacs/wiki.dimacs")
    {
      distances[1] = 0;
    }
    else if (input_file == "./dimacs/rmat15.dimacs")
    {
      distances[1] = 0;
    }
    else if (input_file == "./dimacs/road-NY.dimacs")
    {
      distances[140961] = 0;
    }

    parallel_bf_global(num_threads);
  }

  return 0;
}