/* vim: ts=2 sw=2 et
 * Bedside main routine. */

#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>

#include "model.h"

#define WIDTH 32
#define HEIGHT 32
#define MAX_FRAMERATE 25
#define MIN_TICKS_PER_FRAME (1000 / MAX_FRAMERATE)
#define FONT_PATH "p3x5.ttf"
#define FONT_SIZE 5

void type_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x,
               int y) {
  SDL_Color textColor = {255, 255, 255, 255};
  SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
  SDL_Texture *textTexture =
      SDL_CreateTextureFromSurface(renderer, textSurface);
  int text_width = textSurface->w;
  int text_height = textSurface->h;
  SDL_FreeSurface(textSurface);
  SDL_Rect renderQuad = {x, y, text_width, text_height};
  SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
  SDL_DestroyTexture(textTexture);
}

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

  if (TTF_Init() < 0) {
    std::cerr << "Could not initialize TTF: " << TTF_GetError() << std::endl;
  }

  TTF_Font *font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
  if (font == NULL) {
    std::cerr << "Could not load font" << TTF_GetError() << std::endl;
    return 1;
  }

  BedsideModel model;

  while (true) {
    int frameStart = SDL_GetTicks();

    SDL_Event e;
    SDL_PollEvent(&e);
    if (e.type == SDL_QUIT) {
      break;
    }

    SDL_SetRenderDrawColor(renderer, 50, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Write the time somewhere
    type_text(renderer, font, model.getTime(), 5, 5);

    SDL_RenderPresent(renderer);

    int frameEnd = SDL_GetTicks();
    if (frameEnd - frameStart < MIN_TICKS_PER_FRAME) {
      SDL_Delay(MIN_TICKS_PER_FRAME - frameEnd + frameStart);
    }
  }
  return 0;
}
