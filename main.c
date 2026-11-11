#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <time.h>

#define TILE_SIZE 50

#define WIDTH_IN_TILES 30
#define HEIGHT_IN_TILES 20

#define WINDOW_WIDTH (WIDTH_IN_TILES * TILE_SIZE)
#define WINDOW_HEIGHT (HEIGHT_IN_TILES * TILE_SIZE)

#define MAX_LENGTH (WIDTH_IN_TILES * HEIGHT_IN_TILES)

#define MOVE_DELAY 150

int player_length = 3;
int fruit_count = 1;

typedef struct {
    int x;
    int y;
} Cell;

enum Direction {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
};

void Move_player_head(Cell player[], enum Direction d) {
    if (d == UP) {
        player[0].y--;
    } else if (d == DOWN) {
        player[0].y++;
    } else if (d == LEFT) {
        player[0].x--;
    } else if (d == RIGHT) {
        player[0].x++;
    }
}

void Move_player_body_part(Cell player[], int place) {
    if (place != 0) {
        player[place].x = player[place - 1].x;
        player[place].y = player[place - 1].y;
    }
}

void Move_player_body(Cell player[]) {
    for (int i = 0; i < player_length; i++) {
        Move_player_body_part(player, player_length - i - 1);
    }
}

bool Check_fruit_collision(Cell player, Cell fruits[]) {
    for (int i = 0; i < fruit_count; i++) {
        if (fruits[i].x == player.x && fruits[i].y == player.y) {
            return true;
        }
    }
    return false;
}

int Get_manhattan_distance(Cell first, Cell second) {
    return abs(first.x - second.x) + abs(first.y - second.y);
}

Cell Get_closest_fruit(Cell player[], Cell fruits[]) {
    int dist = Get_manhattan_distance(player[0], fruits[0]);
    Cell closest = fruits[0];
    for (int i = i; i < fruit_count; i++) {
        if (Get_manhattan_distance(player[0], fruits[i]) < dist) {
            closest = fruits[i];
        }
    }
    return closest;
}

int bfs(Cell player[], Cell fruits[], enum Direction directions[]) {
    Cell goal_fruit = Get_closest_fruit(player, fruits);

    bool visited[HEIGHT_IN_TILES][WIDTH_IN_TILES] = {{false}};
    int counter = 0;
    Cell starting_point = player[0];
    Cell order[MAX_LENGTH];

    visited[starting_point.y][starting_point.x] = true;

    int front = 0;
    int back = 0;
    order[back] = starting_point;
    back++;

    Cell fruit_pos = {-1, -1};

    int possible_directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    Cell parent[HEIGHT_IN_TILES][WIDTH_IN_TILES];

    while (front < back) {
        Cell current = order[front];
        front ++;

        

        if (Check_fruit_collision(current, fruits)) {
            fruit_pos = current;
            break;
        }
        for (int i = 0 ; i < 4; i ++) {
            int dx = possible_directions[i][0];
            int dy = possible_directions[i][1];

            int nx = dx + current.x;
            int ny = ny;

            if (nx < 0 || nx >= WIDTH_IN_TILES) {
                continue;
            }
            if (ny < 0 || ny >= HEIGHT_IN_TILES) {
                continue;
            }
            if (visited[ny][nx]) {
                continue;
            }
            

            order[back] = (Cell) {
                nx,
                ny
            };
            back++;

            parent[ny][nx] = current;

            visited[ny][nx] = true;

        }
        //CHECK FOR PLAYER TAIL COLLISION LATER
        

    }
    enum Direction path[MAX_LENGTH];
    Cell current = goal_fruit;
    
    int i = 0;
    while (current.x != player[0].x && current.y != player[0].y) {
        Cell past = parent[current.y][current.x];
        int dx = past.x - current.x;
        int dy = past.y - current.y;

        if (dx == -1) {
            path[i] = RIGHT;
        }
        if (dx == 1) {
            path[i] = LEFT;
        }
        if (dy == -1) {
            path[i] = UP;
        }
        if (dy == 1) {
            path[i] = DOWN;
        }
        i++;

    }
    //REVERSE path, return length, implement directions to snake movement.

}


bool Check_wall_collision(Cell player[]) {
    if (player[0].x < 0 || player[0].x >= WIDTH_IN_TILES ||
        player[0].y < 0 || player[0].y >= HEIGHT_IN_TILES) {
        return true;
    }
    return false;
}

bool Check_self_collision(Cell player[]) {
    for (int i = 1; i < player_length; i++) {
        if (player[0].x == player[i].x && player[0].y == player[i].y) {
            return true;
        }
    }
    return false;
}

void Respawn_fruit(Cell fruits[]) {
    fruits[0].x = rand() % WIDTH_IN_TILES;
    fruits[0].y = rand() % HEIGHT_IN_TILES;
}

void Reset_game(Cell player[], Cell fruits[], enum Direction *dir) {
    player_length = 3;
    *dir = RIGHT;
    for (int i = 0; i < player_length; i++) {
        player[i] = (Cell) {
            .x = player_length - i,
            .y = HEIGHT_IN_TILES / 2,
        };
    }
    Respawn_fruit(fruits);
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));

    Cell player[MAX_LENGTH];
    Cell fruits[MAX_LENGTH];

    enum Direction player_direction = RIGHT;

    enum Direction directions[] = {
        UP,
        UP,
        RIGHT,
        RIGHT,
        RIGHT,
        RIGHT,
        DOWN,
        DOWN,
        DOWN,
        LEFT,
        LEFT,
        LEFT,
        LEFT,
    }; 

    int moves_length = sizeof(directions) / sizeof(directions[0]);

    int move_counter = 0;

    //IF MANUAL OR NOT
    bool isAutomatic = true;

    // Initialize player and fruits
    Reset_game(player, fruits, &player_direction);

    int timer = SDL_GetTicks();
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("Snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("Window/Renderer failed to create: %s", SDL_GetError());
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

        // Input handling (preventing immediate 180-degree self-collisions on keypress)
        const bool *keys = SDL_GetKeyboardState(NULL);
        if ((keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) && player_direction != UP) {
            player_direction = DOWN;
        }
        if ((keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) && player_direction != DOWN) {
            player_direction = UP;
        }
        if ((keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) && player_direction != RIGHT) {
            player_direction = LEFT;
        }
        if ((keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) && player_direction != LEFT) {
            player_direction = RIGHT;
        }

        // Update game 
        if (SDL_GetTicks() - timer > MOVE_DELAY) {
            timer = SDL_GetTicks();
            
            Move_player_body(player);
            if (isAutomatic) {
                Move_player_head(player, directions[move_counter % moves_length]);
                move_counter++;
            } else {
                Move_player_head(player, player_direction);
            }

            // Check collision
            if (Check_wall_collision(player) || Check_self_collision(player)) {
                Reset_game(player, fruits, &player_direction);
            }

            // Check fruit
            if (Check_fruit_collision(player[0], fruits)) {
                if (player_length < MAX_LENGTH) {
                    player_length++;
                }
                Respawn_fruit(fruits);
            }
        }

        // Drawing
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        // Draw Player
        for (int i = 0; i < player_length; i++) {
            SDL_FRect player_square_part = {
                (float)(player[i].x * TILE_SIZE),
                (float)(player[i].y * TILE_SIZE),
                (float)TILE_SIZE,
                (float)TILE_SIZE
            };
            if (i != 0) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Body
            } else {
                SDL_SetRenderDrawColor(renderer, 200, 0, 100, 255); // Head
            }
            SDL_RenderFillRect(renderer, &player_square_part);
        }

        // Draw Fruits
        for (int i = 0; i < fruit_count; i++) {
            SDL_FRect fruit_square = {
                (float)(fruits[i].x * TILE_SIZE),
                (float)(fruits[i].y * TILE_SIZE),
                (float)TILE_SIZE,
                (float)TILE_SIZE
            };
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &fruit_square);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}