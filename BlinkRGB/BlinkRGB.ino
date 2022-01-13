/*
  BlinkRGB
  Turns on each color of an RGB LED on for one second, then off for one second, repeatedly.
  The LED legs are connected directly to the pins described driven with internal pullups to 3.3V.
 */

// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0 / LC
// give it a name:
const int led_pin = 13;
// rgb led with common cathode (k)
const int b_pin = 14;
const int g_pin = 15;
const int k_pin = 16;
const int r_pin = 17;

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
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led_pin, LOW);   // turn the LED on (HIGH is the voltage level)
  pinMode(r_pin, INPUT_PULLUP);
  delay(1000);               // wait for a second
  pinMode(r_pin, INPUT);
  pinMode(g_pin, INPUT_PULLUP);
  delay(1000);
  pinMode(g_pin, INPUT);
  pinMode(b_pin, INPUT_PULLUP);
  delay(1000);
  pinMode(b_pin, INPUT);
  digitalWrite(led_pin, HIGH);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
}
