/*
pin # 4(OC0B)(PG5)pwm signal      to ir_led
pin # 13(OC1C)(PB7)pwm signal     to red_ch1
pin # 12(OC1B)(PB6)pwm signal     to green_ch1
pin # 11(OC1A)(PB5)pwm signal     to blue_ch1
pin # 3(OC3C)(PE5)pwm signal      to red_ch2
pin # 2(OC3B)(PE4)pwm signal      to green_ch2
pin # 5(OC3A)(PE3)pwm signal      to blue_ch2
pin # 8(OC4C)(PH5)pwm signal      to red_ch3
pin # 7(OC4B)(PH4)pwm signal      to green_ch3
pin # 6(OC4A)(PH3)pwm signal      to blue_ch3
pin # 44(OC5C)(PL5)pwm signal     to red_ch4
pin # 45(OC5B)(PL4)pwm signal     to green_ch4
pin # 46(OC5A)(PL3)pwm signal     to blue_ch4
pin # 30  to Enable red_ch1       PORTC7
pin # 31  to Enable green_ch1     PORTC6
pin # 32  to Enable blue_ch1      PORTC5
pin # 33  to Enable red_ch2       PORTC4
pin # 34  to Enable green_ch2     PORTC3
pin # 35  to Enable blue_ch2      PORTC2
pin # 36  to Enable red_ch3       PORTC1
pin # 37  to Enable green_ch3     PORTC0
pin # 38  to Enable blue_ch3      PORTD7
pin # 39  to Enable red_ch4       PORTG2
pin # 40  to Enable green_ch4     PORTG1
pin # 41  to Enable blue_ch4      PORTG0
pin # 42  to Enable ir_led        PORTL7
pin # 22 to IND_1A
pin # 23 to IND_1B
pin # 24 to IND_2A
pin # 25 to IND_2B
pin # 26 to IND_3A
pin # 27 to IND_3B
pin # 28 to IND_4A
pin # 29 to IND_4B
*/

// Variable zum Speichern der eingehenden seriellen Daten als Zeichenkette
String serial_info;
const unsigned long frequencies[] = { 62500, 31250, 15625, 7812, 3906, 1953, 976, 488 };
const unsigned long ir_frequencies[] = { 62500, 7812, 976 };

// 2D-Array zur Speicherung der Duty-Werte für die 12 möglichen Kombinationen von Farben und Kanälen
// Farben: 0 = Rot, 1 = Grün, 2 = Blau
// Kanäle: 0 bis 3
int dutyValues[3][4] = { 0 };  // Initialisierung aller Werte mit 0
// 2D-Array zur Speicherung der EN für die 12 möglichen Kombinationen von Farben und Kanälen
// Farben: 0 = Rot, 1 = Grün, 2 = Blau
// Kanäle: 0 bis 3
int rgbPinEN[3][4] = { 0 };  // Initialisierung aller Werte mit 0
// Variable zur Speicherung des Duty-Werts für den IR-Kanal
int ir_dutyValues = 0;  // Initialisiert mit 0
// Variable zur Speicherung des EN für den IR-Kanal
int irPinEN = 0;  // Initialisiert mit 0

uint16_t old_OCR0B = 0;
uint16_t old_OCR1A = 0;
uint16_t old_OCR1B = 0;
uint16_t old_OCR1C = 0;
uint16_t old_OCR3A = 0;
uint16_t old_OCR3B = 0;
uint16_t old_OCR3C = 0;
uint16_t old_OCR4A = 0;
uint16_t old_OCR4B = 0;
uint16_t old_OCR4C = 0;
uint16_t old_OCR5A = 0;
uint16_t old_OCR5B = 0;
uint16_t old_OCR5C = 0;

int Indikator[8] = { 0 };
int sensorValue = analogRead(A0);

// Setup-Funktion wird einmal beim Start des Mikrocontrollers ausgeführt
void setup() {
  initialize_pin();      // Initialisiert die Pins
  Serial.begin(115200);  // Initialisiert die serielle Kommunikation mit 115200 Baud
}

// Hauptprogramm (Loop-Funktion), das kontinuierlich wiederholt wird
void loop() {
  // Prüft, ob serielle Daten verfügbar sind
  if (Serial.available() > 0) {
    // Liest die ankommende Zeichenkette bis zum Zeilenumbruch ('\n')
    serial_info = Serial.readStringUntil('\n');

    // Gibt die empfangene Zeichenkette zur Überprüfung auf der seriellen Konsole aus
    //Serial.println(serial_info);

    // Leert den seriellen Puffer (nicht unbedingt notwendig)
    Serial.flush();

    // Ruft die Funktion zur Verarbeitung der empfangenen Parameter auf
    getParam();
  }
}

// Funktion zur Extraktion und Verarbeitung der Parameter aus dem seriellen Befehl
int getParam() {
  // Prüft, ob die empfangene Nachricht mit "PWM" beginnt
  if (serial_info.startsWith("PWM")) {
    // Findet die Positionen der Trennzeichen ('_') in der Nachricht
    int spaceIndex1 = serial_info.indexOf('_');                   // Erstes '_'
    int spaceIndex2 = serial_info.indexOf('_', spaceIndex1 + 1);  // Zweites '_'
    int spaceIndex3 = serial_info.indexOf('_', spaceIndex2 + 1);  // Drittes '_'

    // Überprüft, ob alle Trennzeichen vorhanden sind
    if (spaceIndex1 != -1 && spaceIndex2 != -1 && spaceIndex3 != -1) {
      // Extrahiert die Farb-, Kanal- und Duty-Zeichenketten aus der Nachricht
      String colorStr = serial_info.substring(spaceIndex1 + 1, spaceIndex2);
      String channelStr = serial_info.substring(spaceIndex2 + 1, spaceIndex3);
      String dutyStr = serial_info.substring(spaceIndex3 + 1);

      // Konvertiert den Duty-Wert in eine Ganzzahl
      int dutyCycle = dutyStr.toInt();

      // Übergibt die extrahierten Werte an die Funktion zur Aktualisierung des Arrays
      updateDutyValue(colorStr, channelStr, dutyCycle);
    }
  }
  // Für IR-LED Frequenz
  if (serial_info.startsWith("FREQ_ir_")) {
    String freq_str = serial_info.substring(8);  // Extrahiere den Frequenzwert nach "FREQ_ir_"
    unsigned long freq = freq_str.toInt();       // Konvertiere String zu Integer

    // Überprüfen, ob die Frequenz in der Liste enthalten ist
    for (int i = 0; i < 3; i++) {
      if (freq == ir_frequencies[i]) {
        set_ir_frequency(freq);
        Serial.print("IR LED Frequenz gesetzt auf: ");
        Serial.println(freq);
        return;
      }
    }

    // Wenn keine passende Frequenz gefunden wurde
    Serial.println("Ungültige IR Frequenz");
  }

  // Für RGB-LED Frequenz
  else if (serial_info.startsWith("FREQ_rgb_")) {
    String freq_str = serial_info.substring(9);  // Extrahiere den Frequenzwert nach "FREQ_rgb_"
    unsigned long freq = freq_str.toInt();       // Konvertiere String zu Integer

    // Überprüfen, ob die Frequenz in der Liste enthalten ist
    for (int i = 0; i < 8; i++) {
      if (freq == frequencies[i]) {
        set_rgb_frequency(freq);
        Serial.print("RGB LED Frequenz gesetzt auf: ");
        Serial.println(freq);
        return;
      }
    }

    // Wenn keine passende Frequenz gefunden wurde
    Serial.println("Ungültige RGB Frequenz");
  }

  if (serial_info.startsWith("EN")) {
    // Findet die Positionen der Trennzeichen ('_') in der Nachricht
    int spaceIndex1 = serial_info.indexOf('_');                   // Erstes '_'
    int spaceIndex2 = serial_info.indexOf('_', spaceIndex1 + 1);  // Zweites '_'
    int spaceIndex3 = serial_info.indexOf('_', spaceIndex2 + 1);  // Drittes '_'

    // Überprüft, ob alle Trennzeichen vorhanden sind
    if (spaceIndex1 != -1 && spaceIndex2 != -1 && spaceIndex3 != -1) {
      // Extrahiert die Farb-, Kanal- und Pinmode
      String colorStr = serial_info.substring(spaceIndex1 + 1, spaceIndex2);
      String channelStr = serial_info.substring(spaceIndex2 + 1, spaceIndex3);
      String pinModeStr = serial_info.substring(spaceIndex3 + 1);

      // Konvertiert pinModeStr in eine Ganzzahl
      int pinMode = pinModeStr.toInt();

      // Übergibt die extrahierten Werte an die Funktion zur Aktualisierung des Arrays
      updatePinMode(colorStr, channelStr, pinMode);
    }
  }
  if (serial_info.startsWith("IND")) {
    // Findet die Positionen der Trennzeichen ('_') in der Nachricht
    int spaceIndex1 = serial_info.indexOf('_');                   // Erstes '_'
    int spaceIndex2 = serial_info.indexOf('_', spaceIndex1 + 1);  // Zweites '_'
    // Überprüft, ob alle Trennzeichen vorhanden sind
    if (spaceIndex1 != -1 && spaceIndex2 != -1) {
      String indStr = serial_info.substring(spaceIndex1 + 1, spaceIndex2);
      String enStr = serial_info.substring(spaceIndex2 + 1);

      int indPin = indStr.toInt();
      int ENMode = enStr.toInt();
      if (indPin >= 1 && indPin <= 8 && (ENMode == 0 || ENMode == 1)) {
        Indikator[indPin - 1] = ENMode;
        //Serial.print("Gespeichert: IND_");
        //Serial.print(indPin);
        //Serial.print("_");
        //Serial.println(ENMode);
        if (ENMode == 1) {
          PORTA |= (1 << indPin - 1);  // Setzt das entsprechende Bit auf HIGH
        } else {
          PORTA &= ~(1 << indPin - 1);  // Setzt das entsprechende Bit auf LOW
        }
      } else Serial.println("Fehler: Ungültige Aktivität des Indikators");
    }
  }
  if (serial_info.startsWith("TEMP")) {
    float sensorValue_bit = analogRead(A0);
    sensorValue_bit = sensorValue_bit * 5 / 1024;
    float Temperatur = (-1525.04) + sqrt(2.4182e6 + ((1.8015 - sensorValue_bit) / (3.479e-6)));
    Serial.println(Temperatur,2);
  }
}

// Funktion zur Aktualisierung der Duty-Werte basierend auf Farbe und Kanal
void updateDutyValue(String color, String channel, unsigned int duty) {
  int colorIndex = -1;    // Initialisierung des Farbindex
  int channelIndex = -1;  // Initialisierung des Kanalindex

  // Wandelt die Farbzeichenkette in den entsprechenden Index um
  if (color == "red") colorIndex = 0;
  else if (color == "green") colorIndex = 1;
  else if (color == "blue") colorIndex = 2;
  else if (color == "ir") colorIndex = 3;

  // Wandelt die Kanalzeichenkette in den entsprechenden Index um (1-4 → 0-3)
  if (channel == "ch1") channelIndex = 0;
  else if (channel == "ch2") channelIndex = 1;
  else if (channel == "ch3") channelIndex = 2;
  else if (channel == "ch4") channelIndex = 3;
  else if (channel == "led") channelIndex = 4;

  // Überprüft, ob die Eingaben gültig sind
  if (colorIndex != -1 && channelIndex >= 0 && channelIndex < 4) {
    // Speichert den neuen Duty-Wert im Array
    dutyValues[colorIndex][channelIndex] = duty;

    // Gibt eine Bestätigung der Speicherung über die serielle Schnittstelle aus
    //Serial.print("Gespeichert: Duty_");
    //Serial.print(color);
    //Serial.print("_");
    //Serial.print(channel);
    //Serial.print(" = ");
    //Serial.println(duty);
  }
  // Falls es sich um den Infrarot-Kanal (ir) und den LED-Kanal handelt
  else if (colorIndex == 3 && channelIndex == 4) {
    ir_dutyValues = duty;  // Speichert den Wert in der IR-Variable

    // Gibt eine Bestätigung der Speicherung über die serielle Schnittstelle aus
    //Serial.print("Gespeichert: Duty_");
    //Serial.print(color);
    //Serial.print("_");
    //Serial.print(channel);
    //Serial.print(" = ");
    //Serial.println(duty);
  }
  // Falls eine ungültige Eingabe gemacht wurde
  else {
    Serial.println("Fehler: Ungültige Farbe oder Kanal.");
  }
  updatePWMForChannelAndColor(colorIndex, channelIndex, duty);
}



// Funktion zur Aktualisierung der LED Aktivität basierend auf Farbe und Kanal
void updatePinMode(String color, String channel, int mode) {
  int colorIndex = -1;    // Initialisierung des Farbindex
  int channelIndex = -1;  // Initialisierung des Kanalindex

  // Wandelt die Farbzeichenkette in den entsprechenden Index um
  if (color == "red") colorIndex = 0;
  else if (color == "green") colorIndex = 1;
  else if (color == "blue") colorIndex = 2;
  else if (color == "ir") colorIndex = 3;

  // Wandelt die Kanalzeichenkette in den entsprechenden Index um (1-4 → 0-3)
  if (channel == "ch1") channelIndex = 0;
  else if (channel == "ch2") channelIndex = 1;
  else if (channel == "ch3") channelIndex = 2;
  else if (channel == "ch4") channelIndex = 3;
  else if (channel == "led") channelIndex = 4;
  if (mode == 0 || mode == 1) {

    // Überprüft, ob die Eingaben gültig sind
    if (colorIndex != -1 && channelIndex >= 0 && channelIndex <= 4) {
      // Speichert den neuen PinMode im Array
      rgbPinEN[colorIndex][channelIndex] = mode;
      // Gibt eine Bestätigung der Speicherung über die serielle Schnittstelle aus
      //Serial.print("Gespeichert: EN_");
      //Serial.print(color);
      //Serial.print("_");
      //Serial.print(channel);
      //Serial.print(" = ");
      //Serial.println(mode);
      //Serial.println(channelIndex);
      //Serial.println(colorIndex);
      if (OCR0B != 0)
        old_OCR0B = OCR0B;
      if (channelIndex == 0) {
        if (colorIndex == 0) {
          if (mode == 0) {
            PORTC |= (1 << PC7);
          } else {
            PORTC &= ~(1 << PC7);
          }
        } else if (colorIndex == 1) {
          if (mode == 0) {
            PORTC |= (1 << PC6);
          } else {
            PORTC &= ~(1 << PC6);
          }
        } else if (colorIndex == 2) {
          if (mode == 0) {
            PORTC |= (1 << PC5);
          } else {
            PORTC &= ~(1 << PC5);
          }
        }
      } else if (channelIndex == 1) {
        if (colorIndex == 0) {
          if (mode == 0) {
            PORTC |= (1 << PC4);
          } else {
            PORTC &= ~(1 << PC4);
          }
        } else if (colorIndex == 1) {
          if (mode == 0) {
            PORTC |= (1 << PC3);
          } else {
            PORTC &= ~(1 << PC3);
          }
        } else if (colorIndex == 2) {
          if (mode == 0) {
            PORTC |= (1 << PC2);
          } else {
            PORTC &= ~(1 << PC2);
          }
        }
      } else if (channelIndex == 2) {
        if (colorIndex == 0) {
          if (mode == 0) {
            PORTC |= (1 << PC1);
          } else {
            PORTC &= ~(1 << PC1);
          }
        } else if (colorIndex == 1) {
          if (mode == 0) {
            PORTC |= (1 << PC0);
          } else {
            PORTC &= ~(1 << PC0);
          }
        } else if (colorIndex == 2) {
          if (mode == 0) {
            PORTD |= (1 << PD7);
          } else {
            PORTD &= ~(1 << PD7);
          }
        }
      } else if (channelIndex == 3) {
        if (colorIndex == 0) {
          if (mode == 0) {
            PORTG |= (1 << PG2);
          } else {
            PORTG &= ~(1 << PG2);
          }
        } else if (colorIndex == 1) {
          if (mode == 0) {
            PORTG |= (1 << PG1);
          } else {
            PORTG &= ~(1 << PG1);
          }
        } else if (colorIndex == 2) {
          if (mode == 0) {
            PORTG |= (1 << PG0);
          } else {
            PORTG &= ~(1 << PG0);
          }
        }
      }
      // Falls es sich um den Infrarot-Kanal (ir) und den LED-Kanal handelt
      else if (colorIndex == 3 && channelIndex == 4) {
        irPinEN = mode;  // Speichert den Wert in der IR-Variable
        if (mode == 0) {
          PORTL |= (1 << PL7);
          OCR0B = 0;
        } else {
          PORTL &= ~(1 << PL7);
          OCR0B = old_OCR0B;
        }
      }
    }
    // Falls eine ungültige Eingabe gemacht wurde
    else {
      Serial.println("Fehler: Ungültige Farbe oder Kanal.");
    }
  } else Serial.println("Fehler: Ungültige Aktivität");
}