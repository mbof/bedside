/* Bedside main routine. */

#include "rgbmatrix/led-matrix.h"
#include <Magick++.h>
#include <getopt.h>
#include <iostream>
#include <signal.h>

#include "model.h"
#include "render.h"

#define WIDTH 32
#define HEIGHT 32
#define MAX_FRAMERATE 60
#define MIN_TICKS_PER_FRAME (1000 / MAX_FRAMERATE)

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

  while (!interrupt_received) {
    bedsideRenderer.render();
    bedsideRenderer.vsync();
  }

  return 0;
}

// vim: ts=2 sw=2 et
