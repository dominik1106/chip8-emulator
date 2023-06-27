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
bool CHIP48 = true;

uint8_t ram[4096];
int program_counter;
uint16_t index_register;
std::stack<uint16_t> address_stack;
uint8_t sound_timer;
uint8_t delay_timer;
uint8_t registers[16];

uint8_t vram[WINDOW_WIDTH][WINDOW_HEIGHT];

uint8_t FONT[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


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

    // Copy font into memory
    memcpy(&ram[0x050], FONT, 80);

    // Set PC to beginning of read file
    program_counter = 512;

    bool get_key_blocking = false;
    uint8_t get_key_blocking_x;

    while(program_counter < 4095) {
        if(get_key_blocking) { 
            SDL_Event e;

            if(SDL_PollEvent(&e) != 0 && e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym)
                {
                case SDL_SCANCODE_1:
                    registers[get_key_blocking_x] = 0x1;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_2:
                    registers[get_key_blocking_x] = 0x2;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_3:
                    registers[get_key_blocking_x] = 0x3;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_4:
                    registers[get_key_blocking_x] = 0xC;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_Q:
                    registers[get_key_blocking_x] = 0x4;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_W:
                    registers[get_key_blocking_x] = 0x5;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_E:
                    registers[get_key_blocking_x] = 0x6;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_R:
                    registers[get_key_blocking_x] = 0xD;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_A:
                    registers[get_key_blocking_x] = 0x7;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_S:
                    registers[get_key_blocking_x] = 0x8;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_D:
                    registers[get_key_blocking_x] = 0x9;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_F:
                    registers[get_key_blocking_x] = 0xE;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_Z:
                    registers[get_key_blocking_x] = 0xA;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_X:
                    registers[get_key_blocking_x] = 0x0;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_C:
                    registers[get_key_blocking_x] = 0xB;
                    get_key_blocking = false;
                    break;
                case SDL_SCANCODE_V:
                    registers[get_key_blocking_x] = 0xF;
                    get_key_blocking = false;
                    break;
                }

            }
        } else {
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
            case 0x3000:
                if(registers[second_nibble] == second_byte) program_counter += 2;
                break;
            case 0x4000:
                if(registers[second_nibble] != second_byte) program_counter += 2;
                break;
            case 0x5000:
                if(registers[second_nibble] == registers[third_nibble]) program_counter += 2;
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
            case 0x8000:
                switch (instruction & 0x00F)
                {
                case 0x0: // Set
                    registers[second_nibble] = registers[third_nibble];
                    break;
                case 0x1: // Bin. OR
                    registers[second_nibble] |= registers[third_nibble];
                    break;
                case 0x2: // Bin. AND
                    registers[second_nibble] &= registers[third_nibble];
                    break;
                case 0x3:
                    registers[second_nibble] ^= registers[third_nibble];
                    break;
                case 0x4:
                    registers[second_nibble] += registers[third_nibble];
                    break;
                case 0x5:
                    if(registers[second_nibble] > registers[third_nibble]) registers[0xF] = 1;
                    else if(registers[second_nibble] < registers[third_nibble]) registers[0xF] = 0;

                    registers[second_nibble] = registers[second_nibble] - registers[third_nibble];
                    break;
                case 0x7:
                    if(registers[second_nibble] > registers[third_nibble]) registers[0xF] = 0;
                    else if(registers[second_nibble] < registers[third_nibble]) registers[0xF] = 1;

                    registers[second_nibble] = registers[third_nibble] - registers[second_nibble];
                    break;
                case 0x6:
                    if(CHIP48) registers[second_nibble] = registers[third_nibble];
                    registers[0xF] = registers[second_nibble] & 0b00000001;
                    registers[second_nibble] >>= 1;
                    break;
                case 0xE:
                    if(CHIP48) registers[second_nibble] = registers[third_nibble];
                    registers[0xF] = registers[second_nibble] & 0b10000000;
                    registers[second_nibble] <<= 1;
                    break;
                }
                break;
            case 0x9000:
                if(registers[second_nibble] != registers[third_nibble]) program_counter += 2;
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
            case 0xF000:
                switch(instruction & 0x00FF)
                {
                case 0x0A:
                    get_key_blocking = true;
                    break;
                case 0x29:
                    // Base Adress + 5 Bytes per character
                    index_register = 0x050 + 5 * (registers[second_nibble] & 0x0F);
                    break;
                }
                case 0x33:
                    uint8_t number = registers[second_nibble];
                    for(int i = 2; i >= 0; i--) {
                        ram[index_register + i] = number % 10;
                        number /= 10;
                    }
                    break;
                case 0x55:
                    for(int i = 0; i <= second_nibble; i++) {
                        ram[index_register + i] = registers[i];
                    }
                    if(CHIP48) index_register += second_nibble;
                    break;
                case 0x65:
                    for(int i = 0; i <= second_nibble; i++) {
                        registers[i] = ram[index_register + i];
                    }
                    if(CHIP48) index_register += second_nibble;
                    break;
                break;
            }
        }

        std::string s;
        std::cin >> s;
    }



    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit(); //Quit SDL subsystems

    return 0;
}