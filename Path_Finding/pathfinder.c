#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_CITIES 20
#define MAX_NAME 50
#define INF INT_MAX

typedef struct {
    char cities[MAX_CITIES][MAX_NAME];
    int graph[MAX_CITIES][MAX_CITIES];
    int size;
} Graph;

void init_graph(Graph* g) {
    g->size = 0;
    for (int i = 0; i < MAX_CITIES; i++) {
        for (int j = 0; j < MAX_CITIES; j++) {
            g->graph[i][j] = INF;
        }
        g->graph[i][i] = 0;
    }
}

int find_city(Graph* g, const char* city) {
    for (int i = 0; i < g->size; i++) {
        if (strcmp(g->cities[i], city) == 0) {
            return i;
        }
    }
    return -1;
}

int add_city(Graph* g, const char* city) {
    int idx = find_city(g, city);
    if (idx == -1) {
        if (g->size >= MAX_CITIES) {
            printf("Error: Maximum number of cities reached\n");
            return -1;
        }
        strcpy(g->cities[g->size], city);
        idx = g->size;
        g->size++;
    }
    return idx;
}

void add_route(Graph* g, const char* from, const char* to, int time) {
    int from_idx = add_city(g, from);
    int to_idx = add_city(g, to);
    
    if (from_idx != -1 && to_idx != -1) {
        g->graph[from_idx][to_idx] = time;
        g->graph[to_idx][from_idx] = time;  // Undirected graph
    }
}

// Find minimum distance vertex
int min_distance(int dist[], int visited[], int size) {
    int min = INF, min_idx = -1;
    
    for (int v = 0; v < size; v++) {
        if (!visited[v] && dist[v] < min) {
            min = dist[v];
            min_idx = v;
        }
    }
    
    return min_idx;
}

// Print path from source to destination
void print_path(int parent[], Graph* g, int src, int dest) {
    int path[MAX_CITIES];
    int path_len = 0;
    int current = dest;
    
    while (current != src) {
        path[path_len++] = current;
        current = parent[current];
    }
    path[path_len++] = src;
    
    printf("Shortest path: ");
    for (int i = path_len - 1; i >= 0; i--) {
        printf("%s", g->cities[path[i]]);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

void dijkstra(Graph* g, const char* start, const char* end) {
    int src = find_city(g, start);
    int dest = find_city(g, end);
    
    if (src == -1 || dest == -1) {
        printf("Error: City not found\n");
        return;
    }
    
    int dist[MAX_CITIES];
    int visited[MAX_CITIES] = {0};
    int parent[MAX_CITIES];
    
    for (int i = 0; i < g->size; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    
    dist[src] = 0;
    
    for (int count = 0; count < g->size - 1; count++) {
        int u = min_distance(dist, visited, g->size);
        if (u == -1) break;
        
        visited[u] = 1;
        
        for (int v = 0; v < g->size; v++) {
            if (!visited[v] && g->graph[u][v] != INF && 
                dist[u] != INF && dist[u] + g->graph[u][v] < dist[v]) {
                dist[v] = dist[u] + g->graph[u][v];
                parent[v] = u;
            }
        }
    }
    
    if (dist[dest] == INF) {
        printf("No path exists between %s and %s\n", start, end);
    } else {
        print_path(parent, g, src, dest);
        printf("Total time: %d minutes\n", dist[dest]);
    }
}

int main() {
    Graph g;
    init_graph(&g);
    
    // Add given routes
    add_route(&g, "Bumbogo", "Nayinzira", 10);
    add_route(&g, "Bumbogo", "Kanombe", 30);
    add_route(&g, "Nayinzira", "Mushimire", 10);
    add_route(&g, "Mushimire", "Airport", 15);
    add_route(&g, "Kanombe", "Airport", 5);
    add_route(&g, "Mushimire", "Kimironko", 3);
    add_route(&g, "Kimironko", "Remera", 6);
    add_route(&g, "Remera", "Airport", 4);
    
    char start[MAX_NAME], end[MAX_NAME];
    
    while (1) {
        printf("\nEnter starting city (or 'quit' to exit): ");
        scanf("%s", start);
        
        if (strcmp(start, "quit") == 0) break;
        
        printf("Enter destination city: ");
        scanf("%s", end);
        
        dijkstra(&g, start, end);
    }
    
    return 0;
}