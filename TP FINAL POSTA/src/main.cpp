#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Servo.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <TimeLib.h>








#define WIFI_SSID "Iph"
#define WIFI_PASSWORD "fran1234"
















const double sound_speed = 0.0343; // in cm/microsecs








DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);








Servo myServo;








const int feedHour1 = 17;
const int feedMinute1 = 18;
const int feedHour2 = 17;
const int feedMinute2 = 19;
const int feedHour3 = 17;
const int feedMinute3 = 20;
const int feedHour4 = 17;
const int feedMinute4 = 21;
const int feedHour5 = 17;
const int feedMinute5 = 22;
unsigned long previousMillis = 0;








void send_trigger() {
digitalWrite(TRIG_PIN, HIGH);
unsigned long startTime = micros();
while (micros() - startTime < 10);
digitalWrite(TRIG_PIN, LOW);
}








long get_pulse() {
return pulseIn(ECHO_PIN, HIGH); // in microseconds
}








void handleRoot() {
String html = R"=====(
 <!DOCTYPE html>
 <html>
 <head>
   <meta charset="UTF-8">
   <title>ESP32 Sensor Dashboard</title>
   <link rel="shortcut icon" type="image/x-icon" href="/favicon.ico">
   <style>
     @import url('https://fonts.googleapis.com/css2?family=Roboto:wght@300&display=swap');
     * {
       box-sizing: border-box;
       transition: all 0.3s ease-in-out;
     }
     body {
       font-family: 'Roboto', sans-serif;
       margin: 0;
       padding: 0;
       background: linear-gradient(270deg, #373b44, #4286f4, #f46524);
       background-size: 600% 600%;
       color: #fff;
       height: 100vh;
       display: flex;
       justify-content: center;
       align-items: center;
       text-align: center;
       animation: Gradient 15s ease infinite;
     }
     h1 {
       font-size: 2.5rem;
       margin-bottom: 2rem;
       text-transform: uppercase;
       letter-spacing: 5px;
       animation: fadein 2s;
     }
     .sensor-container {
       display: grid;
       grid-template-columns: repeat(3, 1fr);
       gap: 1rem;
       animation: fadein 2s;
     }
     .sensor-card {
       background: rgba(255, 255, 255, 0.1);
       backdrop-filter: blur(10px);
       border-radius: 15px;
       padding: 2rem;
       animation: popIn 0.6s ease-out forwards;
       cursor: pointer;
       transition: transform 0.3s;
       box-shadow: 0px 10px 30px -10px rgba(0,0,0,0.4);
     }
     .sensor-card:hover {
       transform: scale(1.05);
     }
     .sensor-card h2 {
       font-size: 2rem;
       margin-bottom: 1rem;
       color: #FFD700;
     }
     .sensor-card p {
       font-size: 1.5rem;
       color: #fff;
       margin: 0;
     }
     @keyframes Gradient {
       0% { background-position: 0% 50%; }
       50% { background-position: 100% 50%; }
       100% { background-position: 0% 50%; }
     }
     @keyframes fadein {
       from { opacity: 0; }
       to   { opacity: 1; }
     }
     @keyframes popIn {
       0% { transform: scale(0); }
       80% { transform: scale(1.1); }
       100% { transform: scale(1); }
     }
     @keyframes vibrate {
       0% { transform: scale(1) translate(0); }
       20% { transform: scale(1.1) translate(-2px, 2px); }
       40% { transform: scale(1.1) translate(2px, -2px); }
       60% { transform: scale(1.1) translate(-2px, 2px); }
       80% { transform: scale(1.1) translate(2px, -2px); }
       100% { transform: scale(1) translate(0); }
     }
   </style>
   <meta http-equiv="refresh" content="10" >
 </head>
 <body>
   <h1>TurtleCare</h1>
   <div class="sensor-container">
     <div class="sensor-card">
       <h2>Temperatura</h2>
       <p>)=====";
html += String(dht.readTemperature(), 1) + "°C";
html += R"=====(
       </p>
     </div>
     <div class="sensor-card">
       <h2>Humedad</h2>
       <p>)=====";
html += String(dht.readHumidity(), 1) + " %";
html += R"=====(
       </p>
     </div>
     <div class="sensor-card">
       <h2>Estado de la comida</h2>
       <p>)=====";
send_trigger();
long pulse = get_pulse();
double distance = pulse / 2.0 * sound_speed;
if (distance > 11.5) {
 html += "Es necesario recargar";
} else {
 html += "Todo bien";
}
html += R"=====(
       </p>
     </div>
   </div>
 </body>
 </html>
)=====";


html += R"=====(
     </p>
   </div>
   <form action="/" method="post">
     <button type="submit" name="vaciarcomida" style="background-color: #f46524; color: #fff; padding: 10px 20px; font-size: 1.5rem; border: none; cursor: pointer;">Vaciar comida</button>
   </form>
 </body>
 </html>
)=====";


server.send(200, "text/html", html);
}








void feedTurtle() {
myServo.write(20);//15
delay(300);//300
myServo.write(0);
}




void vaciarComida() {
 myServo.write(90);
 delay(4000);
 myServo.write(0);
}




void loop() {
unsigned long currentMillis = millis();




if (server.hasArg("vaciarcomida")) {
 vaciarComida();
}


if (currentMillis - previousMillis >= 60000) {
 previousMillis = currentMillis;








 if ((hour() == feedHour1 && minute() == feedMinute1) || (hour() == feedHour2 && minute() == feedMinute2) ||(hour() == feedHour3 && minute() == feedMinute3) || (hour() == feedHour4 && minute() == feedMinute4) || (hour() == feedHour5 && minute() == feedMinute5)) {
   feedTurtle();
 }
}








server.handleClient();
}








void setup() {
Serial.begin(115200);
pinMode(DHTPIN, INPUT);
dht.begin();
pinMode(TRIG_PIN, OUTPUT);
pinMode(ECHO_PIN, INPUT);
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED) {
 delay(1000);
 Serial.println("Connecting to WiFi...");
}
Serial.println("Connected to WiFi");
if (!MDNS.begin("esp32")) {
 Serial.println("Error setting up MDNS responder!");
 while(1) {
     delay(1000);
 }
}
Serial.println("mDNS responder started");
server.on("/", handleRoot);
server.begin();
Serial.println("HTTP server started");








myServo.attach(MY_SERVO);
setTime(17, 17, 59, 11, 6, 2023); // This should be replaced with a real-time clock or network time protocol (NTP) to keep the correct time
}

