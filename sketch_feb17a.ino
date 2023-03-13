#include <Stepper.h>

#include <Adafruit_MotorShield.h>
#include <stdio.h>
#include <EEPROM.h>

char *input;         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
Adafruit_MotorShield AFMS; 
Adafruit_StepperMotor *myMotor_x;
Adafruit_StepperMotor *myMotor_y;

int cur_x = 0;
int cur_y = 0;
int limited = 1;
long last_save = 0;

//------------------------------------------------------------

void read_state()
{
  uint8_t h1, l1, h2, l2;

  h1 = EEPROM.read(0);
  l1 = EEPROM.read(1);
  h2 = EEPROM.read(2);
  l2 = EEPROM.read(3);

  int v1 = (h1 << 8) | (l1);
  int v2 = (h2 << 8) | (l2);

  Serial.print("v1 = ");Serial.println(v1);
  Serial.print("v2 = ");Serial.println(v2);
  cur_x = v1;
  cur_y = v2;
}


//------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  Serial.println("Initialize");
 
  input = (char*)malloc(512);
  input[0] = 0;
  

  AFMS = Adafruit_MotorShield();
  AFMS.begin(); 
  myMotor_x = AFMS.getStepper(800, 1);
  myMotor_y = AFMS.getStepper(800, 2);

 
  myMotor_x->setSpeed(11141);
  myMotor_y->setSpeed(11141);
  
  Serial.println(myMotor_x->usperstep);
  myMotor_x->usperstep = 1;
  myMotor_y->usperstep = 1;
  read_state();

  //reset();


}

//------------------------------------------------------------

void reset()
{
  //Serial.println("RESET INIT");
  Serial.println(micros());
  //myMotor_y->step(180, FORWARD, DOUBLE);
  myMotor_x->step(1520*2, FORWARD, INTERLEAVE); 
  myMotor_x->step(800*2, BACKWARD, INTERLEAVE); 

  myMotor_y->step(480, FORWARD, DOUBLE);
  myMotor_y->step(220, BACKWARD, DOUBLE);
}
   

//------------------------------------------------------------

void save_state(int x, int y)
{
  Serial.print("Try save ");Serial.print(x);Serial.print(" ");Serial.println(x);
//EEPROM.update(address, val);
  uint8_t h1, l1, h2, l2;

  h1 = (x >> 8) & 0xff;
  l1 = (x & 0xff);

  h2 = (y >> 8) & 0xff;
  l2 = (y & 0xff);

  EEPROM.update(0, h1);
  EEPROM.update(1, l1);
  EEPROM.update(2, h2);
  EEPROM.update(3, l2);

  read_state();
}

//------------------------------------------------------------

void loop() {
  int x, y;
  // print the string when  newline arrives:
  //Serial.println("GO\n");
  //Serial.println(stringComplete);
  if (stringComplete) {

    if (input[1] == 'r') {
      //Serial.println("RESET");
      //reset();
    }

    if (input[1] == 'z') {
      limited = 0;
      cur_x = 0;
      cur_y = 0;
    }

    if (input[1] == 's') {
      save_state(cur_x, cur_y);
    }

    //Serial.println(input[1]);
    if (input[1] == 'g') { 
      sscanf(input, "#g%i %i\n", &x, &y);
      
      if (limited != 0) {
        if (x > 300) {
          x = 300;
        }
        if (x < -300) {
          x = -300;
        }

        if (y > 300) {
          y = 300;
        }

        if (y < -300) {
          y = -300;
        }
      }

      int dx = x - cur_x;
      int dy = y - cur_y;

      while(dx != 0 || dy != 0) {
        if (dx < 0) {
          dx++;
          myMotor_x->onestep(BACKWARD, INTERLEAVE);
          cur_x--;
        }
        if (dx > 0) {
          dx--;
          myMotor_x->onestep(FORWARD, INTERLEAVE);
          cur_x++;
        }
        if (dy < 0) {
          dy++;
          myMotor_y->onestep(BACKWARD, INTERLEAVE);
          cur_y--;
        }

        if (dy > 0) {
          dy--;
          myMotor_y->onestep(FORWARD, INTERLEAVE);
          cur_y++;
        }
      }

    }
    input[0] = 0;
    stringComplete = false;
  }

  long cur_time = micros();

  long dt = cur_time - last_save;
  dt = dt / 1000;       //time in millisecond since last save

  if (dt > 5000) {     //check every 10 seconds
    save_state(cur_x, cur_y);
    last_save = cur_time;
  }
  
}

//------------------------------------------------------------

void serialEvent() {
  char in[2];
  
  in[1] = 0;
  

  while (Serial.available()) {
    int v = Serial.read();

    in[0] = (char)v;

    input = strcat(input, in);
    //Serial.println(input);
    if (in[0] == 13 || in[0] == 10) {
      stringComplete = true;
    }
  }
}


