//antares
#include <AntaresESP8266MQTT.h>     // Inisiasi library MQTT Antares
#define ACCESSKEY "eaa004f588aae95e:3a0abb142e3eafdd" //define accesskey
#define WIFISSID "rime"//define SSID name
#define PASSWORD "senyumdulu"     // Ganti dengan password WiFi anda
#define projectName "NodeMCU_MQTT" //define project name
#define deviceName "Sensor"    // Ganti dengan device Antares yang telah dibuat
AntaresESP8266MQTT antares(ACCESSKEY);    // Buat objek antares

//water sensor
#define POWER_PIN D7
#define SIGNAL_PIN A0
#define SENSOR_MIN 0
#define SENSOR_MAX 521

//ultrasonic
#define echoPin D5 // attach pin D5 to pin Echo of HC-SR04
#define trigPin D6 // attach trigpin D6 to pin trig of HC-SR04

int value = 0; // variable to store the sensor value
int level = 0; // variable to store the water level
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

const int dasar_air = 8;

//buzzer
const int buzzer = D4;

void setup() {
  //Antares
  Serial.begin(115200);     // Buka komunikasi serial dengan baudrate 115200
  antares.setDebug(true);   // Nyalakan debug. Set menjadi "false" jika tidak ingin pesan-pesan tampil di serial monitor
  antares.wifiConnection(WIFISSID,PASSWORD);  // Mencoba untuk menyambungkan ke WiFi
  antares.setMqttServer();  // Inisiasi server MQTT Antares
  
  //ultrasonic
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT 

  //water sensor
  pinMode(POWER_PIN, OUTPUT);   // configure D7 pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF

  //buzzer 
  pinMode(buzzer, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(trigPin, LOW);
  delay(200);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delay(1000);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  distance = dasar_air - distance;
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  //water sensor
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  level = map(value, SENSOR_MIN, SENSOR_MAX, 0, 4); // 3 levels
  Serial.print("Water level: ");
  Serial.println(level);

  delay(1000);
  if (level == 2 && distance >= 5){
  digitalWrite(buzzer, HIGH); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  digitalWrite(buzzer, LOW);
  delay(1000);    
}
  
  //antares
  antares.checkMqttConnection();
  // Tambahkan variabel ke penampungan data sementara
  antares.add("Jarak (cm)", distance);
  antares.add("Level siaga", level);
  
  // Publish data ke database Antares dan juga broker MQTT Antares
  antares.publish(projectName, deviceName);
  delay(10000);
}
