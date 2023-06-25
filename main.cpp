#include <iostream>
#include <cstdint>
#include <stack>
#include <fstream>
#include <SDL2/SDL.h>
#include <stdio.h>

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
    SDL_Surface* screenSurface = NULL;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    } else {
        //Create window
        window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64, 32, SDL_WINDOW_SHOWN );
        
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        } else
        {
            //Get window surface
            screenSurface = SDL_GetWindowSurface( window );

            //Fill the surface white
            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
            
            //Update the surface
            SDL_UpdateWindowSurface( window );

            //Hack to get window to stay up
            SDL_Event e; bool quit = false; while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }
        }
    }

    

    //Destroy window
    SDL_DestroyWindow( window );
    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}