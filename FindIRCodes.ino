#include <IRremote.hpp>
#define IR_RECEIVE_PIN 13

void setup()
{
  Serial.begin(115200); // // Establish serial communication
  IrReceiver.begin(IR_RECEIVE_PIN); // Start the receiver
}

void loop() {
  if (IrReceiver.decode()) {
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
      IrReceiver.resume(); // Enable receiving of the next value
  }
  delay(1500);
}

