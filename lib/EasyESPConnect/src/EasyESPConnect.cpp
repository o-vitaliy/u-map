#include "EasyESPConnect.h"
#include "EasyESPConnectUI.h"

EasyESPConnect::EasyESPConnect() : _server(80)
{
    _apMode = false;
    _scanStarted = false;

    // Read-write (not read-only): opening read-only fails with
    // ESP_ERR_NVS_NOT_FOUND (logged as a scary [E] line) when the
    // namespace hasn't been created yet, e.g. on a fresh board or right
    // after resetSettings(). Read-write auto-creates it instead.
    _prefs.begin("wifi-creds", false);
}

void EasyESPConnect::log(const String &msg, bool newLine)
{
#ifdef EASY_ESP_DEBUG
    if (newLine)
        Serial.println(msg);
    else
        Serial.print(msg);
#endif
}

void EasyESPConnect::setCustomUI(const String &title, const String &token, const String &themeColour)
{
    if (title.length() > 0)
        _uiTitle = title;
    if (token.length() > 0)
        _uiToken = token;
    if (themeColour.length() > 0)
        _uiColour = themeColour;
}

bool EasyESPConnect::tryToConnect()
{
    _loadCredentials();

    if (_ssid.length() == 0)
    {
        log(F("No saved credentials."));
        return false;
    }

    log(String(F("Attempting to connect to: ")) + _ssid + " " + _pass.c_str());

    const int maxAttempts = 3;
    for (int attempt = 1; attempt <= maxAttempts; attempt++)
    {
        if (_connectToWiFi())
        {
            return true;
        }
        log(String(F("Connect attempt ")) + attempt + F(" failed."));
        if (attempt < maxAttempts)
        {
            delay(1000);
        }
    }
    return false;
}

void EasyESPConnect::startWebPortal(const char *apName)
{
    log(String(F("startWebPortal ")) + _ssid);
    _setupAP(apName);
}

void EasyESPConnect::loop()
{
    if (_apMode)
    {
        _dnsServer.processNextRequest();
        _server.handleClient();
    }
}

void EasyESPConnect::_setupAP(const char *apName)
{
    log(String(F("_setupAP ")) + _ssid);
    _apMode = true;
    WiFi.mode(WIFI_AP);
    log(String(F("_setupAP 2")) + _ssid);
    bool result = WiFi.softAP(apName, "01234567");
    log(String(F("AP Started: ")) + result);
    _setupCaptivePortal();
    log(String(F("AP Started: ")) + apName);
}

void EasyESPConnect::_setupCaptivePortal()
{
    _dnsServer.start(53, "*", WiFi.softAPIP());

    // Handle both GET (showing the page) and POST (saving data) on the root path
    _server.on("/", [this]()
               { _handleRoot(); });

    _server.on("/scan", [this]()
               { _handleScan(); });

    // Fallback for captive portal detection
    _server.onNotFound([this]()
                       { _handleRoot(); });
    _server.begin();
}

void EasyESPConnect::_handleRoot()
{
    // CHECK FOR POST DATA HERE
    if (_server.method() == HTTP_POST)
    {
        // Match these strings exactly with the 'name' attributes in your HTML
        String s = _server.arg("ssid");
        String p = _server.arg("password");

        if (s.length() > 0)
        {
            _saveCredentials(s, p);
            // Use send_P for PROGMEM HTML
            _server.send_P(200, "text/html", success_html);
            log(F("Credentials saved. Restarting..."));
            delay(2000);
            ESP.restart();
            return; // Exit to prevent sending the index page
        }
    }

    // Serve the index page for GET requests
    String html = index_html;
    html.replace(F("%TITLE%"), _uiTitle);
    html.replace(F("%TOKEN%"), _uiToken);
    html.replace(F("%THEME_COLOUR%"), _uiColour);

    _server.sendHeader(F("Cache-Control"), F("no-cache"));
    _server.send(200, "text/html", html);
}

void EasyESPConnect::_handleScan()
{
    if (!_scanStarted)
    {
        WiFi.scanNetworks(true);
        _scanStarted = true;
        _server.send(200, "application/json", "{\"status\":\"scanning\"}");
        return;
    }
    int n = WiFi.scanComplete();
    if (n == WIFI_SCAN_RUNNING)
    {
        _server.send(200, "application/json", "{\"status\":\"scanning\"}");
    }
    else
    {
        String json = "{\"status\":\"done\",\"networks\":[";
        for (int i = 0; i < n; i++)
        {
            json += "\"" + WiFi.SSID(i) + "\"" + (i < n - 1 ? "," : "");
        }
        json += "]}";
        WiFi.scanDelete();
        _scanStarted = false;
        _server.send(200, "application/json", json);
    }
}

bool EasyESPConnect::_connectToWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid.c_str(), _pass.c_str());
    int count = 0;
    while (WiFi.status() != WL_CONNECTED && count < 20)
    {
        delay(500);
        log(".", false);
        count++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        log(String(F("\nConnected! IP: ")) + WiFi.localIP().toString());
        return true;
    }
    log(F("\nFailed to connect."));
    return false;
}

void EasyESPConnect::resetSettings()
{
    _prefs.clear();
    delay(500);
    log(F("Credentials erased. Restarting..."));
    ESP.restart();
}

bool EasyESPConnect::getWiFiIsSaved()
{
    // isKey() first: getString() logs a [E] "NOT_FOUND" line whenever the
    // key itself doesn't exist yet (fresh device / after resetSettings()),
    // even though the default value it returns is still correct.
    return _prefs.isKey("ssid") && _prefs.getString("ssid", "").length() > 0;
}

void EasyESPConnect::_loadCredentials()
{
    _ssid = _prefs.isKey("ssid") ? _prefs.getString("ssid", "") : "";
    _pass = _prefs.isKey("pass") ? _prefs.getString("pass", "") : "";
}

void EasyESPConnect::_saveCredentials(const String &s, const String &p)
{
    _prefs.putString("ssid", s);
    _prefs.putString("pass", p);
}