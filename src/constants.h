#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define FALSE 0
#define TRUE 1

#define ERROR -1
#define OK 0

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FPS 120
#define FRAME_TARGET_TIME (1000 / FPS)

#define BALL_X_VELOCITY 250
#define BALL_Y_VELOCITY 250
#define PADDLE_VELOCITY 650