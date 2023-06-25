#include <iostream>
#include <cstdint>
#include <stack>
#include <fstream>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <bitset>


const int WINDOW_WIDTH = 64;
const int WINDOW_HEIGHT = 32;
const int SCALE = 8;


uint8_t ram[4096];
int program_counter;
uint16_t index_register;
std::stack<uint16_t> address_stack;
uint8_t sound_timer;
uint8_t delay_timer;
uint8_t registers[16];

uint8_t vram[WINDOW_WIDTH][WINDOW_HEIGHT];

int read_file_into_ram(std::string path, uint8_t* mem) {
    std::ifstream fin(path, std::ios::binary | std::ios::in);

    fin.seekg(0,std::ios_base::end);
    std::streampos file_size = fin.tellg();
    fin.seekg(0,std::ios_base::beg);

    fin.read(reinterpret_cast<char*> (mem+512), file_size);

    return file_size;
}


int main() {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // Initialize Window
    if(SDL_CreateWindowAndRenderer(WINDOW_WIDTH * SCALE, WINDOW_HEIGHT * SCALE, 0, &window, &renderer) < 0) {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_RenderSetScale(renderer, SCALE, SCALE);

    // Display blank, white screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);



    int size = read_file_into_ram("IBMLogo.ch8", ram);
    // Set PC to beginning of read file
    program_counter = 512;

    while(program_counter < 4095) {
        uint16_t instruction = (ram[program_counter] << 8) | ram[program_counter+1];
        program_counter += 2;


        // Possible variables
        uint8_t second_nibble = ((instruction >> 8) & 0x0F);
        uint8_t third_nibble = (instruction & 0xF0) >> 4;
        uint8_t fourth_nibble = instruction & 0x0F;
        uint8_t second_byte = instruction & 0x00FF;
        uint16_t ram_address = instruction & 0x0FFF;

        std::cout << "0x" << std::hex << instruction << std::dec << " ";

        switch (instruction & 0xF000)
        {
        // Clear Screen
        case 0x0000:
            std::cout << "Clearing Screen" << std::endl;
            if(instruction == 0x00E0) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);
            }
            break;
        // Jump to 0x0FFF
        case 0x1000:
            std::cout << "Jump" << std::endl;
            program_counter = 512 + ram_address;
            break;
        // Set register 0x0F00 to 0x00FF
        case 0x6000:
            std::cout << "Setting Register" << std::endl;
            registers[second_nibble] = second_byte;
            break;
        // Add 0x00FF to register 0x0F00 
        case 0x7000:
            std::cout << "Adding to Register" << std::endl;
            registers[second_nibble] += second_byte;
            break;
        case 0xA000:
            std::cout << "Setting Index Register";
            index_register = ram_address;
            break;
        case 0xD000: 
            std::cout << "Drawing Pixels" << std::endl;

            uint8_t x_start = registers[second_nibble] % 64;
            uint8_t y = registers[third_nibble] % 32;

            registers[0xF] = 0;

            for(int row = 0; row < fourth_nibble; row++) {
                uint8_t sprite_data = ram[index_register + row];
                uint8_t x = x_start;

                for(int i = 0; i < 8; i++) {
                    uint8_t current_pixel = (sprite_data >> (7-i)) & 1;

                    if(current_pixel & 1) {
                        if(vram[x][y] & 1) {
                            vram[x][y] = 0;
                            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                            SDL_RenderDrawPoint(renderer, x, y);

                            registers[0xF] = 1;
                        } else {
                            vram[x][y] = 1;
                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                            SDL_RenderDrawPoint(renderer, x, y);
                        }
                    }

                    x++;
                    if(x >= WINDOW_WIDTH) break;
                }

                y++;
                if(y >= WINDOW_HEIGHT) break;
            }

            SDL_RenderPresent(renderer);

            break;
        }

        std::string s;
        std::cin >> s;
    }



    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit(); //Quit SDL subsystems

    return 0;
}