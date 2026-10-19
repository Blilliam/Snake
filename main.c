#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define SPACE_FROM_EDGES 50

typedef struct {
    float x;
    float y;
    float width;
    float height;
} Player;

typedef struct {
    float x;
    float y;
    float r;
    float dx, dy;
} Ball;

int main(int argc, char *argv[]) {
    char name[] = "will";
    char *last_name = "scudder";
    if (!SDL_Init(SDL_INIT_VIDEO)) 
    {
        SDL_Log("Init failed");
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("Hello SDL3", 640, 480, 0);

    SDL_Renderer *renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("Pong", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("window failed to create %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderVSync(renderer, 1);

    //initialize stuff

    int player_width = 25;
    int player_height = 100;
    int ball_width = 10;
    int ball_speed = 3;
    float ball_angle = SDL_randf() * SDL_PI_F * 2;
    
    Player left = {SPACE_FROM_EDGES, WINDOW_HEIGHT/2 - player_height/2, (float)player_width, (float)player_height};
    Player right = {WINDOW_WIDTH - SPACE_FROM_EDGES - player_width, WINDOW_HEIGHT/2 - player_height/2, (float)player_width, (float)player_height};
    Ball ball = {WINDOW_WIDTH/2 - ball_width/2, WINDOW_HEIGHT/2 - ball_width/2, ball_width/2, SDL_cosf(ball_angle) * ball_speed, SDL_sinf(ball_angle) * ball_speed};

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
            left.y += 5;
        }
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
            left.y -= 5;
        }

        // right tracks

        right.y = ball.y - right.height/2;

        // bounds
        if (left.y < 0) left.y = 0;
        if (left.y + left.height > WINDOW_HEIGHT) left.y = WINDOW_HEIGHT - left.height;

        // ball mvoemnet
        ball.x += ball.dx;
        ball.y += ball.dy;

        // wall collision
        float ball_size = ball.r * 2;
        if (ball.y < 0) {
            ball.y = 0;
            ball.dy = -ball.dy;
        } 
        else if (ball.y + ball_size > WINDOW_HEIGHT) {
            ball.y = WINDOW_HEIGHT - ball_size;
            ball.dy = -ball.dy;
        }

        // left / right bounds
        if (ball.x < 0 || ball.x + ball_size > WINDOW_WIDTH) {
            // reset ball to center
            ball.x = WINDOW_WIDTH / 2.0f - ball.r;
            ball.y = WINDOW_HEIGHT / 2.0f - ball.r;
            ball_angle = SDL_randf() * SDL_PI_F * 2;
            ball.dx = SDL_cosf(ball_angle) * ball_speed;
            ball.dy = SDL_sinf(ball_angle) * ball_speed;
        }

        // RECTANGLE
        SDL_FRect left_square = {left.x, left.y, left.width, left.height};
        SDL_FRect right_square = {right.x, right.y, right.width, right.height};
        SDL_FRect ball_square = {ball.x, ball.y, ball_size, ball_size};

        // Paddle Collision
        SDL_FRect paddles[2] = {left_square, right_square};
        for (int i = 0; i < 2; i++) {
            SDL_FRect intersection;
            if (SDL_GetRectIntersectionFloat(&ball_square, &paddles[i], &intersection)) {
                // choose axis by width vs height overlap 
                if (intersection.w < intersection.h) {
                    // x collision
                    if (ball_square.x < paddles[i].x) {
                        ball.x -= intersection.w; 
                    } else {
                        ball.x += intersection.w; 
                    }
                    ball.dx = -ball.dx;
                } else {
                    // y collision
                    if (ball_square.y < paddles[i].y) {
                        ball.y -= intersection.h; 
                    } else {
                        ball.y += intersection.h;
                    }
                    ball.dy = -ball.dy;
                }
                ball_square.x = ball.x;
                ball_square.y = ball.y;
            }
        }

        // drawing
        SDL_SetRenderDrawColor(renderer, 100, 50, 75, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &left_square);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &right_square);
        SDL_RenderFillRect(renderer, &ball_square);

        SDL_RenderPresent(renderer);
        

    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}