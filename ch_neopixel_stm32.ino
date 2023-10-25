#define NEO_PIN 6

const uint8_t rainbow[] = {
  /* GRB */
  0x00, 0x2A, 0x00,
  0x01, 0x2A, 0x00,
  0x03, 0x2A, 0x00,
  0x09, 0x2A, 0x00,
  0x14, 0x2A, 0x00,
  0x23, 0x2A, 0x00,
  0x2A, 0x1D, 0x00,
  0x2A, 0x10, 0x00,
  0x2A, 0x07, 0x00,
  0x2A, 0x02, 0x00,
  0x2A, 0x00, 0x00,
  0x2A, 0x00, 0x00,
  0x2A, 0x00, 0x01,
  0x2A, 0x00, 0x05,
  0x2A, 0x00, 0x0C,
  0x2A, 0x00, 0x18,
  0x2A, 0x00, 0x2A,
  0x18, 0x00, 0x2A,
  0x0C, 0x00, 0x2A,
  0x05, 0x00, 0x2A,
  0x01, 0x00, 0x2A,
  0x00, 0x00, 0x2A,
  0x00, 0x00, 0x2A,
  0x00, 0x02, 0x2A,
  0x00, 0x07, 0x2A,
  0x00, 0x10, 0x2A,
  0x00, 0x1D, 0x2A,
  0x00, 0x2A, 0x23,
  0x00, 0x2A, 0x14,
  0x00, 0x2A, 0x09,
  0x00, 0x2A, 0x03,
  0x00, 0x2A, 0x01,
};

void setup() {
  pinMode(NEO_PIN, OUTPUT);
  digitalWrite(NEO_PIN, LOW);
  delayMicroseconds(100);
}

void loop() {
  auto gpioPort = digitalPinToPort(NEO_PIN);
  auto gpioPin = STM_LL_GPIO_PIN(digitalPinToPinName(NEO_PIN));

  uint32_t cycles_per_bit = (uint32_t) (F_CPU * 2.5e-6);
  uint32_t cycles_t0h = (uint32_t) (F_CPU * 0.5e-6);
  uint32_t cycles_t1h = (uint32_t) (F_CPU * 1.2e-6);
  uint32_t t0h_midbit_count = cycles_per_bit - cycles_t0h;
  uint32_t t1h_midbit_count = cycles_per_bit - cycles_t1h;

  int i = 0;
  const uint8_t *p = rainbow;
  SysTick->LOAD = cycles_per_bit - 1;
  SysTick->VAL = 0;
  for (; i < sizeof(rainbow); i++, p++) {
    uint8_t bits = *p;
    for (uint8_t mask = 0x80; mask; mask >>= 1) {
      LL_GPIO_SetOutputPin(gpioPort, gpioPin);
      uint32_t midbit_count = (bits & mask) ? t1h_midbit_count : t0h_midbit_count;
      while (SysTick->VAL > midbit_count)
        ;
      LL_GPIO_ResetOutputPin(gpioPort, gpioPin);
      while (SysTick->VAL <= midbit_count)
        ;
    }
  }

  // Sleep, perchance to dream ...
  while (1) {}
}
