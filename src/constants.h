#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }

#define FALSE 0
#define TRUE 1

#define ERROR -1
#define OK 0

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FPS 120
#define FRAME_TARGET_TIME (1000 / FPS)

#define BALL_X_VELOCITY 180
#define BALL_Y_VELOCITY 180
#define PADDLE_VELOCITY 300