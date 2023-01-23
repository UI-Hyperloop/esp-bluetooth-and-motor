//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// use first channel of 16 channels (started from zero)
#define LEDC_CHANNEL_0     0

// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT  13

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     1000

BluetoothSerial SerialBT;

int motorPin1 = 13; // Initializes pin
int SOLENOID_PIN = 14;

int state='0';
int flag=0;        //makes sure that the serial only prints once the state
int duty=0;

// Arduino-like analogWrite
// value has to be between 0 and valueMax
void analogwrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (8191 / valueMax) * min(value, valueMax);

  // write duty to LEDC
  ledcWrite(channel, duty);
}

void setup() {
  // sets the pins as outputs:
  // pinMode(motorPin1, OUTPUT);
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(motorPin1, LEDC_CHANNEL_0);
  
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  if (SerialBT.available()) {
    Serial.println("Bluetooth available");
    state = SerialBT.read();
    Serial.println(state);
    flag = 0;
  }
  
  if (flag == 0) {
    switch (state) {
      case 'n':
        duty -= 15;
        if (duty < 0) { 
          duty = 0;
        }
        analogwrite(LEDC_CHANNEL_0, duty);
        Serial.println(duty);
        flag=1;
        break;
      case 'm':
        duty += 15;
        if (duty > 255) { 
          duty = 255;
        }
        analogwrite(LEDC_CHANNEL_0, duty);
        Serial.println(duty);
        flag=1;
        break;
      case 'b':
        duty = 255;
        analogwrite(LEDC_CHANNEL_0, duty);
        Serial.println(duty);
        flag=1;
        break;
      case 'v':
        duty = 0;
        analogwrite(LEDC_CHANNEL_0, duty);
        Serial.println(duty);
        flag=1;
        break;
      // MAKE SURE TO CONNECT THE GROUND OF ESP TO THE GROUND OF THE 12V BATTERY PACK
      case 'x':
        digitalWrite(SOLENOID_PIN, LOW);
        flag=1;
        break;
      case 'c':
        digitalWrite(SOLENOID_PIN, HIGH);
        flag=1;
        break;
      default:
        break;
    }
  }
}
