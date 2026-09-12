# EasyESPConnect (vendored)

Vendored from https://github.com/4nvesh/EasyESPConnect (MIT licensed, see
LICENSE) so we can add `EasyESPConnect::getWiFiIsSaved()` — an addition not
present upstream that reads the saved SSID from NVS without attempting a
connection, mirroring how the project previously used
`WiFiManager::getWiFiIsSaved()`.

To pick up upstream changes, re-fetch `src/EasyESPConnect.h` and
`src/EasyESPConnect.cpp` from the repo and re-apply the `getWiFiIsSaved()`
addition (see the end of each file).
