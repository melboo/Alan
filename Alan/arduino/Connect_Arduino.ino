

#include <Bridge.h>
#include <YunClient.h>

YunClient client;

  //********* CHANGE THIS ***********//
#define SERVER_ADDRESS "10.0.1.49"
#define SERVER_PORT 5000

unsigned long postingInterval = 500;  //delay between server updates
unsigned long lastPost = 0;         // when you last made a request
unsigned long currTime = 0;            //what time is it right now

String incomingDataString = ""; //this will hold raw incoming data string
String DataStringConvert = ""; 
boolean serverResponded = false;

/*MOTORS*/
const int M1motor1Pin = 3;    // H-bridge leg 1 (pin 2, 1A)
const int M1motor2Pin = 2;    // H-bridge leg 2 (pin 7, 2A)
const int M1speedPin = 5;    // H-bridge enable pin

const int M2motor1Pin = 9;    // H-bridge leg 1 (pin 2, 1A)
const int M2motor2Pin = 4;    // H-bridge leg 2 (pin 7, 2A)
const int M2speedPin = 6;    // H-bridge enable pin

const int M3motor1Pin = 11;    // H-bridge leg 1 (pin 2, 1A)
const int M3motor2Pin = 7;    // H-bridge leg 2 (pin 7, 2A)
const int M3speedPin = 10;    // H-bridge enable pin

int M1switchValue = 0;
int M2switchValue = 0;
int M3switchValue = 0;


int potPin = 0;

int speed = 0;

void setup() {
  //pin setup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  
  //open the bridge to wifi on the yun, open the Console
  Bridge.begin();
  Console.begin();
  

  //connect to YOUR SERVER'S IP ADDRESS + PORT
  client.connect(SERVER_ADDRESS, SERVER_PORT);
  
  /*SETUP MOTOR*/
  pinMode(M1motor1Pin, OUTPUT);
  pinMode(M1motor2Pin, OUTPUT);
  pinMode(M1speedPin, OUTPUT);
    
  pinMode(M2motor1Pin, OUTPUT);
  pinMode(M2motor2Pin, OUTPUT);
  pinMode(M2speedPin, OUTPUT);
    
  pinMode(M3motor1Pin, OUTPUT);
  pinMode(M3motor2Pin, OUTPUT);
  pinMode(M3speedPin, OUTPUT);
    //pinMode(ledPin, OUTPUT);
 
    // set enablePin high so that motor can turn on:
  digitalWrite(M1speedPin, HIGH);
  digitalWrite(M2speedPin, HIGH);
  digitalWrite(M3speedPin, HIGH);
}

String hi = "hello from yun! count: ";
int count = 0;

int LEDValue = 1;
int WindValue = 0;
int WindOn = 0;
int RollValue = 0;
int SpeedValue1 = 0;
int SpeedValue2 = 0;
int SpeedValue3 = 0;

void loop() {
  
  // current time elapsed so you can calculate post intervals:
  currTime = millis();
  
  //check if it's time to post an update to the server
  if (currTime - lastPost >= postingInterval) {
    if (client.connected()){
      String data = hi+count;
      //client.print(data);
      //count++;
    } else {
      //no connection, try to make one again:
      Console.println("\nattempting to connect to server");
      client.connect(SERVER_ADDRESS, SERVER_PORT);
      delay(2000); //delay 2 seconds before trying another server reconnect
    } 
    lastPost = currTime;
  }
  
  //while client.available()==true (meaning >= 1 char of data awaits us) then:
  while (client.available()) {  
    //if(!serverResponded) Console.println(">>> Server Says : <<<"); //expecting something to be received
    char c = client.read();     //read this char
    if(isDigit(c)){
      //Console.println(c);
      //Console.print("received digit!");
      LEDValue = String(c).toInt();
      //Console.print(LEDValue);
      digitalWrite(13, LEDValue);
      
    }
    incomingDataString += c; //add this char to our dataString
    serverResponded = true; //regardless of data, we got a response
    
    //Console.println(incomingDataString);
    int StringLength = incomingDataString.length()-1;
    if (incomingDataString.charAt(StringLength) == 'A') { //MOTOR 1
      DataStringConvert = incomingDataString.substring(0, incomingDataString.length() - 1);
      SpeedValue1 = String(DataStringConvert).toInt();
      Console.print("Speed Motor 1: ");
      Console.println(SpeedValue1);
    }
    
    if (incomingDataString.charAt(StringLength) == 'B') { //MOTOR 2
      DataStringConvert = incomingDataString.substring(0, incomingDataString.length() - 1);
      SpeedValue2 = String(DataStringConvert).toInt();
      Console.print("Speed Motor 2: ");
      Console.println(SpeedValue2);
    }
    if (incomingDataString.charAt(StringLength) == 'C') { //MOTOR 3
      DataStringConvert = incomingDataString.substring(0, incomingDataString.length() - 1);
      SpeedValue3 = String(DataStringConvert).toInt();
      Console.print("Speed Motor 3: ");
      Console.println(SpeedValue3);
    }
    if (incomingDataString.charAt(StringLength) == 'R') { //MOTOR 3
      DataStringConvert = incomingDataString.substring(0, incomingDataString.length() - 1);
      RollValue = String(DataStringConvert).toInt();
      Console.print("Roll is: ");
      Console.println(RollValue);
    }
    if (incomingDataString.charAt(StringLength) == 'D') { //MOTOR 3
      DataStringConvert = incomingDataString.substring(0, incomingDataString.length() - 1);
      LEDValue = String(DataStringConvert).toInt();
      Console.print("Roll is: ");
      Console.println(RollValue);
    }
    if (incomingDataString.charAt(StringLength) == 'W') { //MOTOR 3
      DataStringConvert = incomingDataString.substring(0, incomingDataString.length() - 1);
      WindValue = String(DataStringConvert).toInt();
      Console.print("Wind is: ");
      Console.println(WindValue);
      if(WindOn == 1){
        WindOn = 0;
      }else {
        WindOn = 1;
      }
    }
    
  } 
  
  if(serverResponded){
    //Console.print("received from server: ");
    //Console.println(incomingDataString);
    incomingDataString = ""; //clear out our data string for next server message
    serverResponded = false; //start this at false after server request
  }
  if(RollValue == 1){
        M1(SpeedValue1, LEDValue);
        M2(SpeedValue2, LEDValue);
        M3(SpeedValue3, LEDValue);
      
  }else {
     M1(0, 1);
     M2(0, 1);
     M3(0, 1);  
  }
  
  //Console.println(LEDValue);
}


  void M1(int M1speed, int M1dir){
    if (M1dir == 0) {
      digitalWrite(M1motor1Pin, LOW);   // set leg 1 of the H-bridge low
      digitalWrite(M1motor2Pin, HIGH);  // set leg 2 of the H-bridge high
      //speed = analogRead(potPin);
      //Serial.println(speed);
      //speed = map(speed, 0,690,105,255);
      
      analogWrite (M1speedPin, M1speed);
      delay(50);
    }
    // if the switch is low, motor will turn in the other direction:
    else {
      digitalWrite(M1motor1Pin, HIGH);  // set leg 1 of the H-bridge high
      digitalWrite(M1motor2Pin, LOW);   // set leg 2 of the H-bridge low
      //speed = analogRead(potPin);
      //Serial.println(speed);
      //speed = map(speed, 0,1024,105,255);
      
      analogWrite (M1speedPin, M1speed); 
      delay(50);
    }
  }
  
  void M2(int M2speed, int M2dir){
    if (M2dir == 1) {
      digitalWrite(M2motor1Pin, LOW);   // set leg 1 of the H-bridge low
      digitalWrite(M2motor2Pin, HIGH);  // set leg 2 of the H-bridge high
      speed = analogRead(potPin);
      //Serial.println(speed);
      //speed = map(speed, 0,690,105,255);
      
      analogWrite (M2speedPin, M2speed);
      delay(50);
    }
    // if the switch is low, motor will turn in the other direction:
    else {
      digitalWrite(M2motor1Pin, HIGH);  // set leg 1 of the H-bridge high
      digitalWrite(M2motor2Pin, LOW);   // set leg 2 of the H-bridge low
      //speed = analogRead(potPin);
      //Serial.println(speed);
      //speed = map(speed, 0,1024,105,255);
      
      analogWrite (M2speedPin, M2speed); 
      delay(50);
    }
  }
  
    void M3(int M3speed, int M3dir){
    if (M3dir == 1) {
      digitalWrite(M3motor1Pin, LOW);   // set leg 1 of the H-bridge low
      digitalWrite(M3motor2Pin, HIGH);  // set leg 2 of the H-bridge high
      speed = analogRead(potPin);
      //Serial.println(speed);
      //speed = map(speed, 0,690,105,255);
      
      analogWrite (M3speedPin, M3speed);
      delay(50);
    }
    // if the switch is low, motor will turn in the other direction:
    else {
      digitalWrite(M3motor1Pin, HIGH);  // set leg 1 of the H-bridge high
      digitalWrite(M3motor2Pin, LOW);   // set leg 2 of the H-bridge low
      //speed = analogRead(potPin);
      //Serial.println(speed);
      //speed = map(speed, 0,1024,105,255);
      
      analogWrite (M3speedPin, M3speed); 
      delay(50);
    }
  }

