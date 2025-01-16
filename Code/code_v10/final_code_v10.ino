#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Replace these with your desired AP credentials
const char* ssid = "glowglowdress";
const char* password = "12345678";

int led = 14;           // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by
bool isOn = false;       // Tracks whether the LED is currently on or off
bool isDisco = false;    // Tracks whether the disco mode is active

unsigned long discoTimer = 0;  // Timer for disco mode
unsigned long fadeTimer = 0;   // Timer for fade mode
unsigned long fadeInterval = 30;  // Interval for fade steps
unsigned long discoInterval = 30;  // Interval for disco steps
bool discoIncreasing = true;  // Tracks the direction of the fade

ESP8266WebServer server(80);

// State machine for fade-to-target
bool fadeToTargetActive = false; // Tracks if a fade-to-target operation is ongoing
int targetBrightness = 0;

void fadeToTarget() {
  if (!fadeToTargetActive) return; // Skip if no fade operation is ongoing

  unsigned long currentMillis = millis();
  if (currentMillis - fadeTimer >= fadeInterval) {
    fadeTimer = currentMillis;

    // Adjust brightness towards the target
    if (brightness < targetBrightness) {
      brightness += fadeAmount;
      if (brightness >= targetBrightness) {
        brightness = targetBrightness;
        fadeToTargetActive = false; // Stop fade
        isOn = (brightness > 0);    // Update on/off state
      }
    } else if (brightness > targetBrightness) {
      brightness -= fadeAmount;
      if (brightness <= targetBrightness) {
        brightness = targetBrightness;
        fadeToTargetActive = false; // Stop fade
        isOn = (brightness > 0);    // Update on/off state
      }
    }

    analogWrite(led, brightness);
    Serial.println(brightness);
  }
}

// Function to stop disco mode
void stopDisco(bool fadeToOff = true) {
  isDisco = false; // Disable disco mode
  Serial.println("Disco mode stopped.");

  // Trigger fade to target
  targetBrightness = fadeToOff ? 0 : 255; // Fade to off or on
  fadeToTargetActive = true;
}

// Non-blocking function to manage disco mode
void updateDiscoMode() {
  if (!isDisco) return;

  unsigned long currentMillis = millis();
  if (currentMillis - discoTimer >= discoInterval) {
    discoTimer = currentMillis;

    // Update brightness
    if (discoIncreasing) {
      brightness += fadeAmount;
      if (brightness >= 255) {
        brightness = 255;
        discoIncreasing = false;
      }
    } else {
      brightness -= fadeAmount;
      if (brightness <= 0) {
        brightness = 0;
        discoIncreasing = true;
      }
    }

    analogWrite(led, brightness);
    Serial.println(brightness);
  }
}

// HTML content
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>PARZENICA</title>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <style>
    body {
      font-family: "Garamond", Times, serif;
      margin: 0;
      padding: 0;
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      height: 100vh;
      background-color: #F5F5F5;
    }


    h1 {
      margin-bottom: 30px;
      color: #800020;
    }

    .button-container {
      display: flex;
      flex-direction: column;
      gap: 20px;
      width: 90%;
      max-width: 400px;
    }

    button {
      font-size: 18px;
      padding: 15px;
      border: none;
      border-radius: 5px;
      background-color: #800020;
      color: #F5F5F5;
      cursor: pointer;
      transition: background-color 0.3s ease, transform 0.2s ease;
      box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
    }

    button:hover {
      background-color: #cc0000;
      transform: scale(1.05);
    }

    button:active {
      background-color: #8b0000;
      transform: scale(0.95);
    }

    .rainbow-button:hover {
      animation: rainbow-flash 1s linear infinite;
      background-size: 300% 300%;
      transform: scale(1.1);
    }

    @keyframes rainbow-flash {
      0% { background-color: #ff0000; }
      16% { background-color: #ff9900; }
      33% { background-color: #ffff00; }
      50% { background-color: #00ff00; }
      66% { background-color: #0000ff; }
      83% { background-color: #4b0082; }
      100% { background-color: #ff0000; }
    }

    .disco-container {
      display: none; /* Hidden by default */
      flex-direction: column;
      gap: 10px;
      margin-top: 20px;
      width: 90%;
      max-width: 400px;
    }

    .footer {
      position: absolute;
      bottom: 20px;
      width: 100%;
      text-align: center;
    }

    .toggle-container {
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 10px;
    }

    .toggle-label {
      font-size: 16px;
      color: #555;
      cursor: pointer;
    }

    .toggle-switch {
      position: relative;
      display: inline-block;
      width: 50px;
      height: 25px;
    }

    .toggle-switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }

    .slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      transition: 0.4s;
      border-radius: 25px;
    }

    .slider:before {
      position: absolute;
      content: "";
      height: 20px;
      width: 20px;
      left: 4px;
      bottom: 2.5px;
      background-color: #F5F5F5;
      transition: 0.4s;
      border-radius: 50%;
    }

    input:checked + .slider {
      background-color: #800020;
    }

    input:checked + .slider:before {
      transform: translateX(25px);
    }
  </style>
</head>
<body>
  <h1>PARZENICA</h1>
  <div class="button-container">
    <button onclick="fetch('/fadeOn')">Fade On</button>
    <button onclick="fetch('/fadeOff')">Fade Off</button>
  </div>

  <div class="disco-container" id="discoControls">
    <button class="rainbow-button" onclick="fetch('/discoDisco')">Disco Mode</button>
    <button onclick="fetch('/stopDisco')">Stop Disco</button>
  </div>

  <div class="footer">
    <div class="toggle-container">
      <label class="toggle-switch">
        <input type="checkbox" id="discoToggle" onclick="toggleDiscoControls()">
        <span class="slider"></span>
      </label>
      <span class="toggle-label">Advanced Features</span>
    </div>
  </div>

  <script>
    function toggleDiscoControls() {
      const discoControls = document.getElementById("discoControls");
      const discoToggle = document.getElementById("discoToggle");
      discoControls.style.display = discoToggle.checked ? "flex" : "none";
    }
  </script>
</body>
</html>

)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", htmlPage);
}

void handleFadeOn() {
  if (isDisco) stopDisco(false); // Stop disco mode and fade to on
  else {
    targetBrightness = 255;
    fadeToTargetActive = true;
  }

  server.send(200, "text/plain", "Fading On");
}

void handleFadeOff() {
  if (isDisco) stopDisco(true); // Stop disco mode and fade to off
  else {
    targetBrightness = 0;
    fadeToTargetActive = true;
  }

  server.send(200, "text/plain", "Fading Off");
}

void handleDiscoDisco() {
  isDisco = true;
  server.send(200, "text/plain", "Disco mode started");
}

void handleStopDisco() {
  stopDisco(true); // Stop disco and fade to off
  server.send(200, "text/plain", "Disco mode stopped and fading off");
}

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/fadeOn", handleFadeOn);
  server.on("/fadeOff", handleFadeOff);
  server.on("/discoDisco", handleDiscoDisco);
  server.on("/stopDisco", handleStopDisco);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  fadeToTarget();
  updateDiscoMode();
}
