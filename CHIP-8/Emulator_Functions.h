#pragma once

#include "Definitions.h"

void input_handler(Chip8 *chip);
void emulate_instruction (Chip8 *chip, Win_config config);
void update_screen(Chip8 *chip, const Win_config config, const Sdl_parts *sdl);