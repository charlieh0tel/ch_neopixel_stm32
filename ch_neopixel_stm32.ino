#define NEO_PIN 6
#define NEO_NLEDS 32
#define NEO_400KHZ

#ifdef NEO_400KHZ
#define NEO_BIT_TIME 2.5e-6
#define NEO_T0H_TIME 0.5e-6
#define NEO_T1H_TIME 1.2e-6
#endif

#ifdef NEO_800KHZ
#define NEO_BIT_TIME 1.25e-6
#define NEO_T0H_TIME 0.4e-6
#define NEO_T1H_TIME 0.8e-6
#endif

#if !defined(NEO_BIT_TIME) || !defined(NEO_T0H_TIME) || !defined(NEO_T1H_TIME)
#error "neo bit times not set"
#endif

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

  uint32_t cycles_per_bit = (uint32_t) (F_CPU * NEO_BIT_TIME);
  uint32_t cycles_t0h = (uint32_t) (F_CPU * NEO_T0H_TIME);
  uint32_t cycles_t1h = (uint32_t) (F_CPU * NEO_T1H_TIME);
  uint32_t t0h_midbit_count = cycles_per_bit - cycles_t0h;
  uint32_t t1h_midbit_count = cycles_per_bit - cycles_t1h;

  SysTick->LOAD = cycles_per_bit - 1;
  SysTick->VAL = 0;
  for (int i = 0; i < NEO_NLEDS * 3; i++) {
    uint8_t bits = rainbow[i % sizeof rainbow];
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
