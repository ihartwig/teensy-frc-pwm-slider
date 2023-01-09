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
const int slider_half = slider_max/2;
const int slider_dband = 25;  // ignore +/- from center
// battery voltage sense input
// 12k/2.7k on vbatt = 0.2 of full scale
// 1024 @ 3300mV*(14.7/2.7) => 17967mV
const int vbatt_pin = 18;
const int vbatt_ain = 4;
const int vbatt_to_mv_num = 17967;
const int vbatt_to_mv_den = 1024;
const int vbatt_brownout_low = 9900;
const int vbatt_brownout_high = 11700;
// pwm / servo output
const int pwm_out_pin = 17;
const int pwm_out_max = 180;  // 0~180 deg
const int pwm_out_min = 4;
const int pwm_out_center = ((pwm_out_max-pwm_out_min)/2) + pwm_out_min;
// blink timing
const int blink_ms_min = 100;  // fast blink for max / min angle
const int blink_ms_max = 500;  // slow blink near center
const int blink_ms_range = blink_ms_max - blink_ms_min;
const int blink_pin_mode[] = {INPUT, INPUT_PULLUP};

// loop state variables
int slider_val = slider_max / 2;
Servo pwm_out;
int pwm_out_angle = pwm_out_center;
int r_blink_ms = 0;
int g_blink_ms = 0;
int r_on = 0;
int g_on = 0;
unsigned long last_blink_change_ms = 0;
unsigned long vbatt_mv = 0;  // needs to be long to hold scaling factor
int vbatt_brownout = 1;


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
  // setup battery voltage sense
  pinMode(vbatt_pin, INPUT);
  // setup servo library
  pwm_out.attach(pwm_out_pin);
  pwm_out.write(pwm_out_angle);
  // debug
//  Serial.begin(1000000);
//  Serial.println("hello, world!");
}


// the loop routine runs over and over again forever:
void loop() {
  // read battery voltage sense
  vbatt_mv = analogRead(vbatt_ain);
  vbatt_mv = (vbatt_mv * vbatt_to_mv_num) / vbatt_to_mv_den;
  if (vbatt_mv >= vbatt_brownout_high) {
    vbatt_brownout = 0;
  }
  if (vbatt_mv < vbatt_brownout_low) {
    vbatt_brownout = 1;
  }
  
  // read slider to get angle
  slider_val = analogRead(slider_ain);
  if (slider_val >= slider_half - slider_dband &&
      slider_val <= slider_half + slider_dband) {
    // default back to middle if in deadband
    pwm_out_angle = pwm_out_center;
  }
  else {
    pwm_out_angle = (slider_val * pwm_out_max) / slider_max;
  }
  // and adjust for brownout condition
  if (vbatt_brownout) {
    // default to middle if in brownout
    pwm_out_angle = pwm_out_center;
  }
  // set pwm servo output
  pwm_out.write(pwm_out_angle);
  
  // calculate new led pattern
  if (pwm_out_angle < pwm_out_center) {
    // backwards: red blink slow to fast
    int pwm_out_angle_abs = pwm_out_center - pwm_out_angle;
    r_blink_ms = (pwm_out_angle_abs * blink_ms_range) / pwm_out_center;
    r_blink_ms = blink_ms_max - r_blink_ms;  // invert: slower at center
    g_blink_ms = 0;
  }
  else if (pwm_out_angle > pwm_out_center) {
    // forwards: green blink slow to fast
    int pwm_out_angle_abs = pwm_out_angle - pwm_out_center;
    g_blink_ms = (pwm_out_angle_abs * blink_ms_range) / pwm_out_center;
    g_blink_ms = blink_ms_max - g_blink_ms;  // invert: slower at center
    r_blink_ms = 0;
  }
  else if (vbatt_brownout) {
    r_blink_ms = blink_ms_min / 4;
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
