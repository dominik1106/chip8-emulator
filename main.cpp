#include <iostream>
#include <cstdint>
#include <stack>
#include <fstream>
#include <SDL2/SDL.h>
#include <stdio.h>


const int WINDOW_WIDTH = 64;
const int WINDOW_HEIGHT = 32;


uint8_t memory[4096];
int program_counter;
uint16_t index_register;
std::stack<uint16_t> address_stack;
uint8_t sound_timer;
uint8_t delay_timer;
uint8_t registers[16];


int read_file_into_memory(std::string path, uint8_t* mem) {
    std::ifstream fin(path, std::ios::binary | std::ios::in);

    fin.seekg(0,std::ios_base::end);
    std::streampos file_size = fin.tellg();
    fin.seekg(0,std::ios_base::beg);

    fin.read(reinterpret_cast<char*> (mem+512), file_size);

    return file_size;
}


int main() {
    int size = read_file_into_memory("IBMLogo.ch8", memory);

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // Initialize Window
    if(SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) < 0) {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }

    // Display blank, white screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit(); //Quit SDL subsystems

    return 0;
}