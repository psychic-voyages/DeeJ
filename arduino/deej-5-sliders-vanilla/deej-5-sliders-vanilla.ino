const int NUM_SLIDERS = 4;
const unsigned int sliderBits = pow(2.0,NUM_SLIDERS)-1;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A4, A5};
const int buttonIn = 7;
const int srClock = 6;
const int srLatch = 5;
const int srData = 4;

int analogSliderValues[NUM_SLIDERS];
bool sliderMute[NUM_SLIDERS];
unsigned int ledOut = 0;
unsigned int ledOffset;
bool buttonPressed = false;

void setup() { 
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
    sliderMute[i] = false;
  }
  ledOffset = ledOut << NUM_SLIDERS;


  pinMode(buttonIn, INPUT);
  pinMode(srClock, OUTPUT);
  pinMode(srLatch, OUTPUT);
  pinMode(srData, OUTPUT);

  digitalWrite(srLatch, LOW);
  shiftOut(srData, srClock, LSBFIRST, 0x0F);
  digitalWrite(srLatch, HIGH);

  Serial.begin(9600);
}

void loop() {
  updateButtonValues();
  updateSliderValues();

  sendSliderValues(); // Actually send data (all the time)
  // printSliderValues(); // For debug
  // myDebug();
  delay(10);
}

void updateButtonValues() {
  bool buttonCheck = false;

  if (digitalRead(buttonIn) == HIGH && !buttonPressed) {
    for (int i = 0; i < NUM_SLIDERS; i++) {
      unsigned int buttonOffset = 1 << i;
      digitalWrite(srLatch, LOW);
      for (int j = 0; j < NUM_SLIDERS/4; j++) {
        byte dataOut = ledOffset | buttonOffset >> (8 * j);
        shiftOut(srData, srClock, LSBFIRST, dataOut);
      }
      digitalWrite(srLatch, HIGH);
    
      delay(5);

      if (digitalRead(buttonIn) == HIGH) {
        ledOut = ledOut ^ (1<<i);
        sliderMute[i] = !sliderMute[i];
        break;
      }
    }
    ledOffset = ledOut << NUM_SLIDERS;
    digitalWrite(srLatch, LOW);
    shiftOut(srData, srClock, LSBFIRST, ledOffset | sliderBits);
    digitalWrite(srLatch, HIGH);
    buttonPressed = true;
  } else if (digitalRead(buttonIn) == LOW && buttonPressed) {
    digitalWrite(srLatch, LOW);
    shiftOut(srData, srClock, LSBFIRST, ledOffset | sliderBits);
    digitalWrite(srLatch, HIGH);
    buttonPressed = false;
  }
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
     analogSliderValues[i] = analogRead(analogInputs[i]);
     if (sliderMute[(NUM_SLIDERS - 1) - i]) {
      analogSliderValues[i] = 0;
     }
  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  
  Serial.println(builtString);
}

void myDebug () {
  Serial.println(digitalRead(buttonIn));
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
