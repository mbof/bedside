/* Bedside main routine. */

#include <Magick++.h>
#include <chrono>
#include <getopt.h>
#include <iostream>
#include <rgbmatrix/led-matrix.h>
#include <signal.h>
#include <thread>

#include "model.h"
#include "render.h"

#define WIDTH 32
#define HEIGHT 32
#define MAX_FRAMERATE 25
#define MIN_MS_PER_FRAME (1000 / MAX_FRAMERATE)

static bool interrupt_received = false;

static void handle_interrupt(int signal) { interrupt_received = true; }

int main(int argc, char *argv[]) {

  signal(SIGTERM, handle_interrupt);
  signal(SIGINT, handle_interrupt);

  // Set up default options
  int alarm_hours = 7;
  int alarm_minutes = 0;
  const char *weather_forecast = nullptr;

  // Parse command line options
  for (;;) {
    switch (getopt(argc, argv, "h:m:w:")) {
    case 'h':
      alarm_hours = atoi(optarg);
      continue;
    case 'm':
      alarm_minutes = atoi(optarg);
      continue;
    case 'w':
      weather_forecast = optarg;
      continue;
    case -1:
      break;
    }
    break;
  }

  BedsideModel model(weather_forecast);
  model.setAlarm(alarm_hours, alarm_minutes);

  Magick::InitializeMagick(NULL);
  BedsideRenderer bedsideRenderer(model);
  if (bedsideRenderer.init()) {
    std::cerr << "Could not initialize renderer" << std::endl;
    return 1;
  }

  int frame = 0;
  std::chrono::time_point t0 = std::chrono::system_clock::now();
  while (!interrupt_received) {
    std::chrono::time_point t1 = std::chrono::system_clock::now();
    bedsideRenderer.render();
    bedsideRenderer.vsync();
    frame += 1;
    std::this_thread::sleep_until(t1 +
                                  std::chrono::milliseconds{MIN_MS_PER_FRAME});
    std::chrono::time_point t2 = std::chrono::system_clock::now();
    int t0_t2_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t0).count();
    if (t0_t2_ms > 5000) {
      std::cerr << "Framerate: " << ((double)frame / (t0_t2_ms / 1000.0))
                << std::endl;
      t0 = t2;
      frame = 0;
    }
  }

  return 0;
}

// vim: ts=2 sw=2 et
