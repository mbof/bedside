// vim: ts=2 sw=2 et

#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>

#include "model.h"
#include "render.h"

#define FONT_PATH "p3x5.ttf"
#define FONT_SIZE 5

int BedsideRenderer::init() {
  if (TTF_Init() < 0) {
    std::cerr << "Could not initialize TTF: " << TTF_GetError() << std::endl;
    return 1;
  }

  this->font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
  if (this->font == NULL) {
    std::cerr << "Could not load font" << TTF_GetError() << std::endl;
    return 1;
  }
  return 0;
}

void BedsideRenderer::draw_text_at(const char *text, int x, int y) {
  SDL_Color textColor = {255, 255, 255, 255};
  SDL_Surface *textSurface = TTF_RenderText_Solid(this->font, text, textColor);
  SDL_Texture *textTexture =
      SDL_CreateTextureFromSurface(this->renderer, textSurface);
  int text_width = textSurface->w;
  int text_height = textSurface->h;
  SDL_FreeSurface(textSurface);
  SDL_Rect renderQuad = {x, y, text_width, text_height};
  SDL_RenderCopy(this->renderer, textTexture, NULL, &renderQuad);
  SDL_DestroyTexture(textTexture);
}

void BedsideRenderer::render() {
  SDL_SetRenderDrawColor(this->renderer, 50, 0, 0, 255);
  SDL_RenderClear(this->renderer);

  // Write the time somewhere
  draw_text_at(this->model.getTime(), 5, 5);
}