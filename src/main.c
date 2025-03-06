#include <stdio.h>
#include <SDL2/SDL.h>
#include "./constants.h"

typedef struct {
  int is_game_running;
  int last_frame_time;
  SDL_Window* window;
  SDL_Renderer* renderer;
} State;

static State state = {
  .is_game_running = TRUE,
  .last_frame_time = 0,
  .renderer = NULL,
  .window = NULL,
};

struct GameObject {
  float height;
  float width;
  float x;
  float y;
  float x_velocity;
  float y_velocity;
} ball, paddle_left, paddle_right;

void initialize_window(void) {
  ASSERT(!SDL_Init(SDL_INIT_EVERYTHING), "Error initializing SDL. %s", SDL_GetError());

  state.window = SDL_CreateWindow(
    NULL,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    SDL_WINDOW_BORDERLESS
  );

  ASSERT(state.window, "Error creating SDL Window. %s", SDL_GetError());

  state.renderer = SDL_CreateRenderer(state.window, -1, 0);

  ASSERT(state.renderer, "Error creating SDL Renderer. %s", SDL_GetError());
}

void setup() {
  ball.height = 20;
  ball.width = 20;
  ball.x = WINDOW_WIDTH / 2 - ball.width / 2;
  ball.y = WINDOW_HEIGHT / 2 - ball.height / 2;
  ball.x_velocity = BALL_X_VELOCITY;
  ball.y_velocity = BALL_Y_VELOCITY;

  paddle_left.height = 100;
  paddle_left.width = 20;
  paddle_left.x = 10;
  paddle_left.y = WINDOW_HEIGHT / 2 - paddle_left.height / 2;
  paddle_left.x_velocity = 0;
  paddle_left.y_velocity = 0;

  paddle_right.height = 100;
  paddle_right.width = 20;
  paddle_right.x = WINDOW_WIDTH - paddle_right.width - 10;
  paddle_right.y = WINDOW_HEIGHT / 2 - paddle_right.height / 2;
  paddle_right.x_velocity = 0;
  paddle_right.y_velocity = 0;
}

void on_key_down(SDL_Scancode code) {
  switch (code) {
    case SDL_SCANCODE_ESCAPE:
      state.is_game_running = FALSE;
      break;
    case SDL_SCANCODE_W:
      paddle_left.y_velocity = -PADDLE_VELOCITY;
      break;
    case SDL_SCANCODE_S:
      paddle_left.y_velocity = PADDLE_VELOCITY;
      break;
    case SDL_SCANCODE_UP:
      paddle_right.y_velocity = -PADDLE_VELOCITY;
      break;
    case SDL_SCANCODE_DOWN:
      paddle_right.y_velocity = PADDLE_VELOCITY;
      break;
    default:
      break;
  }
}

void on_key_up(SDL_Scancode code) {
  switch (code) {
    case SDL_SCANCODE_W:
      paddle_left.y_velocity = 0;
      break;
    case SDL_SCANCODE_S:
      paddle_left.y_velocity = 0;
      break;
    case SDL_SCANCODE_UP:
      paddle_right.y_velocity = 0;
      break;
    case SDL_SCANCODE_DOWN:
      paddle_right.y_velocity = 0;
      break;
    default:
      break;
  }
}

void process_input() {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type) {
    case SDL_QUIT:
      state.is_game_running = FALSE;
      break;
    case SDL_KEYDOWN:
      on_key_down(event.key.keysym.scancode);
      break;
    case SDL_KEYUP:
      on_key_up(event.key.keysym.scancode);
      break;
    default:
      break;
  }
}

void update_ball(struct GameObject* ball, float delta_time) {
  ball->x += ball->x_velocity * delta_time;
  ball->y += ball->y_velocity * delta_time;

  if (ball->x < 0) {
    ball->x = 0;
    ball->x_velocity = -ball->x_velocity;
  }

  if (ball->x + ball->width > WINDOW_WIDTH) {
    ball->x = WINDOW_WIDTH - ball->width;
    ball->x_velocity = -ball->x_velocity;
  }

  if (ball->y < 0) {
    ball->y = 0;
    ball->y_velocity = -ball->y_velocity;
  }

  if (ball->y + ball->height > WINDOW_HEIGHT) {
    ball->y = WINDOW_HEIGHT - ball->height;
    ball->y_velocity = -ball->y_velocity;
  }
}

void update_paddle(struct GameObject* paddle, float delta_time) {
  paddle->x += paddle->x_velocity * delta_time;
  paddle->y += paddle->y_velocity * delta_time;

  if (paddle->x < 0) {
    paddle->x = 0;
  }

  if (paddle->x + paddle->width > WINDOW_WIDTH) {
    paddle->x = WINDOW_WIDTH - paddle->width;
  }

  if (paddle->y < 0) {
    paddle->y = 0;
  }

  if (paddle->y + paddle->height > WINDOW_HEIGHT) {
    paddle->y = WINDOW_HEIGHT - paddle->height;
  }
}

void update() {
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - state.last_frame_time);

  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  float delta_time = (SDL_GetTicks() - state.last_frame_time) / 1000.0f;
  state.last_frame_time = SDL_GetTicks();

  update_ball(&ball, delta_time);
  update_paddle(&paddle_left, delta_time);
  update_paddle(&paddle_right, delta_time);
}


void render() {
  SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
  SDL_RenderClear(state.renderer);

  SDL_Rect ball_rect = {
    (int)ball.x,
    (int)ball.y,
    (int)ball.width,
    (int)ball.height,
  };

  SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(state.renderer, &ball_rect);

  SDL_Rect paddle_left_rect = {
    (int)paddle_left.x,
    (int)paddle_left.y,
    (int)paddle_left.width,
    (int)paddle_left.height,
  };

  SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(state.renderer, &paddle_left_rect);

  SDL_Rect paddle_right_rect = {
    (int)paddle_right.x,
    (int)paddle_right.y,
    (int)paddle_right.width,
    (int)paddle_right.height,
  };

  SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(state.renderer, &paddle_right_rect);

  SDL_RenderPresent(state.renderer);
}

void destroy_window() {
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();
}

void game_loop() {
  setup();

  while (state.is_game_running) {
    process_input();
    update();
    render();
  }
}

int main() {
  initialize_window();

  game_loop();

  destroy_window();

  return OK;
}