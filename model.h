// vim: ts=2 sw=2 et

#ifndef MODEL_H
#define MODEL_H

#include <ctime>
#include <future>
#include <string>

class BedsideModel {
public:
  BedsideModel(const char *forecast_url) : forecast_url(forecast_url){};
  char *getTime();
  char *getTemperature();
  void setAlarm(int hours, int minutes);
  void dismissAlarm();
  int getAlarmState();
  void refreshForecast();
  void maybeRefreshForecast();

private:
  char time_str[6];
  char temp_str[12];
  int alarm_hours = 0;
  int alarm_minutes = 0;
  std::time_t alarm_time = 0;
  std::time_t alarm_dismissed_time = 0;
  int forecast_low = 0;
  int forecast_high = 0;
  std::time_t forecast_fetch_time = 0;
  std::future<int> forecast_request_future;
  const char *forecast_url;
};

#endif
