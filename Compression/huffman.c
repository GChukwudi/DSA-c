#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100
#define MAX_CHARS 256

struct MinHeapNode {
    unsigned char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

struct MinHeapNode* newNode(unsigned char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isSizeOne(struct MinHeap* minHeap) {
    return (minHeap->size == 1);
}

struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    int i;
    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

int isLeaf(struct MinHeapNode* root) {
    return !(root->left) && !(root->right);
}

struct MinHeap* createAndBuildMinHeap(unsigned char data[], int freq[], int size) {
    struct MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
    minHeap->size = size;
    buildMinHeap(minHeap);
    return minHeap;
}

struct MinHeapNode* buildHuffmanTree(unsigned char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

void generateCodes(struct MinHeapNode* root, int arr[], int top, char* huffmanCodes[]) {
    if (root->left) {
        arr[top] = 0;
        generateCodes(root->left, arr, top + 1, huffmanCodes);
    }
    if (root->right) {
        arr[top] = 1;
        generateCodes(root->right, arr, top + 1, huffmanCodes);
    }
    if (isLeaf(root)) {
        huffmanCodes[root->data] = (char*)malloc((top + 1) * sizeof(char));
        for (int i = 0; i < top; ++i)
            huffmanCodes[root->data][i] = arr[i] + '0';
        huffmanCodes[root->data][top] = '\0';
    }
}

void compressFile(const char* inputFile, const char* outputFile) {
    FILE *inFile = fopen(inputFile, "rb");
    if (inFile == NULL) {
        printf("Error opening input file.\n");
        return;
    }

    int freq[MAX_CHARS] = {0};
    unsigned char ch;
    while (fread(&ch, 1, 1, inFile) == 1) {
        freq[ch]++;
    }

    unsigned char data[MAX_CHARS];
    int size = 0;
    for (int i = 0; i < MAX_CHARS; i++) {
        if (freq[i] > 0) {
            data[size++] = (unsigned char)i;
        }
    }

    struct MinHeapNode* root = buildHuffmanTree(data, freq, size);

    char* huffmanCodes[MAX_CHARS] = {NULL};
    int arr[MAX_TREE_HT], top = 0;
    generateCodes(root, arr, top, huffmanCodes);

    FILE *outFile = fopen(outputFile, "wb");
    if (outFile == NULL) {
        printf("Error opening output file.\n");
        fclose(inFile);
        return;
    }

    // Write frequency table
    fwrite(freq, sizeof(int), MAX_CHARS, outFile);

    // Compress and write data
    fseek(inFile, 0, SEEK_SET);
    unsigned char buffer = 0;
    int bitCount = 0;
    while (fread(&ch, 1, 1, inFile) == 1) {
        char* code = huffmanCodes[ch];
        for (int i = 0; code[i]; i++) {
            buffer = (buffer << 1) | (code[i] - '0');
            bitCount++;
            if (bitCount == 8) {
                fwrite(&buffer, 1, 1, outFile);
                buffer = 0;
                bitCount = 0;
            }
        }
    }
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        fwrite(&buffer, 1, 1, outFile);
    }

    fclose(inFile);
    fclose(outFile);

    // Free allocated memory
    for (int i = 0; i < MAX_CHARS; i++) {
        if (huffmanCodes[i] != NULL) {
            free(huffmanCodes[i]);
        }
    }
}

void decompressFile(const char* inputFile, const char* outputFile) {
    FILE *inFile = fopen(inputFile, "rb");
    if (inFile == NULL) {
        printf("Error opening input file.\n");
        return;
    }

    FILE *outFile = fopen(outputFile, "wb");
    if (outFile == NULL) {
        printf("Error opening output file.\n");
        fclose(inFile);
        return;
    }

    int freq[MAX_CHARS];
    fread(freq, sizeof(int), MAX_CHARS, inFile);

    unsigned char data[MAX_CHARS];
    int size = 0;
    for (int i = 0; i < MAX_CHARS; i++) {
        if (freq[i] > 0) {
            data[size++] = (unsigned char)i;
        }
    }

    struct MinHeapNode* root = buildHuffmanTree(data, freq, size);
    struct MinHeapNode* current = root;

    unsigned char ch;
    unsigned char bit;
    while (fread(&ch, 1, 1, inFile) == 1) {
        for (int i = 7; i >= 0; i--) {
            bit = (ch >> i) & 1;
            if (bit == 0) {
                current = current->left;
            } else {
                current = current->right;
            }

            if (isLeaf(current)) {
                fwrite(&current->data, 1, 1, outFile);
                current = root;
            }
        }
    }

    fclose(inFile);
    fclose(outFile);
}

long getFileSize(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) return -1;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

void printFileSizes(const char* originalFile, const char* compressedFile) {
    long originalSize = getFileSize(originalFile);
    long compressedSize = getFileSize(compressedFile);
    printf("Original file size: %ld bytes\n", originalSize);
    printf("Compressed file size: %ld bytes\n", compressedSize);
    printf("Compression ratio: %.2f%%\n", (float)(compressedSize * 100) / originalSize);
}

int main() {
    int choice;
    char inputFile[100];

    while (1) {
        printf("\nHuffman Coding\n");
        printf("1. Compress file\n");
        printf("2. Decompress file\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter the file to compress: ");
                scanf("%s", inputFile);
                compressFile(inputFile, "compressed.txt");
                printFileSizes(inputFile, "compressed.txt");
                break;
            case 2:
                decompressFile("compressed.txt", "decompressed.txt");
                printf("File decompressed successfully.\n");
                break;
            case 3:
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
