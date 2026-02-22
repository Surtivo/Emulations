#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL.h"

typedef struct{
    SDL_Window *Window;
    SDL_Renderer *Renderer;
}Sdl_parts;

typedef struct{
    unsigned short int x;
    unsigned short int y;
    unsigned short int scale_factor;
}Win_config;

typedef enum{
    QUIT,
    PAUSED,
    RUNNING, 
}States;

typedef struct{
    unsigned short int opcode;
    unsigned short int NNN;
    uint8_t NN;
    uint8_t N;
    uint8_t x;
    uint8_t y;
}Instructions;

typedef struct{
    States state;
    Instructions instruction;
    const char *rom_name;
    bool key_pad [16];
    bool display [64*32];
    __uint8_t memory [4096];
    __uint8_t V[16];
    __uint8_t delay_timer;
    __uint8_t sound_timer;
    __uint16_t stack[12];
    __uint16_t PC;
    __uint16_t I;    
}Chip8;

bool Initialize_SDL (void){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0){
        SDL_Log("Unable to Initialize SLD subsystems: %s", SDL_GetError());
        return false;
    }
    return true;
}

void Initialize_Window_Default_Size (Win_config *w){
    w->x = 64;
    w->y = 32;
    w->scale_factor = 16;
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

void clear_screen (Sdl_parts* SDL){
    if(SDL_SetRenderDrawColor(SDL->Renderer, 255, 255, 0, 255) != 0){           //Yellow (#FFFF00);
        SDL_Log("Unable to create SDL RenderDrawColor: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }        
    SDL_RenderClear(SDL->Renderer);                                             //Paint the screen with RendererDrawnColor;
    SDL_RenderPresent(SDL->Renderer);                                           //Update the screen;
}


bool Initialize_chip (Chip8 *chip, const char *rom_name){
    const unsigned int entry_point = 0b1000000000; //512;
    
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

    memset(chip, 0, sizeof(Chip8));                    //Initialize all values with 0;
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
    chip->rom_name = rom_name;
    
    return true;
}

void input_handler(Chip8 *chip){
    SDL_Event event;

    while(SDL_PollEvent(&event)){

        switch(event.type){

            case SDL_QUIT:
                chip->state = QUIT;
                return;

            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){

                    case SDLK_ESCAPE:
                        printf("Esc pressionado!\n");
                        chip->state = QUIT;
                        return;
                    
                    case SDLK_SPACE:
                        if (chip->state == RUNNING) {
                            chip->state = PAUSED;  //Pause;
                            printf("====  -PAUSED-  ====\n");
                        } else {
                            chip->state = RUNNING; //Resume;
                            printf("==== -UNPAUSED- ====\n");
                        }
                        break;

                    default:
                        break;        
                }
                break;
                
            default:
                // printf("None\n");
                break;
        }
    }
}

void emulate_instruction (Chip8 *chip, Win_config config){
    chip->instruction.opcode = ( chip->memory[ chip->PC ] << 8) | ( chip->memory[ chip->PC + 1 ] );
    chip->PC += 2;

    chip->instruction.NNN = (chip->instruction.opcode) & 0b111111111111;   //4095;                                     This part is necessary for getting the right bits for each 
    chip->instruction.NN = (chip->instruction.opcode) & 0b11111111;        //255;                                       part of the opcode;
    chip->instruction.N = (chip->instruction.opcode) & 0b1111;             //15;                                       Read Cowgod's Technical Reference for more information & Wiki page;
    chip->instruction.x = (chip->instruction.opcode >> 8) & 0b1111;
    chip->instruction.y = (chip->instruction.opcode >> 4) & 0b1111;                                                    //0100 1010 1010 0100 -> 0000101010100100;
                                                                                                                //0000 1111 1111 1111 -> 0000000000001111;
    switch(chip->instruction.opcode >> 12){

        case 0b0000:
            if(chip->instruction.NN == 0b11100000){                             //(Logical) Clear the screen;
                memset(&chip->display[0], false, sizeof chip->display);         //Clear the pixel state on memory;
                //chip->draw =  true;
                printf("--- Clear ---\n");

            }else if (chip->instruction.NN == 0b11101110){  //Returns from subroutine;
                //Unimplemented;    Minutagem 50 do tutorial!!!!        
            }
            break;

        case 0b0001:    //Jump to NNN address;
            chip->PC = chip->instruction.NNN; 
            printf("--- JUMP ---\n");  
            break;
        
        case 0b0110:    //Set V[x] = NN;
            chip->V[chip->instruction.x] = chip->instruction.NN;
            printf("--- V[x] ---\n");
            break;  
        
        case 0b0111:    //Add value stored in NN to V[x]
            chip->V[chip->instruction.x] += chip->instruction.NN;
            printf("--- ADD ---\n");
            break;
        
        case 0b1010:    //Set index register I;
            chip->I = chip->instruction.NNN;
            printf("--- SET I ---\n");
            break;          
        
        case 0b1101:{ // Instrução DXYN
            uint8_t vx = chip->V[chip->instruction.x] % 64;
            uint8_t vy = chip->V[chip->instruction.y] % 32;
            uint8_t height = chip->instruction.N;

            chip->V[0xF] = 0; // Reset colisão

            for (int row = 0; row < height; row++) {
                if (vy + row >= 32) break; // Clipping vertical

                uint8_t sprite_byte = chip->memory[chip->I + row];

                for (int col = 0; col < 8; col++) {
                    if (vx + col >= 64) break; // Clipping horizontal

                    // Pega o bit específico (do mais significativo para o menos)
                    bool sprite_bit = (sprite_byte & (0x80 >> col)) != 0;
                    bool *display_pixel = &chip->display[(vy + row) * 64 + (vx + col)];

                    if (sprite_bit) {
                        if (*display_pixel) chip->V[0xF] = 1; // Colisão detectada
                        *display_pixel ^= true; // Aplica o XOR
                    }
                }
            }
            // chip->draw_flag = true;
            printf("--- DRAW ---\n");
            break;
        }
        
        default:
            printf("Unknown Instruction / Opcode!\n");
    }
}

// Update window with any changes
void update_screen(Chip8 *chip, const Win_config config, const Sdl_parts *sdl) {
    SDL_Rect rect = {.x = 0, .y = 0, .w = config.scale_factor, .h = config.scale_factor};

    uint8_t fg_r = 255, fg_g = 255, fg_b = 0,   fg_a = 255; // Amarelo
    uint8_t bg_r = 0,   bg_g = 0,   bg_b = 0,   bg_a = 255; // Preto

    // Loop through display pixels, draw a rectangle per pixel to the SDL window
    for (uint32_t i = 0; i < sizeof chip->display; i++) {
        // Translate 1D index i value to 2D X/Y coordinates
        // X = i % window width
        // Y = i / window width
        rect.x = (i % config.x) * config.scale_factor;
        rect.y = (i / config.x) * config.scale_factor;

        if (chip->display[i]) {
            SDL_SetRenderDrawColor(sdl->Renderer, bg_r, bg_g, bg_b, bg_a);
            SDL_RenderFillRect(sdl->Renderer, &rect);
        }else{
            SDL_SetRenderDrawColor(sdl->Renderer, fg_r, fg_g, fg_b, fg_a);
            SDL_RenderFillRect(sdl->Renderer, &rect);
        }
    }
}

void clean_up (Sdl_parts *SDL){
    SDL_DestroyRenderer(SDL->Renderer);
    SDL_DestroyWindow(SDL->Window);
    SDL_Quit();
}

int main(int argc, char **argv){

    //Default Usage message for args;
    if (argc < 2) {
       fprintf(stderr, "Usage: %s <rom_name>\n", argv[0]);
       exit(EXIT_FAILURE);
    }

    if(!Initialize_SDL())
        exit(EXIT_FAILURE);

    Win_config config;
    Sdl_parts SDL;
    printf("Iniciou SDL\n");
        
    if(!Initialize_SDL_Window(&config, &SDL))
        exit(EXIT_FAILURE);

    printf("Iniciou Janela\n");

    Sdl_parts *Renderer;
    if(!Initialize_Renderer(&SDL))
        exit(EXIT_FAILURE);

    printf("Iniciou Renderizador\n");

    clear_screen(&SDL);
    printf("Iniciou o renderizador de cor, limpou o render e atualizou a tela.\n");

    //Initialize chip
    Chip8 chip;
    const char *rom_name = argv[1];
    chip.state = RUNNING;
    if(!Initialize_chip(&chip, rom_name)) exit(EXIT_FAILURE);

    while(chip.state != QUIT){
        input_handler(&chip);

        if(chip.state == PAUSED){
            continue;
        }

        emulate_instruction(&chip, config);

        SDL_Delay(17);
        update_screen(&chip, config, &SDL);
        SDL_RenderPresent(SDL.Renderer);
    }

    bool test = true;
    printf("Terminou a Execucao! %ld\n", sizeof(test));

    clean_up(&SDL);
    exit(EXIT_SUCCESS);
}   