/*
  QuartiLauncherPwm
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
// ir led sensor (pull up _s to read)
const int ir_sensor_a = 1;  // red
const int ir_sensor_k = 2;  // orange/black
const int ir_sensor_s = 3;  // yellow
// ir led beacon
const int ir_beacon_a = 5;  // red/brown
const int ir_beacon_k = 6;  // black
// pwm / servo output
const int pwm_servo_pin = 17;
const int pwm_servo_max = 180;  // 0~180 deg
//const int pwm_servo_dband = 5;  // ignore +/- from center
// blink timing
const int blink_ms = 25;
//const int blink_pin_mode[] = {INPUT, INPUT_PULLUP};
// motor control profile - lengths must match!
// the last step is the enforced idle time
// the first step delay is not used - moved forward by sensor
const int motor_idle_angle = pwm_servo_max / 2;  // 0~180, 90 center
const int motor_profile_angle[] = {motor_idle_angle, 88, 75, 78, 82, 85, 88, motor_idle_angle};
const int motor_profile_delay_ms[] = {0, 20, 1500, 220, 270, 320, 420, 3000};
const int motor_profile_steps = sizeof(motor_profile_angle) / sizeof(motor_profile_angle[0]);

// loop state variables
int motor_profile_step = 0;
Servo pwm_servo;
unsigned long last_blink_change_ms = 0;
unsigned long last_motor_change_ms = 0;


// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);
  // setup ir led 
  pinMode(ir_beacon_a, OUTPUT);
  pinMode(ir_beacon_k, OUTPUT);
  pinMode(ir_sensor_a, OUTPUT);
  pinMode(ir_sensor_k, OUTPUT);
  pinMode(ir_sensor_s, INPUT_PULLUP);
  digitalWrite(ir_beacon_a, HIGH);
  digitalWrite(ir_beacon_k, LOW);
  digitalWrite(ir_sensor_a, HIGH);
  digitalWrite(ir_sensor_k, LOW);
  // setup servo library
  pwm_servo.attach(pwm_servo_pin);
  pwm_servo.write(motor_idle_angle);
  // debug
//  Serial.begin(1000000);
//  Serial.println("hello, world!");
}


// the loop routine runs over and over again forever:
void loop() {
  unsigned long now_ms;
  
  // find ir sensor state - 0 is obstructed
  const int ir_sensor_state = digitalRead(ir_sensor_s);
  now_ms = millis();
  if (ir_sensor_state == 0) {
    if (now_ms > last_blink_change_ms + blink_ms) {
      digitalToggle(led_pin);
      last_blink_change_ms = now_ms;
    }
  }
  else {
    digitalWrite(led_pin, HIGH);
  }

  // find desired motor state
  now_ms = millis();
  if (motor_profile_step == 0 && ir_sensor_state == 0) {  // start motor profile
    last_motor_change_ms = now_ms;
    motor_profile_step++;
    pwm_servo.write(motor_profile_angle[motor_profile_step]);
  }
  if (motor_profile_step != 0 && now_ms > last_motor_change_ms + motor_profile_delay_ms[motor_profile_step]) {
    last_motor_change_ms = now_ms;
    motor_profile_step = (motor_profile_step + 1) % motor_profile_steps;
    pwm_servo.write(motor_profile_angle[motor_profile_step]);
  }

  // debug
//  Serial.print("now_ms: ");
//  Serial.print(now_ms);
//  Serial.print(", motor_profile_step: ");
//  Serial.print(motor_profile_step);
//  Serial.print(", motor_profile_angle: ");
//  Serial.print(motor_profile_angle[motor_profile_step]);
//  Serial.print(", ir_sensor_state: ");
//  Serial.print(ir_sensor_state);
//  Serial.println(" ");
//  }
}
