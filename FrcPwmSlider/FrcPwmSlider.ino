/*
  FrcPwmSlider
  Generate a PWM control signal from a slider potentiometer. Give status through a red/green LED
  blink pattern similar to Victor and Talon controllers.
 */

#include <Servo.h> 


// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0 / LC
// give it a name:
const int led_pin = 13;
// rgb led with common cathode (k)
const int b_pin = 10;
const int g_pin = 9;
const int k_pin = 8;
const int r_pin = 7;
// slider potentiometer input
const int slider_3v3_pin = 13;
const int slider_pin = 14;
const int slider_ain = 0;
const int slider_max = 1024;
// pwm / servo output
const int pwm_servo_pin = 17;
const int pwm_servo_max = 180;  // 0~180 deg
const int pwm_servo_dband = 5;  // ignore +/- from center
// blink timing
const int blink_ms_min = 100;  // fast blink for max / min angle
const int blink_ms_max = 500;  // slow blink near center
const int blink_pin_mode[] = {INPUT, INPUT_PULLUP};

// loop state variables
int slider_val = slider_max / 2;
int slider_angle = pwm_servo_max / 2;
Servo pwm_servo;
int r_blink_ms = 0;
int g_blink_ms = 0;
int r_on = 0;
int g_on = 0;
unsigned long last_blink_change_ms = 0;


// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led_pin, OUTPUT);
  // setup rgb LED to be off and common 
  digitalWrite(r_pin, LOW);
  digitalWrite(g_pin, LOW);
  digitalWrite(b_pin, LOW);
  digitalWrite(k_pin, LOW);
  pinMode(r_pin, INPUT);
  pinMode(g_pin, INPUT);
  pinMode(b_pin, INPUT);
  pinMode(k_pin, OUTPUT);
  // setup slider input
  pinMode(slider_pin, INPUT);
  pinMode(slider_3v3_pin, OUTPUT);
  digitalWrite(slider_3v3_pin, HIGH);
  // setup servo library
  pwm_servo.attach(pwm_servo_pin);
  pwm_servo.write(slider_angle);
  // debug
//  Serial.begin(1000000);
//  Serial.println("hello, world!");
}


// the loop routine runs over and over again forever:
void loop() {
  const int pwm_servo_half = (pwm_servo_max / 2);
  const int blink_ms_range = blink_ms_max - blink_ms_min;
  
  // read slider to get angle
  slider_val = analogRead(slider_ain);
  slider_angle = (slider_val * pwm_servo_max) / slider_max;
  if (slider_angle >= pwm_servo_half - pwm_servo_dband &&
      slider_angle <= pwm_servo_half + pwm_servo_dband) {
    // default back to middle if in deadband
    slider_angle = pwm_servo_max / 2;
  }
  
  // set pwm servo output
  pwm_servo.write(slider_angle);
  
  // calculate new led pattern
  if (slider_angle < pwm_servo_half) {
    // backwards: red blink slow to fast
    int slider_angle_abs = pwm_servo_half - slider_angle;
    r_blink_ms = (slider_angle_abs * blink_ms_range) / pwm_servo_half;
    r_blink_ms = blink_ms_max - r_blink_ms;  // invert: slower at center
    g_blink_ms = 0;
  }
  else if (slider_angle > pwm_servo_half) {
    // forwards: green blink slow to fast
    int slider_angle_abs = slider_angle - pwm_servo_half;
    g_blink_ms = (slider_angle_abs * blink_ms_range) / pwm_servo_half;
    g_blink_ms = blink_ms_max - g_blink_ms;  // invert: slower at center
    r_blink_ms = 0;
  }
  else {
    // no blink
    r_blink_ms = 0;
    g_blink_ms = 0;
  }
  
  // turn on or off leds as needed
  unsigned long now_ms = millis();
  if (r_blink_ms == 0) {
    r_on = 0;
    pinMode(r_pin, INPUT);
  }
  else if (last_blink_change_ms + r_blink_ms < now_ms) {
    // change r state
    r_on = !r_on;
    pinMode(r_pin, blink_pin_mode[r_on]);
    last_blink_change_ms = now_ms;
  }
  if (g_blink_ms == 0) {
    g_on = 0;
    pinMode(g_pin, INPUT);
  } else if (last_blink_change_ms + g_blink_ms < now_ms) {
    // change g state
    g_on = !g_on;
    pinMode(g_pin, blink_pin_mode[g_on]);
    last_blink_change_ms = now_ms;
  }

  // debug
//  Serial.print("slider_val: ");
//  Serial.print(slider_val);
//  Serial.print(", slider_angle: ");
//  Serial.print(slider_angle);
//  Serial.print(", r_blink_ms: ");
//  Serial.print(r_blink_ms);
//  Serial.print(", g_blink_ms: ");
//  Serial.println(g_blink_ms);
}
