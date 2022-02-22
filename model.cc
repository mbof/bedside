// vim: ts=2 sw=2 et
// Bedside model. This contains the logical state of the device.

#include <chrono>
#include <cpr/cpr.h>
#include <cstdio>
#include <ctime>
#include <curl/curl.h>
#include <iostream>
#include <json/json.hpp>

#include "model.h"

// 4 hours
#define MAX_FORECAST_STALENESS_SECONDS (60 * 60 * 4)

// 1 min (for testing only)
// #define MAX_FORECAST_STALENESS_SECONDS 60

using json = nlohmann::json;

std::chrono::seconds zero_seconds{0};

char *BedsideModel::getTime() {
  std::time_t now = std::time(nullptr);
  struct tm *timeinfo;
  timeinfo = localtime(&now);
  std::strftime(this->time_str, 8, now % 2 == 0 ? "%I:%M%p" : "%I.%M%p",
                timeinfo);
  return this->time_str;
}

char *BedsideModel::getTemperature() {
  snprintf(this->temp_str, 12, "%d/%dF", this->forecast_low,
           this->forecast_high);
  this->maybeRefreshForecast();
  return this->temp_str;
}

void BedsideModel::maybeRefreshForecast() {
  if (std::difftime(std::time(nullptr), this->forecast_fetch_time) <
      MAX_FORECAST_STALENESS_SECONDS) {
    return;
  }
  if (forecast_request_future.valid() &&
      forecast_request_future.wait_for(zero_seconds) !=
          std::future_status::ready) {
    return;
  }
  this->refreshForecast();
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
    std::cerr << "Alarm setting for tomorrow" << std::endl;
    timeinfo->tm_mday += 1;
  }
  timeinfo->tm_hour = hours;
  timeinfo->tm_min = minutes;
  timeinfo->tm_sec = 0;
  this->alarm_time = mktime(timeinfo);
  std::cerr << "Alarm set for " << hours << ":" << minutes << std::endl;
}

void BedsideModel::acknowledgeAlarm() {
  this->alarm_acknowledged_time = std::time(nullptr);
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
    if (this->alarm_time <= this->alarm_acknowledged_time) {
      return 2;
    }
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
  this->forecast_request_future = cpr::GetCallback(
      [this](cpr::Response r) {
        std::cerr << "Status: " << r.status_code << std::endl;
        if (r.status_code != 200) {
          std::cerr << r.error.message << std::endl;
          return 1;
        }
        // std::cerr << r.text << std::endl;
        auto f = json::parse(r.text, nullptr, false);
        if (f.is_discarded()) {
          std::cerr << "No valid JSON found" << std::endl;
          return 1;
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
        this->forecast_fetch_time = std::time(nullptr);
        return 0;
      },
      cpr::Url{this->forecast_url},
      // The following header is added as workaround to
      // https://github.com/weather-gov/api/discussions/492
      cpr::Header{{"Feature-Flags", std::to_string(std::time(nullptr))}});
}

const char *BedsideModel::getMotd() { return motd; }
