#include <Servo.h> 

// The traffic light structure. This is made of a red, yellow, and green light.
struct TrafficLight {	
	int redPin;
	int yellowPin;
	int greenPin;
	
	bool red;
	bool yellow;
	bool green;
};

// Define the light colours.
enum lightColours {
    RED,
    YELLOW,
    GREEN
};

unsigned char lightState; // The current state of the traffic lights, in the range [0, 5].

unsigned long previousMillis; // The time since we last changed the light.

int delayTime; // How long to wait before changing the light.

const int TWO_RED_TIME = 1000; // How long to stay at two red lights.
const int YELLOW_TIME = 1500; // How long to stay at a yellow light.
const int GREEN_TIME = 3000; // How long to stay at a green light.

const int DOUBLE_RED = 13; // Output pin to the two red lights.
const int DOUBLE_YELLOW = 12; // Output pin to the two yellow lights.
const int DOUBLE_GREEN = 11; // Output pin to the two green lights.

const int SINGLE_RED = 10; // Output pin to the single red light.
const int SINGLE_YELLOW = 9; // Output pin to the single yellow light.
const int SINGLE_GREEN = 8; // Output pin to the single green light.

const int SERVO_CONTROL = 7; // The pin to control the servo.

const int STREET_LIGHT_PIN = 6; // The pin for the street light.

const int LDR_PIN = A1; // Pin for LDR input.

const int IR_INPUT = A0; // Pin for IR input.
const int IR_OUTPUT = 5; // Pin for IR output.

Servo gate; // The gate servo.

struct TrafficLight singleLight; // The light that operates on its own.
struct TrafficLight doubleLight; // The two lights that operate together.

bool speedingUp; // If the button has been pressed during this state.

// The setup function runs once when you press reset or power the board
void setup() {

	// Set the pins of the single light.
	singleLight.redPin = SINGLE_RED;
	singleLight.yellowPin = SINGLE_YELLOW;
	singleLight.greenPin = SINGLE_GREEN;
	
	// Set the pins of the double light.
	doubleLight.redPin = DOUBLE_RED;
	doubleLight.yellowPin = DOUBLE_YELLOW;
	doubleLight.greenPin = DOUBLE_GREEN;
	
	// Set the outputs to output mode.
	pinMode(doubleLight.redPin, OUTPUT);
	pinMode(doubleLight.yellowPin, OUTPUT);
	pinMode(doubleLight.greenPin, OUTPUT);
	pinMode(singleLight.redPin, OUTPUT);
	pinMode(singleLight.yellowPin, OUTPUT);
	pinMode(singleLight.greenPin, OUTPUT);
	pinMode(STREET_LIGHT_PIN, OUTPUT);
	pinMode(IR_OUTPUT, OUTPUT);

	// Set the inputs to input mode.
	pinMode(IR_INPUT, INPUT);

	// Set the variables to their inital values.
	lightState = 0;
	previousMillis = 0;
	delayTime = GREEN_TIME;
	speedingUp = false;
	
	// Turn off all of the lights at the start.
	digitalWrite(DOUBLE_RED, LOW);
	digitalWrite(DOUBLE_YELLOW, LOW);
	digitalWrite(DOUBLE_GREEN, LOW);
	digitalWrite(SINGLE_RED, LOW);
	digitalWrite(SINGLE_YELLOW, LOW);
	digitalWrite(SINGLE_GREEN, LOW);
	
	digitalWrite(IR_OUTPUT, HIGH);
	
	gate.attach(SERVO_CONTROL);
	
	Serial.begin(9600);
}

// Returns if it's dark right now, based on the cutoff.
bool isDark(int cutoff) {
	int lightLevel = analogRead(LDR_PIN); 
	
	if (lightLevel < cutoff) // If it's dark.
		return true;
	else // If it's bright.
		return false;
}

void printPinSet(int pin, int level) {
	Serial.print("Set pin ");
	Serial.print(pin);
	Serial.print(" to ");
	Serial.print(level);
	Serial.println(".");
}

// Set the specified light to the specified colour.
void setLight(TrafficLight light, enum lightColours colour) {
	digitalWrite(light.redPin, LOW);
	digitalWrite(light.yellowPin, LOW);
	digitalWrite(light.greenPin, LOW);
	
	printPinSet(light.redPin, LOW);
	printPinSet(light.yellowPin, LOW);
	printPinSet(light.greenPin, LOW);

	if (colour == RED) {
		digitalWrite(light.redPin, HIGH);
		printPinSet(light.redPin, HIGH);
	} else if (colour == YELLOW) {
		digitalWrite(light.yellowPin, HIGH);
		printPinSet(light.yellowPin, HIGH);
	} else {
		digitalWrite(light.greenPin, HIGH);
		printPinSet(light.greenPin, HIGH);
	}
}

/*
 * Update the traffic lights depending on the value of lightState.
 * Set delayTime to the appropriate amount for the lightState.
 * Reset lightState to 0 if it's greater than or equal to 5.
 */
void updateLights() {
	if (lightState == 0) {
		setLight(doubleLight, RED);
		setLight(singleLight, GREEN);
		delayTime = GREEN_TIME;
	} else if (lightState == 1) {
		setLight(doubleLight, RED);
		setLight(singleLight, YELLOW);
		delayTime = YELLOW_TIME;
	} else if (lightState == 2) {
		setLight(doubleLight, RED);
		setLight(singleLight, RED);
		delayTime = TWO_RED_TIME;
	} else if (lightState == 3) {
		setLight(doubleLight, GREEN);
		setLight(singleLight, RED);
		delayTime = GREEN_TIME;
	} else if (lightState == 4) {
		setLight(doubleLight, YELLOW);
		setLight(singleLight, RED);
		delayTime = YELLOW_TIME;
	} else {
		setLight(doubleLight, RED);
		setLight(singleLight, RED);
		delayTime = TWO_RED_TIME;
		lightState = 0;
	}
}

// Return whether or now the button is down.
bool isButtonDown() {
	return true;
}

// The loop function runs over and over again forever
void loop() {
	/*int irReading = analogRead(IR_INPUT);
	Serial.println(irReading);
	if (irReading >= 1005) {
		buzz(50);
	}
	*/
	
	// TODO: Check later if speedingUp variable is necessary.
	if (isButtonDown() && !speedingUp && lightState == 0) {
		speedingUp = true;
		delayTime -= (delayTime - (currentMillis - previousMillis)) / 2;
	}
	
	unsigned long currentMillis = millis();
	if (currentMillis - previousMillis >= delayTime) {
		previousMillis = millis();
		lightState++;
		updateLights();
    }
	
	/*
	if (isDark(125))
		digitalWrite(STREET_LIGHT_PIN, HIGH); // Turn off the street light.
	else
		digitalWrite(STREET_LIGHT_PIN, LOW); // Turn off the street light.
	
	for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		gate.write(pos);              // tell servo to go to position in variable 'pos'
		delay(15);                       // waits 15ms for the servo to reach the position
	}
	for (int pos = 180; pos >= -180; pos -= 1) { // goes from 180 degrees to 0 degrees
		gate.write(pos);              // tell servo to go to position in variable 'pos'
		delay(15);                       // waits 15ms for the servo to reach the position
	}
	
	gate.write(servoPosition);
	
	if (servoPosition == 0)
		servoPosition = 180;
	else
		servoPosition = 0;
	
	
	*/
}

