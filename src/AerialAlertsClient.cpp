#include "AerialAlertsClient.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "Regions.h"

AerialAlertsClient::AerialAlertsClient(const char *url, unsigned long pollIntervalMs)
    : url_(url),
      pollIntervalMs_(pollIntervalMs),
      regionsMutex_(xSemaphoreCreateMutex()),
      hasReceivedResponse_(false),
      pollingTask_(nullptr) {}

void AerialAlertsClient::loop() {
  if (pollingTask_ == nullptr) {
    startPollingTask();
  }
}

void AerialAlertsClient::startPollingTask() {
  BaseType_t created = xTaskCreate(pollingTaskEntry, "alerts-poll", 8192, this,
                                  1, &pollingTask_);
  if (created != pdPASS) {
    pollingTask_ = nullptr;
    Serial.println("Aerial alerts: failed to create polling task");
  }
}

void AerialAlertsClient::pollingTaskEntry(void *param) {
  AerialAlertsClient *self = static_cast<AerialAlertsClient *>(param);

  while (true) {
    if (WiFi.status() == WL_CONNECTED) {
      self->poll();
      vTaskDelay(pdMS_TO_TICKS(self->pollIntervalMs_));
    } else {
      vTaskDelay(pdMS_TO_TICKS(250));
    }
  }
}

void AerialAlertsClient::poll() {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  if (!http.begin(client, url_)) {
    Serial.println("Aerial alerts: failed to begin request");
    return;
  }

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    parseResponse(http.getString());
  } else {
    Serial.printf("Aerial alerts: request failed: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void AerialAlertsClient::parseResponse(const String &payload) {
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.printf("Aerial alerts: JSON parse failed: %s\n", err.c_str());
    return;
  }

  JsonArray alerts = doc["alerts"].as<JsonArray>();
  if (alerts.isNull()) {
    Serial.println("Aerial alerts: response has no \"alerts\" array");
    return;
  }

  std::map<String, AlertLevel> updatedRegions;
  for (JsonObject alert : alerts) {
    const char *title = alert["location_title"] | "";
    const char *titleEn = alert["location_title_en"] | "";
    const char *oblast = alert["location_oblast"] | "";
    const char *levelName = alert["alert_level"] | "";

    AlertLevel level;
    if (strcmp(levelName, "red") == 0) {
      level = AlertLevel::RED;
    } else if (strcmp(levelName, "yellow") == 0) {
      level = AlertLevel::YELLOW;
    } else {
      continue;
    }

    const char *regionName = Regions::canonicalNameForName(title);
    if (regionName == nullptr) {
      regionName = Regions::canonicalNameForName(titleEn);
    }
    if (regionName == nullptr) {
      regionName = Regions::canonicalNameForName(oblast);
    }
    if (regionName == nullptr) {
      continue;
    }

    auto existing = updatedRegions.find(String(regionName));
    if (existing == updatedRegions.end() ||
        static_cast<uint8_t>(level) > static_cast<uint8_t>(existing->second)) {
      updatedRegions[String(regionName)] = level;
    }
  }

  const size_t regionCount = updatedRegions.size();
  Serial.printf("Aerial alerts: parsed %u regions\n", regionCount);
  for (const auto &region : updatedRegions) {
    const char *levelName = region.second == AlertLevel::RED ? "red" : "yellow";
    Serial.printf("  %s: %s\n", region.first.c_str(), levelName);
  }

  if (xSemaphoreTake(regionsMutex_, portMAX_DELAY) == pdTRUE) {
    regions_.swap(updatedRegions);
    hasReceivedResponse_ = true;
    xSemaphoreGive(regionsMutex_);
  }
}

void AerialAlertsClient::copyRegionsTo(
  std::map<String, AlertLevel> &destination) const {
  if (xSemaphoreTake(regionsMutex_, portMAX_DELAY) == pdTRUE) {
    destination = regions_;
    xSemaphoreGive(regionsMutex_);
  }
}

bool AerialAlertsClient::hasReceivedResponse() const {
  if (xSemaphoreTake(regionsMutex_, portMAX_DELAY) != pdTRUE) {
    return false;
  }
  bool received = hasReceivedResponse_;
  xSemaphoreGive(regionsMutex_);
  return received;
}

bool AerialAlertsClient::anyAlertActive() const {
  std::map<String, AlertLevel> snapshot;
  copyRegionsTo(snapshot);
  for (const auto &region : snapshot) {
    if (region.second != AlertLevel::NONE) {
      return true;
    }
  }
  return false;
}
