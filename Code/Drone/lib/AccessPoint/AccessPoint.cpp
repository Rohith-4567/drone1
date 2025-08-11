#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "ESP32-AP";
const char *password = "12345678"; // Must be at least 8 chars

WebServer server(80); // HTTP port 80

float Kp_roll = 1.0, Ki_roll = 0.0, Kd_roll = 0.0;
float Kp_pitch = 1.0, Ki_pitch = 0.0, Kd_pitch = 0.0;
float throttle = 0.0;

void setupAccessPoint()
{
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.println(WiFi.softAPIP());

  server.on("/", []()
            {
    String html = R"rawliteral(
<html>
<head>
<title>PID Tuning</title>
<script>
let pidFields = [
  "Kp_roll", "Ki_roll", "Kd_roll",
  "Kp_pitch", "Ki_pitch", "Kd_pitch"
];
function sendPIDUpdate(field, value) {
  fetch('/update?' + field + '=' + encodeURIComponent(value))
    .then(response => response.json())
    .then(data => {
      // Optionally show a status or update UI
    });
}
function fetchPID() {
  fetch('/getPID').then(r => r.json()).then(data => {
    for (let key in data) {
      if(document.getElementById(key)) {
        document.getElementById(key).value = data[key];
        if (key === "throttle") {
          document.getElementById('throttleValue').textContent = data[key];
        }
      }
    }
  });
}
window.onload = function() {
  fetchPID();
};
</script>
</head>
<body>
<h1>PID Tuning</h1>
<form onsubmit="return false;">
  Kp_roll: <input type="text" id="Kp_roll" value="" oninput="sendPIDUpdate('Kp_roll', this.value)"><br>
  Ki_roll: <input type="text" id="Ki_roll" value="" oninput="sendPIDUpdate('Ki_roll', this.value)"><br>
  Kd_roll: <input type="text" id="Kd_roll" value="" oninput="sendPIDUpdate('Kd_roll', this.value)"><br>
  Kp_pitch: <input type="text" id="Kp_pitch" value="" oninput="sendPIDUpdate('Kp_pitch', this.value)"><br>
  Ki_pitch: <input type="text" id="Ki_pitch" value="" oninput="sendPIDUpdate('Ki_pitch', this.value)"><br>
  Kd_pitch: <input type="text" id="Kd_pitch" value="" oninput="sendPIDUpdate('Kd_pitch', this.value)"><br>
  <label for="throttle">Throttle:</label>
  <input type="range" id="throttle" min="0" max="100" value="" oninput="sendPIDUpdate('throttle', this.value)">
  <span id="throttleValue"></span><br>
  <script>
  document.getElementById('throttle').addEventListener('input', function() {
    document.getElementById('throttleValue').textContent = this.value;
  });
  </script>
</form>
</body>
</html>
)rawliteral";
    server.send(200, "text/html", html); });

  server.on("/update", []()
            {
    bool updated = false;
    if (server.hasArg("Kp_roll")) { Kp_roll = server.arg("Kp_roll").toFloat(); updated = true; }
    if (server.hasArg("Ki_roll")) { Ki_roll = server.arg("Ki_roll").toFloat(); updated = true; }
    if (server.hasArg("Kd_roll")) { Kd_roll = server.arg("Kd_roll").toFloat(); updated = true; }
    if (server.hasArg("Kp_pitch")) { Kp_pitch = server.arg("Kp_pitch").toFloat(); updated = true; }
    if (server.hasArg("Ki_pitch")) { Ki_pitch = server.arg("Ki_pitch").toFloat(); updated = true; }
    if (server.hasArg("Kd_pitch")) { Kd_pitch = server.arg("Kd_pitch").toFloat(); updated = true; }
    if (server.hasArg("throttle")) { throttle = server.arg("throttle").toFloat(); updated = true; }
    String json = "{";
    json += "\"success\":"; json += (updated ? "true" : "false"); json += ",";
    json += "\"Kp_roll\":" + String(Kp_roll, 6) + ",";
    json += "\"Ki_roll\":" + String(Ki_roll, 6) + ",";
    json += "\"Kd_roll\":" + String(Kd_roll, 6) + ",";
    json += "\"Kp_pitch\":" + String(Kp_pitch, 6) + ",";
    json += "\"Ki_pitch\":" + String(Ki_pitch, 6) + ",";
    json += "\"Kd_pitch\":" + String(Kd_pitch, 6);
    json += ",\"throttle\":" + String(throttle, 6);
    json += "}";
    server.send(200, "application/json", json); });

  server.on("/getPID", []()
            {
    String json = "{";
    json += "\"Kp_roll\":" + String(Kp_roll, 6) + ",";
    json += "\"Ki_roll\":" + String(Ki_roll, 6) + ",";
    json += "\"Kd_roll\":" + String(Kd_roll, 6) + ",";
    json += "\"Kp_pitch\":" + String(Kp_pitch, 6) + ",";
    json += "\"Ki_pitch\":" + String(Ki_pitch, 6) + ",";
    json += "\"Kd_pitch\":" + String(Kd_pitch, 6);
    json += ",\"throttle\":" + String(throttle, 6);
    json += "}";
    server.send(200, "application/json", json); });

  server.begin();
}

void accessPointLoop()
{
  server.handleClient();
}