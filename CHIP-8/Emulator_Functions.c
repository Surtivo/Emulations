#include "Emulator_Functions.h"

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

    chip->instruction.NNN = (chip->instruction.opcode) & TWELVE_BIT_MASK;   //4095;                                     This part is necessary for getting the right bits for each 
    chip->instruction.NN = (chip->instruction.opcode) & EIGHT_BIT_MASK;     //255;                                       part of the opcode;
    chip->instruction.N = (chip->instruction.opcode) & FOUR_BIT_MASK;       //15;                                       Read Cowgod's Technical Reference for more information & Wiki page;
    chip->instruction.x = (chip->instruction.opcode >> 8) & FOUR_BIT_MASK;
    chip->instruction.y = (chip->instruction.opcode >> 4) & FOUR_BIT_MASK;                                              //0100 1010 1010 0100 -> 0000101010100100;
                                                                                                                        //0000 1111 1111 1111 -> 0000000000001111;
    switch(chip->instruction.opcode >> 12){

        case ZERO:
            if (chip->instruction.NN == CLEAR){                                          //(Logical) Clear the screen;
                memset(&chip->display[0], false, sizeof chip->display);                 //Clear the pixel state on memory;
                //chip->draw =  true;
                printf("--- Clear ---\n");

            }else if (chip->instruction.NN == SUB_CALL_RET){                            //Returns from subroutine;
                //Unimplemented;    Minutagem 50 do tutorial!!!!        
            }
            break;

        case JUMP_NNN:                                                                  //Jump to NNN address;
            chip->PC = chip->instruction.NNN; 
            printf("--- JUMP ---\n");  
            break;
        
        case SET_V:                                                                     //Set V[x] = NN;
            chip->V[chip->instruction.x] = chip->instruction.NN;
            printf("--- V[x] ---\n");
            break;  
        
        case ADD:                                                                       //Add value stored in NN to V[x]
            chip->V[chip->instruction.x] += chip->instruction.NN;
            printf("--- ADD ---\n");
            break;
        
        case SET_I:                                                                     //Set index register I;
            chip->I = chip->instruction.NNN;
            printf("--- SET I ---\n");
            break;          
        
        case DRAW:{                                                                     //Draw (DXYN) Instruction;
            uint8_t vx = chip->V[chip->instruction.x] % config.x;
            uint8_t vy = chip->V[chip->instruction.y] % config.y;

            chip->V[0b1111] = 0;                                                        //Reset colision;

            for (uint8_t row = 0; row < chip->instruction.N; row++) {                   //For each pixel of height N;
                if (vy + row >= config.y) break;                                        //Vertical clipping;

                uint8_t sprite_byte = chip->memory[chip->I + row];                      //Reads one byte from memory each time;

                for (uint8_t col = 0; col < 8; col++) {
                    if (vx + col >= config.x) break;                                    //Horizontal clipping;

                    bool sprite_bit = (sprite_byte & (0b10000000 >> col)) != 0;         //Tests each bit on byte;
                    bool *display_pixel = &chip->display[(vy + row) * config.x + (vx + col)];

                    if (sprite_bit){
                        if (*display_pixel) chip->V[0b1111] = 1;                        //Colision detected;
                        *display_pixel ^= true;                                         //XOR;
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

void update_screen(Chip8 *chip, const Win_config config, const Sdl_parts *sdl) {
    SDL_Rect rect = {.x = 0, .y = 0, .w = config.scale_factor, .h = config.scale_factor};

    uint8_t fg_r = 255, fg_g = 255, fg_b = 0,   fg_a = 255;     //Yellow;
    uint8_t bg_r = 0,   bg_g = 0,   bg_b = 0,   bg_a = 255;     //Black;

    for (uint32_t i = 0; i < sizeof chip->display; i++) {       //Loop through display pixels, draw a rectangle per pixel to the SDL window;
        //Translate 1D index i value to 2D X/Y coordinates;
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