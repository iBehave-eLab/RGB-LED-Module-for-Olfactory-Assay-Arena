void updatePWMForChannelAndColor(int colorIndex, int channelIndex, long duty) {
  long pwmValue = 0;
  // Umrechnung von Prozent (0-100) auf Timer-Werte (0-255)

  // Aktualisiere das entsprechende OCR-Register basierend auf Kanal und Farbe
  switch (channelIndex) {
    case 0:  // Kanal 1 (Timer 1)
      pwmValue = calculatePWM(duty, TCCR1A, TCCR1B);
      if (colorIndex == 0) OCR1C = pwmValue;       // Rot
      else if (colorIndex == 1) OCR1B = pwmValue;  // Grün
      else if (colorIndex == 2) OCR1A = pwmValue;  // Blau
      break;

    case 1:  // Kanal 2 (Timer 3)
      pwmValue = calculatePWM(duty, TCCR3A, TCCR3B);
      if (colorIndex == 0) OCR3C = pwmValue;       // Rot
      else if (colorIndex == 1) OCR3B = pwmValue;  // Grün
      else if (colorIndex == 2) OCR3A = pwmValue;  // Blau
      break;

    case 2:  // Kanal 3 (Timer 4)
      pwmValue = calculatePWM(duty, TCCR4A, TCCR4B);
      if (colorIndex == 0) OCR4C = pwmValue;       // Rot
      else if (colorIndex == 1) OCR4B = pwmValue;  // Grün
      else if (colorIndex == 2) OCR4A = pwmValue;  // Blau
      break;

    case 3:  // Kanal 4 (Timer 5)
      pwmValue = calculatePWM(duty, TCCR5A, TCCR5B);
      if (colorIndex == 0) OCR5C = pwmValue;       // Rot
      else if (colorIndex == 1) OCR5B = pwmValue;  // Grün
      else if (colorIndex == 2) OCR5A = pwmValue;  // Blau
      break;
    case 4:  // (Timer 0)
      pwmValue = (duty * 255) / 100;
      OCR0B = pwmValue;  // IR LEd
      break;
  }
}

uint16_t calculatePWM(long duty, volatile uint8_t &TCCRnA, volatile uint8_t &TCCRnB) {
  long pwmValue = 0;
  if ((TCCRnA & _BV(WGM10)) && (TCCRnA & _BV(WGM11)) && (TCCRnB & _BV(WGM12))) {
    pwmValue = (duty * 1023) / 100;
  } else if ((TCCRnA & _BV(WGM10)) && (TCCRnB & _BV(WGM12))) {
    pwmValue = (duty * 255) / 100;
  } else if ((TCCRnA & _BV(WGM11)) && (TCCRnB & _BV(WGM12))) {
    pwmValue = (duty * 511) / 100;
  }
  return pwmValue;
}