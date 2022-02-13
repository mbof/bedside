// vim: ts=2 sw=2 et
// Bedside model. This contains the logical state of the device.

#include <cpr/cpr.h>
#include <ctime>
#include <curl/curl.h>
#include <iostream>
#include <json/json.hpp>

#include "model.h"

using json = nlohmann::json;

char *BedsideModel::getTime() {
  std::time_t now = std::time(nullptr);
  struct tm *timeinfo;
  timeinfo = localtime(&now);
  std::strftime(this->time_str, 6, now % 2 == 0 ? "%I:%M" : "%I.%M", timeinfo);
  return this->time_str;
}

void BedsideModel::setAlarm(int hours, int minutes) {
  this->alarm_hours = hours;
  this->alarm_minutes = minutes;
  std::time_t now = std::time(nullptr);
  struct tm *timeinfo = localtime(&now);
  // Are we already past the alarm time today?
  // TODO: this is probably wrong around daylight savings time
  if (timeinfo->tm_hour > hours ||
      (timeinfo->tm_hour == hours && timeinfo->tm_min > minutes)) {
    timeinfo->tm_mday += 1;
  }
  timeinfo->tm_hour = hours;
  timeinfo->tm_min = minutes;
  timeinfo->tm_sec = 0;
  this->alarm_time = mktime(timeinfo);
}

void BedsideModel::dismissAlarm() {
  this->alarm_dismissed_time = std::time(nullptr);
}

int BedsideModel::getAlarmState() {
  if (this->alarm_time <= this->alarm_dismissed_time) {
    // Alarm never set or already dismissed
    return 0;
  }
  std::time_t now = std::time(nullptr);
  double diff = difftime(now, this->alarm_time);
  if (diff > 0 && diff < 10 * 60) {
    // Ring for 10 minutes (600 seconds) after the alarm time.
    return 1;
  }
  return 0;
}

void BedsideModel::refreshForecast() {
  if (!this->forecast_url) {
    this->forecast_low = this->forecast_high = 0;
    return;
  }
  std::cerr << "Refreshing from " << this->forecast_url << std::endl;
  cpr::GetCallback(
      [this](cpr::Response r) {
        std::cerr << "Status: " << r.status_code << std::endl;
        if (r.status_code != 200) {
          std::cerr << r.error.message << std::endl;
          return;
        }
        std::cerr << r.text << std::endl;
        auto f = json::parse(r.text, nullptr, false);
        if (f.is_discarded()) {
          std::cerr << "No valid JSON found" << std::endl;
        }
        auto periods = f["properties"]["periods"];
        int t1 = periods[0]["temperature"].get<int>();
        int t2 = periods[1]["temperature"].get<int>();
        if (t1 > t2) {
          this->forecast_low = t2;
          this->forecast_high = t1;
        } else {
          this->forecast_low = t1;
          this->forecast_high = t2;
        }
        std::cerr << "Hi / lo: " << this->forecast_high << " / "
                  << this->forecast_low << std::endl;
      },
      cpr::Url{this->forecast_url});
}
