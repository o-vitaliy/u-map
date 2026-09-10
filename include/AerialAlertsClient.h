#pragma once

#include <Arduino.h>
#include <vector>

struct RegionAlert {
  String name;
  bool alertNow;
};

class AerialAlertsClient {
 public:
  AerialAlertsClient(const char *url, unsigned long pollIntervalMs);

  // Call from the main loop(). Polls the endpoint at most once per
  // pollIntervalMs, and only while WiFi is connected.
  void loop();

  // Fires a request immediately, regardless of the poll interval.
  void poll();

  // Per-region alert state from the last successful poll.
  const std::vector<RegionAlert> &regions() const { return regions_; }

  // True if any region in the last successful poll has alertnow=true.
  bool anyAlertActive() const;

 private:
  const char *url_;
  unsigned long pollIntervalMs_;
  unsigned long lastPollMs_;
  std::vector<RegionAlert> regions_;

  void parseResponse(const String &payload);
};
