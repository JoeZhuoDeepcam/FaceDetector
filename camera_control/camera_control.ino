/* Camera control
https://www.arduino.cc/en/Tutorial/StringToIntExample
*/

#include <Servo.h>  //Used to control the Pan/Tilt Servos
#include <WString.h> //provides easy string handling
String readString = String(100);

//These are variables that hold the servo IDs.
char tiltChannel=0, panChannel=1;
String inString="";
int n=0;

//These are the objects for each servo.
Servo servoPan;

//This is a character that will hold data from the Serial port.
char serialChar=0;


int pos=90;
void setup(){
  //servoTilt.attach(2);  //The Tilt servo is attached to pin 2.
  servoPan.attach(3);   //The Pan servo is attached to pin 3.
  //servoTilt.write(90);  //Initially put the servos both
  servoPan.write(pos);      //at 90 degress.
  
  Serial.begin(9600);  //Set up a serial connection for 57600 bps.
}


void loop(){
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char
      // and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string,
    // then the string's value:
    if (inChar == ',') { //for others use ','
      //Serial.print("Value:");
      //Serial.println(inString.toInt());
      //Serial.print("String: ");
      //Serial.println(inString);
      // clear the string for new input:
      n=inString.toInt();
      inString = "";
      Serial.println(n);
      int err;
      int bias;
      bias=0;
      err=map(n,0,640,-5+bias,5+bias);
      pos=pos-err;
      if(pos<0){
        pos=0;
      }else if(pos>180){
        pos=180;
      }
      servoPan.writeMicroseconds(pos);
      servoPan.write(pos);
//      delay(15);

    }
  }
}

/*
  while (Serial.available()) {
       delay(10);  
       //if (Serial.available() >0) { //possibly replace with while Serial.available()<2
       //  n=Serial.parseInt();
       //} 
       while (Serial.available()<2);
       n=Serial.parseInt();
       if (n>0) {
         //Serial.println(n); //now it is working
         Serial.print(n);
         int err;
         int bias;
         bias=0;
         err=map(n,0,640,5+bias,-5+bias);
         pos=pos-err;
         if(pos<0){
           pos=0;
         }else if(pos>180){
           pos=180;
         }
         servoPan.writeMicroseconds(pos);
         servoPan.write(pos);
       }
  }
       
//     if (readString.length() >0) {
//     Serial.println(readString);
//     int n;
//     n = readString.toInt(); //convert string to number
//     //Serial.println(n);
//     int err;
//     int bias;
//     bias=-7;
//     err=map(n,0,640,10+bias,-10+bias);
//     pos=pos-0.5*err;
//     if(pos<0){
//       pos=0;
//     }else if(pos>180){
//       pos=180;
//     }
     //int msb = pos / 256;
     //int lsb = pos % 256;
     //Serial.print(msb);
     //Serial.println(lsb);
     servoPan.writeMicroseconds(pos);
     servoPan.write(pos);
     //readString="";
     //delay(100);
     }
}
*/

