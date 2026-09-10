#include <Arduino.h>
#include <WiFiManager.h>

#include "AerialAlertsClient.h"
#include "AppTypes.h"
#include "Button.h"
#include "ClockPage.h"
#include "Display.h"
#include "NoWifiPage.h"
#include "Page.h"

#define BUTTON1_PIN 5
#define BUTTON2_PIN 6
#define OLED_SDA_PIN 8
#define OLED_SCL_PIN 9
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_I2C_ADDRESS 0x3C

// Europe/Kyiv.
const char *TIMEZONE_INFO = "EET-2EEST,M3.5.0/3,M10.5.0/4";

WiFiManager wifiManager;
// AerialAlertsClient alertsClient("https://ubilling.net.ua/aerialalerts/", 15000);
Button button1(BUTTON1_PIN);
Button button2(BUTTON2_PIN);
Display display(OLED_SDA_PIN, OLED_SCL_PIN, OLED_WIDTH, OLED_HEIGHT, OLED_I2C_ADDRESS);

NoWifiPage noWifiPage(display, wifiManager);
ClockPage clockPage(display, TIMEZONE_INFO);

Page *currentPage = &noWifiPage;

void switchToPage(AppState state) {
  currentPage = (state == AppState::CLOCK) ? static_cast<Page *>(&clockPage)
                                            : static_cast<Page *>(&noWifiPage);
  currentPage->setup();
}

void setup() {
  Serial.begin(115200);

  button1.begin();
  button1.onClick([]() { currentPage->buttonClick(UiAction::RIGHT); });
  button1.onLongPress([]() { currentPage->buttonClick(UiAction::OK); });

  button2.begin();
  button2.onClick([]() { currentPage->buttonClick(UiAction::LEFT); });
  button2.onLongPress([]() { currentPage->buttonClick(UiAction::CANCEL); });

  if (!display.begin()) {
    Serial.println("Display: init failed");
  }

  noWifiPage.stateChangeCallback(switchToPage);
  clockPage.stateChangeCallback(switchToPage);

  // getWiFiIsSaved() only reads stored credentials from NVS, so it's an
  // instant way to rule out the "definitely no wifi" case before paying
  // for an actual (slower) connection attempt.
  wifiManager.setEnableConfigPortal(false);
  AppState initialState = AppState::NO_WIFI;
  if (wifiManager.getWiFiIsSaved()) {
    initialState = AppState::CLOCK;
    // Never auto-opens the config portal on failure
    // (setEnableConfigPortal(false)) so a bad/out-of-range saved network
    // falls back to the "no connection" screen instead of blocking here.
    if (!wifiManager.autoConnect("u-lamp-setup")) {
      initialState = AppState::NO_WIFI;
    }
  }
  switchToPage(initialState);
}

void loop() {
  button1.loop();
  button2.loop();
  //alertsClient.loop();

  currentPage->loop();
}
