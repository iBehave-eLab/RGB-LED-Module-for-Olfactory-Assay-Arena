void initialize_pin() {
  // Pins 22-41 als Ausgänge definieren
  // Port A (Pins 22-29)
  DDRA = 0xFF;  // Alle Pins von Port A als Ausgang

  // Port C (Pins 30-37)
  DDRC = 0xFF;  // Alle Pins von Port C als Ausgang

  // Port D (Pin 38)
  DDRD = _BV(DDD7);  // Pin 38 als Ausgang

  // Port G (Pins 39-41)
  DDRG = _BV(DDG2) | _BV(DDG1) | _BV(DDG0);  // Pins 39, 40, 41 als Ausgang

  DDRL |= (1 << PL7);  // Setzt Pin 42 als Ausgang

  // PWM-Pins konfigurieren
  // Port B (Pins 11, 12, 13 für PWM)
  DDRB |= _BV(DDB7) | _BV(DDB6) | _BV(DDB5);  // OC1C, OC1B, OC1A (Pins 13, 12, 11)

  // Port E (Pins 2, 3, 5 für PWM)
  DDRE |= _BV(DDE5) | _BV(DDE4) | _BV(DDE3);  // OC3C, OC3B, OC3A (Pins 3, 2, 5)

  // Port H (Pins 6, 7, 8 für PWM)
  DDRH |= _BV(DDH5) | _BV(DDH4) | _BV(DDH3);  // OC4C, OC4B, OC4A (Pins 8, 7, 6)

  // Port L (Pins 44, 45, 46 für PWM)
  DDRL |= _BV(DDL5) | _BV(DDL4) | _BV(DDL3);  // OC5C, OC5B, OC5A (Pins 44, 45, 46)

  // Port G (Pin 4 für PWM)
  DDRG |= _BV(DDG5);  // OC0B (Pin 4)

  // Alle Ausgänge auf LOW setzen
  PORTA = 0;
  PORTC = 0xFF;                                // Alle Ausgänge 30-37 auf HOGH setzen
  PORTD |= _BV(PD7);                           // Nur Pin 38 auf HIGH setzen, andere nicht ändern
  PORTG |= _BV(PG2) | _BV(PG1) | _BV(PG0);     // Pins 39, 40, 41 auf HIGH setzen
  PORTL |= _BV(PL7);                           // Setzt Pin 42 auf HIGH
  PORTG &= ~_BV(PG5);                          // Pin 4 separat auf LOW setzen
  PORTB &= ~(_BV(PB7) | _BV(PB6) | _BV(PB5));  // Pins 13, 12, 11 auf LOW setzen
  PORTE &= ~(_BV(PE5) | _BV(PE4) | _BV(PE3));  // Pins 3, 2, 5 auf LOW setzen
  PORTH &= ~(_BV(PH5) | _BV(PH4) | _BV(PH3));  // Pins 8, 7, 6 auf LOW setzen
  PORTL &= ~(_BV(PL5) | _BV(PL4) | _BV(PL3));  // Pins 44, 45, 46 auf LOW setzen


  // Timer 0 konfigurieren (8-bit) - für ir_led (Pin 4)
  TCCR0A = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);  // Fast PWM 8bit, nicht-invertierend
  TCCR0B = _BV(CS01);                              // Kein Prescaler
  OCR0B = 0;                                       // PWM auf 0% setzen



  // Timer 1 konfigurieren (8-bit) - für RGB ch1 (Pins 11, 12, 13)
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1C1)| _BV(WGM11) | _BV(WGM10);  // Fast PWM 10-bit, nicht-invertierend
  TCCR1B = _BV(WGM12) | _BV(CS10);                                // Fast PWM, kein Prescaler
  OCR1A = 0;                                                      // Blue Ch1 PWM auf 0%
  OCR1B = 0;                                                      // Green Ch1 PWM auf 0%
  OCR1C = 0;                                                      // Red Ch1 PWM auf 0%

  // Timer 3 konfigurieren (8-bit) - für RGB ch2 (Pins 2, 3, 5)
  TCCR3A = _BV(COM3A1) | _BV(COM3B1) | _BV(COM3C1)| _BV(WGM31) | _BV(WGM30);  // Fast PWM 10-bit, nicht-invertierend
  TCCR3B = _BV(WGM32) | _BV(CS30);                                // Fast PWM, kein Prescaler
  OCR3A = 0;                                                      // Blue Ch2 PWM auf 0%
  OCR3B = 0;                                                      // Green Ch2 PWM auf 0%
  OCR3C = 0;                                                      // Red Ch2 PWM auf 0%

  // Timer 4 konfigurieren (8-bit) - für RGB ch3 (Pins 6, 7, 8)
  TCCR4A = _BV(COM4A1) | _BV(COM4B1) | _BV(COM4C1)| _BV(WGM41) | _BV(WGM40);  // Fast PWM 10-bit, nicht-invertierend
  TCCR4B = _BV(WGM42) | _BV(CS40);                                // Fast PWM, kein Prescaler
  OCR4A = 0;                                                      // Blue Ch3 PWM auf 0%
  OCR4B = 0;                                                      // Green Ch3 PWM auf 0%
  OCR4C = 0;                                                      // Red Ch3 PWM auf 0%

  // Timer 5 konfigurieren (8-bit) - für RGB ch4 (Pins 44, 45, 46)
  TCCR5A = _BV(COM5A1) | _BV(COM5B1) | _BV(COM5C1)| _BV(WGM51) | _BV(WGM50);  // Fast PWM 10-bit, nicht-invertierend
  TCCR5B = _BV(WGM52) | _BV(CS50);                                // Fast PWM, kein Prescaler
  OCR5A = 0;                                                      // Blue Ch4 PWM auf 0%
  OCR5B = 0;                                                      // Green Ch4 PWM auf 0%
  OCR5C = 0;                                                      // Red Ch4 PWM auf 0%
}