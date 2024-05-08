#include <List.hpp>
#include <TimerOne.h>

#define JOSTIC_X A1
#define JOSTIC_Y A0
#define JOSTIC_BUTTON 2

#define X_SIZE 7
#define Y_SIZE 7

int X_PINS[] = { 10, A2, A3, A4, A5, 11, 12 };
int Y_PINS[] = { 9, 3, 4, 5, 6, 7, 8 };

typedef void (*fun)(uint8_t, uint8_t);
const fun X_WRITE[] = {
  digitalWrite,
   analogWrite,
   analogWrite,
   analogWrite,
   analogWrite,
  digitalWrite,
  digitalWrite,
};
const int X_HIGH[] = { 1, 255, 255, 255, 255, 1, 1 };

int DefaultX = 512, DefaultY = 512;

int X = 0, Y = 0;
int NextX = 0, NextY = 0;
int NeedSave = 0;

int needSaveCurrent = 0;
struct Point { uint8_t x, y; };
List<Point> Saved;

unsigned SleepTime = 500, Speed = 512;
unsigned long StartTime = 0;

void setup() {
  for (int i = 0; i < X_SIZE; i++) pinMode(X_PINS[i], OUTPUT);
  for (int i = 0; i < Y_SIZE; i++) pinMode(Y_PINS[i], OUTPUT);

  for (int i = 0; i < X_SIZE; i++)   X_WRITE[i](X_PINS[i], LOW);
  for (int i = 0; i < Y_SIZE; i++) digitalWrite(Y_PINS[i], LOW);

  X_WRITE[0](X_PINS[0], X_HIGH[0]);
  digitalWrite(Y_PINS[0], HIGH);
  
  pinMode(JOSTIC_X     , INPUT);
  pinMode(JOSTIC_Y     , INPUT);
  pinMode(JOSTIC_BUTTON, INPUT);

  DefaultX = analogRead(JOSTIC_X);
  DefaultY = analogRead(JOSTIC_Y);

  Timer1.initialize(100000000);
  Timer1.attachInterrupt(onPressed);
  // attachInterrupt(digitalPinToInterrupt(JOSTIC_BUTTON), onPressed, FALLING);

  Saved.add(Point { 2, 3 });
  Saved.add(Point { 0, 0 });
  Saved.add(Point { 4, 1 });
  Saved.add(Point { 6, 0 });
  Saved.add(Point { 0, 6 });

  Serial.begin(9600);
  Serial.println("Ready");
}

void loop() {
  for (int i = 0; i < Saved.getSize(); i++) {
    auto save = Saved.get(i);
    X_WRITE[save.x](X_PINS[save.x], X_HIGH[save.x]);
    digitalWrite(Y_PINS[save.y], HIGH);
    delay(1);
    X_WRITE[save.x](X_PINS[save.x], LOW);
    digitalWrite(Y_PINS[save.y], LOW);
  }

  X_WRITE[X](X_PINS[X], X_HIGH[X]);
  digitalWrite(Y_PINS[Y], HIGH);
  delay(1);
  X_WRITE[X](X_PINS[X], LOW);
  digitalWrite(Y_PINS[Y], LOW);

  if (millis() - StartTime < SleepTime) return;
  Serial.println("After sleep");

  if (wasMoved()) {
    if (!needSaveCurrent) {
      int index = getIndexOfCurrent();
      if (index != -1) Saved.remove(index);
    }
    needSaveCurrent = 0;
  }

  X = NextX;
  Y = NextY;

  int offsetX = analogRead(JOSTIC_X) - DefaultX;
  int offsetY = analogRead(JOSTIC_Y) - DefaultY;

  Serial.print(offsetX);
  Serial.print(" ");
  Serial.println(offsetY);

  if (offsetX > -50 && offsetX < 50) offsetX = 0;
  if (offsetY > -50 && offsetY < 50) offsetY = 0;

  NextX = X + (offsetX > 0 ) - (offsetX < 0);
  if (NextX < 0)       NextX = X_SIZE - 1;
  if (NextX >= X_SIZE) NextX = 0;
  NextY = Y + (offsetY > 0 ) - (offsetY < 0);
  if (NextY < 0)       NextY = Y_SIZE - 1;
  if (NextY >= Y_SIZE) NextY = 0;

  if (!offsetX && !offsetY) SleepTime = 200;
  else {
    Speed = offsetX + offsetY;
    SleepTime = 1000*1024/Speed;
    if (SleepTime > 1000) SleepTime = 1000;
    if (SleepTime <  400) SleepTime =  400;
  }

  StartTime = millis();
}

void onPressed() {
  for (int i = 0; i < Saved.getSize(); i++) {
    auto save = Saved.get(i);
    if (save.x != X || save.y != Y) continue;
    return;
  }
  Saved.add(Point { X, Y });
  needSaveCurrent = 1;
  Serial.println("Pushed");
}

int getIndexOfCurrent() {
  for (int i = 0; i < Saved.getSize(); i++) {
    auto save = Saved.get(i);
    if (save.x != X || save.y != Y) continue;
    return i;
  }
  return -1;
}

int wasMoved() {
  return X != NextX || Y != NextY;
}