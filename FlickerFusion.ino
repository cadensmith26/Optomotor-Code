#include "IRremote.h"
#include <AccelStepper.h>

#define IR_RECEIVE_PIN 13

int receiver = 13; // Signal Pin of IR receiver to Arduino Digital Pin 6

enum
stepper_Motor {
  FUNCTION = 0, DRIVER = 1, FULL2WIRE = 2, FULL3WIRE = 3,
  FULL4WIRE = 4, HALF3WIRE = 6, HALF4WIRE = 8
};

#define sleep_pwr 4

AccelStepper stepper_Motor(DRIVER, 3, 2); //stepper(driver, step, dir)

// Variables for motor speed

int stepsPerRev = 200; // Stepper motors are often 200 steps per revolution
int gearRatio = 4; // number of motor rotations required to rotate the drum one full revolution (e.g. motor pulley has 20 teeth, and the drum pulley has 80, so 4:1 ratio)
int microSteps = 16; // this can be manually specified using the "mode" pins on the DRV8825 board:

float speedRPM = 1; // CHANGED TO ZERO!!! the speed of rotation. Optomotor experiments often use around 2-4 RPM
float timeRot = 60.0; // number of seconds to rotate in each direction
int nRot = 3; // the number of rotations in each direction
float stepperAccell = 5000; // acceleration and deceleration of stepper motor
int dir = -1; // initial direction: -1 = clockwise, 1 = anticlockwise

const int maxSpeed = 15;
const int minSpeed = 0;


int nRem = 0;
//int nDec = 256; // number of decelleration steps when halt/reverse occurs

//int motorSpeed = 0;              // Motor speed (0 to 255)
const int minustwoRPM = 2;
const int speedStep = 1;

void setup() {


  Serial.begin(9600);           // Initialize serial for debugging

  Serial.println("Initialised"); 
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK); // Start the receiver
  pinMode(sleep_pwr, OUTPUT);
  digitalWrite(sleep_pwr,LOW);
  stepper_Motor.setCurrentPosition(0);

}

void loop() {
  // Check if an IR signal has been received
  if (IrReceiver.decode()) {

    // Print the received IR code for debugging
    Serial.println(IrReceiver.decodedIRData.decodedRawData);
    switch(IrReceiver.decodedIRData.decodedRawData){

    // Handle specific IR remote button codes

      case 0xFE01EF00: // power - stop everything!
        stepper_Motor.setCurrentPosition(0);
        nRem = 0;
      break;

      case 0xF807EF00:  // Replace with another IR button code (e.g., "DOWN")
        speedRPM -= minustwoRPM;
        if (speedRPM < minSpeed) speedRPM = minSpeed;
      break;

      case 0xFD02EF00: // OK - switch direction manually
        stepper_Motor.setCurrentPosition(0);
         nRem = nRem+2;
      break;

      case 0xEB14EF00: // up - clockwise
         speedRPM += speedStep;
         if (speedRPM > maxSpeed) speedRPM = maxSpeed;
         stepper_Motor.setCurrentPosition(0);
         nRem = nRem+2;
         dir = -1;
      break;

      case 0xEA15EF00: // down - counter clockwise
         speedRPM += speedStep;
         if (speedRPM > maxSpeed) speedRPM = maxSpeed;
         stepper_Motor.setCurrentPosition(0);
         nRem = nRem+2;
         dir = 1;
      break;

    }

    // Resume receiving the next signal
    Serial.println(speedRPM);
    IrReceiver.resume();
  }


  if(stepper_Motor.distanceToGo()==0)  {
    if(nRem > 0){

      stepper_Motor.setCurrentPosition(0);
      
      nRem = nRem-1;
      dir = dir*-1;
      long nSteps = round(stepsPerRev * gearRatio * microSteps * (speedRPM/60) * timeRot * dir);
      float stepperSpeed = nSteps/timeRot;
      
      stepper_Motor.setMaxSpeed(stepperSpeed);
      stepper_Motor.setAcceleration(stepperAccell);
      stepper_Motor.moveTo(nSteps);
    
      Serial.println((String)"nSteps = " + (nSteps));
      Serial.println((String)"stepperSpeed = " + (stepperSpeed));
    
      digitalWrite(sleep_pwr,HIGH);
  
    } else {
      digitalWrite(sleep_pwr,LOW);
    }
  
  }

  stepper_Motor.run();
  


}