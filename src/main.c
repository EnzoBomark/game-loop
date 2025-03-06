#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./constants.h"

typedef struct {
  int left;
  int right;
} Score;

typedef struct {
  int is_paused;
  int is_game_running;
  int last_frame_time;
  Score score;
  SDL_Window* window;
  SDL_Renderer* renderer;
  TTF_Font* font;
} State;

static State state = {
  .is_paused = TRUE,
  .is_game_running = TRUE,
  .last_frame_time = 0,
  .score = {.left = 0, .right = 0},
  .renderer = NULL,
  .window = NULL,
  .font = NULL,
};

struct Text {
  SDL_Texture* texture;
  SDL_Color color;
  float height;
  float width;
  float x;
  float y;
  char* value;
} score_string;

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
  ASSERT(!TTF_Init(), "Error initializing TTF. %s", SDL_GetError());

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

  state.font = TTF_OpenFont("assets/fonts/dogica.ttf", 24);

  ASSERT(state.font, "Error creating TTF Font. %s", TTF_GetError());

}

void setup() {
  SDL_Color white = { 255, 255, 255 };
  score_string.texture = NULL;
  score_string.color = white;
  score_string.height = 0;
  score_string.width = 0;
  score_string.x = 0;
  score_string.y = 0;
  score_string.value = "0 - 0";

  ball.height = 20;
  ball.width = 20;
  ball.x = WINDOW_WIDTH / 2 - ball.width / 2;
  ball.y = WINDOW_HEIGHT / 2 - ball.height / 2;
  ball.x_velocity = 0;
  ball.y_velocity = 0;

  paddle_left.height = 100;
  paddle_left.width = 20;
  paddle_left.x = 30;
  paddle_left.y = WINDOW_HEIGHT / 2 - paddle_left.height / 2;
  paddle_left.x_velocity = 0;
  paddle_left.y_velocity = 0;

  paddle_right.height = 100;
  paddle_right.width = 20;
  paddle_right.x = WINDOW_WIDTH - paddle_right.width - 30;
  paddle_right.y = WINDOW_HEIGHT / 2 - paddle_right.height / 2;
  paddle_right.x_velocity = 0;
  paddle_right.y_velocity = 0;
}

void start() {
  ball.x_velocity = BALL_X_VELOCITY;
  ball.y_velocity = BALL_Y_VELOCITY;
}

void reset() {
  ball.height = 20;
  ball.width = 20;
  ball.x = WINDOW_WIDTH / 2 - ball.width / 2;
  ball.y = WINDOW_HEIGHT / 2 - ball.height / 2;
  ball.x_velocity = 0;
  ball.y_velocity = 0;

  paddle_left.height = 100;
  paddle_left.width = 20;
  paddle_left.x = 30;
  paddle_left.y = WINDOW_HEIGHT / 2 - paddle_left.height / 2;
  paddle_left.x_velocity = 0;
  paddle_left.y_velocity = 0;

  paddle_right.height = 100;
  paddle_right.width = 20;
  paddle_right.x = WINDOW_WIDTH - paddle_right.width - 30;
  paddle_right.y = WINDOW_HEIGHT / 2 - paddle_right.height / 2;
  paddle_right.x_velocity = 0;
  paddle_right.y_velocity = 0;
}

void on_key_down(SDL_Scancode code) {
  switch (code) {
  case SDL_SCANCODE_ESCAPE:
    state.is_game_running = FALSE;
    break;
  case SDL_SCANCODE_SPACE:
    if(state.is_paused) start();
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

int is_collision(
  struct GameObject* a,
  struct GameObject* b
) {
  if (a->y > b->y + b->height) return FALSE;
  if (b->y > a->y + a->height) return FALSE;
  if (a->x > b->x + b->width) return FALSE;
  if (b->x > a->x + a->width) return FALSE;
  return TRUE;
}

void update_ball(
  struct GameObject* ball,
  struct GameObject* paddle_left,
  struct GameObject* paddle_right,
  float delta_time
) {
  ball->x += ball->x_velocity * delta_time;
  ball->y += ball->y_velocity * delta_time;


  if (ball->x < 0) {
    ball->x = 0;
    ball->x_velocity = -ball->x_velocity;
    state.score.right += 1;
    reset();
  }

  if (ball->x + ball->width > WINDOW_WIDTH) {
    ball->x = WINDOW_WIDTH - ball->width;
    ball->x_velocity = -ball->x_velocity;
    state.score.left += 1;
    reset();
  }

  char score_text[16];
  snprintf(score_text, sizeof(score_text), "%d - %d", state.score.left, state.score.right);
  score_string.value = score_text;

  if (ball->y < 0) {
    ball->y = 0;
    ball->y_velocity = -ball->y_velocity;
  }

  if (ball->y + ball->height > WINDOW_HEIGHT) {
    ball->y = WINDOW_HEIGHT - ball->height;
    ball->y_velocity = -ball->y_velocity;
  }

  if (is_collision(ball, paddle_left)) {
    ball->x_velocity = min(fabs(ball->x_velocity * 1.1), BALL_X_VELOCITY * 3);
  }

  if (is_collision(ball, paddle_right)) {
    ball->x_velocity = -min(fabs(ball->x_velocity * 1.1), BALL_X_VELOCITY * 3);
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

  update_ball(&ball, &paddle_left, &paddle_right, delta_time);
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



  SDL_Surface* surface = TTF_RenderText_Solid(state.font, score_string.value, score_string.color);
  ASSERT(surface, "Failed to render text: %s\n", SDL_GetError());

  score_string.width = surface->w;
  score_string.height = surface->h;
  score_string.x = WINDOW_WIDTH / 2 - score_string.width / 2;
  score_string.y = 20;
  score_string.texture = SDL_CreateTextureFromSurface(state.renderer, surface);
  SDL_FreeSurface(surface);

  SDL_Rect score_string_rect = {
    (int)score_string.x,
    (int)score_string.y,
    (int)score_string.width,
    (int)score_string.height,
  };

  SDL_RenderCopy(state.renderer, score_string.texture, NULL, &score_string_rect);

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