#include <iostream>
#include <SDL2/SDL.h>

const int WIDTH = 800, HEIGHT = 600;

int main(int argv, char** args){
    //Init SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        SDL_Log("SDL init failed: %s\n", SDL_GetError());
        return 1;
    }

    //Init window with constant WIDTH & HEIGHT
    SDL_Window* window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    if (NULL == window){
        SDL_Log("Window init failed: %s\n", SDL_GetError());
        return 1;
    }

    //Init the renderer which will be displaying the texture on th screen
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (NULL == renderer){
        SDL_Log("Failed to init renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, WIDTH/2, HEIGHT/2);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    while(true){
        if(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                break;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}