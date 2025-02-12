#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 50
#define MAX_SUGGESTIONS 5
#define SIMILARITY_THRESHOLD 0.7

// BST node structure
typedef struct Node {
    char word[MAX_WORD_LENGTH];
    struct Node *left, *right;
} Node;

// Function to create a new node
Node* create_node(const char* word) {
    Node* node = (Node*)malloc(sizeof(Node));
    strncpy(node->word, word, MAX_WORD_LENGTH - 1);
    node->word[MAX_WORD_LENGTH - 1] = '\0';
    node->left = node->right = NULL;
    return node;
}

// Function to insert a word into BST
Node* insert(Node* root, const char* word) {
    if (!root) return create_node(word);
    
    int cmp = strcmp(word, root->word);
    if (cmp < 0)
        root->left = insert(root->left, word);
    else if (cmp > 0)
        root->right = insert(root->right, word);
    
    return root;
}

// Function to search for a word in BST
Node* search(Node* root, const char* word) {
    if (!root || strcmp(word, root->word) == 0)
        return root;
    
    if (strcmp(word, root->word) < 0)
        return search(root->left, word);
    return search(root->right, word);
}

// Calculate Levenshtein distance between two strings
int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int matrix[MAX_WORD_LENGTH][MAX_WORD_LENGTH];
    
    for (int i = 0; i <= len1; i++)
        matrix[i][0] = i;
    for (int j = 0; j <= len2; j++)
        matrix[0][j] = j;
    
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            int delete_cost = matrix[i-1][j] + 1;
            int insert_cost = matrix[i][j-1] + 1;
            int substitute_cost = matrix[i-1][j-1] + cost;
            
            matrix[i][j] = delete_cost;
            if (insert_cost < matrix[i][j])
                matrix[i][j] = insert_cost;
            if (substitute_cost < matrix[i][j])
                matrix[i][j] = substitute_cost;
        }
    }
    
    return matrix[len1][len2];
}

// Structure to store similar words
typedef struct {
    char word[MAX_WORD_LENGTH];
    float similarity;
} Suggestion;

// Function to find similar words
void find_similar_words(Node* root, const char* target, Suggestion* suggestions, int* count) {
    if (!root) return;
    
    // Calculate similarity based on Levenshtein distance
    int distance = levenshtein_distance(target, root->word);
    float similarity = 1.0 - ((float)distance / (float)strlen(target));
    
    if (similarity >= SIMILARITY_THRESHOLD) {
        suggestions[*count].similarity = similarity;
        strncpy(suggestions[*count].word, root->word, MAX_WORD_LENGTH - 1);
        suggestions[*count].word[MAX_WORD_LENGTH - 1] = '\0';
        (*count)++;
    }
    
    find_similar_words(root->left, target, suggestions, count);
    find_similar_words(root->right, target, suggestions, count);
}

// Function to load dictionary from file
Node* load_dictionary(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open dictionary file\n");
        return NULL;
    }
    
    Node* root = NULL;
    char word[MAX_WORD_LENGTH];
    
    while (fscanf(file, "%s", word) != EOF) {
        // Convert to lowercase for consistency
        for (int i = 0; word[i]; i++)
            word[i] = tolower(word[i]);
        root = insert(root, word);
    }
    
    fclose(file);
    return root;
}

// Comparison function for sorting suggestions
int compare_suggestions(const void* a, const void* b) {
    return ((Suggestion*)b)->similarity - ((Suggestion*)a)->similarity;
}

// Function to free BST memory
void free_tree(Node* root) {
    if (root) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <dictionary_file>\n", argv[0]);
        return 1;
    }
    
    // Load dictionary
    Node* root = load_dictionary(argv[1]);
    if (!root) return 1;
    
    char word[MAX_WORD_LENGTH];
    printf("Spell Checker (type 'quit' to exit)\n");
    
    while (1) {
        printf("\nEnter word: ");
        scanf("%s", word);
        
        if (strcmp(word, "quit") == 0)
            break;
        
        // Convert to lowercase
        for (int i = 0; word[i]; i++)
            word[i] = tolower(word[i]);
        
        // Search for exact match
        if (search(root, word)) {
            printf("Word is correctly spelled\n");
            continue;
        }
        
        // Find similar words
        Suggestion suggestions[MAX_SUGGESTIONS];
        int count = 0;
        find_similar_words(root, word, suggestions, &count);
        
        if (count > 0) {
            // Sort suggestions by similarity
            qsort(suggestions, count, sizeof(Suggestion), compare_suggestions);
            
            printf("Did you mean:\n");
            for (int i = 0; i < count && i < MAX_SUGGESTIONS; i++) {
                printf("- %s (%.2f%% similar)\n", 
                       suggestions[i].word, 
                       suggestions[i].similarity * 100);
            }
        } else {
            printf("No suggestions found\n");
        }
    }
    
    free_tree(root);
    return 0;
}
