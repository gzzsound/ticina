#include <AccelStepper.h>

#define HALFSTEP 8
#define INPUT_PIN 2
#define motorPin1  6     // IN1 on the ULN2003 driver 1
#define motorPin2  7     // IN2 on the ULN2003 driver 1
#define motorPin3  8     // IN3 on the ULN2003 driver 1
#define motorPin4  9     // IN4 on the ULN2003 driver 1

AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

volatile bool needToRun;

void setup() { 
  stepper1.setMaxSpeed(2000.0);
  stepper1.setAcceleration(2000.0);
  stepper1.setSpeed(2000);
  stepper1.disableOutputs();
  stepper1.setCurrentPosition(0);
  Serial.begin(9600);
  pinMode(INPUT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), start, CHANGE);
  needToRun = false;
} 

void start(){
  static unsigned long lastInterruptEffect1 = 0;
  unsigned long interruptTimeEffect1 = millis();
  if (interruptTimeEffect1 - lastInterruptEffect1 > 100) {
    needToRun = !needToRun;
  }
  lastInterruptEffect1 = interruptTimeEffect1;
}

void loop() {
 if (needToRun) {
    stepper1.enableOutputs();
    stepper1.move(-1000);
    stepper1.run();
  }else{
    stepper1.stop();
    stepper1.setCurrentPosition(0);
    stepper1.disableOutputs();
  }
}
