// vim: ts=2 sw=2 et

#ifndef RENDER_H
#define RENDER_H

#include <Magick++.h>
#include <future>
#include <list>
#include <rgbmatrix/led-matrix.h>

#include "model.h"

class BedsideRenderer {
public:
  BedsideRenderer(BedsideModel &model) : model(model){};
  int init();
  void render();
  void draw_text_at(const char *text, int x, int y);
  void render_background();
  void copy_to_canvas();
  void vsync();

private:
  rgb_matrix::RGBMatrix *matrix;
  rgb_matrix::FrameCanvas *canvas;
  Magick::Drawable font;
  Magick::Drawable point_size;
  Magick::Image image;
  std::list<Magick::Drawable> draw_ops;
  BedsideModel &model;
  std::future<void> audio_playback_future;
};

#endif
