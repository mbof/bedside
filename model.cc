// vim: ts=2 sw=2 et
// Bedside model. This contains the logical state of the device.

#include <ctime>

#include "model.h"

char *BedsideModel::getTime() {
  std::time_t now = std::time(nullptr);
  struct tm *timeinfo;
  timeinfo = localtime(&now);
  std::strftime(this->time_str, 6, now % 2 == 0 ? "%I:%M" : "%I.%M", timeinfo);
  return this->time_str;
}
