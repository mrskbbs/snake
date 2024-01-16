#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <format>
using namespace std;

//An renderer scale factor
const int SCALE = 25;

//Window dimensions
const int W_WIDTH = 800, W_HEIGHT = 800;

//Game frame dimensions
const int WIDTH = W_WIDTH/SCALE, HEIGHT = W_HEIGHT/SCALE;

//Colors
Uint8 WHITE[4] = {255, 255, 255, 255};
Uint8 RED[4] = {255, 0, 0, 255};

// This function draws a vector of points to texture
void renderPointsToTexture(SDL_Renderer* renderer, SDL_Texture* texture, vector<int> points, Uint8 color[4]){
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
    int length = size(points);
    for(int i = 0; i < length; i++){
        SDL_RenderDrawPoint(renderer, points[i] % WIDTH, points[i] / WIDTH);
    }
}

//Clears the texture
void renderClearTexture(SDL_Renderer* renderer, SDL_Texture* texture){
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

//If the array contains target, return index. Else -1
int contains(vector<int>& arr, int target){
    int length = size(arr);
    for(int i = 0; i < length; i++){
        if(arr[i] == target){
            return i;
        }
    }
    return -1;
}

//Fix coords issues
int borderCheck(vector<int>& arr, int ind, int action){
    int temp = arr[ind] + action;
    if(temp >= HEIGHT*WIDTH){
        temp -= HEIGHT*WIDTH;
    }
    if(temp < 0){
        temp += HEIGHT*WIDTH;
    }
    if((action == -1) && (arr[ind]%WIDTH == 0)){
        temp += WIDTH; 
    }
    if((action == 1) && (arr[ind]%WIDTH == WIDTH-1)){
        temp -= WIDTH;
    }
    return temp;
}

//Renders custom menu screen with text
void renderMenu(SDL_Renderer* renderer, std::string header, std::string par, std::string score, SDL_Color color){
    //Init fonts
    TTF_Font* h1 = TTF_OpenFont("./misc/fonts/8bit.ttf", 80);
    TTF_Font* p = TTF_OpenFont("./misc/fonts/8bit.ttf", 24);

    //Create textures
    SDL_Surface* header_surf = TTF_RenderText_Solid(h1, header.c_str(), color);
    SDL_Texture* header_tex = SDL_CreateTextureFromSurface(renderer, header_surf);

    SDL_Surface* par_surf = TTF_RenderText_Solid(p, par.c_str(), color);
    SDL_Texture* par_tex = SDL_CreateTextureFromSurface(renderer, par_surf);

    //Setup bounding box for text
    SDL_Rect box;
    box.x = W_WIDTH/2 - header_surf->w/2;
    box.y = W_HEIGHT/2 - header_surf->h;
    box.h = header_surf->h;
    box.w = header_surf->w;
    SDL_RenderCopy(renderer, header_tex, nullptr, &box);
    //If the score isn't null render score text
    if(score != ""){
        SDL_Surface* score_surf = TTF_RenderText_Solid(p, score.c_str(), color);
        SDL_Texture* score_tex = SDL_CreateTextureFromSurface(renderer, score_surf);
        //Bounding box update
        box.x = W_WIDTH/2 - score_surf->w/2;
        box.y = box.y + box.h + 20;
        box.h = score_surf->h;
        box.w = score_surf->w;
        SDL_RenderCopy(renderer, score_tex, nullptr, &box);
    }
    //Bounding box update
    box.x = W_WIDTH/2 - par_surf->w/2;
    box.y = box.y + box.h + 20;
    box.h = par_surf->h;
    box.w = par_surf->w;
    SDL_RenderCopy(renderer, par_tex, nullptr, &box);

    SDL_RenderPresent(renderer);
}

int main(int argv, char** args){
    //Init SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        SDL_Log("SDL init failed: %s\n", SDL_GetError());
        return 1;
    }

    //Fonts init
    if (TTF_Init() == -1){
        SDL_Log("TTF init failed!\n");
        return 1;
    }
    TTF_Font* p = TTF_OpenFont("./misc/fonts/8bit.ttf", 24);

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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderSetScale(renderer, 1, 1);

    //Init two frames
    SDL_Texture* frame = SDL_CreateTexture(renderer, 
                                            SDL_PIXELFORMAT_RGBA8888, 
                                            SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);

    SDL_Texture* frame1 = SDL_CreateTexture(renderer,
                                            SDL_PIXELFORMAT_RGBA8888, 
                                            SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);

    //Frame switcher
    SDL_Texture* frames[2] = {frame, frame1};
    int frame_ind = 0;
    SDL_RenderCopy(renderer, frames[frame_ind], nullptr, nullptr);
    
    //Init events
    SDL_Event event;

    //Title screen present
    bool anykey = false;
    while(!anykey){    
        renderMenu(renderer, "SNAKE", "press any key to play", "", {255,255,255,255});
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 0;
            }
            if (event.type == SDL_KEYDOWN){
                anykey = true;
                break;
            }
        }
    }
    
    //Main window loop
    bool windowloop = true;
    while(windowloop){
        vector<int> snake = {528, 496, 464};
        int score = 0;
        int action = -1*WIDTH; /* [WIDTH] - DOWN 
                                  [-WIDTH] - UP 
                                  [1] - RIGHT 
                                  [-1] - LEFT*/
        int action_temp = action;
        vector<int> apples = {};

        //Game loop
        bool gameloop = true;
        while(gameloop){
            //Show new frame
            SDL_RenderPresent(renderer);
            Uint32 time0 = SDL_GetTicks();
            //Handle events
            while(SDL_PollEvent(&event)){
                if(event.type == SDL_QUIT){
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return 0;
                }
                //Handling the inputs
                if(event.type == SDL_KEYDOWN){
                    switch (event.key.keysym.sym){
                    case SDLK_RIGHT:
                        if(action != -1){
                            action_temp = 1;
                        }
                        break;
                    case SDLK_LEFT:
                        if(action != 1){
                            action_temp = -1;
                        }
                        break;
                    case SDLK_UP:
                        if(action != WIDTH){
                            action_temp = -1*WIDTH;
                        }
                        break;
                    case SDLK_DOWN:
                        if(action != -1*WIDTH){
                            action_temp = WIDTH;
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            //Move snake
            action = action_temp;
            int head = size(snake) - 1;
            int temp = borderCheck(snake, head, action);
            for(int i = 0; i < head; i++){
                snake[i] = snake[i+1];
            }

            //If snake intersects with itself leave
            if(contains(snake, temp) != -1){
                gameloop = false;
                break;
            }        
            snake[head] = temp;

            //Extend the snake, when it eats apple
            int snake_length = size(snake);
            int ind = contains(apples, snake[head]);
            if(ind != -1){
                int tail = borderCheck(snake, 0, action);
                snake.insert(snake.begin(), tail);
                apples.erase(apples.begin()+ind);
                score++;
            }

            //Randomly add apples on canvas
            int ratio = std::rand()%10000;
            if(ratio <= 85 && size(apples) < 10){
                int pos = std::rand()%(HEIGHT*WIDTH);
                while (!contains(snake,pos)){
                    pos = std::rand()%(HEIGHT*WIDTH);
                }
                apples.insert(apples.end(), std::rand()%(HEIGHT*WIDTH));
            }

            //Swap frames
            frame_ind = (frame_ind == 0) ? 1 : 0;

            //Clear texture
            renderClearTexture(renderer, frames[frame_ind]);

            //Render apples
            if(size(apples) != 0){
                renderPointsToTexture(renderer, frames[frame_ind], apples, RED);
            }

            //Draw snake
            renderPointsToTexture(renderer, frames[frame_ind], snake, WHITE);

            //Finish rendering
            SDL_SetRenderTarget(renderer, nullptr);

            //Keep a steady framerate
            Uint32 time1 = SDL_GetTicks();
            SDL_Delay(60 - (time1 - time0));

            //Put new frame to render
            SDL_RenderCopy(renderer, frames[frame_ind], nullptr, nullptr);
            SDL_Surface* score_surf = TTF_RenderText_Solid(p, to_string(score).c_str(), {255,100,90,255});
            SDL_Texture* score_tex = SDL_CreateTextureFromSurface(renderer, score_surf);

            //Put HUD on top of the game (the HUD is an amoint of score)
            SDL_Rect box;
            box.x = W_WIDTH/2 - score_surf->w/2;
            box.y = 10;
            box.h = score_surf->h;
            box.w = score_surf->w;
            SDL_RenderCopy(renderer, score_tex, nullptr, &box);
        }

        //Present defeat screen
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);
        anykey = false;
        while(!anykey){    
            renderMenu(renderer, "YOU LOST", "press any key to play again", std::string("your score: ").append(to_string(score)), {255,255,255,255});
            while(SDL_PollEvent(&event)){
                if(event.type == SDL_QUIT){
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return 0;
                }
                if (event.type == SDL_KEYDOWN){
                    anykey = true;
                    break;
                }
            }
        }
    }


    //Uninit everything
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}