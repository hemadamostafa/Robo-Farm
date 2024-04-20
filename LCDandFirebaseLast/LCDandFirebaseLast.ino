//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
/*
  Make sure your Firebase project's '.read' and '.write' rules are set to 'true'. 
  Ignoring this will prevent the MCU from communicating with the database. 
  For more details- https://github.com/Rupakpoddar/ESP32Firebase 

  Download the Android app from- https://play.google.com/store/apps/details?id=com.rupak.firebaseremote 
*/
#include <DHT.h> // Include the DHT library
#include <ESP32Firebase.h>
#include <WiFi.h>  

// LCD Part and Librarys
 /*#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display */

#define _SSID "zezo"          // Your WiFi SSID
#define _PASSWORD "123456789"      // Your WiFi Password
#define REFERENCE_URL "https://robofarm-d8c24-default-rtdb.firebaseio.com"  // Your Firebase project reference url

/* #define M1A 3 // D5: Output 1 for motor driver
#define M1B 4 // D6: Output 2 for motor driver
#define M2A 5 // D7: Output 3 for motor driver
#define M2B 6 // D8: Output 4 for motor driver

#define TURN_DELAY 100
#define FORWARD_BACKWARD_DELAY 500 */

#define DHT_PIN 2   // Digital pin connected to the DHT sensor
#define DHT_TYPE DHT22 // Type of the DHT sensor

#define ANALOG_PIN 32 // Analog pin connected to the MQ-135 sensor
#define THRESHOLD_PURE 80
#define THRESHOLD_RELATIVELY_PURE 140

#define RL_VALUE 47 //The value of resistor RL is 47K
#define m -0.263 //Enter calculated Slope
#define b 0.42 //Enter calculated intercept
#define Ro 20 //Enter found Ro value
#define MQ_sensor 33 //Sensor is connected to A4


DHT dht(DHT_PIN, DHT_TYPE); // Initialize the DHT sensor

Firebase firebase(REFERENCE_URL);

/*int v1 = 10;
  int v2 = 20;
  int v3 = 30;
  int v4 = 40;*/



//#include <Wire.h> 
//#include <LiquidCrystal_I2C.h>

//LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Resetting
#define resetLeg 4

void setup()
{
    Serial.begin(9600); // Initialize serial communication
  delay(2000); // Wait for display to show info

    // LCD
  /*lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();

  lcd.setCursor(0,0);
   lcd.print("Robo Farm!");
  lcd.setCursor(2,1);
  lcd.print("Ywrobot Arduino!");
   lcd.setCursor(0,2);
  lcd.print("Arduino LCM IIC 2004");
   lcd.setCursor(2,3);
  lcd.print("Power By Ec-yuan!");*/
 

  dht.begin(); // Initialize the DHT sensor

  // pinMode(LED_BUILTIN, OUTPUT);
  // digitalWrite(LED_BUILTIN, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  // Define Reset Leg
  pinMode(resetLeg, OUTPUT);



}


void loop()
{

 // int command = firebase.getInt("cmd/Robot"); // Get data from database.

  // Read data from MQ sensor
  float VRL_1; // Voltage drop across the MQ sensor
  float Rs_1; // Sensor resistance at gas concentration
  float ratio_1; // Define variable for ratio
  
  VRL_1 = analogRead(MQ_sensor) * (5.0 / 1023.0); // Measure the voltage drop and convert to 0-5V
  Rs_1 = ((5.0 * RL_VALUE) / VRL_1) - RL_VALUE; // Use formula to get Rs value
  ratio_1 = Rs_1 / Ro;  // Find ratio Rs/Ro

  float ppm = pow(10, ((log10(ratio_1) - b) / m)); // Use formula to calculate ppm

  // Read data from DHT sensor
  float temperature = dht.readTemperature(); // Read temperature in Celsius
  float humidity = dht.readHumidity(); // Read humidity

  /*   Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" °C,  Humidity: ");
  Serial.print(humidity);
  Serial.print("%");
  Serial.print("   NH3(ppm) = "); // Display ammonia in ppm
  Serial.print(ppm); */


  // Air quality based on MQ sensor
  int airQuality = analogRead(ANALOG_PIN); // Read the analog value from the MQ-135 sensor

  /*   Serial.print(",  Air Quality: ");
  Serial.print(airQuality);
  if (airQuality < THRESHOLD_PURE) {
    Serial.println(" (The air is clear)");
  } else if (airQuality >= THRESHOLD_PURE && airQuality < THRESHOLD_RELATIVELY_PURE) {
    Serial.println(" (the air is polluted)");
  } else {
    Serial.println(" (The air is toxic)");
  } */

  // Zyad touch
  firebase.setFloat("FarmData/ammonia", ppm);//ppm
  if (airQuality < THRESHOLD_PURE)
  {  
    firebase.setString("FarmData/gas", "The air is clear");//airQuality
  }

else if (airQuality >= THRESHOLD_PURE && airQuality < THRESHOLD_RELATIVELY_PURE) 
  {
    firebase.setString("FarmData/gas", "the air is polluted");//airQuality
  }
else 
 {
    firebase.setString("FarmData/gas", "The air is toxic");//airQuality
 }
  //firebase.setFloat("FarmData/gas", int(airQuality));//airQuality
  firebase.setFloat("FarmData/humidty", int(humidity)); //humidity
  firebase.setFloat("FarmData/temp", int(temperature));

  int resetting = firebase.getInt("Trigger/Situation");

  if(resetting == 1){
    digitalWrite(resetLeg,HIGH);
    delay(1000);
    digitalWrite(resetLeg,LOW);
    firebase.setInt("Trigger/Situation",0);
  }

  Serial.println(ppm);
  Serial.println(airQuality);
  Serial.println(temperature);
  Serial.println(humidity); 

  // LCD Printing
/*    lcd.setCursor(0,0);
  String temp = "temp:";
  String humed = "Humid:";
  lcd.print("Hi");
  lcd.setCursor(1,0);
  lcd.print("Test");   */



  /*
  for(int x;x<20;x++){
     lcd.setCursor(x,0);
  lcd.print("Hello, world!");
  }*/

  delay(4000);
}
