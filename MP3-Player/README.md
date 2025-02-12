README
# MP3 Player with Doubly Linked List

A command-line MP3 player that uses a doubly linked list to manage and play songs.

## Requirements

- GCC compiler
- SDL2 and SDL2_mixer libraries

## Installation (Ubuntu/Debian)

```
sudo apt-get install libsdl2-dev libsdl2-mixer-dev
```
## Compilation

```gcc -o mp3player mp3player.c -lSDL2 -lSDL2_mixer```

## Usage
```./mp3player test_music/ ```
NOTE: Populate <test_music> with your mp3 files

## Controls
    n: Next song
    p: Previous song
     : Pause/Resume (spacebar)
    s: Stop
    q: Quit

## The implementation includes:
1. Doubly linked list for song management
2. Audio playback using SDL2_mixer
3. File handling for MP3 files
4. User input handling
5. Resource cleanup