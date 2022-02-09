/* vim: ts=2 sw=2 et
 * Bedside main routine. */

#include <SDL.h>
#include <iostream>

#include "model.h"
#include "render.h"

#define WIDTH 32
#define HEIGHT 32
#define MAX_FRAMERATE 25
#define MIN_TICKS_PER_FRAME (1000 / MAX_FRAMERATE)
#define FONT_PATH "p3x5.ttf"
#define FONT_SIZE 5

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Could not initialize sdl2: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_Window *window;
  SDL_Renderer *renderer;
  if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &window,
                                  &renderer)) {
    std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
    return 1;
  }

  BedsideModel model;
  BedsideRenderer bedsideRenderer(renderer, model);
  if (bedsideRenderer.init()) {
    return 1;
  }

  while (true) {
    int frameStart = SDL_GetTicks();

    SDL_Event e;
    SDL_PollEvent(&e);
    if (e.type == SDL_QUIT) {
      break;
    }

    bedsideRenderer.render();

    SDL_RenderPresent(renderer);

    int frameEnd = SDL_GetTicks();
    if (frameEnd - frameStart < MIN_TICKS_PER_FRAME) {
      SDL_Delay(MIN_TICKS_PER_FRAME - frameEnd + frameStart);
    }
  }
  return 0;
}
