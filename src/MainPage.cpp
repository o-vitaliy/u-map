#include "MainPage.h"

namespace {
const uint32_t CONNECT_TASK_STACK_BYTES = 8192;
}  // namespace

MainPage::MainPage(Display &display, EasyESPConnect &wifiManager,
                   ClockComponent &clockComponent,
                   AlertComponent &alertComponent)
    : display_(display),
      wifiManager_(wifiManager),
      clockComponent_(clockComponent),
      alertComponent_(alertComponent),
      connectStatus_(ConnectStatus::CONNECTING),
      componentsStarted_(false) {}

void MainPage::setup() {
  connectStatus_ = ConnectStatus::CONNECTING;
  componentsStarted_ = false;
  display_.showMessage("Connecting...");
  startConnectTask();
}

void MainPage::startConnectTask() {
  BaseType_t created = xTaskCreate(connectTaskEntry, "wifi-connect",
                                    CONNECT_TASK_STACK_BYTES, this, 1, nullptr);
  if (created != pdPASS) {
    Serial.println("MainPage: failed to create wifi connect task");
    connectStatus_ = ConnectStatus::FAILED;
  }
}

// Runs on a dedicated FreeRTOS task so the blocking tryToConnect() call
// doesn't stall the main loop (buttons, display). Only touches WiFi and
// the atomic status flag -- never the Display, which is not safe to
// drive from two tasks at once.
void MainPage::connectTaskEntry(void *param) {
  MainPage *self = static_cast<MainPage *>(param);

  // tryToConnect() never opens the AP/portal itself -- on failure this
  // just falls back to the "no connection" state, and NoWifiPage's RIGHT
  // button explicitly opens the portal from there.
  bool connected = self->wifiManager_.tryToConnect();
  self->connectStatus_ =
      connected ? ConnectStatus::CONNECTED : ConnectStatus::FAILED;

  vTaskDelete(nullptr);
}

void MainPage::loop() {
  ConnectStatus connectStatus = connectStatus_;

  if (connectStatus == ConnectStatus::CONNECTING) {
    return;
  }

  if (connectStatus == ConnectStatus::FAILED) {
    changeState(AppState::NO_WIFI);
    return;
  }

  if (!componentsStarted_) {
    componentsStarted_ = true;
    clockComponent_.setup();
    alertComponent_.setup();
  }

  clockComponent_.loop();
  alertComponent_.loop();
}

void MainPage::buttonClick(UiAction action) {
  // Settings menu not implemented yet.
}
