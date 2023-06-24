#include <iostream>
#include <cstdint>
#include <stack>

uint8_t memory[4096];
int program_counter;
uint16_t index_register;
std::stack<uint16_t> address_stack;
uint8_t sound_timer;
uint8_t delay_timer;
uint8_t registers[16];

int main() {
    
}