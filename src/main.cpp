#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "Button.h"
#include "Display.h"
#include "Page.h"
#include "Regions.h"
#if defined(LED_REGION_DEBUG_PAGE)
#include "RegionDebugPage.h"
#elif defined(LED_DEBUG_PAGE)
#include "LEDDebugPage.h"
#else
#include <EasyESPConnect.h>

#include "AlertComponent.h"
#include "ClockComponent.h"
#include "MainPage.h"
#include "NoWifiPage.h"
#include <Consts.h>
#include <esp_bt.h>
#endif

#ifndef ALERTS_API_TOKEN
#define ALERTS_API_TOKEN ""
#endif

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
#if defined(LED_DEBUG_PAGE) || defined(LED_REGION_DEBUG_PAGE)
constexpr size_t LED_COUNT = 26;
#else
constexpr size_t LED_COUNT = Regions::INDEX_COUNT;
#endif

Adafruit_NeoPixel leds(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Europe/Kyiv.
const char *TIMEZONE_INFO = "EET-2EEST,M3.5.0/3,M10.5.0/4";

Button *button1;
Button *button2;
Display *display;
#if defined(LED_REGION_DEBUG_PAGE)
RegionDebugPage *regionDebugPage;
#elif defined(LED_DEBUG_PAGE)
LEDDebugPage *debugPage;
#else
EasyESPConnect *wifiManager;
NoWifiPage *noWifiPage;
MainPage *mainPage;
ClockComponent *clockComponent;
AlertComponent *alertComponent;
#endif

Page *currentPage;

#if !defined(LED_DEBUG_PAGE) && !defined(LED_REGION_DEBUG_PAGE)
void switchToPage(AppState state)
{
  currentPage = (state == AppState::CLOCK) ? static_cast<Page *>(mainPage)
                                           : static_cast<Page *>(noWifiPage);
  currentPage->setup();
}
#endif

void setup()
{
  Serial.begin(115200);
  leds.setBrightness(128);

  pinMode(0, OUTPUT);

   delay(1000);

#if !defined(LED_DEBUG_PAGE) && !defined(LED_REGION_DEBUG_PAGE)
  esp_bt_controller_disable();
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_11dBm);
#endif

  Serial.println("Initial setup");

  display = new Display(OLED_SDA_PIN, OLED_SCL_PIN, OLED_WIDTH, OLED_HEIGHT,
                        OLED_I2C_ADDRESS);
  if (!display->begin())
  {
    delay(1000);
    Serial.println("Display: init failed");
    ESP.restart();
  }

  display->showMessage("Starting");

  button1 = new Button(BUTTON1_PIN);
  button2 = new Button(BUTTON2_PIN);

#if defined(LED_REGION_DEBUG_PAGE)
  regionDebugPage = new RegionDebugPage(*display, leds);
  currentPage = regionDebugPage;
#elif defined(LED_DEBUG_PAGE)
  debugPage = new LEDDebugPage(*display, leds);
  currentPage = debugPage;
#else
  wifiManager = new EasyESPConnect();
  clockComponent = new ClockComponent(*display, TIMEZONE_INFO);
  alertComponent = new AlertComponent(
      "https://api.alerts.in.ua/v1/alerts/"
      "active.json?token=" ALERTS_API_TOKEN,
      15000, leds);
  noWifiPage = new NoWifiPage(*display, *wifiManager);
  mainPage = new MainPage(*display, *wifiManager, *clockComponent, *alertComponent);
#endif

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

#if defined(LED_DEBUG_PAGE) || defined(LED_REGION_DEBUG_PAGE)
  currentPage->setup();
#else
  noWifiPage->stateChangeCallback(switchToPage);
  mainPage->stateChangeCallback(switchToPage);
  // getWiFiIsSaved() only reads stored credentials from NVS, so it's an
  // instant way to rule out the "definitely no wifi" case before paying
  // for an actual (slower) connection attempt in MainPage::setup().
  AppState initialState =
      wifiManager->getWiFiIsSaved() ? AppState::CLOCK : AppState::NO_WIFI;
  switchToPage(initialState);
#endif
}

void loop()
{
  button1->loop();
  button2->loop();
#if !defined(LED_DEBUG_PAGE) && !defined(LED_REGION_DEBUG_PAGE)
  wifiManager->loop();
#endif

  currentPage->loop();
}
