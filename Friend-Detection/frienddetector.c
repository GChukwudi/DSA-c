#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUMBERS 20
#define PHONE_LENGTH 5  // "0781\0"

typedef struct {
    char numbers[MAX_NUMBERS][PHONE_LENGTH];
    int adjacency[MAX_NUMBERS][MAX_NUMBERS];
    int size;
} Graph;

// Initialize graph
void init_graph(Graph* g) {
    g->size = 0;
    memset(g->adjacency, 0, sizeof(g->adjacency));
}

// Find index of a phone number, return -1 if not found
int find_number(Graph* g, const char* number) {
    for (int i = 0; i < g->size; i++) {
        if (strcmp(g->numbers[i], number) == 0) {
            return i;
        }
    }
    return -1;
}

// Add new phone number to graph
int add_number(Graph* g, const char* number) {
    int idx = find_number(g, number);
    if (idx == -1) {
        if (g->size >= MAX_NUMBERS) {
            printf("Error: Maximum number of nodes reached\n");
            return -1;
        }
        strcpy(g->numbers[g->size], number);
        idx = g->size;
        g->size++;
    }
    return idx;
}

// Add connection between two numbers
void add_connection(Graph* g, const char* from, const char* to) {
    int from_idx = add_number(g, from);
    int to_idx = add_number(g, to);
    
    if (from_idx != -1 && to_idx != -1) {
        g->adjacency[from_idx][to_idx] = 1;
        g->adjacency[to_idx][from_idx] = 1;  // Undirected graph
    }
}

// Print all direct contacts of a number
void print_direct_contacts(Graph* g, const char* number) {
    int idx = find_number(g, number);
    if (idx == -1) {
        printf("Number not found in the network\n");
        return;
    }

    printf("Direct contacts of %s:\n", number);
    int found = 0;
    for (int i = 0; i < g->size; i++) {
        if (g->adjacency[idx][i]) {
            printf("- %s\n", g->numbers[i]);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No direct contacts found\n");
    }
}

// Print adjacency matrix
void print_adjacency_matrix(Graph* g) {
    printf("\nAdjacency Matrix:\n");
    printf("    ");
    for (int i = 0; i < g->size; i++) {
        printf("%s ", g->numbers[i]);
    }
    printf("\n");

    for (int i = 0; i < g->size; i++) {
        printf("%s ", g->numbers[i]);
        for (int j = 0; j < g->size; j++) {
            printf("%d     ", g->adjacency[i][j]);
        }
        printf("\n");
    }
}

int main() {
    Graph g;
    init_graph(&g);

    // Add given connections
    add_connection(&g, "0781", "0782");
    add_connection(&g, "0781", "0783");
    add_connection(&g, "0782", "0784");
    add_connection(&g, "0783", "0785");
    add_connection(&g, "0784", "0785");
    add_connection(&g, "0784", "0786");
    add_connection(&g, "0785", "0787");
    add_connection(&g, "0786", "0788");

    char query[PHONE_LENGTH];
    while (1) {
        printf("\nEnter phone number to investigate (or 'quit' to exit): ");
        scanf("%s", query);
        
        if (strcmp(query, "quit") == 0) {
            break;
        }

        print_direct_contacts(&g, query);
        print_adjacency_matrix(&g);
    }

    return 0;
}
