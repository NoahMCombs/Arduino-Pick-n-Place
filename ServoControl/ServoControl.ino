#include <Servo.h>
#include <math.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

const int ledPin = 13; // the pin that the LED is attached to
const byte buffSize = 40;
unsigned int inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;
byte coordinates[3];

void setup() {
  //attach servos
  servo1.attach(9);
  servo2.attach(10);
  servo3.attach(11);
  servo4.attach(12);
  
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  getDataFromPC();
  if(newDataFromPC){
    sendSuspendCmd();
    moveServo();
    sendEnableCmd();
    sendCoordinatesToPC();
    newDataFromPC = false;
  }
}


void sendSuspendCmd(){
  // send the suspend-true command
  Serial.println("<S1>");
}
void sendEnableCmd(){
  // send the suspend-false command
  Serial.println("<S0>");
}
void sendCoordinatesToPC(){
  // send the point data to the PC
  Serial.print("<P");
  Serial.print(coordinates[0]);
  Serial.print(",");
  Serial.print(coordinates[1]);
  Serial.print(",");
  Serial.print(coordinates[2]);
  Serial.println(">");
  delay(2000);
}
// alternative to the readBytes function:
void getDataFromPC() {
  // receive data from PC and save it into inputBuffer
  if(Serial.available() > 0) {
    char x = Serial.read();
    // the order of these IF clauses is significant
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      coordinates[0] = inputBuffer[0];;
    }
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }
    if (x == startMarker) {
      bytesRecvd = 0;
      readInProgress = true;
    }
  }
}

//grabs shape, drops it in correct pile, moves back to center
void moveServo()
{
  double distance;
  double angle1;
  double angle2;
  double angle3;
  double angle4;

//calculate angles
  distance = sqrt(square((coordinates[0]*2)-5.5)+ square((coordinates[1]*2)+4.75));
  angle1 = atan((coordinates[0] - 5.5) / (coordinates[1] + 4.75));
  angle2 = acos((distance / 13.5) / 2) - 90;
  angle3 = angle2;
  angle4 = angle1;

//move servos into position to grab the shape
  servo1.write(angle1);
  delay(1000);
  servo2.write(angle2);
  delay(1000);
  servo3.write(angle3);
  delay(1000);
  servo4.write(angle4);
  delay(1000);

  //Turn on magnet

  servo2.write(45);
  delay(1000);
  servo3.write(0);
  delay(1000);

  //determine which pile to drop in
  if(coordinates[2] == 3){
    servo1.write(0);
  }
  else{
    servo1.write(180);
  }
  delay(1000);
  //turn off magnet

  //return to center
  servo1.write(90);
}
