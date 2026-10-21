#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


#define TILE_SIZE 50

#define WIDTH_IN_TILES 30
#define HEIGHT_IN_TILES 20

#define WINDOW_WIDTH (WIDTH_IN_TILES * TILE_SIZE)
#define WINDOW_HEIGHT (HEIGHT_IN_TILES * TILE_SIZE)

#define MAX_LENGTH (WIDTH_IN_TILES * HEIGHT_IN_TILES)

#define MOVE_DELAY 300

typedef struct {
    int x;
    int y;
} Cell;

enum Direction{
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
};

void Move_player(Cell player[], enum Direction d) {
    if (d == UP) {
        player[0].y--;
    } else if (d == DOWN) {
        player[0].y++;
    } else if  (d == LEFT) {
        player[0].x--;
    } else if (d == RIGHT) {
        player[0].x++;
    }
}


int main(int argc, char *argv[]) {
    Cell player[MAX_LENGTH];

    int player_length = 2;
    enum Direction direction = UP;

    for (int i = 1; i >= 0; i--) {
        player[i] = (Cell) {
            .x = i,
            .y = HEIGHT_IN_TILES/2,
        };
    }
    
    int timer = SDL_GetTicks();

    if (!SDL_Init(SDL_INIT_VIDEO)) 
    {
        SDL_Log("Init failed");
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("Hello SDL3", 640, 480, 0);

    SDL_Renderer *renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("window failed to create %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderVSync(renderer, 1);

    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // input
        const bool *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
            //
        }
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
            //
        }

        //update
        if (SDL_GetTicks() - timer > MOVE_DELAY) {
            timer = SDL_GetTicks();
            Move_player(player, direction);
        }

        //drawing
        SDL_RenderClear(renderer);

        SDL_FRect background = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &background);
        for (int i = 0; i < player_length; i++) {
            SDL_FRect player_square_part = {
                player[i].x * TILE_SIZE,
                player[i].y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
            };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &player_square_part);
        }



        SDL_RenderPresent(renderer);
        

    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}