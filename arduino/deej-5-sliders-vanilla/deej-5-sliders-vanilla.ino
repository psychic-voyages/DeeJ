const int NUM_SLIDERS = 6;
const unsigned int sliderBits = pow(2.0,NUM_SLIDERS)-1;
const unsigned int shiftSegments = ceil(((float)NUM_SLIDERS)/4);
// const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A4};
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A4, A5};
const int buttonIn = 7;
const int srClock = 6;
const int srLatch = 5;
const int srData = 4;

int analogSliderValues[NUM_SLIDERS];
bool sliderMute[NUM_SLIDERS];
unsigned int ledOut = 0;
unsigned int ledOffset = 0;
bool buttonPressed = false;

void PRINTBIN (unsigned int input) {
  Serial.print("Binary Output: ");
  Serial.println(32768 | input, BIN);
}

void PRINTBYTE (unsigned int input) {
  Serial.print("Binary Output: ");
  Serial.println(0B100000000 | input, BIN);
}

void shiftData (unsigned int dataRaw) {
  digitalWrite(srLatch, LOW);
    for (int i = 0; i < shiftSegments; i++) {
      byte data = dataRaw >> (8 * i);
      shiftOut(srData, srClock, LSBFIRST, data);
      // delay(2);
    }
    digitalWrite(srLatch, HIGH);
}

void setup() { 
  Serial.begin(9600);
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
    sliderMute[i] = false;
  }

  pinMode(buttonIn, INPUT);
  pinMode(srClock, OUTPUT);
  pinMode(srLatch, OUTPUT);
  pinMode(srData, OUTPUT);

  for (int i = 0; i < NUM_SLIDERS; i++) {
    shiftData(((1 << (i+NUM_SLIDERS)) | 0));
    delay(75);
  }

  shiftData(0 | sliderBits);
}

void loop() {
  updateButtonValues();
  updateSliderValues();

  sendSliderValues(); // Actually send data (all the time)
  // printSliderValues(); // For debug
  delay(10);
}

void updateButtonValues() {
  bool buttonCheck = false;

  if (digitalRead(buttonIn) == HIGH && !buttonPressed) {
    // Serial.println("tuuched");
    for (int i = 0; i < NUM_SLIDERS; i++) {
      shiftData(ledOffset | (1 << i));
      
      delay(5);

      // Serial.print(i);
      if (digitalRead(buttonIn) == HIGH) {
        ledOut = ledOut ^ (1<<i);
        sliderMute[i] = !sliderMute[i];
        // Serial.println("<-");
        break;
      }
      // Serial.println();
    }
    ledOffset = ledOut << NUM_SLIDERS;

    shiftData(ledOffset | sliderBits);

    buttonPressed = true;
  } else if (digitalRead(buttonIn) == LOW && buttonPressed) {
    // Serial.println("untuuched");
    buttonPressed = false;
  }
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
     analogSliderValues[i] = analogRead(analogInputs[i]);
     if (sliderMute[i]) {
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
