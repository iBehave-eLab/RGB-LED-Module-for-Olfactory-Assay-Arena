// Funktion zum Einstellen der IR-LED Frequenz (Timer 0)
void set_ir_frequency(unsigned long freq) {
  // Timer 0 Konfiguration (8-bit Timer für IR-LED auf Pin 4)

  // Aktuelle Zustände speichern
  uint16_t old_OCR0B = OCR0B;

  // Timer stoppen
  TCCR0B &= ~(_BV(CS02) | _BV(CS01) | _BV(CS00));

  // Fast PWM Mode beibehalten
  TCCR0A = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);  // Fast PWM, nicht-invertierend

  // Prescaler basierend auf der gewünschten Frequenz einstellen
  switch (freq) {
    case 62500:
      // Kein Prescaler
      TCCR0B |= _BV(CS00);
      break;
    case 7812:
      // Prescaler = 8
      TCCR0B |= _BV(CS01);
      break;
    case 976:
      // Prescaler = 64
      TCCR0B |= _BV(CS01) | _BV(CS00);
      break;
    default:
      // Standard: Kein Prescaler
      break;
  }

  // Alten PWM-Wert wiederherstellen
  //OCR0B = old_OCR0B;
  OCR0B = 0;
}

// Funktion zum Einstellen der RGB-LED Frequenz (Timer 1, 3, 4, 5)
void set_rgb_frequency(unsigned long freq) {
  // Aktuelle Zustände speichern
  uint16_t old_OCR1A = OCR1A;
  uint16_t old_OCR1B = OCR1B;
  uint16_t old_OCR1C = OCR1C;
  uint16_t old_OCR3A = OCR3A;
  uint16_t old_OCR3B = OCR3B;
  uint16_t old_OCR3C = OCR3C;
  uint16_t old_OCR4A = OCR4A;
  uint16_t old_OCR4B = OCR4B;
  uint16_t old_OCR4C = OCR4C;
  uint16_t old_OCR5A = OCR5A;
  uint16_t old_OCR5B = OCR5B;
  uint16_t old_OCR5C = OCR5C;
  // Timer stoppen
  TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
  TCCR3B &= ~(_BV(CS32) | _BV(CS31) | _BV(CS30));
  TCCR4B &= ~(_BV(CS42) | _BV(CS41) | _BV(CS40));
  TCCR5B &= ~(_BV(CS52) | _BV(CS51) | _BV(CS50));
  // Timer 1
  TCCR1A &= ~(_BV(WGM11) | _BV(WGM10));
  TCCR1B &= ~(_BV(WGM12));
  // Timer 3
  TCCR3A &= ~(_BV(WGM31) | _BV(WGM30));
  TCCR3B &= ~(_BV(WGM32));

  // Timer 4
  TCCR4A &= ~(_BV(WGM41) | _BV(WGM40));
  TCCR4B &= ~(_BV(WGM42));

  // Timer 5
  TCCR5A &= ~(_BV(WGM51) | _BV(WGM50));
  TCCR5B &= ~(_BV(WGM52));


  switch (freq) {
    case 62500:
      TCCR1A |= _BV(WGM10);
      TCCR1B |= _BV(WGM12) | _BV(CS10);

      TCCR3A |= _BV(WGM10);
      TCCR3B |= _BV(WGM12) | _BV(CS10);

      TCCR4A |= _BV(WGM10);
      TCCR4B |= _BV(WGM12) | _BV(CS10);

      TCCR5A |= _BV(WGM10);
      TCCR5B |= _BV(WGM12) | _BV(CS10);
      break;

    case 31250:
      TCCR1A |= _BV(WGM11);
      TCCR1B |= _BV(WGM12) | _BV(CS10);

      TCCR3A |= _BV(WGM11);
      TCCR3B |= _BV(WGM12) | _BV(CS10);

      TCCR4A |= _BV(WGM11);
      TCCR4B |= _BV(WGM12) | _BV(CS10);

      TCCR5A |= _BV(WGM11);
      TCCR5B |= _BV(WGM12) | _BV(CS10);
      break;

    case 15625:
      TCCR1A |= _BV(WGM10) | _BV(WGM11);
      TCCR1B |= _BV(WGM12) | _BV(CS10);

      TCCR3A |= _BV(WGM10) | _BV(WGM11);
      TCCR3B |= _BV(WGM12) | _BV(CS10);

      TCCR4A |= _BV(WGM10) | _BV(WGM11);
      TCCR4B |= _BV(WGM12) | _BV(CS10);

      TCCR5A |= _BV(WGM10) | _BV(WGM11);
      TCCR5B |= _BV(WGM12) | _BV(CS10);
      break;

    case 7812:
      TCCR1A |= _BV(WGM10);
      TCCR1B |= _BV(WGM12) | _BV(CS11);

      TCCR3A |= _BV(WGM10);
      TCCR3B |= _BV(WGM12) | _BV(CS11);

      TCCR4A |= _BV(WGM10);
      TCCR4B |= _BV(WGM12) | _BV(CS11);

      TCCR5A |= _BV(WGM10);
      TCCR5B |= _BV(WGM12) | _BV(CS11);
      break;

    case 3906:
      TCCR1A |= _BV(WGM11);
      TCCR1B |= _BV(WGM12) | _BV(CS11);

      TCCR3A |= _BV(WGM11);
      TCCR3B |= _BV(WGM12) | _BV(CS11);

      TCCR4A |= _BV(WGM11);
      TCCR4B |= _BV(WGM12) | _BV(CS11);

      TCCR5A |= _BV(WGM11);
      TCCR5B |= _BV(WGM12) | _BV(CS11);
      break;

    case 1953:
      TCCR1A |= _BV(WGM10) | _BV(WGM11);
      TCCR1B |= _BV(WGM12) | _BV(CS11);

      TCCR3A |= _BV(WGM10) | _BV(WGM11);
      TCCR3B |= _BV(WGM12) | _BV(CS11);

      TCCR4A |= _BV(WGM10) | _BV(WGM11);
      TCCR4B |= _BV(WGM12) | _BV(CS11);

      TCCR5A |= _BV(WGM10) | _BV(WGM11);
      TCCR5B |= _BV(WGM12) | _BV(CS11);
      break;

    case 976:
      TCCR1A |= _BV(WGM10);
      TCCR1B |= _BV(WGM12) | _BV(CS10) | _BV(CS11);

      TCCR3A |= _BV(WGM10);
      TCCR3B |= _BV(WGM12) | _BV(CS10) | _BV(CS11);

      TCCR4A |= _BV(WGM10);
      TCCR4B |= _BV(WGM12) | _BV(CS10) | _BV(CS11);

      TCCR5A |= _BV(WGM10);
      TCCR5B |= _BV(WGM12) | _BV(CS10) | _BV(CS11);
      break;

    case 488:
      TCCR1A |= _BV(WGM11);
      TCCR1B |= _BV(WGM12) | _BV(CS10) | _BV(CS11);

      TCCR3A |= _BV(WGM11);
      TCCR3B |= _BV(WGM12) | _BV(CS10) | _BV(CS11);

      TCCR4A |= _BV(WGM11);
      TCCR4B |= _BV(WGM12) | _BV(CS10) | _BV(CS11);

      TCCR5A |= _BV(WGM11);
      TCCR5B |= _BV(WGM12) | _BV(CS10) | _BV(CS11);
      break;
    default:
      //TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
      //TCCR3B &= ~(_BV(CS32) | _BV(CS31) | _BV(CS30));
      // TCCR4B &= ~(_BV(CS42) | _BV(CS41) | _BV(CS40));
      // TCCR5B &= ~(_BV(CS52) | _BV(CS51) | _BV(CS50));
      break;
  }

  // Alte PWM-Werte wiederherstellen
  /*OCR1A = old_OCR1A;
  OCR1B = old_OCR1B;
  OCR1C = old_OCR1C;
  OCR3A = old_OCR3A;
  OCR3B = old_OCR3B;
  OCR3C = old_OCR3C;
  OCR4A = old_OCR4A;
  OCR4B = old_OCR4B;
  OCR4C = old_OCR4C;
  OCR5A = old_OCR5A;
  OCR5B = old_OCR5B;
  OCR5C = old_OCR5C;*/

  OCR1A = 0;
  OCR1B = 0;
  OCR1C = 0;
  OCR3A = 0;
  OCR3B = 0;
  OCR3C = 0;
  OCR4A = 0;
  OCR4B = 0;
  OCR4C = 0;
  OCR5A = 0;
  OCR5B = 0;
  OCR5C = 0;
}