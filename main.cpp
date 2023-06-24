#include <iostream>
#include <cstdint>
#include <stack>
#include <fstream>

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

    if(size > -1) {
        for(int i = 0; i < size; i++) {
            std::cout << +memory[512 + i] << ',';
        }
    }
}