#include "Initialize_Functions.h"
#include "Clear_Functions.h"
#include "Emulator_Functions.h"

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