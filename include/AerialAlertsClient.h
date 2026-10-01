#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <map>

class AerialAlertsClient {
 public:
  AerialAlertsClient(const char *url, unsigned long pollIntervalMs);

  // Starts the background polling task. Returns immediately.
  void loop();

  // Fires a request immediately, regardless of the poll interval.
  void poll();

  // Copies the per-region alert state from the last successful poll.
  void copyRegionsTo(std::map<String, bool> &destination) const;

  // True if any region in the last successful poll has alertnow=true.
  bool anyAlertActive() const;

 private:
  const char *url_;
  unsigned long pollIntervalMs_;
  std::map<String, bool> regions_;
  SemaphoreHandle_t regionsMutex_;
  TaskHandle_t pollingTask_;

  void startPollingTask();
  static void pollingTaskEntry(void *param);
  void parseResponse(const String &payload);
};
