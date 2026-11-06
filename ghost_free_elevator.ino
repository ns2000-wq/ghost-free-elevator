/*
 * Ghost-Free Elevator  ---  Arduino reference sketch
 * -------------------------------------------------------------
 * Idea: an elevator should only STOP at a requested floor if a
 * person is actually waiting there. If the floor is empty, PASS.
 *
 * LEDs:  Yellow = floor requested (standby)
 *        Blue   = person detected -> STOP
 *        Red    = no one there    -> PASS
 *
 * Parts: Arduino Uno, PIR motion sensor, push button, 3 LEDs (+330R each)
 *
 * NOTE (호윤): 이건 로직을 깔끔히 정리한 "참고 스케치"야.
 *   핀 번호/딜레이는 네 회로에 맞게 바꾸고, 주석도 네 식으로 다듬어.
 * -------------------------------------------------------------
 */

// ---- Pin assignments (change to match your wiring) ----
const int BUTTON_PIN = 2;   // floor "stop" request button
const int PIR_PIN    = 3;   // PIR motion sensor (HIGH = motion)
const int LED_YELLOW = 11;  // requested / standby
const int LED_BLUE   = 12;  // person waiting -> STOP
const int LED_RED    = 13;  // empty -> PASS

// ---- Timing (milliseconds) ----
const unsigned long APPROACH_MS = 1500; // time for the car to "reach" the floor
const unsigned long RESULT_MS   = 2000; // how long to show the STOP/PASS result

// ---- States ----
enum State { IDLE, REQUESTED, APPROACHING, DECIDE };
State state = IDLE;
unsigned long stateStart = 0;

void setLeds(bool y, bool b, bool r) {
  digitalWrite(LED_YELLOW, y);
  digitalWrite(LED_BLUE,   b);
  digitalWrite(LED_RED,    r);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // button to GND, pressed = LOW
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  setLeds(false, false, false);
  Serial.begin(9600);
  Serial.println("Ghost-Free Elevator ready.");
}

void loop() {
  switch (state) {

    case IDLE:
      // Wait for a floor request (button press).
      if (digitalRead(BUTTON_PIN) == LOW) {
        setLeds(true, false, false);      // Yellow ON
        Serial.println("Floor requested.");
        state = REQUESTED;
        stateStart = millis();
      }
      break;

    case REQUESTED:
      // Small debounce, then the car starts approaching.
      if (millis() - stateStart > 300) {
        Serial.println("Elevator approaching...");
        state = APPROACHING;
        stateStart = millis();
      }
      break;

    case APPROACHING:
      // Car travels to the floor; when it arrives, check the sensor.
      if (millis() - stateStart > APPROACH_MS) {
        state = DECIDE;
      }
      break;

    case DECIDE: {
      bool personWaiting = (digitalRead(PIR_PIN) == HIGH);
      if (personWaiting) {
        setLeds(false, true, false);      // Blue ON
        Serial.println("Person detected -> STOP.");
      } else {
        setLeds(false, false, true);      // Red ON
        Serial.println("No one here -> PASS.");
      }
      delay(RESULT_MS);                    // show the result
      setLeds(false, false, false);
      state = IDLE;                        // ready for the next request
      break;
    }
  }
}
