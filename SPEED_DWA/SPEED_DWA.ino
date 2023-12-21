#include "DFRobot_LedDisplayModule.h"

// Definicje dla wyświetlaczy
DFRobot_LedDisplayModule LED1(&Wire, 0xE6); // Pierwszy wyświetlacz dla prędkości
DFRobot_LedDisplayModule LED2(&Wire, 0xE0); // Drugi wyświetlacz dla napięcia i temperatury

// Zmienne dla prędkości
const int hallPin = 2;
// Zmienne dla napięcia i temperatury
String inputString = "";     
int Vol = 0;
int Temp = 0;

void setup() {
  pinMode(9, OUTPUT)  // dioda LED HIGH VOLTAGE
  Serial.begin(9600);

  while(LED1.begin(LED1.e8Bit) != 0) {
    Serial.println("Failed to initialize the first display, please confirm the chip connection!");
    delay(1000);
  }
  LED1.setDisplayArea(2,3,4);

  while(LED2.begin(LED2.e8Bit) != 0) {
    Serial.println("Failed to initialize the second display, please confirm the chip connection!");
    delay(1000);
  }
  LED2.setDisplayArea(3,4,6,7,8);

  pinMode(hallPin, INPUT);
}

void loop() 
{
  bool currentState = digitalRead(hallPin);
  unsigned long currentMillis = millis();
  if (previousState == HIGH && currentState == LOW && (currentMillis - previousMillis > debounceDelay)) {
    unsigned long interval = currentMillis - previousMillis;
    int speedMPS = (wheelCircumference / interval) * 1000;
    int speedKMH = speedMPS * 3.6;
    displaySpeedOnLED1((int)speedKMH);
    displaySpeedOnLED2(speedKMH);

    previousMillis = currentMillis;
  }
  previousState = currentState;

  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;

    if (inChar == '\n') {
      if (inputString.startsWith("V:")) {
        int splitIndex = inputString.indexOf(",T:");
        String voltStr = inputString.substring(2, splitIndex);
        String tempStr = inputString.substring(splitIndex + 3, inputString.length() - 1);
        
        Vol = voltStr.toInt();
        Temp = tempStr.toInt();
        if (Temp < 0) Temp = -Temp;

        displayVoltageAndTemperature(Vol, Temp);
      }
      inputString = "";
    }
  }
}

void displaySpeedOnLED1(int speed) {
  if (speed < 40) {
    LED1.setDisplayArea();
  } else if (speed < 80) {
    LED1.setDisplayArea(1);
  } else if (speed < 100) {
    LED1.setDisplayArea(1, 2);
  } else if (speed < 120) {
    LED1.setDisplayArea(1, 2, 3);
  } else if (speed < 140) {
    LED1.setDisplayArea(1, 2, 3, 4);
  } else if (speed < 160) {
    LED1.setDisplayArea(1, 2, 3, 4, 5);
  } else if (speed < 180) {
    LED1.setDisplayArea(1, 2, 3, 4, 5, 6);
  } else {
    LED1.setDisplayArea(1, 2, 3, 4, 5, 6, 7, 8);
  }

  LED1.print(88888888);
}

void displayVoltageAndTemperature(int voltage, int temperature) {
  int voltageDigits = countDigits(voltage);

  if (voltageDigits == 2) {
    LED2.setDisplayArea(6, 7);
  } else if (voltageDigits == 3 ) {
    LED2.setDisplayArea(6, 7, 8);
  }

  String combinedStr = String(voltage) + String(temperature);
  int displayValue = combinedStr.toInt();
  LED2.print(displayValue);
}

int countDigits(int number) {
  int digits = 0;
  while (number) {
    number /= 10;
    digits++;
  }
  return digits;
}




void displaySpeedOnLED2(int speedKMH) {
  // Ustal obszar wyświetlania na drugim wyświetlaczu
  LED2.setDisplayArea(1, 2, 3);

  // Wyświetl prędkość na wyświetlaczu
  LED2.print((int)speedKMH);
  Serial.print("Predkosc: ");
  Serial.print(speedKMH);
  Serial.println(" km/h");
}
