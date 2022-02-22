// vim: ts=2 sw=2 et

#include <Magick++.h>
#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <list>
#include <unistd.h>

#include "model.h"
#include "render.h"

#define FONT_PATH "p3x5.ttf"
#define FONT_SIZE 5

int BedsideRenderer::init() {

  rgb_matrix::RGBMatrix::Options matrix_options;
  matrix_options.hardware_mapping = "adafruit-hat-pwm";
  matrix_options.disable_hardware_pulsing = false;
  if (!matrix_options.Validate(NULL)) {
    std::cerr << "Invalid options." << std::endl;
    return 1;
  }

  rgb_matrix::RuntimeOptions runtime_options;
  runtime_options.drop_privileges = -1;
  matrix =
      rgb_matrix::RGBMatrix::CreateFromOptions(matrix_options, runtime_options);
  if (this->matrix == NULL) {
    std::cerr << "Could not initialize matrix." << std::endl;
    return 1;
  }
  this->canvas = this->matrix->CreateFrameCanvas();

  Magick::Geometry geometry = Magick::Geometry(32, 32);
  Magick::Color color = Magick::Color(0, 255, 0);
  this->image = Magick::Image(geometry, color);

  this->font = Magick::DrawableFont(FONT_PATH);
  this->point_size = Magick::DrawablePointSize(FONT_SIZE);

  return 0;
}

void BedsideRenderer::draw_text_at(const char *text, int x, int y) {
  draw_ops.push_back(Magick::DrawableFillColor(Magick::Color(255, 255, 255)));
  draw_ops.push_back(this->font);
  draw_ops.push_back(this->point_size);
  draw_ops.push_back(Magick::DrawableText(x, y, text));
}

void BedsideRenderer::render_background() {
  // Render a pulsating background
  int amplitude = 80;
  double period = 20;
  if (this->model.getAlarmState()) {
    amplitude = 128;
    period = 0.5;
  }
  auto now = std::chrono::high_resolution_clock::now();
  int millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                   now.time_since_epoch())
                   .count();
  for (size_t x = 0; x < 32; x++) {
    for (size_t y = 0; y < 32; y++) {
      float theta = 2 * M_PI * millis / (period * 1000);
      float dx = (x - 15.5) / 23;
      float dy = (y - 15.5) / 23;
      float dx2 = dx * sin(theta) - dy * cos(theta);
      float dy2 = dx * cos(theta) + dy * sin(theta);
      draw_ops.push_back(Magick::DrawableFillColor(
          Magick::Color((dx2 + 1) * amplitude, (dy2 + 1) * amplitude,
                        (2 - dx2 - dy2) * amplitude / 2)));
      draw_ops.push_back(Magick::DrawablePoint(x, y));
    }
  }
}

void BedsideRenderer::copy_to_canvas() {
  for (size_t y = 0; y < 32; y++) {
    for (size_t x = 0; x < 32; x++) {
      const Magick::Color &c = this->image.pixelColor(x, y);
      this->canvas->SetPixel(x, y, c.redQuantum(), c.greenQuantum(),
                             c.blueQuantum());
    }
  }
}

void BedsideRenderer::render() {
  // graphics
  render_background();
  draw_text_at(model.getTime(), 3, 6);
  draw_text_at(model.getTemperature(), 4, 12);
  draw_text_at(model.getMotd(), 2, 18);
  image.draw(draw_ops);
  copy_to_canvas();
  draw_ops.clear();

  // audio
  if (this->model.getAlarmState() == 1) {
    if (!this->audio_playback_future.valid()) {
      this->model.acknowledgeAlarm();
      this->audio_playback_future = std::async(
          []() { std::system("/usr/bin/mpg321 \"${ALARM_SOUND_FILE}\""); });
    }
  }
}

void BedsideRenderer::vsync() { matrix->SwapOnVSync(canvas); }
