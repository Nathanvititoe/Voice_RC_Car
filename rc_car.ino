
// Nathan Vititoe - Voice Controlled RC Car

// Prototype code for connecting to arduino from PC using BLE and
// using the Vosk Voice Recognition ML model on the PC for voice recognition RC Car control

// available commands are "stop", "go", "left", "right", "down"

// stop makes the car stop
// go makes the car move forward
// left makes the car turn left
// right makes the car turn right
// down makes the car go in reverse

// back left
const int motor1_enable = 11;  // D11 (pwm)
const int motor1_input1 = 13;  // D13 (neg)
const int motor1_input2 = 12;  // D12 (pos)

// front left
const int motor2_enable = 10;  // D10 (pwm)
const int motor2_input3 = 4;   // D4 (pos)
const int motor2_input4 = 5;   // D5 (neg)

// back right
const int motor3_enable = 9;  // D9 (pwm)
const int motor3_input1 = 8;  // D8 (pos)
const int motor3_input2 = 7;  // D7 (neg)

// front right
const int motor4_enable = 6;  // D6 (pwm)
const int motor4_input3 = 3;  // D3 (pos)
const int motor4_input4 = 2;  // D2 (neg)

#include "WebConnection.h"

WebConnection network;

struct Motor {
  uint8_t enable;  // EN (PWM)
  uint8_t input1;  // IN1
  uint8_t input2;  // IN2
  bool inverted;   // for opposites
};

enum DriveMode : uint8_t {
  FWD,
  REV,
  LEFT,
  RIGHT,
  STOP
};


const int DEFAULT_SPEED = 60;           // starting speed before changes (60%)
void handleCommand(const String& raw);  // declare command handler
DriveMode current;                      // variable to hold current direction

// init motors
Motor motor1 = { 11, 13, 12, false };  // pins for motor 1
Motor motor2 = { 10, 4, 5, false };    // pins for motor 2
Motor motor3 = { 9, 8, 7, true };      // pins for motor 3
Motor motor4 = { 6, 3, 2, true };      // pins for motor 4

// init arr of motors
Motor motors[4] = { motor1, motor2, motor3, motor4 };

/*
* initialize motor pins
* @param m - motor to initialize
*/
void initMotor(const Motor& m) {
  pinMode(m.enable, OUTPUT);
  pinMode(m.input1, OUTPUT);
  pinMode(m.input2, OUTPUT);
  analogWrite(m.enable, 0);
  digitalWrite(m.input1, LOW);
  digitalWrite(m.input2, LOW);
}

void setup() {
  for (auto& m : motors) initMotor(m);  // init each motor
  Serial.begin(19200);
  while (!Serial)
    ;

  // init network connection, print status
  bool connectSuccess = network.begin();
  Serial.print("Network Connection : ");
  Serial.println(connectSuccess ? "Connected" : "Failed");
  Serial.print("IP Address : ");
  Serial.println(network.localIP());  // will be all zero until connection

  setAllMotors(STOP, 0);
  Serial.println("Setup Complete.");
}

void loop() {
  String cmd;
  // poll for incoming commands
  if (network.poll(cmd)) handleCommand(cmd);
}

// ========= COMMAND HANDLER BELOW ================
/*
* Handle incoming commands
* @param command - incoming command
* command options: stop, forward, reverse, left, right, speed (x)
*/
void handleCommand(const String& command) {
  String cmd = command;
  cmd.trim();
  cmd.toLowerCase();
  Serial.print("Handling Command : ");
  Serial.println(cmd);

  if (cmd == "stop") {
    setAllMotors(STOP, 0);
    current = STOP;
  } else if (cmd == "forward") {
    setAllMotors(FWD, DEFAULT_SPEED);
    current = FWD;
  } else if (cmd == "reverse") {
    setAllMotors(REV, DEFAULT_SPEED);
    current = REV;
  } else if (cmd == "left") {
    turnLeft();
    current = LEFT;
  } else if (cmd == "right") {
    turnRight();
    current = RIGHT;
  } else if (cmd == "fast") {
    if (current == FWD || current == REV) {
      Serial.println("Changing Speed to : 100");
      setAllMotors(current, 100);
    } else if (current == RIGHT || current == LEFT) {
      setAllMotors(STOP, 0);
      Serial.println("Can't change speed when turning");
    } else {
      Serial.print("Moving Forward at full Speed.");
      setAllMotors(FWD, 100);
      current = FWD;
    }
  } else if (cmd == "slow") {
    if (current == FWD || current == REV) {
      Serial.println("Changing Speed to : 50");
      setAllMotors(current, 50);
    } else if (current == RIGHT || current == LEFT) {
      setAllMotors(STOP, 0);
      Serial.println("Can't change speed when turning");
    } else {
      Serial.print("Moving Forward at half Speed.");
      setAllMotors(FWD, 50);
      current = FWD;
    }
  } else {
    Serial.print("Unknown cmd: ");
    Serial.println(cmd);
  }
}
// ========= MOVEMENT FUNCTIONS BELOW ================
/*
* Configurable function to move a motor/wheel fwd, reverse or full stop (can also pass the speed)
* @param m - motor to control
* @param mode - fwd, reverse or stop
* @param speed - value between 0-100% for speed control
*/
void setMotor(const Motor& m, DriveMode mode, int speed) {
  speed = constrain(speed, 0, 100);  // passed speeds must be between 0-100%
  if (speed == 0) mode = STOP;       // if no speed, just run stop

  // convert pct speed to analog, above minimum threshold
  speed = (uint8_t)map(speed, 0, 100, 160, 255);

  // stop power before direction changes
  analogWrite(m.enable, 0);
  delay(2);

  switch (mode) {
    case FWD:
      digitalWrite(m.input1, !m.inverted ? HIGH : LOW);
      digitalWrite(m.input2, !m.inverted ? LOW : HIGH);
      analogWrite(m.enable, speed);
      break;

    case REV:
      digitalWrite(m.input1, m.inverted ? HIGH : LOW);
      digitalWrite(m.input2, m.inverted ? LOW : HIGH);
      analogWrite(m.enable, speed);
      break;
    case STOP:
      digitalWrite(m.input1, LOW);
      digitalWrite(m.input2, LOW);
      break;
    default:
      Serial.println("Unknown Drive Mode.");
  }
}

/*
* stop, turn right, stop again and wait for command
*/
void turnRight() {
  for (auto& m : motors) setMotor(m, STOP, 0);  // stop all motors
  delay(100);

  // drive left side fwd
  setMotor(motor1, FWD, 50);  // always turn at half speed
  setMotor(motor2, FWD, 50);

  // drive right side in rev
  setMotor(motor3, REV, 50);
  setMotor(motor4, REV, 50);
  delay(3000);  // test for correct turn time

  for (auto& m : motors) setMotor(m, STOP, 0);  // stop all motors
  current = STOP;
}

/*
* stop, turn left, stop again and wait for command
*/
void turnLeft() {
  for (auto& m : motors) setMotor(m, STOP, 0);  // stop all motors
  delay(100);

  // drive left side rev
  setMotor(motor1, REV, 50);  // always turn at half speed
  setMotor(motor2, REV, 50);

  // drive right side fwd
  setMotor(motor3, FWD, 50);
  setMotor(motor4, FWD, 50);

  delay(3000);  // test for correct turn time

  for (auto& m : motors) setMotor(m, STOP, 0);  // stop all motors
  current = STOP;
}

/*
  * Set all motors to the same drive mode
  * @param mode - drive mode (fwd, rev, stop)
  * @param speed - speed to drive at (0-100%)
  */
void setAllMotors(DriveMode mode, int speed) {
  for (auto& m : motors) setMotor(m, mode, speed);
}
