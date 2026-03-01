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

                    //Map qwerty keys to CHIP8 keypad;
                    case SDLK_1: chip->keypad[0x1] = true; break;
                    case SDLK_2: chip->keypad[0x2] = true; break;
                    case SDLK_3: chip->keypad[0x3] = true; break;
                    case SDLK_4: chip->keypad[0xC] = true; break;

                    case SDLK_q: chip->keypad[0x4] = true; break;
                    case SDLK_w: chip->keypad[0x5] = true; break;
                    case SDLK_e: chip->keypad[0x6] = true; break;
                    case SDLK_r: chip->keypad[0xD] = true; break;

                    case SDLK_a: chip->keypad[0x7] = true; break;
                    case SDLK_s: chip->keypad[0x8] = true; break;
                    case SDLK_d: chip->keypad[0x9] = true; break;
                    case SDLK_f: chip->keypad[0xE] = true; break;

                    case SDLK_z: chip->keypad[0xA] = true; break;
                    case SDLK_x: chip->keypad[0x0] = true; break;
                    case SDLK_c: chip->keypad[0xB] = true; break;
                    case SDLK_v: chip->keypad[0xF] = true; break;                        

                    default:
                        break;        
                }
                break;
            
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    //Map qwerty keys to CHIP8 keypad;
                    case SDLK_1: chip->keypad[0x1] = false; break;
                    case SDLK_2: chip->keypad[0x2] = false; break;
                    case SDLK_3: chip->keypad[0x3] = false; break;
                    case SDLK_4: chip->keypad[0xC] = false; break;

                    case SDLK_q: chip->keypad[0x4] = false; break;
                    case SDLK_w: chip->keypad[0x5] = false; break;
                    case SDLK_e: chip->keypad[0x6] = false; break;
                    case SDLK_r: chip->keypad[0xD] = false; break;

                    case SDLK_a: chip->keypad[0x7] = false; break;
                    case SDLK_s: chip->keypad[0x8] = false; break;
                    case SDLK_d: chip->keypad[0x9] = false; break;
                    case SDLK_f: chip->keypad[0xE] = false; break;

                    case SDLK_z: chip->keypad[0xA] = false; break;
                    case SDLK_x: chip->keypad[0x0] = false; break;
                    case SDLK_c: chip->keypad[0xB] = false; break;
                    case SDLK_v: chip->keypad[0xF] = false; break;

                    default: break;
                }
                break;
                
            default:
                // printf("None\n");
                break;
        }
    }
}

void emulate_instruction (Chip8 *chip, Win_config config){

    __uint16_t calculation;
    __uint8_t auxiliary;
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
            if (chip->instruction.NN == CLEAR){                                         //(Logical) Clear the screen;
                memset(&chip->display[0], false, sizeof chip->display);                 //Clear the pixel state on memory;
                //chip->draw =  true;
                printf("--- Clear ---\n");

            }else if (chip->instruction.NN == SUB_CALL_RET){                            //Returns from subroutine;
                if (chip->stack_pointer <= &chip->stack[0]){                            //If the stack_pointer is still on the first
                    printf("Error: Stack Underflow!\n");                                // possible element, stack underflow;
                    exit(EXIT_FAILURE); 
                }
                chip->PC = *--chip->stack_pointer;  
                printf("--- SUB_RET ---\n");
            }
            
            break;
            
        case SUB_CALL_NNN:
            if (chip->stack_pointer >= &chip->stack[15]){                               //If the stack_pointer is already at the last
                printf("Error: Stack Overflow!\n");                                     // possible element, stack overflow;
                exit(EXIT_FAILURE); 
            }
            *chip->stack_pointer++ = chip->PC;
            chip->PC = chip->instruction.NNN;
            printf("--- SUB_CALL ---\n");
            break;
        
        case VX_EAQUAL_NN_SKIP:
            if(chip->V[chip->instruction.x] == chip->instruction.NN) chip->PC += 2;
            printf("--- VX = NN SKIP ---\n");
            break;
        
        case VX_NOTEQUAL_NN_SKIP:
            if(chip->V[chip->instruction.x] != chip->instruction.NN) chip->PC += 2;
                printf("--- VX != NN SKIP ---\n");
            break;
        
        case VX_EQUAL_VY_SKIP:
            printf("--- VX = VY SKIP ---\n");
            if(chip->instruction.N != 0)    break;
            if(chip->V[chip->instruction.x] == chip->V[chip->instruction.y]) chip->PC += 2;
            break;

        case JUMP_NNN:                                                                  //Jump to NNN address;
            chip->PC = chip->instruction.NNN; 
            printf("--- JUMP ---\n");  
            break;
        
        case SET_VX_NN:                                                                     //Set V[x] = NN;
            chip->V[chip->instruction.x] = chip->instruction.NN;
            printf("--- V[x] ---\n");
            break;  
        
        case ADD:                                                                       //Add value stored in NN to V[x]
            chip->V[chip->instruction.x] += chip->instruction.NN;
            printf("--- ADD ---\n");
            break;
        
        case SET_VX:{
            switch(chip->instruction.N){

                case 0:
                    chip->V[chip->instruction.x] = chip->V[chip->instruction.y];
                    printf("--- VX = VY ---\n");
                    break;
                
                case 1:
                    chip->V[chip->instruction.x] |= chip->V[chip->instruction.y];
                    printf("--- VX |= VY ---\n");
                    break;
                
                case 2:
                    chip->V[chip->instruction.x] &= chip->V[chip->instruction.y];
                    printf("--- VX &= VY ---\n");
                    break;
                
                case 3:
                    chip->V[chip->instruction.x] ^= chip->V[chip->instruction.y];
                    printf("--- VX ^= VY ---\n");
                    break;
                
                case 4:
                    calculation = chip->V[chip->instruction.x] + chip->V[chip->instruction.y];
                    if(calculation > 255){
                        chip->V[0b1111] = 1;
                    }else    chip->V[0b1111] = 0;

                    chip->V[chip->instruction.x] += chip->V[chip->instruction.y];
                    printf("--- VX += VY ---\n");
                    break;
                
                case 5:
                    if(chip->V[chip->instruction.x] >= chip->V[chip->instruction.y]){
                        chip->V[0b1111] = 1;
                    }else chip->V[0b1111] = 0;
                    chip->V[chip->instruction.x] -= chip->V[chip->instruction.y];
                    printf("--- VX -= VY ---\n");
                    break;
                
                case 6:
                    chip->V[0b1111] = chip->V[chip->instruction.x] & 0b01;
                    chip->V[chip->instruction.x] >>= 1;
                    printf("--- VX >> 1 ---\n");
                    break;
                
                case 7:
                    if(chip->V[chip->instruction.y] >= chip->V[chip->instruction.x]){
                        chip->V[0b1111] = 1;
                    }else chip->V[0b1111] = 0;
                    chip->V[chip->instruction.x] = chip->V[chip->instruction.y] - chip->V[chip->instruction.x];
                    printf("--- VX = VY - VX ---\n");
                    break;

                case 14: 
                    chip->V[0xF] = (chip->V[chip->instruction.x] >> 7) & 0x1;
                    chip->V[chip->instruction.x] <<= 1;
                    break;
                
                default:
                    printf("Unimplement / Wrong opcode of inner switch (OPCODE: N)");
                    break;
            }
            break;
        }   
        
        case VX_NOTEAQUAL_VY_SKIP:
            if(chip->V[chip->instruction.x] != chip->V[chip->instruction.y])    chip->PC += 2;
            printf("--- VX != VY SKIP ---\n");
            break;
        
        case JUMP_NNN_V0:
            chip->PC = chip->V[0] + chip->instruction.NNN;
            printf("--- JUMP V[0] + NNN ---\n");
            break;
        
        case RAND:
            chip->V[chip->instruction.x] = (rand() % 256) & chip->instruction.NN;
            printf("--- RAND ---\n");
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

        case KEY_VX:
            if(chip->instruction.NN == 0x9E){
                if(chip->keypad[chip->V[chip->instruction.x]])
                    chip->PC += 2;
            }else if(chip->instruction.NN == 0xa1){
                if(!chip->keypad[chip->V[chip->instruction.x]])
                    chip->PC += 2;
            }
            break;
        
        case OUTRO:
            switch(chip->instruction.NN){
                case 7:
                    chip->V[chip->instruction.x] = chip->delay_timer;
                    break;
                    
                case 10:{                                                   //A key press is waited, and then stored in V[x];
                    bool key_pressed = false;                               // (blocking operation, all instruction halted until next key event, delay and sound timers should continue processing);
                    __uint8_t pressed_key = -1;

                    for(__uint8_t i = 0; i < 16 && key_pressed == false; i++){  //Scan if a key was pressed;
                        if(chip->keypad[i]){
                            pressed_key = i;
                            key_pressed = true;
                            // break;
                        }
                    }

                    if(!key_pressed)    chip->PC -= 2;                         //Waits for a key be pressed;
                    else{
                        if(chip->keypad[pressed_key])  chip->PC -= 2;          //Waits for the key be released;
                        else{
                            chip->V[chip->instruction.x] = pressed_key;
                            key_pressed = false;
                            pressed_key = -1;
                        }
                    }
                    break;  
                }

                case 21:
                    chip->delay_timer = chip->V[chip->instruction.x];
                    break;

                case 24:
                    chip->sound_timer = chip->V[chip->instruction.x];
                    break;

                case 30:
                    if (chip->I + chip->V[chip->instruction.x] > 0xFFF) {
                        chip->V[0xF] = 1;
                    } else {
                        chip->V[0xF] = 0;
                    }
                    chip->I += chip->V[chip->instruction.x];
                    break;

                case 41:
                    chip->I = chip->V[chip->instruction.x]*5;                  //Set register I to sprite location in memory for character in VX;
                    break; 

                case 51:    
                    //Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I,
                    // the tens digit at location I+1, and the ones digit at location I+2.[23]
                    __uint8_t bcd = chip->V[chip->instruction.x];
                    chip->memory[chip->I+2] = bcd % 10;
                    bcd /= 10;
                    chip->memory[chip->I+1] = bcd % 10;
                    bcd /= 10;
                    chip->memory[chip->I] = bcd % 10;
                    break;

                case 85:
                    for(__uint8_t i = 0; i <= chip->instruction.x; i++) {
                        chip->memory[chip->I + i] = chip->V[i];
                    }
                    break;

                case 101:
                    for(__uint8_t i = 0; i <= chip->instruction.x; i++) {
                        chip->V[i] = chip->memory[chip->I + i];
                    }
                    break;
                default:
                    break;
            }
            break;
        
        default:
            printf("Unknown Instruction / Opcode!\n");
            break;
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

void update_timers(Chip8 *chip, Sdl_parts *sdl){
    if(chip->delay_timer > 0)
        chip->delay_timer--;
}