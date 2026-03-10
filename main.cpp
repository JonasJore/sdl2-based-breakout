#include "iostream"
#include <SDL.h>
#include <vector>

struct Brick {
  float x;
  float y;
  float height;
  float width;
  bool active;
};

void handleInput(bool &running, bool &moveLeft, bool &moveRight) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      running = false;
    }
    if (event.type == SDL_KEYDOWN) {
      if (event.key.keysym.sym == SDLK_q) {
        running = false;
      }
      if (event.key.keysym.sym == SDLK_LEFT) {
        moveLeft = true;
      }
      if (event.key.keysym.sym == SDLK_RIGHT) {
        moveRight = true;
      }
    }
    if (event.type == SDL_KEYUP) {
      if (event.key.keysym.sym == SDLK_LEFT) {
        moveLeft = false;
      }
      if (event.key.keysym.sym == SDLK_RIGHT) {
        moveRight = false;
      }
    }
  }
}

void update(float &paddleX, float &paddleY, int paddleSpeed, float paddleWidth,
            int windowWidth, bool moveLeft, bool moveRight, float ballSize,
            float &ballX, float &ballVelocityX, float &ballY,
            float &ballVelocityY, float deltaTime, int paddleHeight,
            int windowHeight, std::vector<Brick> &bricks, bool &hasWon) {

  bool allBricksDestroyed = true;
  for (const Brick &brick : bricks) {
    if (brick.active) {
      allBricksDestroyed = false;
      break;
    }
  }

  if (allBricksDestroyed == true) {
    hasWon = true;
  }

  if (!hasWon) {
    ballX += ballVelocityX * deltaTime;
    ballY += ballVelocityY * deltaTime;
  }
  if (ballX < 0) {
    ballX = 0;
    ballVelocityX = -ballVelocityX;
  }
  if (ballX + ballSize > windowWidth) {
    ballX = windowWidth - ballSize;
    ballVelocityX = -ballVelocityX;
  }
  if (ballY < 0) {
    ballY = 0;
    ballVelocityY = -ballVelocityY;
  }
  if (ballY + ballSize > windowHeight) {
    // TODO: clean up and put these setters into reset function...
    ballX = 400;
    ballY = 300;
    ballVelocityX = 200.0f;
    ballVelocityY = -200.0f;
    paddleX = 300.0f;
  }
  if (moveLeft) {
    paddleX -= paddleSpeed * deltaTime;
  }
  if (moveRight) {
    paddleX += paddleSpeed * deltaTime;
  }
  if (paddleX < 0) {
    paddleX = 0;
  }
  if (paddleX + paddleWidth > windowWidth) {
    paddleX = windowWidth - paddleWidth;
  }

  SDL_Rect paddle = {(int)paddleX, (int)paddleY, (int)paddleWidth,
                     (int)paddleHeight};
  SDL_Rect ball = {(int)ballX, (int)ballY, (int)ballSize, (int)ballSize};

  if (SDL_HasIntersection(&paddle, &ball)) {
    float paddleCenterX = paddleX + paddleWidth / 2;
    float ballCenterX = ballX + ballSize / 2;

    float paddleOffset = ballCenterX - paddleCenterX;

    ballVelocityX = paddleOffset * 3;

    ballY = paddleY - ballSize;
    ballVelocityY = -ballVelocityY;
  }
  for (Brick &brickData : bricks) {
    if (!brickData.active) {
      continue;
    }
    SDL_Rect brick = {(int)brickData.x, (int)brickData.y, (int)brickData.width,
                      (int)brickData.height};
    if (SDL_HasIntersection(&brick, &ball)) {
      brickData.active = false;
      ballVelocityY = -ballVelocityY;
      break;
    }
  }
}

void render(SDL_Renderer *renderer, int paddleX, int paddleY, int paddleWidth,
            int paddleHeight, float ballX, float ballY, float ballSize,
            std::vector<Brick> &bricks) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_Rect paddle = {(int)paddleX, (int)paddleY, paddleWidth, paddleHeight};
  SDL_RenderFillRect(renderer, &paddle);

  SDL_Rect ball = {(int)ballX, (int)ballY, (int)ballSize, (int)ballSize};
  SDL_RenderFillRect(renderer, &ball);

  for (const Brick &brickData : bricks) {
    if (brickData.active) {
      SDL_Rect brick = {(int)brickData.x, (int)brickData.y,
                        (int)brickData.width, (int)brickData.height};
      SDL_RenderFillRect(renderer, &brick);
    }
  }

  SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "SDL_Init failed: " << SDL_GetError() << "\n";
    return 1;
  }

  int windowWidth = 800;
  int windowHeight = 600;

  SDL_Window *window =
      SDL_CreateWindow("Breakout", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);

  if (!window) {
    std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer) {
    std::cout << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Game
  bool running = true;
  bool hasWon = false;
  // Paddle
  float paddleX = 300.0f;
  float paddleY = 550.0f;
  int paddleWidth = 120;
  int paddleHeight = 10;
  float paddleSpeed = 300.0f;
  // Ball
  float ballX = 400;
  float ballY = 300;
  float ballSize = 16;
  float ballVelocityY = -200.0f;
  float ballCenterX = ballX + ballSize / 2;
  float ballVelocityX = 5;
  // Mechanic
  bool moveRight = false;
  bool moveLeft = false;
  Uint32 lastTick = SDL_GetTicks();
  // Bricks
  int rows = 5;
  int columns = 8;
  float brickWidth = 80.0f;
  float brickHeight = 25.0f;
  float brickGap = 10.0f;
  float brickStartX = 40.0f;
  float brickStartY = 60.0f;
  std::vector<Brick> bricks;

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      float x = brickStartX + col * (brickWidth + brickGap);
      float y = brickStartY + row * (brickHeight + brickGap);
      Brick brick;
      brick.x = x;
      brick.y = y;
      brick.width = brickWidth;
      brick.height = brickHeight;
      brick.active = true;
      bricks.push_back(brick);
    }
  }

  while (running) {
    Uint32 currentTick = SDL_GetTicks();
    float deltaTime = (currentTick - lastTick) / 1000.0f;
    lastTick = currentTick;

    handleInput(running, moveLeft, moveRight);
    update(paddleX, paddleY, paddleSpeed, paddleWidth, windowWidth, moveLeft,
           moveRight, ballSize, ballX, ballVelocityX, ballY, ballVelocityY,
           deltaTime, paddleHeight, windowHeight, bricks, hasWon);
    render(renderer, paddleX, paddleY, paddleWidth, paddleHeight, ballX, ballY,
           ballSize, bricks);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
