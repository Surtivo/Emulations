#pragma once

#include <stdio.h>
#include <stdlib.h>
// #include <stdint.h>
#include <stdbool.h>
#include "SDL.h"


#define ZERO 0b00000000
#define CLEAR 0b11100000
#define SUB_CALL_RET 0b11101110
#define JUMP_NNN 0b00000001
#define SUB_CALL_NNN 0b00000010
#define VX_EAQUAL_NN_SKIP 0b00000011
#define VX_NOTEQUAL_NN_SKIP 0b00000100
#define VX_EQUAL_VY_SKIP 0b00000101
#define SET_VX_NN 0b00000110
#define ADD 0b00000111
#define SET_VX 0b00001000
#define VX_NOTEAQUAL_VY_SKIP 0b00001001
#define SET_I 0b00001010
#define JUMP_NNN_V0 0b00001011
#define RAND 0b00001100
#define DRAW 0b00001101
#define KEY_VX 0b00001110
#define OUTRO 0b00001111

#define TWELVE_BIT_MASK 0b111111111111
#define EIGHT_BIT_MASK 0b11111111
#define FOUR_BIT_MASK 0b1111

#define ENTRY_POINT 0b1000000000

#define DEFAULT_WINDOW_WIDTH_SIZE 64
#define DEFAULT_WIN_HEIGHT_SIZE 32
#define DEFAULT_SCALE_FACTOR 16


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
    bool keypad[16];
    __uint8_t memory [4096];
    __uint8_t V[16];
    __uint8_t delay_timer;
    __uint8_t sound_timer;
    __uint16_t stack[12];
    __uint16_t *stack_pointer;
    __uint16_t PC;
    __uint16_t I;    
}Chip8;
