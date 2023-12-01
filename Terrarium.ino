#include <DS3231.h>
#include <TM1637Display.h>
#include <Wire.h>

// 7-segment display connection pins (Digital Pins)
#define CLK 6
#define DIO 7
TM1637Display display(CLK, DIO);

DS3231 rtc(SDA, SCL);
Time t;
boolean dark = false;
int hourchange = 0;
int BUTTONstate = 0;

void setup() {

  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, INPUT_PULLUP);  //dark at night
  digitalWrite(12, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(9, HIGH);
  Serial.begin(9600);
  // Initialize the rtc object
  Wire.begin();
  rtc.begin();
  display.setBrightness(0x0f);
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(MONDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(23, 01, 20);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(21, 8, 2023);   // Set the date to DD/MM/YYYY
}


void loop() {
  t = rtc.getTime();  // Get data from the DS3231
  int hour = t.hour;
  int minute = t.min;
  int time_to_display = (hour * 100) + minute;

  // Display the time
  display.showNumberDecEx(time_to_display, 0x40, true);

  BUTTONstate = digitalRead(8);

  if (BUTTONstate == LOW) {
    int newMinutes = t.min + 1;
    int newHours = t.hour;
    if (t.min == 59) {
      newMinutes = 0;
      newHours++;
      if (newHours == 24) {
        newHours = 0;
      }
    }
    rtc.setTime(newHours, newMinutes, 00);
    delay(250);
    return;
  };

  // Send date over serial connection
  Serial.print("Date: ");
  Serial.print(t.date, DEC);
  Serial.print("/");
  Serial.print(t.mon, DEC);
  Serial.print("/");
  Serial.print(t.year, DEC);
  Serial.println();

  // Send Day-of-Week and time
  Serial.print("Day of Week: ");
  Serial.print(t.dow, DEC);
  Serial.println();

  Serial.print("Time: ");
  Serial.print(t.hour, DEC);
  Serial.print(":");
  Serial.print(t.min, DEC);
  Serial.print(":");
  Serial.print(t.sec, DEC);
  Serial.println();
  Serial.print("Dark status: ");
  Serial.print(hourchange, DEC);
  Serial.print(BUTTONstate, DEC);
  Serial.println();
  Serial.println("--------------------------------");

  delay(1000);  //Delay is for displaying the time in 1 second interval.



  if (t.hour >= 7 && t.hour <= 21)  // RG LED ; 7:00 - 21:59
  {
    digitalWrite(12, LOW);
  } else {
    digitalWrite(12, HIGH);
  }

  if (t.hour >= 7 && t.hour <= 21)  // B LED ; 7:00 - 2:00
  {
    digitalWrite(11, LOW);
  } else {
    if ((t.hour > 21 /* || t.hour < 2*/) && dark == false) {
      digitalWrite(11, LOW);
    } else {
      digitalWrite(11, HIGH);
    }
  }

  if (t.hour == 4) {
    dark = false;
  }

  if ((t.hour == 7 && t.min == 1 && t.sec < 30) || (t.hour == 22 && t.min == 1 && t.sec < 30))  // Water sprinkler ; 7:01, 22:01, 30 seconds
  {
    digitalWrite(10, LOW);
  } else {
    digitalWrite(10, HIGH);
  }

  if (t.hour >= 7 && t.hour <= 21)  // Heater ; 7:00 - 21:59
  {
    digitalWrite(9, LOW);
  } else {
    digitalWrite(9, HIGH);
  }
}
