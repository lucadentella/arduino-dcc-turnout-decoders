#define LONG_PRESS_TIME   1000
#define SHORT_PRESS_TIME  100
#define BOUNCE_TIME       40

void checkButtons() {

  // Program Button, supports short press and long press
  btnProgram.currentState = digitalRead(BTN_PROGRAM_PIN);
  if(btnProgram.lastState == HIGH && btnProgram.currentState == LOW) btnProgram.pressedTime = millis();
  else if(btnProgram.lastState == LOW && btnProgram.currentState == HIGH) {
    btnProgram.releasedTime = millis();
    long pressDuration = btnProgram.releasedTime - btnProgram.pressedTime;
    if(pressDuration > LONG_PRESS_TIME) btnProgramLongPressed();
    else if(pressDuration > BOUNCE_TIME) btnProgramShortPressed();
  }
  btnProgram.lastState = btnProgram.currentState;

  // UP Button
  btnUp.currentState = digitalRead(BTN_UP_PIN);
  if(btnUp.lastState == HIGH && btnUp.currentState == LOW) btnUp.pressedTime = millis();
  else if(btnUp.lastState == LOW && btnUp.currentState == LOW) {
    long pressDuration = millis() - btnUp.pressedTime;
    if(pressDuration > SHORT_PRESS_TIME) {
      btnUpPressed();
      btnUp.pressedTime = millis();
    }
  }
  else if(btnUp.lastState == LOW && btnUp.currentState == HIGH) {
    long pressDuration = millis() - btnUp.pressedTime;
    if(pressDuration > BOUNCE_TIME) btnUpPressed();
  }
  btnUp.lastState = btnUp.currentState;

  // DOWN Button
  btnDown.currentState = digitalRead(BTN_DOWN_PIN);
  if(btnDown.lastState == HIGH && btnDown.currentState == LOW) btnDown.pressedTime = millis();
  else if(btnDown.lastState == LOW && btnDown.currentState == LOW) {
    long pressDuration = millis() - btnDown.pressedTime;
    if(pressDuration > SHORT_PRESS_TIME) {
      btnDownPressed();
      btnDown.pressedTime = millis();
    }
  }  
  else if(btnDown.lastState == LOW && btnDown.currentState == HIGH) {
    long pressDuration = millis() - btnDown.pressedTime;
    if(pressDuration > BOUNCE_TIME) btnDownPressed();
  }
  btnDown.lastState = btnDown.currentState;  
}

// Long press on PROGRAM button: enter/exit program mode
void btnProgramLongPressed() {

  toggleProgramMode();
}

// Short press on PROGRAM button: move to next config item
void btnProgramShortPressed() {

  if(inProgramMode) changeConfigItem();
}

// UP button: increase current value
void btnUpPressed() {

  if(inProgramMode) changeCurrentValue(1);
}

// DOWN button: decrease current value
void btnDownPressed() {

  if(inProgramMode) changeCurrentValue(-1);
}