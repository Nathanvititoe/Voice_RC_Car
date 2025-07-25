
// Nathan Vititoe - Voice Controlled RC Car

// Prototype code for connecting to arduino from PC using BLE and
// using the Vosk Voice Recognition ML model on the PC for voice recognition RC Car control

// available commands are "stop", "go", "left", "right", "down"

// stop makes the car stop
// go makes the car move forward
// left makes the car turn left
// right makes the car turn right
// down makes the car go in reverse

#include <ArduinoBLE.h>

// init BLE control
BLEService controlService("180D");                // Custom service UUID
BLECharacteristic command("2A56", BLEWrite, 20);  // Command up to 20 bytes (as string)

// Motor pins
const int left_ena_1 = 6;
const int left_ena_3 = 2;
const int left_front_neg = 3;
const int left_front_pos = 9;
const int left_rear_pos = 12;
const int left_rear_neg = 8;

const int right_ena_1 = 11;
const int right_ena_3 = 14;
const int right_front_neg = 10;
const int right_front_pos = 4;
const int right_rear_pos = 5;
const int right_rear_neg = 7;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  setup_motors();
  setup_BLE();
  Serial.println("Setup Complete.");
}

void loop() {
  BLE.poll();  // poll the BLE connection for incoming commands
}

/*
* inititalizes all motor pins as output
*/
void setup_motors() {
  // set motor pins to output
  pinMode(left_ena_1, OUTPUT);
  pinMode(left_ena_3, OUTPUT);
  pinMode(left_front_neg, OUTPUT);
  pinMode(left_front_pos, OUTPUT);
  pinMode(left_rear_pos, OUTPUT);
  pinMode(left_rear_neg, OUTPUT);

  pinMode(right_ena_1, OUTPUT);
  pinMode(right_ena_3, OUTPUT);
  pinMode(right_front_neg, OUTPUT);
  pinMode(right_front_pos, OUTPUT);
  pinMode(right_rear_pos, OUTPUT);
  pinMode(right_rear_neg, OUTPUT);
}

/*
* configure the BLE service
*/
void setup_BLE() {
  // ensure BLE setup is successful
  if (!BLE.begin()) {
    Serial.println("BLE failed to start.");
    while (1)
      ;
  }

  BLE.setLocalName("Voice_RC");
  BLE.setAdvertisedService(controlService);
  controlService.addCharacteristic(command);
  BLE.addService(controlService);
  BLE.advertise();

  command.setEventHandler(BLEWritten, handleCommand);

  Serial.println("BLE Ready");
}

/*
* Interpret incoming BLE commands and call their related functions
* @param central the central device (PC)
* @param command the command that triggered this function
*/
void handleCommand(BLEDevice central, BLECharacteristic command) {
  if (!command.written()) return;  // if no commands, do nothing

  String cmd = String((const char*)command.value());  // get the command
  cmd.toUpperCase(); // ensure its uppercase
  
  // print the received command
  Serial.print("Received BLE command: ");
  Serial.println(cmd);

  // map incoming commands to movement functions
  if (cmd == "FWD") go_forward();
  else if (cmd == "STOP") stop_motors();
  else if (cmd == "LEFT") turn_left();
  else if (cmd == "RIGHT") turn_right();
  else if (cmd == "REV") reverse();
  else if (cmd == "FAST") set_speed(200);
  else if (cmd == "SLOW") set_speed(100);
  else {
    Serial.println("Unknown command received.");
    stop_motors();
  }
}

// ========= MOVEMENT FUNCTIONS BELOW ================
int currentSpeed = 100;  // default 1/2 speed

/*
* step up or down speed incrementally 
* @param enaPin - enablePin to change speed on
* @param startValue - starting speed
* @param endValue - ending speed
* @param delay - how long between steps in ms
*/
void increment_speed(int enaPin, int startValue, int endValue, int delayMs = 10) {
  // determine if we are stepping up or down
  int step = (startValue < endValue) ? 1 : -1;

  for (int value = startValue; value != endValue; value += step) {
    analogWrite(enaPin, value);
    delay(delayMs);
  }
  analogWrite(enaPin, endValue);
}

/*
* Change the speed of the car
* @param speed - speed to change the car to
*/
void set_speed(int newSpeed) {
  // ensure new speed stays in range, and always below 255 to prevent motor burnout
  newSpeed = constrain(newSpeed, 0, 200); 
  if (newSpeed == currentSpeed) return;

  Serial.print("Changing speed from ");
  Serial.print(currentSpeed);
  Serial.print(" to ");
  Serial.println(newSpeed);

  // ramp speed on each enable pin
  increment_speed(left_ena_1, currentSpeed, newSpeed);
  increment_speed(left_ena_3, currentSpeed, newSpeed);
  increment_speed(right_ena_1, currentSpeed, newSpeed);
  increment_speed(right_ena_3, currentSpeed, newSpeed);

  currentSpeed = newSpeed;
}

/*
* move the car forward
*/
void go_forward() {
  // using the current speed
  // move left wheels forward
  digitalWrite(left_front_pos, HIGH);
  digitalWrite(left_rear_neg, HIGH);

  // ensure left reverse is off
  digitalWrite(left_front_neg, LOW);
  digitalWrite(left_rear_pos, LOW);

  // move right wheels forward
  digitalWrite(right_front_pos, HIGH);
  digitalWrite(right_rear_neg, HIGH);

  // ensure right reverse is off
  digitalWrite(right_front_neg, LOW);
  digitalWrite(right_rear_pos, LOW);
}

/*
* move the car backwards
*/
void reverse() {
  // using the current speed
  // move left wheels backwards
  digitalWrite(left_front_neg, HIGH);
  digitalWrite(left_rear_neg, HIGH);

  // ensure left forwards is off
  digitalWrite(left_front_pos, LOW);
  digitalWrite(left_rear_pos, LOW);

  // move right wheels backwards
  digitalWrite(right_front_neg, HIGH);
  digitalWrite(right_rear_neg, HIGH);

  // ensure right forwards is off
  digitalWrite(right_front_pos, LOW);
  digitalWrite(right_rear_pos, LOW);
}

/*
* stop the car
*/
void stop_motors() {
  // set all enable pins to change speed to 0
  set_speed(0);

  // turn all motor pins low
  digitalWrite(right_front_pos, LOW);
  digitalWrite(right_front_neg, LOW);
  digitalWrite(left_front_pos, LOW);
  digitalWrite(left_front_neg, LOW);
}

/*
* turn the car left
*/
void turn_left() {
  // stop all motor activity before turning
  stop_motors();

  // set right wheel speed to 50 through enable pins
  analogWrite(right_ena_1, 50);
  analogWrite(right_ena_3, 50);

  // move right wheels fwd
  digitalWrite(right_front_pos, HIGH);
  digitalWrite(right_rear_neg, HIGH);

  // ensure right wheels reverse is off
  digitalWrite(right_front_neg, LOW);
  digitalWrite(right_rear_pos, LOW);

  // stop all motor activity after turning, waiting for next command
  delay(1000);
  stop_motors();
}

/*
* turn the car right
*/
void turn_right() {
  // stop all motor activity before turning
  stop_motors();

  // set left wheel speed to 100 through enable pins
  analogWrite(left_ena_1, 50);
  analogWrite(left_ena_3, 50);

  // move left wheels fwd
  digitalWrite(left_front_pos, HIGH);
  digitalWrite(left_rear_neg, HIGH);

  // ensure left wheels reverse is off
  digitalWrite(left_front_neg, LOW);
  digitalWrite(left_rear_pos, LOW);

  // stop all motor activity after turning, waiting for next command
  delay(1000);
  stop_motors();
}