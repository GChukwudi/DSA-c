#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define MAX_PATH 512
#define AUDIO_FREQUENCY 44100
#define AUDIO_CHANNELS 2
#define AUDIO_CHUNK_SIZE 2048

// Node structure for doubly linked list
typedef struct Node {
    char filepath[MAX_PATH];
    struct Node* prev;
    struct Node* next;
} Node;

// Global variables for audio state
Mix_Music* current_music = NULL;
int is_playing = 0;
int is_paused = 0;

// Function to create a new node
Node* create_node(const char* filepath) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node) {
        strncpy(new_node->filepath, filepath, MAX_PATH - 1);
        new_node->filepath[MAX_PATH - 1] = '\0';
        new_node->prev = NULL;
        new_node->next = NULL;
    }
    return new_node;
}

// Function to add a node to the end of the list
Node* append_node(Node* head, const char* filepath) {
    Node* new_node = create_node(filepath);
    if (!new_node) return head;

    if (!head) return new_node;

    Node* current = head;
    while (current->next) {
        current = current->next;
    }
    current->next = new_node;
    new_node->prev = current;
    return head;
}

// Function to load songs from directory
Node* load_songs(const char* directory) {
    DIR* dir;
    struct dirent* entry;
    Node* head = NULL;
    char filepath[MAX_PATH];

    dir = opendir(directory);
    if (!dir) {
        printf("Error: Could not open directory %s\n", directory);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".mp3")) {
            snprintf(filepath, MAX_PATH, "%s/%s", directory, entry->d_name);
            head = append_node(head, filepath);
        }
    }

    closedir(dir);
    return head;
}

// Function to initialize SDL and SDL_mixer
int init_audio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 0;
    }

    if (Mix_OpenAudio(AUDIO_FREQUENCY, MIX_DEFAULT_FORMAT, AUDIO_CHANNELS, AUDIO_CHUNK_SIZE) < 0) {
        printf("SDL_mixer initialization failed: %s\n", Mix_GetError());
        return 0;
    }

    return 1;
}

// Function to play a song
void play_song(const char* filepath) {
    if (current_music) {
        Mix_FreeMusic(current_music);
    }

    current_music = Mix_LoadMUS(filepath);
    if (!current_music) {
        printf("Error loading music: %s\n", Mix_GetError());
        return;
    }

    if (Mix_PlayMusic(current_music, 1) == -1) {
        printf("Error playing music: %s\n", Mix_GetError());
        return;
    }

    is_playing = 1;
    is_paused = 0;
    printf("Now playing: %s\n", filepath);
}

// Function to handle user commands
void handle_command(char cmd, Node** current) {
    switch (cmd) {
        case 'n':
            if ((*current)->next) {
                *current = (*current)->next;
                play_song((*current)->filepath);
            } else {
                printf("End of playlist reached\n");
            }
            break;

        case 'p':
            if ((*current)->prev) {
                *current = (*current)->prev;
                play_song((*current)->filepath);
            } else {
                printf("Start of playlist reached\n");
            }
            break;

        case ' ':  // Space for pause/resume
            if (is_playing && !is_paused) {
                Mix_PauseMusic();
                is_paused = 1;
                printf("Music paused\n");
            } else if (is_paused) {
                Mix_ResumeMusic();
                is_paused = 0;
                printf("Music resumed\n");
            }
            break;

        case 's':  // Stop
            Mix_HaltMusic();
            is_playing = 0;
            is_paused = 0;
            printf("Music stopped\n");
            break;

        case 'q':  // Quit
            break;

        default:
            printf("Invalid command\n");
    }
}

// Function to cleanup resources
void cleanup(Node* head) {
    while (head) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }

    if (current_music) {
        Mix_FreeMusic(current_music);
    }
    Mix_CloseAudio();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <music_directory>\n", argv[0]);
        return 1;
    }

    // Initialize audio
    if (!init_audio()) {
        return 1;
    }

    // Load songs into doubly linked list
    Node* head = load_songs(argv[1]);
    if (!head) {
        printf("No MP3 files found in directory\n");
        cleanup(head);
        return 1;
    }

    // Start with first song
    Node* current = head;
    play_song(current->filepath);

    // Main control loop
    printf("\nControls:\n");
    printf("n - Next song\n");
    printf("p - Previous song\n");
    printf("Space - Pause/Resume\n");
    printf("s - Stop\n");
    printf("q - Quit\n");

    char cmd;
    while (1) {
        cmd = getchar();
        if (cmd == 'q') break;
        if (cmd == '\n') continue;
        handle_command(cmd, &current);
    }

    // Cleanup
    cleanup(head);
    return 0;
}
