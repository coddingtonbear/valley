#define RESCUETIME_API_KEY ""

struct wifiPasscode {
    const char* ssid;
    const char* pwd;
};

const wifiPasscode wifiPasscodes[] = {
    (wifiPasscode){"My WiFi SSID", "My Wifi Passcode"},
};
