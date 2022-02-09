// vim: ts=2 sw=2 et

#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>
#include <SDL_ttf.h>

#include "model.h"

class BedsideRenderer {
public:
  BedsideRenderer(SDL_Renderer *renderer, BedsideModel &model)
      : renderer(renderer), model(model){};
  int init();
  void render();
  void draw_text_at(const char *text, int x, int y);
  void render_background();

private:
  SDL_Renderer *renderer;
  TTF_Font *font;
  BedsideModel &model;
};

#endif
