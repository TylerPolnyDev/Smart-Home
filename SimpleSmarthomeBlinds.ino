//update below with template ID from blynk
#define BLYNK_TEMPLATE_ID "BLYNK TEMPLATE ID GOES HERE"
#define BLYNK_TEMPLATE_NAME "BLYNK TEMPLATE NAME GOES HERE"
#define BLYNK_AUTH_TOKEN "BLYNK AUTH TOKEN GOES HERE"
#include <BlynkSimpleEsp8266.h>
#include <AccelStepper.h>
#include <stdlib.h>

//adjust these values to tune how far blinds travel when set to open or closed. 
int closedPosition = -41000;
int openPosition = 0;

// Define the pinout
#define STEP_PIN D2
#define DIR_PIN D3
#define ENABLE_PIN D4
#define BUTTON_PIN D8

//initilize button
const int buttonPin = D8;
int buttonState = 0;
int previousButtonState = 0;
int toggleValue = 0; // Initial value for V1

//insert WIFI credentials here
char ssid[] = "WIFI NAME GOES HERE";
char pass[] = "WIFI PASSWORD GOES HERE";
// Blynk authentication token
char auth[] = BLYNK_AUTH_TOKEN;

// Create an instance of the AccelStepper class
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

//make sure blinds are "open" when device first powers on. 
AccelStepper setCurrentPosition();

// Variable to store the current state of V1 pin
int v1State = 0;
BLYNK_WRITE(V1) {
  // Called when V1 button changes state

  v1State = param.asInt();
  Serial.print("V1 Slider value is: ");
  Serial.println(v1State);

  // Move the stepper motor based on the V1 pin state
  stepper.enableOutputs();
  Serial.println("Stepper Motor Enabled!\n"); // Indicate moter has started

  if (v1State == 1) {
    stepper.moveTo(closedPosition);
    Serial.println("Stepper Moving Down"); // Indicate movement
  } else if (v1State == 0) {
    stepper.moveTo(openPosition);
    Serial.println("Stepper Moving Up"); // Indicate movement
  }
}

void setup() {
  Serial.begin(115200);
  // Set up Blynk
  Blynk.begin(auth,ssid,pass);

  // Set up the stepper motor
  stepper.setMaxSpeed(1500.0);
  stepper.setAcceleration(500.0);
  stepper.setPinsInverted(false,true,true);
  stepper.setEnablePin(ENABLE_PIN);
  stepper.disableOutputs();
  // Set up the button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {

  Blynk.run();

  //handeling a button press on device phyisical button
  buttonState = digitalRead(buttonPin);
  if (buttonState != previousButtonState && buttonState == LOW) {
    toggleValue = !toggleValue; // Toggle the value
    Blynk.virtualWrite(V1, toggleValue); // Send the new value to V1
    Blynk.syncVirtual(V1);
  }
  previousButtonState = buttonState;

  stepper.run();

  //release power from moter once traveling has finished
  bool running = stepper.isRunning();
  if (!running){
    stepper.disableOutputs();
  }

}

