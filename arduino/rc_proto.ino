
// Nathan Vititoe
// Prototype wiring for simple rc car  
// one Bridge for the left side wheels
// and one bridge for the right side wheels

#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "micro_features_micro_model.h"
#include "micro_features_micro_features_generator.h"
#include "micro_features_micro_model_settings.h"
#include "micro_features_micro_features_generator.h"
#include "micro_features_micro_model_settings.h"
#include "micro_features_micro_speech_model_data.h"
#include "micro_features_micro_model.h"
#include "micro_features_micro_features_generator.h"
#include "micro_features_micro_model_settings.h"

// init left side wheel/motor pins
const int left_ena_1 = 6;
const int left_ena_3 = 2;
const int left_front_neg = 3;
const int left_front_pos = 9;
const int left_rear_pos = 12;
const int left_rear_neg = 8;

// init right side wheel/motor pins
const int right_ena_1 = 11;
const int right_ena_3 = 14;
const int right_front_neg = 10;
const int right_front_pos = 4;
const int right_rear_pos = 5;
const int right_rear_neg = 7;

void setup()
{
  // create arr of all motor pins
  const int motor_pins[] = {
    left_ena_1, 
    left_ena_3, 
    left_front_neg, 
    left_front_pos, 
    left_rear_pos, 
    left_rear_neg,
    right_ena_1, 
    right_ena_3,
    right_front_neg,
    right_front_pos,
    right_rear_pos,
    right_rear_neg,
  };
  // set all motor pins to output
  for (int i = 0; i < 12; i++) {
    pinMode(motor_pins[i], OUTPUT);
  }
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // go_forward();
  reverse();
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100); 
  digitalWrite(LED_BUILTIN, LOW);
}

// move car forward, 

void go_forward() {
  // set both left motors speed to 200
  analogWrite(left_ena_1, 100); 
  analogWrite(left_ena_3, 100);

  // set left motors forward
  digitalWrite(left_front_pos, HIGH);
  digitalWrite(left_rear_neg, HIGH);
  
  // ensure left reverse is off
  digitalWrite(left_front_neg, LOW);
  digitalWrite(left_rear_pos, LOW);
	
  // set both right motors speed to 200
  analogWrite(right_ena_1, HIGH);
  analogWrite(right_ena_3, HIGH);
  
  // set right motors forward 
  digitalWrite(right_front_pos, HIGH);
  digitalWrite(right_rear_neg, HIGH);
  
  // ensure right reverse is off
  digitalWrite(right_front_neg, LOW);
  digitalWrite(right_rear_pos, LOW);

  delay(2000); // Move forward for 2s
}

// move car backward,
void reverse() {
  // set both left motors speed to 200
  analogWrite(left_ena_1, 100); 
  analogWrite(left_ena_3, 100);

  // set left motors backward
  digitalWrite(left_front_neg, HIGH);
  digitalWrite(left_rear_neg, HIGH);
  
  // ensure left forward is off
  digitalWrite(left_front_pos, LOW);
  digitalWrite(left_rear_pos, LOW);
	
  // set both right motors speed to 200
  analogWrite(right_ena_1, 200);
  analogWrite(right_ena_3, 200);
  
  // set right motors backward 
  digitalWrite(right_front_neg, HIGH);
  digitalWrite(right_rear_neg, HIGH);
  
  // ensure right forward is off
  digitalWrite(right_front_pos, LOW);
  digitalWrite(right_rear_pos, LOW);

  delay(2000); // Move backward for 2s
}