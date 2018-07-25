/*
  SD card datalogger

 This example shows how to log data from three analog sensors
 to an SD card using the SD library.

 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created  24 Nov 2010
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <LiquidCrystal.h>
#include "cactus_io_DHT22.h"
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

RTC_DS1307 RTC;

const int rs = 9, en = 8, d4 = 2 , d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define DHT22_PIN A2     // what pin on the arduino is the DHT22 data line connected to
DHT22 dht(DHT22_PIN);

#define ONE_WIRE_BUS A3 // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS); // Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


void setup() {
 
  lcd.begin(16, 2); // set up the LCD's number of columns and rows:
  dht.begin();
  Wire.begin();
  sensors.begin();
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));
  Serial.begin(9600);  // Open serial communications and wait for port to open:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  lcd.setCursor(0,0);
  lcd.print("Inside Temp:");
  
  lcd.setCursor(0,1);
  lcd.print("Tem:    Hum:");

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}

void loop() {

  dht.readHumidity();
  dht.readTemperature();

  lcd.setCursor(0,1); 
  lcd.print(" ");
  lcd.print(dht.temperature_C,1);
  lcd.print("C");
  
  lcd.setCursor(8,1);
  lcd.print(" ");
  lcd.print(dht.humidity,1);
  lcd.print("%");

   sensors.requestTemperatures();  // Send the command to get temperatures
   Serial.print("Temperature is: ");
   Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  
   lcd.setCursor(0,0); 
   lcd.print("InsideTemp: ");
   lcd.print(sensors.getTempCByIndex(0));
   lcd.print("C");
   delay(5000); 

  delay(5000);

  DateTime now = RTC.now();

  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  delay(5000);
  

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
     dataFile.println("Date      ,Time   ,Temperature,Humidity,InsideTemp");
    
      dataFile.print(now.month(), DEC);
      dataFile.print('/');
      dataFile.print(now.day(), DEC);
      dataFile.print('/');
      dataFile.print(now.year(), DEC);
      dataFile.print(" , ");
      dataFile.print(now.hour(), DEC);
      dataFile.print(':');
      dataFile.print(now.minute(), DEC);
      dataFile.print(" , ");

      dataFile.print(dht.temperature_C); //Store date on SD card
      dataFile.print(",     "); //Move to next column using a ","

      dataFile.print(dht.humidity); //Store date on SD card
      dataFile.print(",     "); //Move to next column using a ","

      dataFile.print(sensors.getTempCByIndex(0)); //Store date on SD card
      dataFile.print("."); //Move to next column using a ","
      
      dataFile.println(); //End of Row move to next row
      dataFile.close(); //Close the file
    
    delay(5000);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");

  }
}









