// ESP32-C6 ADC Min/Max ve Yüzde Gösterimi
// Bağlantı: Ölçüm hattını GPIO4'e takın (ADC1_CH4)

const int   ADC_PIN       = 4;             // ADC1_CH4 (ESP32-C6'da analog giriş)
const int   NUM_SAMPLES   = 16;            // Gürültü için ortalama
const float DIVIDER_GAIN  = 1.0f;          // Voltaj bölücü varsa ayarlayın
const float K_EPS         = 1e-6f;         // Sıfıra bölme koruması

float vMin = NAN, vMax = NAN;

void setup() {
  Serial.begin(115200);
  delay(300);

  analogReadResolution(12);                // 0..4095 aralığı
  analogSetPinAttenuation(ADC_PIN, ADC_11db); // 0..3.3V giriş aralığı

  Serial.println("ESP32-C6 ADC - Min/Max ve Yuzde Gosterimi (GPIO4)");
  Serial.println("Seri porttan 'r' gonderirsen min/max sifirlanir.");
}

float readVoltage() {
  // Ortalama ADC okuma (ham)
  uint32_t sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(ADC_PIN);
    delay(2);
  }
  float avg = (float)sum / NUM_SAMPLES;

  // 12-bit çözünürlükte maksimum 4095 değeri ≈ 3.3V
  float v = (avg / 4095.0f) * 3.3f * DIVIDER_GAIN;
  return v;
}

void resetMinMax(float v) {
  vMin = v;
  vMax = v;
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'r' || c == 'R') {
      float v = readVoltage();
      resetMinMax(v);
      Serial.println("Min/Max sifirlandi.");
    }
  }

  float v = readVoltage();

  if (isnan(vMin) || isnan(vMax)) {
    resetMinMax(v);
  }

  if (v < vMin) vMin = v;
  if (v > vMax) vMax = v;

  float span = vMax - vMin;
  float pct  = (span > K_EPS) ? ((v - vMin) / span) * 100.0f : 0.0f;

  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;

  Serial.print("V: ");
  Serial.print(v, 3);
  Serial.print(" V  |  Min: ");
  Serial.print(vMin, 3);
  Serial.print(" V  |  Max: ");
  Serial.print(vMax, 3);
  Serial.print(" V  |  %: ");
  Serial.println(pct, 1);

  delay(200);
}
