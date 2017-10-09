#include <Bounce2.h>
#include <Servo.h>

// ************************ Global Stuff ************************

const byte buttonPin = 12;
const byte servoPin = 9;
const byte ledPin = 2;

enum dirState {forward, reverse};

typedef struct timer_t{
	uint32_t current;
	uint32_t last;
	uint32_t freq;
};

typedef struct myServo_t{
	byte currentPos;
	byte targetPos;
	bool active;
	dirState direction;
};

Servo myServo;
Bounce button = Bounce();

// ************************ Arduino Stuff ***********************

void setup() {
	Serial.begin(9600);
	pinMode(buttonPin, INPUT_PULLUP);
	pinMode(ledPin, OUTPUT);
	myServo.attach(servoPin);
	button.attach(buttonPin);
}

void loop() {
	timer_t timerServo;
	myServo_t mainServo;
	initTimer(timerServo, 15);
	initServo(mainServo);

	// faux main loop
	while(true){ 
		if(getButton()){
			flipServoDirection(mainServo);
			serialDebug(mainServo);     
		}
		if(mainServo.active){
			if (checkTime(timerServo)){
				updateServo(mainServo);
				myServo.write(mainServo.currentPos);
			}
		}
		digitalWrite(ledPin, mainServo.active);
	}
}

// ************************ Super Fun Time Happy Land! **********

// Set intial time - run once per timer instance at start
void initTimer(timer_t &t, uint32_t freq){
	t.current = millis();
	t.last = t.current;
	t.freq = freq;
}

// Set intial servo state - run once per servo instance at start
void initServo(myServo_t &s){
	s.direction = reverse;
	s.active = false;
	s.targetPos = 0;
	s.currentPos = 0;
	digitalWrite(ledPin, HIGH);
	myServo.write(s.targetPos);
	// give a second to let the servo get to position 0 from wherever it is
	delay(1000);
}

// get if button have been pressed
bool getButton(){  
	if(button.update() && button.read() == LOW){  
		return true;
	}
	return false;
}

// Returns if it is time to trigger some action
bool checkTime(timer_t &t){
	t.current = millis();
	if (t.current > (t.last + t.freq)){
		t.last = t.current;
		return true;
	}
	return false;
}

// Sets direction and target position
void flipServoDirection(myServo_t &s){
	switch(s.direction){
	case forward:
		s.targetPos = 0;
		s.direction = reverse;
		break;
	case reverse:
		s.targetPos = 180;
		s.direction = forward;
		break;
	}
	s.active = true;
}

// Follows state of servo and update step forward/backward
void updateServo(myServo_t &s){
	switch(s.direction){
	case forward:
		if(s.currentPos < s.targetPos){
			s.currentPos++;
		} else {
			s.active = false;
		}
		break;
	case reverse:
		if(s.currentPos > s.targetPos){
			s.currentPos--;
		} else {
			s.active = false;
		}
		break;
	}
}

// View the servo_t struct contents in serial monitor
void serialDebug(myServo_t &s){  
	byte strSize = 60;
	char strServoOut[strSize];
	snprintf(strServoOut, strSize, "act: %d | cPos:  %3d | tPos: %3d | dir: %d\n", 
	s.active, s.currentPos, s.targetPos, s.direction); 
	Serial.print(strServoOut); 
}