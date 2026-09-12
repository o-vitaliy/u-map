#include <Arduino.h>
#include <EasyESPConnect.h>
#include <FastLED.h>

#include "AerialAlertsClient.h"
#include "AppTypes.h"
#include "Button.h"
#include "ClockPage.h"
#include "Display.h"
#include "NoWifiPage.h"
#include "Page.h"
#include <Consts.h>
#include <esp_bt.h>

#define BUTTON1_PIN 5
#define BUTTON2_PIN 6
#define OLED_SDA_PIN 8
#define OLED_SCL_PIN 9
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_I2C_ADDRESS 0x3C
#define NO_GLOBAL_SERIAL true
#define NO_GLOBAL_INSTANCES true

#define LED_PIN 0
#define LED_COUNT 2

CRGB leds[LED_COUNT];

// Europe/Kyiv.
const char *TIMEZONE_INFO = "EET-2EEST,M3.5.0/3,M10.5.0/4";

// AerialAlertsClient *alertsClient;
EasyESPConnect *wifiManager;
Button *button1;
Button *button2;
Display *display;
NoWifiPage *noWifiPage;
ClockPage *clockPage;

Page *currentPage;

void switchToPage(AppState state)
{
  currentPage = (state == AppState::CLOCK) ? static_cast<Page *>(clockPage)
                                           : static_cast<Page *>(noWifiPage);
  currentPage->setup();
}

void setup()
{
  Serial.begin(115200);

   delay(1000);

  // fix wifi
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  
  esp_bt_controller_disable();

 

  Serial.println("Initial setup");

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
  fill_solid(leds, LED_COUNT, CRGB::Green);

  FastLED.show();

  display = new Display(OLED_SDA_PIN, OLED_SCL_PIN, OLED_WIDTH, OLED_HEIGHT,
                        OLED_I2C_ADDRESS);
  if (!display->begin())
  {
    delay(1000);
    Serial.println("Display: init failed");
    ESP.restart();
  }

  display->showMessage("Starting");

  wifiManager = new EasyESPConnect();

  button1 = new Button(BUTTON1_PIN);
  button2 = new Button(BUTTON2_PIN);

  noWifiPage = new NoWifiPage(*display, *wifiManager);
  clockPage = new ClockPage(*display, *wifiManager, TIMEZONE_INFO);
  // alertsClient = new AerialAlertsClient("https://ubilling.net.ua/aerialalerts/", 15000);

  button1->begin();
  button1->onClick([]()
                   { currentPage->buttonClick(UiAction::RIGHT); });
  button1->onLongPress([]()
                       { currentPage->buttonClick(UiAction::OK); });

  button2->begin();
  button2->onClick([]()
                   { currentPage->buttonClick(UiAction::LEFT); });
  button2->onLongPress([]()
                       { currentPage->buttonClick(UiAction::CANCEL); });

  noWifiPage->stateChangeCallback(switchToPage);
  clockPage->stateChangeCallback(switchToPage);

  // getWiFiIsSaved() only reads stored credentials from NVS, so it's an
  // instant way to rule out the "definitely no wifi" case before paying
  // for an actual (slower) connection attempt in ClockPage::setup().
  AppState initialState =
      wifiManager->getWiFiIsSaved() ? AppState::CLOCK : AppState::NO_WIFI;
  switchToPage(initialState);
}

void loop()
{
  button1->loop();
  button2->loop();
  // alertsClient->loop();

  // Services the captive portal (once NoWifiPage has opened it) and the
  // library's own hardware factory-reset pin. Safe to call unconditionally.
  wifiManager->loop();

  currentPage->loop();
}
