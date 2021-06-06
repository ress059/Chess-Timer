///////////////// PROGRAM DESCRIPTION /////////////////
/* This program tests the ADAFRUIT 343X Buttons
 * 
 * 1) The green button LED will originally be turned on.
 * 2) The green button LED will only be turned off once it is pressed.
 * 3) Once pressed, blue button LED will then turn on.
 * 4) The blue button LED will only be turned off once it is pressed.
 * 5) Green LED illuminated again and cycle repeats indefinately.
 */

/////////////// CONSTS ////////////////
const uint8_t greenbuttonpin = 2;
const uint8_t greenledpin = 13;
const uint8_t bluebuttonpin = 4;
const uint8_t blueledpin = 10;

////////////// TYPEDEFS ///////////////
typedef enum{
  blue,
  green
} buttontracker;

buttontracker led; 

///////////// GLOBAL PROTOTYPES ///////////
void test(buttontracker a);

void setup() {
  //Serial.begin(9600);
  pinMode(greenbuttonpin, INPUT);
  pinMode(greenledpin, OUTPUT);
  pinMode(bluebuttonpin, INPUT);
  pinMode(blueledpin, OUTPUT);
  
  digitalWrite(greenledpin, HIGH);
  led = green;
}

void loop() {
  test(led);
}

void test(buttontracker a){
  while (a == green){
    if (digitalRead(greenbuttonpin)==0){ //pressed
      digitalWrite(greenledpin, LOW);
      digitalWrite(blueledpin, HIGH);
      a = blue;
    }
  }

  while (a == blue){
    if (digitalRead(bluebuttonpin)==0){ //pressed
      digitalWrite(blueledpin, LOW);
      digitalWrite(greenledpin, HIGH);
      a = green;
    }
  }
}
