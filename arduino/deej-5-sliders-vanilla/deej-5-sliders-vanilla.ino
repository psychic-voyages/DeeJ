const int NUM_SLIDERS = 4;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3};
const int buttonInputs[NUM_SLIDERS] = {7,6,5,4};
const int ledOutputs[NUM_SLIDERS] = {11,10,9,8};


int analogSliderValues[NUM_SLIDERS];
bool sliderMute[NUM_SLIDERS];
bool currentRead[NUM_SLIDERS];
bool buttonPressed = false;

void setup() { 
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
    pinMode(buttonInputs[i], INPUT);
    pinMode(ledOutputs[i], OUTPUT);
    sliderMute[i] = false;
  }

  Serial.begin(9600);
}

void loop() {
  updateSliderValues();

  // move to -> updateMuteValues()
  bool buttonCheck = false;
  for (int i = 0; i < NUM_SLIDERS; i++) {
    currentRead[i] = digitalRead(buttonInputs[i]);
    if (digitalRead(buttonInputs[i]) == true) {
      buttonCheck = true;
    }
  }

  if (buttonCheck && !buttonPressed) {
    for (int i = 0; i < NUM_SLIDERS; i++) {
      if (currentRead[i]) {
        sliderMute[i] = !sliderMute[i];
      }
      digitalWrite(ledOutputs[i], sliderMute[i]);
    }
    buttonPressed = true;
  } else if (!buttonCheck && buttonPressed) {
    buttonPressed = false;
  }

  sendSliderValues(); // Actually send data (all the time)
  // printSliderValues(); // For debug
  myDebug();
  delay(10);
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
     analogSliderValues[i] = analogRead(analogInputs[i]);
  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    analogSliderValues[i] = sliderMute[i] ? 0 : analogSliderValues[i];
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  
  Serial.println(builtString);
}

void myDebug () {

}

void printSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(analogSliderValues[i]) + String(" mV");
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1) {
      Serial.write(" | ");
    } else {
      Serial.write("\n");
    }
  }
}
