#include <FastLED.h>

#define CLOCK_PIN_STRIP_1 8
#define DATA_PIN_STRIP_1 9 

#define CLOCK_PIN_STRIP_2  7
#define DATA_PIN_STRIP_2  6 

#define NUM_STRIPS 2

#define CHIPSET     APA102
#define NUM_LEDS    60
#define BRIGHTNESS 240
#define NUMBEROFPIXELS 60
#define NUM_STRIPS 2
#define EFFECT_1_PIN 2
#define EFFECT_2_PIN 3

CRGB leds[NUM_STRIPS][NUMBEROFPIXELS];

volatile byte effect1State = false;
volatile byte effect2State = false;
bool running = false;
int brightness = 0;
int fadeAmount = 5;

void setup() {
  FastLED.addLeds<CHIPSET, DATA_PIN_STRIP_2, CLOCK_PIN_STRIP_2, RGB>(leds[0], NUMBEROFPIXELS);
  FastLED.addLeds<CHIPSET, DATA_PIN_STRIP_1, CLOCK_PIN_STRIP_1, RGB>(leds[1], NUMBEROFPIXELS);
   
  resetLeds();
  Serial.begin(115200);
  pinMode(EFFECT_1_PIN, INPUT);
  pinMode(EFFECT_2_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(EFFECT_1_PIN), effect1Swap, CHANGE);
  attachInterrupt(digitalPinToInterrupt(EFFECT_2_PIN), effect2Swap, CHANGE);
}

void effect1Swap() {
  static unsigned long lastInterruptEffect1 = 0;
  unsigned long interruptTimeEffect1 = millis();
  if (interruptTimeEffect1 - lastInterruptEffect1 > 100 && effect2State == false) {
    effect1State = true;
    Serial.println("effect1State = true");
  }
  lastInterruptEffect1 = interruptTimeEffect1;
}

void effect2Swap(){
  Serial.println("effect2State = reverse");
  static unsigned long lastInterruptEffect2 = 0;
  unsigned long interruptTimeEffect2 = millis();
  if (interruptTimeEffect2 - lastInterruptEffect2 > 100 && effect1State == false) {
    effect2State = !effect2State;
    
    if(effect2State == false ){
      resetLeds();
    }else{
      Serial.println("effect2Swap - turn on");
    }
  }
  lastInterruptEffect2 = interruptTimeEffect2;
}

void loop(){
  if(effect1State){ triggerEffect1(); }
  if(effect2State){ pulse(CRGB::LightBlue, 9); }
}

void triggerEffect1(){
  color_chase(CRGB::LightBlue, 30);
  effect1State = false;
  Serial.println("Effect 1 is off");
}

void resetLeds(){
  Serial.println("resetLeds");
  for(int y = 0; y < NUM_STRIPS; y++){
    for(int i = 0; i < NUM_LEDS; i++ ){ leds[y][i] = CRGB::Black; }
  }
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.show();
}

void pulse(uint32_t color, uint8_t wait){
  for(int i = 0; i < NUM_LEDS; i=i+2 ){
   leds[1][i] = color;
   leds[1][i].fadeLightBy(brightness);
  }
  FastLED.show();
  delay(wait);
  brightness = brightness + fadeAmount;
  if(brightness == 0 || brightness == BRIGHTNESS){
    fadeAmount = -fadeAmount ;
  }   
  FastLED.show();
  delay(wait);
}

void color_chase(uint32_t color, uint8_t wait){
  Serial.println("Effect 1 is on");
  FastLED.setBrightness( BRIGHTNESS );
  int count = NUM_LEDS-1;
  for(int led_number = count; led_number >= 0; led_number--){
    leds[0][led_number] = color;

    int reversed_count = NUM_LEDS-led_number;
    int d = reversed_count /10;
    for(int lto = 1; lto <= d; lto++){
      int led_to_turn_on = led_number - d;
      if(led_to_turn_on > 0) { leds[0][led_to_turn_on] = color; }
    }
  
    delay(wait-d*4);
   
    if(led_number + 1 <= NUM_LEDS ) { leds[0][led_number + 1] = CRGB::Black; }
    if(led_number == 0) { leds[0][led_number] = CRGB::Black; }

    FastLED.show();
  }
  return;
}

