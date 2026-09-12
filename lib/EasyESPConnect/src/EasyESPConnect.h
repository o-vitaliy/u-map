#ifndef EASY_ESP_CONNECT_H
#define EASY_ESP_CONNECT_H

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <ESPmDNS.h>

// Comment this line to remove all Serial debugging and save Flash/RAM
#define EASY_ESP_DEBUG

class EasyESPConnect {
public:
    EasyESPConnect();

    // Optimized with const String& to save RAM
    void setCustomUI(const String& title, const String& token, const String& themeColour);

    // Vendored: replaces upstream's begin(). Tries the saved credentials
    // (if any) and returns whether that succeeded -- it never opens the
    // AP/portal itself. Returns false immediately if no credentials are
    // saved, or false after a failed connection attempt.
    bool tryToConnect();

    // Vendored: explicitly starts the AP + captive portal so the user can
    // configure WiFi credentials. Call this yourself when tryToConnect()
    // returns false and you want to open the portal (e.g. in response to
    // a button press) -- it is never called automatically.
    void startWebPortal(const char* apName = "EasyESP-Config");

    void loop();
    void resetSettings();

    // Reads the saved SSID from NVS without attempting a connection.
    // Vendored addition, not part of upstream EasyESPConnect.
    bool getWiFiIsSaved();

private:
    bool _apMode;
    bool _scanStarted;
    String _ssid, _pass;

    String _uiTitle = "EasyESP Connect";
    String _uiToken = "1xTARS1HA26AP2";
    String _uiColour = "#DFECEA";

    WebServer _server;
    DNSServer _dnsServer;
    Preferences _prefs;

    void _setupAP(const char* apName);
    void _handleRoot();
    void _handleScan();
    bool _connectToWiFi();
    void _loadCredentials();
    void _saveCredentials(const String& s, const String& p);
    void _setupCaptivePortal();

    // Internal helper for conditional logging
    void log(const String& msg, bool newLine = true);
};

#endif
