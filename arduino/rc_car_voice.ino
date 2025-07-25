
// Nathan Vititoe - Voice Controlled RC Car

// Prototype code for implementing TF Lite micro speech for RC Car control
// using voice recognition
// available commands are "stop", "go", "left", "right", "down"

// stop makes the car stop
// go makes the car move forward
// left makes the car turn left
// right makes the car turn right
// down makes the car go in reverse

// TODO: find library that supports mbed_nano architecture
#include <MicroTFLite.h>
#include "micro_speech_model_data.h"

//  define tensor size
constexpr int kTensorArenaSize = 10 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

// init input/output
int16_t* input;
int8_t* output;

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

#define AUDIO_PIN A0
const int sampleWindow = 62;  // 62 microsecond samples
const int numSamples = 512;

void setup() {
  Serial.begin(115200);

  // Set motor pins to OUTPUT
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

  // init model, interpreter, and error reporter
  ModelInit(g_micro_speech_model_data, tensor_arena, kTensorArenaSize);

  Serial.println("Setup Complete.");
}

void loop() {
  // fill model input tensor with audio samples
  constexpr int sampleCount = 512;
  static int16_t micBuffer[sampleCount];

  for (int i = 0; i < sampleCount; i++) {
    int16_t sample = analogRead(AUDIO_PIN) - 512;
    float normalized = sample / 512.0f;  // normalize values
    ModelSetInput(normalized, i);
    delayMicroseconds(sampleWindow);
  }

  // throw error if model cant run an inference
  if (!ModelRunInference()) {
    Serial.println("Inference failed.");
    return;
  }

  // TODO: fix this
  // assign confidence scores
  int max_score = -128;
  int max_index = -1;
  constexpr int outputSize = 12;
  for (int i = 0; i < outputSize; i++) {
    int8_t score = (int8_t)ModelGetOutput(i, true);  // get quantized output
    if (score > max_score) {
      max_score = score;
      max_index = i;
    }
  }

  // if confidence is above 50, map it to a word
  if (max_score > 50) {
    const char* cmd = commandLabel(max_index);
    Serial.print("Heard: ");
    Serial.println(cmd);
    handleCommand(cmd);
  }
}

// map voice commands
const char* commandLabel(int index) {
  switch (index) {
    case 2: return "stop";
    case 3: return "go";
    case 4: return "left";
    case 5: return "right";
    case 10: return "down";
    default: return "unknown";
  }
}

// handle each voice command
void handleCommand(const char* cmd) {
  if (strcmp(cmd, "go") == 0) {
    go_forward();
  } else if (strcmp(cmd, "down") == 0) {
    reverse();
  } else if (strcmp(cmd, "left") == 0) {
    turn_left();
  } else if (strcmp(cmd, "right") == 0) {
    turn_right();
  } else if (strcmp(cmd, "stop") == 0) {
    stop();
  } else {
    return;
  }
}

// move the car forward
void go_forward() {
  // set left speed to 100 through enable pins
  analogWrite(left_ena_1, 100);
  analogWrite(left_ena_3, 100);

  // move left wheels forward
  digitalWrite(left_front_pos, HIGH);
  digitalWrite(left_rear_neg, HIGH);

  // ensure left reverse is off
  digitalWrite(left_front_neg, LOW);
  digitalWrite(left_rear_pos, LOW);

  // set right speed to 100 through enable pins
  analogWrite(right_ena_1, 100);
  analogWrite(right_ena_3, 100);

  // move right wheels forward
  digitalWrite(right_front_pos, HIGH);
  digitalWrite(right_rear_neg, HIGH);

  // ensure right reverse is off
  digitalWrite(right_front_neg, LOW);
  digitalWrite(right_rear_pos, LOW);
}

// move the car backwards
void reverse() {
  // set left speed to 100 through enable pins
  analogWrite(left_ena_1, 100);
  analogWrite(left_ena_3, 100);

  // move left wheels backwards
  digitalWrite(left_front_neg, HIGH);
  digitalWrite(left_rear_neg, HIGH);

  // ensure left forwards is off
  digitalWrite(left_front_pos, LOW);
  digitalWrite(left_rear_pos, LOW);

  // set right speed to 100 through enable pins
  analogWrite(right_ena_1, 100);
  analogWrite(right_ena_3, 100);

  // move right wheels backwards
  digitalWrite(right_front_neg, HIGH);
  digitalWrite(right_rear_neg, HIGH);

  // ensure right forwards is off
  digitalWrite(right_front_pos, LOW);
  digitalWrite(right_rear_pos, LOW);
}

// stop the car
void stop() {
  // set all enable pins to change speed to 0
  analogWrite(left_ena_1, 0);
  analogWrite(left_ena_3, 0);
  analogWrite(right_ena_1, 0);
  analogWrite(right_ena_3, 0);
}

// turn the car left
void turn_left() {
  // stop all left motor activity
  analogWrite(left_ena_1, 0);
  analogWrite(left_ena_3, 0);
  digitalWrite(left_front_pos, LOW);
  digitalWrite(left_rear_neg, LOW);
  digitalWrite(left_front_neg, LOW);
  digitalWrite(left_rear_pos, LOW);

  // set right wheel speed to 100 through enable pins
  analogWrite(right_ena_1, 100);
  analogWrite(right_ena_3, 100);

  // move right wheels fwd
  digitalWrite(right_front_pos, HIGH);
  digitalWrite(right_rear_neg, HIGH);

  // ensure right wheels reverse is off
  digitalWrite(right_front_neg, LOW);
  digitalWrite(right_rear_pos, LOW);
}

// turn the car left
void turn_right() {
  // stop all right motor activity
  analogWrite(right_ena_1, 0);
  analogWrite(right_ena_3, 0);
  digitalWrite(right_front_pos, LOW);
  digitalWrite(right_rear_neg, LOW);
  digitalWrite(right_front_neg, LOW);
  digitalWrite(right_rear_pos, LOW);

  // set left wheel speed to 100 through enable pins
  analogWrite(left_ena_1, 100);
  analogWrite(left_ena_3, 100);

  // move left wheels fwd
  digitalWrite(left_front_pos, HIGH);
  digitalWrite(left_rear_neg, HIGH);

  // ensure left wheels reverse is off
  digitalWrite(left_front_neg, LOW);
  digitalWrite(left_rear_pos, LOW);
}