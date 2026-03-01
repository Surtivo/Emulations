#include "Initialize_Functions.h"

bool Initialize_SDL (void){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0){
        SDL_Log("Unable to Initialize SLD subsystems: %s", SDL_GetError());
        return false;
    }
    return true;
}

void Initialize_Window_Default_Size (Win_config *w){
    w->x = DEFAULT_WINDOW_WIDTH_SIZE;
    w->y = DEFAULT_WIN_HEIGHT_SIZE;
    w->scale_factor = DEFAULT_SCALE_FACTOR;
}

bool Initialize_SDL_Window (Win_config *w, Sdl_parts *p){
    Initialize_Window_Default_Size(w);
    p->Window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w->x * w->scale_factor, w->y * w->scale_factor, 0);

    if(p->Window == NULL){
        SDL_Log("Unable to create SDL window: %s", SDL_GetError());
        return false;
    }
    return true;
}

bool Initialize_Renderer (Sdl_parts *p){
    p->Renderer = SDL_CreateRenderer(p->Window, -1, SDL_RENDERER_ACCELERATED);
    if(p->Renderer == NULL){
        SDL_Log("Unable to create SDL renderer: %s", SDL_GetError());
        return false;
    }
    return true;
}

bool Initialize_chip (Chip8 *chip, const char *rom_name){
    const unsigned int entry_point = ENTRY_POINT; //512;
    
    const uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0   
        0x20, 0x60, 0x20, 0x20, 0x70,   // 1  
        0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2 
        0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,   // 4    
        0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
        0xF0, 0x80, 0xF0, 0x80, 0x80,   // F
    };

    memset(chip, 0, sizeof(Chip8));                      //Initialize all values with 0;
    memcpy(&chip->memory[0], font, sizeof(font));        //Copy 'font' to memory;

    FILE *rom = fopen(rom_name, "rb");
    if(!rom){
        SDL_Log("Rom file %s invalir or non existent\n", rom_name);
        return false;
    }
    
    fseek(rom, 0, SEEK_END);                            //Get and check rom size;
    const size_t rom_size = ftell(rom);
    const size_t max_size = (sizeof(chip->memory) - entry_point);
    rewind(rom);

    if (rom_size > max_size) {
        SDL_Log("Rom file %s is too big! Rom size: %llu, Max size allowed: %llu\n", rom_name, (long long unsigned)rom_size, (long long unsigned)max_size);
        return false;
    }

    if(!fread(&chip->memory[entry_point], rom_size, 1, rom) < 0){
        SDL_Log("Could not pass ROM (%s) to CHIP-8 memory", rom_name);
        return false;
    }
    fclose(rom);

    chip->state = RUNNING;
    chip->PC = entry_point;
    chip->stack_pointer = &chip->stack[0];
    chip->rom_name = rom_name;
    
    return true;
}