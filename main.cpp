#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
using namespace std;

//An renderer scale factor
const int SCALE = 25;
//Window dimensions
const int W_WIDTH = 800, W_HEIGHT = 800;
//Game frame dimensions
const int WIDTH = W_WIDTH/SCALE, HEIGHT = W_HEIGHT/SCALE;

// This function draws a vector of points to texture
void renderPointsToTexture(SDL_Renderer* renderer, SDL_Texture* texture, vector<int> points, Uint8 color[4]){
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
    int length = size(points);
    for(int i = 0; i < length; i++){
        SDL_RenderDrawPoint(renderer, points[i] % WIDTH, points[i] / WIDTH);
        SDL_Log("%d | x - %d | y - %d", length, points[i] % WIDTH, points[i] / WIDTH);
    }
    SDL_SetRenderTarget(renderer, nullptr);
}

int main(int argv, char** args){
    //Init SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        SDL_Log("SDL init failed: %s\n", SDL_GetError());
        return 1;
    }

    //Init window with constant WIDTH & HEIGHT
    SDL_Window* window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W_WIDTH, W_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
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
    SDL_RenderSetScale(renderer, SCALE, SCALE); 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetRenderTarget(renderer, nullptr);
    
    //Init two frames
    SDL_Texture* frame = SDL_CreateTexture(renderer, 
                                            SDL_PIXELFORMAT_RGBA8888, 
                                            SDL_TEXTUREACCESS_TARGET, 32, 32);

    SDL_Texture* frame1 = SDL_CreateTexture(renderer,
                                            SDL_PIXELFORMAT_RGBA8888, 
                                            SDL_TEXTUREACCESS_TARGET, 32, 32);

    //Frame switcher
    SDL_Texture* frames[2] = {frame, frame1};
    int frame_ind = 0;
    SDL_RenderCopy(renderer, frames[frame_ind], nullptr, nullptr);

    vector<int> snake = {528+64,528+32,528, 496, 464, 464-32};
    int action = -32; /* [WIDTH] - DOWN 
                        [-WIDTH] - UP 
                             [1] - RIGHT 
                            [-1] - LEFT*/

    vector<int> apples = {};

    bool isrunning = true;
    SDL_Event event;
    while(isrunning){
        //Show new frame
        SDL_RenderPresent(renderer);
        Uint32 time0 = SDL_GetTicks();
        //Handle events
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                isrunning = false;
            }

            if(event.type == SDL_KEYDOWN){
                switch (event.key.keysym.sym){
                case SDLK_RIGHT:
                    action = 1;
                    break;
                case SDLK_LEFT:
                    action = -1;
                    break;
                case SDLK_UP:
                    action = -1*WIDTH;
                    break;
                case SDLK_DOWN:
                    action = WIDTH;
                    break;
                default:
                    break;
                }
            }
        }
        //Mutate snake AKA Snake logic
        int head = size(snake) - 1;
        int temp = snake[head] + action;
        if(temp >= HEIGHT*WIDTH){
            temp -= HEIGHT*WIDTH;
        }
        if(temp < 0){
            temp += HEIGHT*WIDTH;
        }
        if((action == -1) && (snake[head]%WIDTH == 0)){
            temp += WIDTH; 
        }
        if((action == 1) && (snake[head]%WIDTH == WIDTH-1)){
            temp -= WIDTH;
        }

        for(int i = 0; i < head; i++){
            snake[i] = snake[i+1];
        }
        snake[head] = temp;

        /*!!! TODO: 
              APPLE LOGIC GOES HERE 
        */

        /*!!! TODO:
              APPLE RENDERING GOES HERE 
        */

        //Swap frames
        frame_ind = (frame_ind == 0) ? 1 : 0;
        
        //Draw snake
        Uint8 color[4] = {255, 255, 255, 255};
        renderPointsToTexture(renderer, frames[frame_ind], snake, color);

        Uint32 time1 = SDL_GetTicks();

        //!!! TODO: Replace it. NOW!
        //Such a stupid fix but ok for now
        SDL_Delay(60);

        //Put new frame to render
        SDL_RenderCopy(renderer, frames[frame_ind], nullptr, nullptr);
    }

    //Uninit everything
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}