
#include <stdio.h>
#include <DS1302.h>

namespace {

const int kCePin   = 2;  // Chip Enable
const int kIoPin   = 4;  // Input/Output
const int kSclkPin = 5;  // Serial Clock

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);
String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}
}
int rainPin = A5;
int signal = 8;
int button = 7;
int lastState = 0, buttonPushCounter = 0, state=0;
void checkstate() 
{
   int state = digitalRead(button);
   // compare the buttonState to its previous state
   if (state != lastState) {
   // if the state has changed, increment the counter
   if (state == HIGH) {
      buttonPushCounter++;
    } 
    delay(50);
  }
  lastState = state;
}
void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
  Serial.println(buf);
}
void autoplay()
{
  int sensorValue = analogRead(rainPin);
  int percent = map(sensorValue, 244, 594, 100, 0);
  int sum=0, avg;
  for (int i=0; i<5; i++) 
  {
    checkstate();
    delay(100);  
    sum+=sensorValue;
    checkstate();
  }
  avg=sum/5;
  if (percent<=20) digitalWrite(signal, HIGH);
  else if (percent>=60) digitalWrite(signal, LOW);
  Serial.print("Độ ẩm:");
  Serial.println(percent);
}
void manualplay()
{
  checkstate();
  Time T = rtc.time();
  if (T.hr==8||T.hr==16) digitalWrite(signal, HIGH);
  else digitalWrite(signal, LOW);
  printTime();
}
void setup(){
  Serial.begin(9600);
  pinMode(rainPin, INPUT);
  pinMode(signal, OUTPUT);
  pinMode(button, INPUT);
  digitalWrite(signal, LOW);
}
void loop() {
  checkstate();
  if (buttonPushCounter % 2 == 0) autoplay();
  else manualplay();
  int stat = digitalRead(signal);
}