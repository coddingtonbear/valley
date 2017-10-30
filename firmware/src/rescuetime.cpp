#include "rescuetime.h"


const char* rescuetime_ca_cert = \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIEtjCCA56gAwIBAgIQDHmpRLCMEZUgkmFf4msdgzANBgkqhkiG9w0BAQsFADBs\n" \
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
    "d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
    "ZSBFViBSb290IENBMB4XDTEzMTAyMjEyMDAwMFoXDTI4MTAyMjEyMDAwMFowdTEL\n" \
    "MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
    "LmRpZ2ljZXJ0LmNvbTE0MDIGA1UEAxMrRGlnaUNlcnQgU0hBMiBFeHRlbmRlZCBW\n" \
    "YWxpZGF0aW9uIFNlcnZlciBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\n" \
    "ggEBANdTpARR+JmmFkhLZyeqk0nQOe0MsLAAh/FnKIaFjI5j2ryxQDji0/XspQUY\n" \
    "uD0+xZkXMuwYjPrxDKZkIYXLBxA0sFKIKx9om9KxjxKws9LniB8f7zh3VFNfgHk/\n" \
    "LhqqqB5LKw2rt2O5Nbd9FLxZS99RStKh4gzikIKHaq7q12TWmFXo/a8aUGxUvBHy\n" \
    "/Urynbt/DvTVvo4WiRJV2MBxNO723C3sxIclho3YIeSwTQyJ3DkmF93215SF2AQh\n" \
    "cJ1vb/9cuhnhRctWVyh+HA1BV6q3uCe7seT6Ku8hI3UarS2bhjWMnHe1c63YlC3k\n" \
    "8wyd7sFOYn4XwHGeLN7x+RAoGTMCAwEAAaOCAUkwggFFMBIGA1UdEwEB/wQIMAYB\n" \
    "Af8CAQAwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEF\n" \
    "BQcDAjA0BggrBgEFBQcBAQQoMCYwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRp\n" \
    "Z2ljZXJ0LmNvbTBLBgNVHR8ERDBCMECgPqA8hjpodHRwOi8vY3JsNC5kaWdpY2Vy\n" \
    "dC5jb20vRGlnaUNlcnRIaWdoQXNzdXJhbmNlRVZSb290Q0EuY3JsMD0GA1UdIAQ2\n" \
    "MDQwMgYEVR0gADAqMCgGCCsGAQUFBwIBFhxodHRwczovL3d3dy5kaWdpY2VydC5j\n" \
    "b20vQ1BTMB0GA1UdDgQWBBQ901Cl1qCt7vNKYApl0yHU+PjWDzAfBgNVHSMEGDAW\n" \
    "gBSxPsNpA/i/RwHUmCYaCALvY2QrwzANBgkqhkiG9w0BAQsFAAOCAQEAnbbQkIbh\n" \
    "hgLtxaDwNBx0wY12zIYKqPBKikLWP8ipTa18CK3mtlC4ohpNiAexKSHc59rGPCHg\n" \
    "4xFJcKx6HQGkyhE6V6t9VypAdP3THYUYUN9XR3WhfVUgLkc3UHKMf4Ib0mKPLQNa\n" \
    "2sPIoc4sUqIAY+tzunHISScjl2SFnjgOrWNoPLpSgVh5oywM395t6zHyuqB8bPEs\n" \
    "1OG9d4Q3A84ytciagRpKkk47RpqF/oOi+Z6Mo8wNXrM9zwR4jxQUezKcxwCmXMS1\n" \
    "oVWNWlZopCJwqjyBcdmdqEU79OX2olHdx3ti6G8MdOu42vi/hw15UJGQmxg7kVkn\n" \
    "8TUoE6smftX3eg==\n" \
    "-----END CERTIFICATE-----\n";


RescueTime::RescueTime(String api_key_v) {
    api_key = api_key_v;
};

float RescueTime::getProductivityScore() {
    String url = "https://www.rescuetime.com";
    url.concat("/anapi/data?key=");
    url.concat(api_key);
    url.concat("&perspective=interval");
    url.concat("&restrict_kind=efficiency");
    url.concat("&interval=hour");

    Serial.println("Connecting to rescuetime...");

    String response = "";

    HTTPClient rtClient;
    rtClient.begin(url, rescuetime_ca_cert);
    rtClient.addHeader(
        "Accept",
        "application/json"
    );

    int statusCode = rtClient.GET();

    if(statusCode < 200 || statusCode >= 300) {
        Serial.println("Rescuetime responded with " + String(statusCode));
        return -1;
    }

    String data = rtClient.getString();

    DynamicJsonBuffer jsonBuffer(1024);
    JsonObject& root = jsonBuffer.parseObject(data);

    if(!root.success()) {
        Serial.println("Parsing of Rescuetime response failed.");
        Serial.println(data);
        return -2;
    }

    rtClient.end();

    JsonArray& rows = root["rows"];

    float productivityValue = 0;
    int totalSeconds = 0;

    for(int i = rows.size() -1; i >=0; i--) {
        int transferrableSeconds = _min(
            LOOKBACK_SECONDS - totalSeconds,
            rows[i][1].as<int>()
        );
        if (transferrableSeconds <= 0) {
            break;
        }
        float rowProductivityValue = (
            rows[i][4].as<float>() * transferrableSeconds
        );
        productivityValue += rowProductivityValue;
        Serial.print(rows[i][0].as<String>());
        Serial.print(": ");
        Serial.print(transferrableSeconds);
        Serial.print(" * ");
        Serial.print(rows[i][4].as<float>());
        Serial.print(" = ");
        Serial.println(rowProductivityValue);

        totalSeconds += transferrableSeconds;
    }
    float displayedProductivity = (
        productivityValue / (float)totalSeconds
    );
    Serial.print(productivityValue);
    Serial.print(" / ");
    Serial.print(totalSeconds);
    Serial.print(" = ");
    Serial.println(displayedProductivity);

    return displayedProductivity;
}
