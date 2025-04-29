#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <omp.h>

using namespace std;

class Graph {
    int V; // number of vertices
    vector<vector<int>> adj; // adjacency list

public:
    Graph(int V) : V(V), adj(V) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // void addEdge(int v, int w) {
    //     adj[v].push_back(w);
    // }


    void parallelBFS(int start);
    void parallelDFS(int start);
};

void Graph::parallelBFS(int start) {
    vector<int> visited(V, 0);
    queue<int> q;

    visited[start] = true;
    q.push(start);

    cout << "Parallel BFS starting from node " << start << ":\n";

    while (!q.empty()) {
        int size = q.size();
        vector<int> nextLevel;

        #pragma omp parallel for
        for (int i = 0; i < size; ++i) {
            int node;
            
            #pragma omp critical
            {
                node = q.front();
                q.pop();
            }

            #pragma omp critical
            {
                cout << node << " ";
            }

            #pragma omp parallel for
            for (int j = 0; j < adj[node].size(); ++j) {
                int neighbor = adj[node][j];

                if (!visited[neighbor]) {
                    bool expected = false;
                    if (__sync_bool_compare_and_swap(&visited[neighbor], expected, true)) {
                        #pragma omp critical
                        nextLevel.push_back(neighbor);
                    }
                }
            }
        }

        for (int n : nextLevel) 
            q.push(n);
    }

    cout << endl;
}

void parallelDFSUtil(int node, vector<vector<int>>& adj, vector<int>& visited) {
    #pragma omp critical
    cout << node << " ";

    visited[node] = true;

    #pragma omp parallel for
    for (int i = 0; i < adj[node].size(); ++i) {
        int neighbor = adj[node][i];
        if (!visited[neighbor]) {
            bool expected = false;
            if (__sync_bool_compare_and_swap(&visited[neighbor], expected, true)) {
                parallelDFSUtil(neighbor, adj, visited);
            }
        }
    }
}

void Graph::parallelDFS(int start) {
    vector<int> visited(V, 0);
    cout << "Parallel DFS starting from node " << start << ":\n";

    // Start DFS in a single thread initially
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            parallelDFSUtil(start, adj, visited);
        }
    }

    cout << endl;
}

int main() {
    Graph g(7);

    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 5);
    g.addEdge(2, 6);

    g.parallelBFS(0);
    g.parallelDFS(0);

    // cout << "Enter the edges (format: vertex1 vertex2):" << endl;
    // for (int i = 0; i < E; ++i) {
    //     int v, w;
    //     cin >> v >> w;
    //     g.addEdge(v, w);
    // }

    return 0;
}


/*
    RUN:
    g++ -fopenmp parallel_graph.cpp -o parallel_graph
    ./parallel_graph
*/