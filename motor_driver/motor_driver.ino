#include <AccelStepper.h>
#include <Wire.h>

#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.println (x)
#else
 #define DEBUG_PRINT(x)
#endif

#define EFFECT_1 "EFFECT_1"
#define HALFSTEP 8
#define INPUT_PIN 2
#define motorPin1  6     // IN1 on the ULN2003 driver 1
#define motorPin2  7     // IN2 on the ULN2003 driver 1
#define motorPin3  8     // IN3 on the ULN2003 driver 1
#define motorPin4  9     // IN4 on the ULN2003 driver 1

AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

volatile bool needToRun;
volatile int bytesCount;
volatile int j;

void setup() { 
  stepper1.setMaxSpeed(1400);
  stepper1.setAcceleration(700);
  stepper1.moveTo(15000);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
} 

void receiveEvent(int howMany) {
  bytesCount = Wire.available();
  char buff[bytesCount];
  j=0;
  for(int i=0; i< bytesCount; i++){
    buff[j++] = (char)Wire.read();
  }

  if(strncmp(buff, EFFECT_1, bytesCount) == 0){
    start();
  }else{
    DEBUG_PRINT("Command not recognised");
  }
}

void start(){
  static unsigned long lastInterruptEffect1 = 0;
  unsigned long interruptTimeEffect1 = millis();
  if (interruptTimeEffect1 - lastInterruptEffect1 > 100) {
    needToRun = !needToRun;
    DEBUG_PRINT("Switching to...");
  }
  lastInterruptEffect1 = interruptTimeEffect1;
}

void loop() {
 if (needToRun) {
    stepper1.enableOutputs();
    stepper1.move(1000);
    stepper1.run();
  }else{
    stepper1.stop();
    stepper1.setCurrentPosition(0);
    stepper1.disableOutputs();
  }
}
