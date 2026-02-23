#include "Clear_Functions.h"

void clear_screen (Sdl_parts* SDL){
    if(SDL_SetRenderDrawColor(SDL->Renderer, 255, 255, 0, 255) != 0){           //Yellow (#FFFF00);
        SDL_Log("Unable to create SDL RenderDrawColor: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }        
    SDL_RenderClear(SDL->Renderer);                                             //Paint the screen with RendererDrawnColor;
    SDL_RenderPresent(SDL->Renderer);                                           //Update the screen;
}

void clean_up (Sdl_parts *SDL){
    SDL_DestroyRenderer(SDL->Renderer);
    SDL_DestroyWindow(SDL->Window);
    SDL_Quit();
}