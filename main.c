#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdbool.h>
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
    for (int i = 1; i < fruit_count; i++) {
        int current_dist = Get_manhattan_distance(player[0], fruits[i]);
        if (current_dist < dist) {
            dist = current_dist;
            closest = fruits[i];
        }
    }
    return closest;
}

int bfs(Cell player[], Cell fruits[], enum Direction directions[]) {
    bool visited[HEIGHT_IN_TILES][WIDTH_IN_TILES] = {{false}};
    Cell parent[HEIGHT_IN_TILES][WIDTH_IN_TILES];
    Cell order[MAX_LENGTH];

    // Mark snake body segments as obstacles (excluding head and the tail tip that will vacate)
    for (int i = 1; i < player_length - 1; i++) {
        if (player[i].x >= 0 && player[i].x < WIDTH_IN_TILES &&
            player[i].y >= 0 && player[i].y < HEIGHT_IN_TILES) {
            visited[player[i].y][player[i].x] = true;
        }
    }

    Cell starting_point = player[0];
    visited[starting_point.y][starting_point.x] = true;

    int front = 0;
    int back = 0;
    order[back++] = starting_point;

    Cell fruit_pos = {-1, -1};
    int possible_directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    while (front < back) {
        Cell current = order[front++];

        if (Check_fruit_collision(current, fruits)) {
            fruit_pos = current;
            break;
        }

        for (int i = 0; i < 4; i++) {
            int nx = current.x + possible_directions[i][0];
            int ny = current.y + possible_directions[i][1];

            if (nx < 0 || nx >= WIDTH_IN_TILES || ny < 0 || ny >= HEIGHT_IN_TILES) {
                continue;
            }
            if (visited[ny][nx]) {
                continue;
            }

            visited[ny][nx] = true;
            parent[ny][nx] = current;
            order[back++] = (Cell){nx, ny};
        }
    }

    // Fallback: if trapped with no path to fruit, pick any safe adjacent tile to stall
    if (fruit_pos.x == -1) {
        for (int i = 0; i < 4; i++) {
            int nx = starting_point.x + possible_directions[i][0];
            int ny = starting_point.y + possible_directions[i][1];

            if (nx >= 0 && nx < WIDTH_IN_TILES && ny >= 0 && ny < HEIGHT_IN_TILES && !visited[ny][nx]) {
                if (possible_directions[i][0] == 1) directions[0] = RIGHT;
                else if (possible_directions[i][0] == -1) directions[0] = LEFT;
                else if (possible_directions[i][1] == 1) directions[0] = DOWN;
                else if (possible_directions[i][1] == -1) directions[0] = UP;
                return 1;
            }
        }
        return 0;
    }

    // Backtrack from the found fruit to the snake's head
    enum Direction path[MAX_LENGTH];
    Cell current = fruit_pos;
    int path_length = 0;

    while (current.x != starting_point.x || current.y != starting_point.y) {
        Cell past = parent[current.y][current.x];
        int dx = current.x - past.x;
        int dy = current.y - past.y;

        if (dx == 1) {
            path[path_length] = RIGHT;
        } else if (dx == -1) {
            path[path_length] = LEFT;
        } else if (dy == 1) {
            path[path_length] = DOWN;
        } else if (dy == -1) {
            path[path_length] = UP;
        }

        path_length++;
        current = past;
    }

    // Reverse the path so directions[0] is the immediate next move
    for (int i = 0; i < path_length; i++) {
        directions[i] = path[path_length - 1 - i];
    }

    return path_length;
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

void Respawn_fruit(Cell player[], Cell fruits[]) {
    if (player_length >= MAX_LENGTH) return;

    for (int f = 0; f < fruit_count; f++) {
        bool valid = false;
        while (!valid) {
            fruits[f].x = rand() % WIDTH_IN_TILES;
            fruits[f].y = rand() % HEIGHT_IN_TILES;
            valid = true;

            for (int i = 0; i < player_length; i++) {
                if (player[i].x == fruits[f].x && player[i].y == fruits[f].y) {
                    valid = false;
                    break;
                }
            }
        }
    }
}

void Reset_game(Cell player[], Cell fruits[], enum Direction *dir) {
    player_length = 3;
    *dir = RIGHT;
    for (int i = 0; i < player_length; i++) {
        player[i] = (Cell){
            .x = player_length - i,
            .y = HEIGHT_IN_TILES / 2,
        };
    }
    Respawn_fruit(player, fruits);
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));

    Cell player[MAX_LENGTH];
    Cell fruits[MAX_LENGTH];

    enum Direction player_direction = RIGHT;
    enum Direction directions[MAX_LENGTH];

    int moves_length = 0;
    int move_counter = 0;

    // Toggle AI vs manual control
    bool isAutomatic = true;

    // Initialize player and fruits
    Reset_game(player, fruits, &player_direction);

    Uint64 timer = SDL_GetTicks();
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
                if (move_counter >= moves_length) {
                    moves_length = bfs(player, fruits, directions);
                    move_counter = 0;
                }
                if (moves_length > 0) {
                    player_direction = directions[move_counter];
                    move_counter++;
                }
                Move_player_head(player, player_direction);
            } else {
                Move_player_head(player, player_direction);
            }

            // Check collision
            if (Check_wall_collision(player) || Check_self_collision(player)) {
                Reset_game(player, fruits, &player_direction);
                moves_length = 0;
                move_counter = 0;
            }

            // Check fruit
            if (Check_fruit_collision(player[0], fruits)) {
                if (player_length < MAX_LENGTH) {
                    player[player_length] = player[player_length - 1];
                    player_length++;
                }
                Respawn_fruit(player, fruits);
                moves_length = 0;
                move_counter = 0;
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