//update below with template ID from blynk
#define BLYNK_TEMPLATE_ID "[get that stuff from blynk]"
#define BLYNK_TEMPLATE_NAME "[get that stuff from blynk]"
#define BLYNK_AUTH_TOKEN "[get that stuff from blynk]"
#define NETWORK "YOUR WIFI NETWORK"
#define NETWORKPASS "YOUR WIFI PASSWORD"
#include <BlynkSimpleEsp8266.h>
#include <AccelStepper.h>
#include <stdlib.h>

int stepsPerInch = 344;
int windowWidth = 0; 
//int closedPosition = (windowWidth/2)*stepsPerInch;
int closedPosition = 0;
int openPosition = 0;
//-955 should be travel dis we need 
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
char ssid[] = NETWORK;
char pass[] = NETWORKPASS;
// Blynk authentication token
char auth[] = BLYNK_AUTH_TOKEN;

// Create an instance of the AccelStepper class
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

//make sure blinds are "open" when device first powers on. 
AccelStepper setCurrentPosition();

// Variable to store the current state of V1 pin
int v1State = 0;
int currentState = 0;
BLYNK_WRITE(V1) {
  // Called when V1 button changes state
  v1State = param.asInt();
  Serial.print("V1 Slider value is: ");
  Serial.println(v1State);

  // Move the stepper motor based on the V1 pin state
  stepper.enableOutputs();
  Serial.println("Stepper Motor Enabled!\n"); // Indicate moter has started
  Blynk.virtualWrite(V4, v1State);
  Blynk.syncVirtual(V4);
 // Send the new value to V1

  if (v1State == 1) {
    stepper.moveTo(closedPosition);
    Serial.println("Stepper Moving Down"); // Indicate movement
  } else if (v1State == 0) {
    stepper.moveTo(openPosition);
    Serial.println("Stepper Moving Up"); // Indicate movement
  }
}
BLYNK_WRITE(V2) {
  //called when v2 value changes
  windowWidth = param.asInt();
  Serial.print("Window Width in Inches:");
  Serial.println(windowWidth);
  closedPosition = (windowWidth/2)*stepsPerInch;
}

BLYNK_WRITE(V4) {
  //called when v4 value changes
  currentState = param.asInt();
  Serial.print("Value for CurrentState:");
  Serial.println(currentState);

}

void setup() {
  Serial.begin(115200);
  // Set up Blynk
  Blynk.begin(auth, NETWORK, NETWORKPASS);

  // Set up the stepper motor
  stepper.setMaxSpeed(1500.0);
  stepper.setAcceleration(500.0);
  stepper.setPinsInverted(false,true,true);
  stepper.setEnablePin(ENABLE_PIN);
  stepper.disableOutputs();
  // Set up the button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V4);
  // add logic to check value for currentState i.e V4. if closed set current posssition to 0, of open set to closedPosition value. 
  if (currentState == 1){
    stepper.setCurrentPosition(openPosition);
  }
  if (currentState == 0){
    stepper.setCurrentPosition(closedPosition);
  }


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

