#include <Adafruit_MotorShield.h>


char *input;         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
Adafruit_MotorShield AFMS; 
Adafruit_StepperMotor *myMotor_x;
Adafruit_StepperMotor *myMotor_y;

int cur_x = 0;
int cur_y = 0;

//------------------------------------------------------------

void setup() {
  input = (char*)malloc(512);
  input[0] = 0;
  
  Serial.begin(9600);
  AFMS = Adafruit_MotorShield();
  AFMS.begin(); 
  myMotor_x = AFMS.getStepper(400, 1);
  myMotor_y = AFMS.getStepper(400, 2);

 
  myMotor_x->setSpeed(733);
  myMotor_y->setSpeed(733);
  /*
  Serial.println(myMotor_x->PWMApin);
  Serial.println(myMotor_x->AIN1pin);
  Serial.println(myMotor_x->AIN2pin);
  Serial.println(myMotor_x->PWMBpin);
  Serial.println(myMotor_x->BIN1pin);
  Serial.println(myMotor_x->BIN2pin);
  */
  reset();


}

//------------------------------------------------------------

void reset()
{
  myMotor_x->step(580, FORWARD, DOUBLE); 
  myMotor_x->step(290, BACKWARD, DOUBLE); 

  myMotor_y->step(480, FORWARD, DOUBLE);
  myMotor_y->step(200, BACKWARD, DOUBLE);
}
   
//------------------------------------------------------------

void loop() {
  int x, y;
  // print the string when a newline arrives:
  if (stringComplete) {
    if (input[1] == 'r') {
      reset();
    }
    if (input[1] == 'g') { 
      sscanf(input, "#g%i %i\n", &x, &y);
      int dx = x - cur_x;
      int dy = y - cur_y;

      while(dx != 0 || dy != 0) {
        if (dx < 0) {
          dx++;
          myMotor_x->onestep(BACKWARD, DOUBLE);
        }
        if (dx > 0) {
          dx--;
          myMotor_x->onestep(FORWARD, DOUBLE);
        }
        if (dy < 0) {
          dy++;
          myMotor_y->onestep(BACKWARD, DOUBLE);
        }
        if (dy > 0) {
          dy--;
          myMotor_y->onestep(FORWARD, DOUBLE);
        }
      }

      cur_x = x;
      cur_y = y;
      Serial.println(x);
      Serial.println(y);
    }
    input[0] = 0;
    stringComplete = false;
  }
}

//------------------------------------------------------------

void serialEvent() {
  char in[2];
  
  in[1] = 0;
  

  while (Serial.available()) {
    in[0] = (char)Serial.read();
    input = strcat(input, in);
    if (in[0] == 13) {
      stringComplete = true;
    }
  }
}



