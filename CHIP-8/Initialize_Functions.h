#pragma once

#include "Definitions.h"

bool Initialize_SDL (void);
void Initialize_Window_Default_Size (Win_config *w);
bool Initialize_SDL_Window (Win_config *w, Sdl_parts *p);
bool Initialize_Renderer (Sdl_parts *p);
bool Initialize_chip (Chip8 *chip, const char *rom_name);