#include <Arduino.h>
#include <Wire.h>    //include arduinos i2c library
#include <ESP32Time.h>
#include <WiFi.h>
#include <Ezo_i2c.h> //include the EZO I2C library from https://github.com/Atlas-Scientific/Ezo_I2c_lib
#include <Ezo_i2c_util.h> //brings in common print statements
#include <SparkFun_ADS1219.h> // Click here to get the library: http://librarymanager/All#SparkFun_ADS1219
#include "Adafruit_MCP9808.h"
#include <ESP_Google_Sheet_Client.h>

// For SD/SD_MMC mounting helper
#include <GS_SDHelper.h>

//esp internal clock
ESP32Time rtc;

//I2C port objects
#define DEFAULT_I2C_PORT Wire
#define SECONDARY_I2C_PORT Wire1

// device addresses
#define fresh_pmp_addr 103
#define empty_pmp_addr 102
#define stock_pmp_addr 101

// wifi cred.
#define WIFI_SSID "Berkeley-IoT"
#define WIFI_PASSWORD "%O'p,nZ3"
// Google Project ID
#define PROJECT_ID "eexperimentalapparatus"
// Service Account's client email
#define CLIENT_EMAIL "experimentalapparatus@eexperimentalapparatus.iam.gserviceaccount.com"
// Service Account's private key
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCyJKqKWS7SXfqH\nvUp/pjbuJSQNiuNeXTxjnFbeipUyjQw8oB+4usW7LZm5GgniWIO7WbU7pUhvnx/w\nwxrWKIJM5wOF6xhQBXv51g7Wh+N9I3gXHhsPU7PfVK1Lh047gjbgid9ZT95Nxc+D\nlBLpPgBvaW703H8T7+NUkd60RMIyS+DwiVEO+TVPjb3S0WPSxfsVm6XhdJ94YPPi\nA4csaPxSsmrK75ogxKOBekjZ+nlPccdG5OV9hPBseV7T/37N5/AM8WX2Zf5ifQmz\nKXd9y11JHJl5W30f1PK3us8vQYA9y6JdSC72xzG9sstU3WvKIWIcT5ttWzXopKux\nvvPmZI9lAgMBAAECggEAKvgtKrHYWWXRyGwgTCoLleXfjGSz808KAfzz9zn2QvtC\nuFznSefDSiL1sXBWcgJnxAkfbgOwNTxst+ZX0tBynZobf3H1q1enT9wSOC0O8mBb\n1zerz0ahXH1QT6eTd0cxdXfMJ/7b4GaWZ2aQKTKLaFRGL0oAXQ7ABuYokdMEBJ1g\nwBN6MBgpAb1zfznu+ymhPT20WrLC6fvuJlGk1dWHtaCKfVABFoqveT52v4oXY2e8\n2lJKQDMTDN5BSb5oviXd8e4OT9jeAMrx3YaX19WW4C0E6Bn+LMG5RTtXpx++GeuB\nN3sEEKLjkYxXfD1BdVoBvjqBOLcwBnqsQkiY0YitAQKBgQD3dMB1RSF1jiabXmFl\nbdpYAfspVLRYyDIBYcDGW8fbn8IXhgtYgwmfFt/zWhEf0kMep55GFA76X3uyZ40j\n9tDAFEwZrM5ecWZtECpJZoLahwo4w0sxZ1SV1nYn2qU/KmuZ6ADUk9tUmEEeyZ7b\nTxdd0TwT82k2ERUwZWVhoNdzwwKBgQC4S0NAQ9hhBS9XXe/Dfm7N+kBUxR8mF9iI\nOtKjJ8gIriQlLrKIgY5aYIfv9g1A6gW3GVSKYj/BMYESYhZUrWWAbZGaDeGBeDLP\nzZzkAtrtrUesB3EZ3MdC2zvk9WXBslAMCYzd00Mdl7ZzZqIZgdlyzLETmjKimbge\nEXQUcIoFtwKBgECJTnhHI0bJYAA9SSFu2PcC7UIGOVK+Q2xg1inlXeZFcBU6fFMF\nOckn6hhJcRXffGnORVT5fj6GEHii0QiEIkU2BfLcQqYW5SOkh6LE/XOIrH3JUR79\nV+4MFJyoRViH87Ab9bt7JESXwMQKyUdhBTMOkqCRZusW4XPRvfMMQ6SJAoGAdXCo\nq3RQmAr4zyDyYE7ah3H4usxe/CRn/PAXOJdqNnXaXMN0S97beyZWmgjfC0XWXIbM\nw962KAPG75MIDe/tZbjs9xHvaVTSdG/fkcFsB9fLe4nGfied5zHT6iSjMprRvcn8\nNYOiGmKKhuGZXM3NZwWfthEmQU3Z8XdH1JqeqeMCgYAwT+91sjixDBHBU5Orypbw\nq7bUGiCvNeoFY1FxH7C5182bmw3eS39m9tonYlkxyEk6djnIMEDVyPaSEaLk/NoC\nWY9NQDaZuRMoF+CkGem7cd1QYEpNntBqN8N9tRcEmIe7xHgL0WAHiVzfLrY2h3T9\nC7Aeezjt9/GcaZg7/E3KyQ==\n-----END PRIVATE KEY-----\n";
// The ID of the spreadsheet where you'll publish the data
const char spreadsheetId[] = "1VZ1ix_fLu-vz5J6pJpVXgVxdQ-Gk8cABXr4Oq334iaQ";

// device objects
Ezo_board fresh_pmp = Ezo_board(fresh_pmp_addr, "fresh");                   //create a pump circuit object, whose address is 103 and name is "PMP_UP". This pump dispenses pH up solution.
Ezo_board empty_pmp = Ezo_board(empty_pmp_addr, "empty");                   //create a pump circuit object, whose address is 103 and name is "PMP_UP". This pump dispenses pH up solution.
Ezo_board stock_pmp = Ezo_board(stock_pmp_addr, "stock");                   //create a pump circuit object, whose address is 103 and name is "PMP_UP". This pump dispenses pH up solution.
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
SfeADS1219ArdI2C myADC;

//Pumps
int dt[] = {900000, 900000, 900000, 900000, 900000, 900000, 900000};
double dv[] = {28, 31, 35, 84, 53, 61};
int now, then, step, interval, offset, fresh=0, empty=0, stock=0;
int totalSteps = sizeof(dt) / sizeof(dt[0]);

//ADC
int bits;
float E,Vo;
//avg
int i=0,ADC0,ADC1,ADC2,ADC3;

// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";
// Variable to save current epoch time
unsigned long epochTime; 
// Timer variables for data writing
unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;

// Token Callback function
void tokenStatusCallback(TokenInfo info){
    if (info.status == token_status_error){
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
    else{
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    }
}

// Example of a while loop with timeout
bool readWithTimeout(int timeout_ms) {
  unsigned long startTime = millis();
  
  // Loop until timeout is reached
  while ((millis() - startTime) < timeout_ms) {
    // Do your operation here
    if (Serial.available() > 0) {
      // Process data
      return true;  // Success
    }
    
    // Optional: add a small delay to prevent CPU hogging
    delay(1);
  }
  
  // If we get here, we timed out
  Serial.println("timeout");
  return false;  // Timeout occurred
}

void parseCmd(){
  String command = Serial.readStringUntil('\n');  // Read until newline character
  command.trim();  // Remove any whitespace
    
  // Process command
  if (command == "help") {
    Serial.println("PMP - forwards pump commands to the atlas scientific dosing pump");  
    Serial.println("  avaiable pump commands:");
    Serial.println("      D,[ml] dispense [this specific volume]");
    Serial.println("      D,* dispense until the stop command is given");
  } else if(command == "fresh_pmp"){
  Serial.print("What is your fresh pump command? ");
    if(readWithTimeout(10000)){
      switch(command[0]){
        case 'd':
          Serial.print("dose command: ");
          break;
      }
      command = Serial.readStringUntil('\n');
      command.trim();  // Remove any whitespace
      Serial.println(command);
      fresh_pmp.send_cmd(command.c_str());
    }
  } else if(command == "stock_pmp"){
  Serial.print("What is your stock pump command? ");
    if(readWithTimeout(10000)){
      command = Serial.readStringUntil('\n');
      command.trim();  // Remove any whitespace
      Serial.println(command);
      stock_pmp.send_cmd(command.c_str());
    }  

  } else if(command == "SKIP"){
    dt[step]=0;
  } else if(command == "OFF"){
    fresh_pmp.send_cmd("X");
    stock_pmp.send_cmd("X");
    empty_pmp.send_cmd("X");
    step=totalSteps;
  } else{
    Serial.println("I do not understand you, type help for help.");
  }
}

int printElecVoltage(ads1219_input_multiplexer_config_t channel){

  myADC.setInputMultiplexer(channel); // Read the voltage between AIN0 and GND
  myADC.startSync(); // Start a single-shot conversion.

  while (myADC.dataReady() == false) // Check if the conversion is complete. This will return true if data is ready.
  {
    delay(10); // The conversion is not complete. Wait a little to avoid pounding the I2C bus.
  }

  myADC.readConversion(); // Read the conversion result from the ADC. Store it internally.
  bits = myADC.getConversionRaw();
  Serial.print(bits);//Serial.print(',');
  //Serial.print(float(bits)*0.000053-0.0282);Serial.print(',');
  //Serial.print(float(bits)*0.000265+0.31);

  return bits;
}

void setup() {

  //Wire = Wire1;
  //Wire.setPins(SDA1, SCL1);
  Wire.begin();                           //start the I2C
  Serial.begin(115200);                     //start the serial communication to the computer

  //PMP.send_cmd_with_num("d,", 10);


  //PMP.send_cmd("I2C,101");

  // // Initialize ADC - this also performs a soft reset
  // while (myADC.begin(Wire, 0x45) == false)
  // {
  //   Serial.println("ADC failed to begin. Please check your wiring! Retrying...");
  //   delay(1000);
  // }

  // myADC.setConversionMode(ADS1219_CONVERSION_SINGLE_SHOT);
  // myADC.setDataRate(ADS1219_DATA_RATE_20SPS);
  // myADC.setVoltageReference(ADS1219_VREF_INTERNAL); // Use the REFP and REFN pins
  // myADC.setGain(ADS1219_GAIN_1);

  // Serial.println("ADC initialized");

  //Temperature Sensor
  // if (!tempsensor.begin(0x18, &Wire)) {
  //   Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
  //   while (1);
  // }

  //Serial.println("Found MCP9808!");

  //tempsensor.setResolution(3); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms

  // //Google Sheets
  // GSheet.printf("ESP Google Sheet Client v%s\n\n", ESP_GOOGLE_SHEET_CLIENT_VERSION);
  
  //Serial.println(WiFi.macAddress());//Printthe MAC address

  // // Connect to Wi-Fi
  // WiFi.setAutoReconnect(true);
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Serial.print("Connecting to Wi-Fi");
  // while (WiFi.status() != WL_CONNECTED) {
  //   Serial.print(".");
  //   delay(1000);
  // }
  // Serial.println();
  // Serial.print("Connected with IP: ");
  // Serial.println(WiFi.localIP());
  // Serial.println();
  // Serial.println(WiFi.macAddress());//Printthe MAC address

  // // Synchronize the device's time using NTP
  // configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  // Serial.println("Waiting for NTP time sync...");
  // time_t now = time(nullptr);
  // while (now < 100000) { // Wait until time is set (timestamp > 100000)
  //   delay(500);
  //   now = time(nullptr);
  // }
  // Serial.println("Time synchronized.");


  // // Use the synchronized time to set the library's system time
  // GSheet.setSystemTime(now);

  // // Set the callback for Google API access token generation status (for debug only)
  // GSheet.setTokenCallback(tokenStatusCallback);

  // // Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
  // GSheet.setPrerefreshSeconds(10 * 60);

  // // Begin the access token generation for Google API authentication
  // GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

  step = 0;
  then = millis();
  interval=millis();
  offset=0;

}



void loop() {

  int now = millis()-offset;
  //bool ready = GSheet.ready();

  if(now-then>dt[step] && step<totalSteps){
    Serial.println("dosing step " + String(step) + " dispensing " + String(dv[step]));
    stock_pmp.send_cmd_with_num("d,-", dv[step]);
    stock+=dv[step];
    step++;
    then = millis()-offset;
  } 
  else if(step==totalSteps){
    //set temperature
    int removal = 522;
    Serial.println("emptying step removing " + String(removal));
    empty_pmp.send_cmd_with_num("d,-", removal);
    empty+=removal;
    step = 0;
    offset = millis()+30000;
    then = millis()-offset;
  }
  else if(now-then==30000 && offset>0){
    int DI = 400;
    Serial.println("adding DI step" + String(400));
    fresh_pmp.send_cmd_with_num("d,-", 400);
    fresh+=DI;
  }

  // if(ready && now-lastTime>timerDelay){
  //   lastTime = millis();

  //   FirebaseJson response;

  //   Serial.println("\nAppend spreadsheet values...");
  //   Serial.println("----------------------------");

  //   FirebaseJson valueRange;
  //   valueRange.add("majorDimension", "COLUMNS");
  //   valueRange.set("values/[0]/[0]", rtc.getTime());
  //   valueRange.set("values/[1]/[0]", step);
  //   valueRange.set("values/[2]/[0]", tempsensor.readTempC());
  //   valueRange.set("values/[3]/[0]", ADC0/i);
  //   valueRange.set("values/[4]/[0]", ADC1/i);
  //   valueRange.set("values/[5]/[0]", ADC2/i);
  //   valueRange.set("values/[6]/[0]", ADC3/i);
  //   ADC0=0;ADC1=0;ADC2=0;ADC3=0;i=0;


    

  //   bool success = GSheet.values.append(&response /* returned response */, spreadsheetId /* spreadsheet Id to append */, "Sheet1!A1" /* range to append */, &valueRange /* data range to append */);
  //   if (success){
  //       response.toString(Serial, true);
  //       valueRange.clear();
  //   }
  //   else{
  //       Serial.println(GSheet.errorReason());
  //   }
  //   Serial.println();
  //   Serial.println(ESP.getFreeHeap());
  // }

  if (Serial.available() > 0) {
    parseCmd();
  }

  //averaging
  if(now-interval>1000){
    Serial.print(rtc.getTime());Serial.print(',');
    Serial.print(interval);Serial.print(',');
    Serial.print(step);Serial.print(',');
    Serial.print(stock);Serial.print(',');
    Serial.print(empty);Serial.print(',');
    Serial.println(fresh);//Serial.print(',');
    //Serial.println(tempsensor.readTempC());
    // ADC0 += printElecVoltage(ADS1219_CONFIG_MUX_SINGLE_0);Serial.print(',');
    // ADC1 += printElecVoltage(ADS1219_CONFIG_MUX_SINGLE_1);Serial.print(',');
    // ADC2 += printElecVoltage(ADS1219_CONFIG_MUX_SINGLE_2);Serial.print(',');
    // ADC3 += printElecVoltage(ADS1219_CONFIG_MUX_SINGLE_3);Serial.println();
    // i += 1;
    //printElecVoltage(ADS1219_CONFIG_MUX_SINGLE_1);Serial.print(',');
    //printElecVoltage(ADS1219_CONFIG_MUX_SINGLE_2);Serial.print(',');
    //printElecVoltage(ADS1219_CONFIG_MUX_SINGLE_3);Serial.print(',');
    //Serial.print(tempsensor.readTempC(), 3);Serial.print(',');
    //Serial.println(step);
    //circ_pmp.send_cmd("d,-*");
    interval=millis()-offset;
  }


}

