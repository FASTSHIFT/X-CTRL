/***************************************************************************
  This is a library for the AMG88xx GridEYE 8x8 IR camera

  This sketch shows how to use the interrupt on the sensor.
  it will print which pixels triggered the interrupt when one is received.

  Designed specifically to work with the Adafruit AMG88 breakout
  ----> http://www.adafruit.com/products/3538

  These sensors use I2C to communicate. The device's I2C address is 0x69

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <Adafruit_AMG88xx.h>

Adafruit_AMG88xx amg;

//INT pin from the sensor board goes to this pin on your microcontroller board
#define INT_PIN 3

//interrupt levels (in degrees C)
//any reading on any pixel above TEMP_INT_HIGH degrees C, or under TEMP_INT_LOW degrees C will trigger an interrupt
//30 degrees should trigger when you wave your hand in front of the sensor
#define TEMP_INT_HIGH 30
#define TEMP_INT_LOW 15

volatile bool intReceived = false;
uint8_t pixelInts[8];
/******* 
we can tell which pixels triggered the interrupt by reading the
bits in this array of bytes. Any bit that is a 1 means that pixel triggered

         bit 0  bit 1  bit 2  bit 3  bit 4  bit 5  bit 6  bit 7
byte 0 |  0      1      0      0      0      0      0      1
byte 1 |  0      0      0      0      0      0      0      0
byte 2 |  0      0      0      0      0      0      0      0
byte 3 |  0      0      0      1      0      0      0      0
byte 4 |  0      0      0      0      0      0      0      0
byte 5 |  0      0      0      0      0      0      0      0
byte 6 |  0      0      0      0      0      1      0      0
byte 7 |  0      0      0      0      0      0      0      0

*****/

void setup() {

  pinMode(INT_PIN, INPUT);
  
  Serial.begin(9600);
  Serial.println(F("AMG88xx interrupt test"));

  bool status;
  
  // default settings
  status = amg.begin();
  if (!status) {
      Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
      while (1);
  }
  
  amg.setInterruptLevels(TEMP_INT_HIGH, TEMP_INT_LOW);

  //set to absolue value mode
  amg.setInterruptMode(AMG88xx_ABSOLUTE_VALUE);

  //enable interrupts
  amg.enableInterrupt();

  //attach to our Interrupt Service Routine (ISR)
  attachInterrupt(digitalPinToInterrupt(INT_PIN), AMG88xx_ISR, FALLING);

}

void loop() {
  
  if(intReceived){
    //get which pixels triggered
    amg.getInterrupt(pixelInts);

    Serial.println("**** interrupt received! ****");
    for(int i=0; i<8; i++){
      Serial.println(pixelInts[i], BIN);
    }
    Serial.println();

    //clear the interrupt so we can get the next one!
    amg.clearInterrupt();
    
    intReceived = false;
  }
}

void AMG88xx_ISR() {
  //keep your ISR short!
  //we don't really want to be reading from or writing to the sensor from inside here.
  intReceived = true;
}

